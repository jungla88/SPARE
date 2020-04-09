//  SymTensorProduct class, part of the SPARE library.
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

/** @brief File SymTensorProduct.hpp, that contains the symmetric tensor product class.
 *
 * Contains the declaration of the SymTensorProduct class.
 *
 * @file SymTensorProduct.hpp
 * @author Lorenzo Livi
 */

#ifndef SYMTENSORPRODUCT_HPP
#define SYMTENSORPRODUCT_HPP

//BOOST INCLUDES
#include <boost/graph/graph_utility.hpp>
#include <boost/numeric/ublas/matrix.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {  // Inclusione in namespace spare.


/** @brief Tensor product computation for labeled graphs, assuming symmetric matrix representations.
 *
 * This class implements the @a Operator concept of the graphs domain (i.e., an operator between graphs).
 * It contains two main types of interfaces: the first one uses references, the second one uses pointers (mainly for multi-threading).
 * The methods assumes that the employed matrix representations are always symmetric.
 */
template <class KernelType, class MatrixRepresentation>
class SymTensorProduct {
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
     * Tensor product computation explicitly allocating and computing the resulting matrix.
     *
     * @param[in] g1 A reference to the first graph
     * @param[in] g2 A reference to the second graph
     * @param[out] mOut A reference to the (weighted) matrix of the tensor product graph
     * @return The weight of the tensor product graph.
     */
    template <class GraphType>
    RealType product(const GraphType& g1, const GraphType& g2,
            boost::numeric::ublas::matrix<RealType>& mOut) const;

    /**
     * Tensor product computation using pointers instead of references in the interface (for threads compliance).
     * No explicit output tensor product matrix computation is performed.
     *
     * @param[in] g1 A pointer to the first graph
     * @param[in] g2 A pointer to the second graph
     * @param[out] weight A pointer to the weight of the tensor product graph
     */
    template <class GraphType>
    void productP(const GraphType* g1, const GraphType* g2, RealType* weight) const;


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

private:

    /**
     * Main kernel module
     */
    KernelType kernel;

    /**
     * Matrix representation for the graph
     */
    MatrixRepresentation mr;
};


//IMPLEMENTATION


template <class KernelType, class MatrixRepresentation>
template <typename GraphType>
RealType SymTensorProduct<KernelType, MatrixRepresentation>::product(const GraphType& g1, const GraphType& g2) const
{
    NaturalType o1 = boost::num_vertices(g1);
    NaturalType o2 = boost::num_vertices(g2);
    RealType weight=0.;

    //matrix representations of the input graphs
    boost::numeric::ublas::matrix<RealType> m1(o1, o1), m2(o2, o2);

    mr.getMatrix(g1, m1);
    mr.getMatrix(g2, m2);

    for(NaturalType rowG1 = 0; rowG1<o1; rowG1++)
    {
        for(NaturalType colG1=0; colG1<rowG1; colG1++)
        {
            if(m1(rowG1, colG1))
            {
                //data of g2
                for(NaturalType rowG2=0; rowG2<o2; rowG2++)
                {
                    for(NaturalType colG2=0; colG2<o2; colG2++)
                    {
                        if(m2(rowG2, colG2))
                        {
                            weight+=kernel.Sim(
                                    boost::get(v_info_t(), g1, boost::vertex(rowG1, g1)),
                                    boost::get(v_info_t(), g2, boost::vertex(rowG2, g2)),
                                    boost::get(v_info_t(), g1, boost::vertex(colG1, g1)),
                                    boost::get(v_info_t(), g2, boost::vertex(colG2, g2)),
                                    boost::get(e_info_t(), g1, boost::edge(rowG1, colG1, g1).first),
                                    boost::get(e_info_t(), g2, boost::edge(rowG2, colG2, g2).first)
                                    );
                        }
                    }
                }
            }//if
        }
    }

    return weight;
}


template <class KernelType, class MatrixRepresentation>
template <typename GraphType>
void SymTensorProduct<KernelType, MatrixRepresentation>::productP(const GraphType* g1,
        const GraphType* g2, RealType* weight) const
{
    *weight=product(*g1, *g2);
}


template <class KernelType, class MatrixRepresentation>
template <typename GraphType>
RealType SymTensorProduct<KernelType, MatrixRepresentation>::product(const GraphType& g1, const GraphType& g2,
        boost::numeric::ublas::matrix<RealType>& mOut) const
{
    NaturalType o1 = boost::num_vertices(g1);
    NaturalType o2 = boost::num_vertices(g2);
    NaturalType rowPos=0, colPos=0;
    RealType weight=0.;

    //matrix representations of the input graphs
    boost::numeric::ublas::matrix<RealType> m1(o1, o1), m2(o2, o2);

    mr.getMatrix(g1, m1);
    mr.getMatrix(g2, m2);

    for(NaturalType rowG1 = 0; rowG1<o1; rowG1++)
    {
        for(NaturalType colG1=0; colG1<rowG1; colG1++)
        {
            if(m1(rowG1, colG1))
            {
                //data of g2
                for(NaturalType rowG2=0; rowG2<o2; rowG2++)
                {
                    for(NaturalType colG2=0; colG2<o2; colG2++)
                    {
                        RealType f=0.;

                        //positions in the weighted adjacency matrix of the product graph
                        rowPos=rowG2+(rowG1*o2);
                        colPos=colG2+(colG1*o2);

                        if(m2(rowG2, colG2))
                        {
                            f=kernel.Sim(
                                    boost::get(v_info_t(), g1, boost::vertex(rowG1, g1)),
                                    boost::get(v_info_t(), g2, boost::vertex(rowG2, g2)),
                                    boost::get(v_info_t(), g1, boost::vertex(colG1, g1)),
                                    boost::get(v_info_t(), g2, boost::vertex(colG2, g2)),
                                    boost::get(e_info_t(), g1, boost::edge(rowG1, colG1, g1).first),
                                    boost::get(e_info_t(), g2, boost::edge(rowG2, colG2, g2).first)
                                    );

                            weight+=f;
                        }

                        mOut(rowPos, colPos)=f;
                        mOut(colPos, rowPos)=f;
                    }
                }
            }//if
        }
    }

    return weight;
}


}

#endif /* SYMTENSORPRODUCT_HPP */
