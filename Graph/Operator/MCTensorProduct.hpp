//  MCTensorProduct class, part of the SPARE library.
//  Copyright (C) 2011 Lorenzo Livi
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

/** @brief File MCTensorProduct.hpp, that contains the tensor product class.
 *
 * Contains the declaration of the Multicore TensorProduct class.
 * It is a multi-threaded version of the standard tensor product computation, using a user-defined number of threads.
 *
 * @file MCTensorProduct.hpp
 * @author Lorenzo Livi
 */

#ifndef MCTENSORPRODUCT_HPP
#define MCTENSORPRODUCT_HPP


//STD INCLUDES
#include <vector>

//BOOST INCLUDES
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {  // Inclusione in namespace spare.


/** @brief Multicore computation of the tensor product between two labeled graphs.
 *
 * This class implements the @a Operator concept of the graphs domain (i.e., an operator between graphs).
 * It contains two main types of interfaces: the first one uses object references, the second one uses pointers (mainly for multi-threading).
 * The number of threads is configurable by the user in the third template argument.
 */
template <class KernelType, class MatrixRepresentation, NaturalType NThreads>
class MCTensorProduct {
public:

    /**
     * Tensor product computation.
     * No explicit output tensor product matrix computation, and allocation, is performed.
     *
     * @param[in] g1 A reference to the first graph
     * @param[in] g2 A reference to the second graph
     * @return The weight of the tensor product graph.
     */
    template <class GraphType>
    RealType product(const GraphType& g1, const GraphType& g2) const;

    /**
     * Tensor product computation using pointers instead of references in the interface (for threads compliance).
     * No explicit output tensor product matrix computation is performed.
     *
     * @param[in] g1 A pointer to the first graph
     * @param[in] g2 A pointer to the second graph
     * @param[out] weight A pointer to the weight of the tensor product graph
     */
    template <class GraphType>
    void product(const GraphType* g1, const GraphType* g2, RealType* weight) const;


    /**
     * Read/Write access to the main kernel function
     */
    KernelType& KernelAgent() { return kernel; }

    /**
     * Read-only access to the main kernel function
     */
    const KernelType& KernelAgent() const { return kernel; }

    /**
     * Read/Write access to the matrix representation for the graph
     */
    MatrixRepresentation& RepresentationAgent() { return mr; }

    /**
     * Read-only access to the matrix representation for the graph
     */
    const MatrixRepresentation& RepresentationAgent() const { return mr; }

    /**
     * Read-only access to the number of threads
     */
    const NaturalType& ThreadsAgent() const { return NThreads; }

private:

    /**
     * Thread function for block-matrix weight computation
     * @param[in] g1 A pointer to the first graph
     * @param[in] g2 A pointer to the second graph
     * @param[in] m1 A pointer to the first adjacency matrix
     * @param[in] m2 A pointer to the second adjacency matrix
     * @param[in] pos
     * @param[in] g2Order
     * @param[out] weight
     */
    template <class GraphType>
    void threadProduct(const GraphType* g1, const GraphType* g2,
            const boost::numeric::ublas::matrix<RealType>* m1,
            const boost::numeric::ublas::matrix<RealType>* m2,
            const NaturalType* pos, const NaturalType g2Order,
            RealType* weight) const;

    /**
     * Main composite kernel function
     */
    KernelType kernel;

    /**
     * Matrix representation for the graph
     */
    MatrixRepresentation mr;

    //mutex
    mutable boost::mutex boostMutex;
};


//IMPL


template <class KernelType, class MatrixRepresentation, NaturalType NThreads>
template <class GraphType>
void MCTensorProduct<KernelType, MatrixRepresentation, NThreads>::product(const GraphType* g1,
        const GraphType* g2, RealType* weight) const
{
    *weight=product(*g1, *g2);
}


template <class KernelType, class MatrixRepresentation, NaturalType NThreads>
template <class GraphType>
RealType MCTensorProduct<KernelType, MatrixRepresentation, NThreads>::product(const GraphType& g1,
        const GraphType& g2) const
{
    NaturalType o1=boost::num_vertices(g1);
    NaturalType o2=boost::num_vertices(g2);
    std::vector<NaturalType*> t;
    NaturalType rStart, rEnd, cStart, cEnd, block, tot, totThreads;
    //the weight of the tensor product graph
    RealType weight=0.;

    //matrix representations of the two input graphs
    boost::numeric::ublas::matrix<RealType> m1(o1, o1), m2(o2, o2);
    //threads
    boost::thread_group threadGroup;

    if(NThreads>o1)
        totThreads=o1;
    else
        totThreads=NThreads;

    //matrix representations of the graphs
    mr.getMatrix(g1, m1);
    mr.getMatrix(g2, m2);

    //compute tasks subdivision
    tot=o1*o1;
    block=tot/totThreads;
    RealType v=(RealType)block/(RealType)o1;
    NaturalType flooredRows=floor(v);
    NaturalType ceiledRows=ceil(v);
    //oldCEnd is equal to the next cStart
    NaturalType oldCEnd=0, rowsShift=0;

//    std::cout<<std::endl<<"Threads: "<<NThreads<<std::endl;
//    std::cout<<"Tot: "<<tot<<std::endl;
//    std::cout<<"BLock: "<<block<<std::endl;

    //threads dispatching
    for(NaturalType i=0;i<totThreads;i++)
    {
//        std::cout<<std::endl<<"T: "<<i<<std::endl;
//        std::cout<<"FLOORED blocks/o1: "<<flooredRows<<std::endl;
//        std::cout<<"CEILED  blocks/o1: "<<ceiledRows<<std::endl;
//        std::cout<<"ROWS SHIFT: "<<rowsShift<<std::endl;
//        std::cout<<"Old cEnd: "<<oldCEnd<<std::endl;

        //TODO: il bug sembra essere risolto, ma e' sempre meglio fare ulteriori verifiche...

        rStart=i*flooredRows+rowsShift;
        rEnd=rStart+ceiledRows;

        cStart=oldCEnd;
        cEnd=block-(((rEnd-rStart)-1)*o1)+oldCEnd;

        //bounds check
        if(rEnd>o1)
            rEnd=o1;

        if(cEnd>o1)
            cEnd=o1;

        //next column start
        if(cEnd>=o1)
            oldCEnd=0;
        else
            oldCEnd=cEnd;

        //the next start row is incremented by 1
        if(cStart!=0&&cEnd==o1)
            rowsShift++;

        if(i==totThreads-1)
        {
            rEnd=o1;
            cEnd=o1;
        }

//        std::cout<<"Row Start: "<<rStart<<std::endl;
//        std::cout<<"Row End (excluded): "<<rEnd<<std::endl;
//        std::cout<<"Col Start: "<<cStart<<std::endl;
//        std::cout<<"Col End (excluded): "<<cEnd<<std::endl;
//        std::cout<<"Old cEnd (next cStart): "<<oldCEnd<<std::endl<<std::endl;

        //dynamic positions buffer
        NaturalType* pos=new NaturalType[5];
        pos[0]=rStart;
        pos[1]=rEnd;
        pos[2]=cStart;
        pos[3]=cEnd;
        pos[4]=o1;
        t.push_back(pos);

        //new thread
        threadGroup.add_thread(new boost::thread(
                boost::bind(&MCTensorProduct::threadProduct<GraphType>,
                            this, &g1, &g2, &m1, &m2, pos, o2, &weight)));
    }

    threadGroup.join_all();

    //free dynamic positions buffer
    for(NaturalType i=0;i<totThreads;i++)
        delete t[i];

    return weight;
}

template <class KernelType, class MatrixRepresentation, NaturalType NThreads>
template <class GraphType>
void MCTensorProduct<KernelType, MatrixRepresentation, NThreads>::threadProduct(
        const GraphType* g1, const GraphType* g2,
        const boost::numeric::ublas::matrix<RealType>* m1, const boost::numeric::ublas::matrix<RealType>* m2,
        const NaturalType* pos, const NaturalType g2Order,
        RealType* weight) const
{
    //part of the weight computed by this thread
    RealType localWeight=0.;

    bool isFirst=true;
    NaturalType startColM1, endColM1=0;

    //fetch the assigne block of positions
    for(NaturalType rM1=pos[0]; rM1<pos[1]; rM1++)
    {
//        std::cout<<"RM1: "<<rM1<<std::endl;

        //g1 columns determination based on the current row rM1
        if(isFirst)
        {
            isFirst=false;
            startColM1=pos[2];
        }
        else
            startColM1=0;

        if(rM1==pos[1]-1)
            endColM1=pos[3];
        else
            endColM1=pos[4];

//        std::cout<<"START COL: "<<startColM1<<std::endl;
//        std::cout<<"END COL: "<<endColM1<<std::endl;

        for(NaturalType cM1=startColM1; cM1<endColM1; cM1++)
        {
//            std::cout<<"CM1: "<<cM1<<std::endl;

            if((*m1)(rM1, cM1))
            {
                //data of g2
                for(NaturalType rM2=0; rM2<g2Order; rM2++)
                {
                    for(NaturalType cM2=0; cM2<g2Order; cM2++)
                    {
                        if((*m2)(rM2, cM2))
                        {
                            localWeight+=kernel.Sim(
                                    boost::get(v_info_t(), *g1, boost::vertex(rM1, *g1)),
                                    boost::get(v_info_t(), *g2, boost::vertex(rM2, *g2)),
                                    boost::get(v_info_t(), *g1, boost::vertex(cM1, *g1)),
                                    boost::get(v_info_t(), *g2, boost::vertex(cM2, *g2)),
                                    boost::get(e_info_t(), *g1, boost::edge(rM1, cM1, *g1).first),
                                    boost::get(e_info_t(), *g2, boost::edge(rM2, cM2, *g2).first)
                                    );
                        }
                    }
                }
            }//if
        }
    }

    //synchronized access to the weight
    boostMutex.lock();
    *weight+=localWeight;
    boostMutex.unlock();
}

}

#endif /* MCTENSORPRODUCT_HPP */
