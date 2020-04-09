//  BMF class, part of the SPARE library.
//  Copyright (C) 2011 Guido Del Vescovo
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

/** @brief File BMF.hpp, that contains the (vertex) weighted best match first (BMF) graph edit distance class, optionally with a vertices shuffle procedure.
 *
 * @file 4wBMF.hpp
 * @author Luca Baldini
 */

#ifndef W4BMF_HPP
#define W4BMF_HPP

// BOOST INCLUDES
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>


//SPARE INCLUDES
#include <spare/SpareTypes.hpp>

//STD INCLUDES
#include <cmath>
#include <ctime>
#include <random>
#include <algorithm>

//FOR DEBUG params
#include <assert.h>

namespace spare {  // Inclusione in namespace spare.


/** @brief BMF dissimilarity function for labeled graphs with shuffle option.
 *
 * This class implements the @a Dissimilarity concept for labeled graphs.
 * This class contains functions for the computation of the dissimilarity of two arbitrary-labeled graphs,
 * following the Graph Edit Distance (GED) algorithmic scheme. The method is called Best Match First (BMF)
 * because tries to associate first, following a greedy strategy, vertices with higher similarity (derived in turn from the related labels similarity).
 * The edit operations on the edges are induced from the ones performed on the vertices.
 * In the vertices match phase, insertion and deletion are weighted with the same parameter.
 * In the induced edges match phase, the sum of both insertion and deletion is weighted with a common parameter.
 * Member function Diss() assumed the BoostGraphType graph has struct with "v_info_t" and "e_info_t" containing vertex
 * and edge properties.
 *
 * Added read-only access to edge/vertex dissimilarity. Usefull for custom normalization.
 */




template <class VerticesDiss, class EdgesDiss>
class BMF {

public:

    /**
     * Default constructor.
     */
    BMF()
    {
        //vertices
        //default weighting factors
        mPVertexSubstitution=1;
        mPVertexInsDel=1;
        //default insertion and deletion costs
        mVertexInsDelCost=1;

        //edges
        //default weighting factors
        mPEdgeSubstitution=1;
        mPEdgeInsDel=1;
        //default insertion and deletion costs
        mEdgeInsDelCost=1;
        
        //dissimilarity initialization
        mVertexDissValue=0.0;
        mEdgeDissValue=0.0;

        //normalization
        mNormalize=false;
        mNormValue=1.0;

        //Optional shuffle parameters
        isShuffle=false;
        mNShuffles=0;
        mSeed=time(NULL);
        generator.seed(mSeed);

        //Fully 4-weights without sum of weights to 1
        paramsBounded=false;

    }

    /**
     * Vertices dissimilarity type
     */
    typedef VerticesDiss VerticesDissType;

    /**
     * Edges dissimilarity type
     */
    typedef EdgesDiss EdgesDissType;

    /**
     * Hash Set Type
     */
    typedef boost::unordered_set<NaturalType> HashSetType;

    /**
     * Map Type
     */
    typedef boost::unordered_map<NaturalType, NaturalType> HashMapType;


    /**
     * Main graph dissimilarity method w/ optional shuffle.
     * If isShuffle is true, the method
     * @param[in] g1 The first graph
     * @param[in] g2 The second graph
     * @return The dissimilarity value
     */
    template<class BoostGraphType >
    RealType Diss(const BoostGraphType& g1, const BoostGraphType& g2) const;

    //ACCESS


    //vertices

    /**
     * Read/Write access to the vertices insertion cost.
     */
    RealType& VertexInsDelCost() { return mVertexInsDelCost; }

    /**
     * Read-only access to the vertices insertion cost.
     */
    const RealType& VertexInsDelCost() const { return mVertexInsDelCost; }

    /**
     * Read/Write access to the parameter for vertices substitutions weight.
     */
    RealType& PVertexSubstitution() { return mPVertexSubstitution; }

    /**
     * Read-only access to the parameter for vertices substitutions weight.
     */
    const RealType& PVertexSubstitution() const { return mPVertexSubstitution; }

    /**
     * Read/Write access to the parameter for vertices insertion weight.
     */
    RealType& PVertexInsDel() { return mPVertexInsDel; }

    /**
     * Read-only access to the parameter for vertices insertion weight.
     */
    const RealType& PVertexInsDel() const { return mPVertexInsDel; }



    //edges

    /**
     * Read-only access to the edges insertion/deletion cost.
     */
    const RealType& EdgeInsDelCost() const { return mEdgeInsDelCost; }

    /**
     * Read/Write access to the parameter for edges substitutions weight.
     */
    RealType& PEdgeSubstitution() { return mPEdgeSubstitution; }

    /**
     * Read-only access to the parameter for vertices substitutions weight.
     */
    const RealType& PEdgeSubstitution() const { return mPEdgeSubstitution; }

    /**
     * Read/Write access to the parameter for edges insertion weight.
     */
    RealType& PEdgeInsDel() { return mPEdgeInsDel; }

    /**
     * Read-only access to the parameter for vertices insertion weight.
     */
    const RealType& PEdgeInsDel() const { return mPEdgeInsDel; }

    /**
     * Read/Write access to the vertices dissimilarity agent.
     */
    VerticesDiss& VerticesDissAgent() { return mVerticesDissimilarity; }

    /**
     * Read-only access to the vertices dissimilarity agent.
     */
    const VerticesDiss& VerticesDissAgent() const { return mVerticesDissimilarity; }

    /**
     * Read/Write access to the edges dissimilarity agent.
     */
    EdgesDiss& EdgesDissAgent() { return mEdgesDissimilarity; }

    /**
     * Read-only access to the edges dissimilarity agent.
     */
    const EdgesDiss& EdgesDissAgent() const { return mEdgesDissimilarity; }

    /**
     * Read/Write access to the normalization flag
     */
    bool& Normalize() { return mNormalize; }

    /**
     * Read-only access to the normalization flag
     */
    const bool& Normalize() const { return mNormalize; }

    /**
     * Read/Write access to the value used for the normalization (default 1)
     */
    RealType& NormValue() { return mNormValue; }

    /**
     * Read-only access to the value used for the normalization (default 1)
     */
    const RealType& NormValue() const { return mNormValue; }
    
    /**
    * Strictly Read-Only access for dissimilarity result for external methods requests.
    * Added by Luca Baldini
    */
    const RealType& getVertexDissValue() const {return mVertexDissValue;}
    const RealType& getEdgeDissValue()  const {return mEdgeDissValue;}
    
    /**
      * Enabler for getting a parameter from the remanings if weights sum one
      */

    bool& setParamsBounded(){return paramsBounded;}

    /**
      * Read access to setParamsBounded
      */
    const bool& setParamsBounded() const {return paramsBounded;}

    /**
      * Access to shuffle enabler
      */

    bool& setShuffle() { return isShuffle;}

    /**
      * Read shuffle enable
      */
    const bool& setShuffle() const {return isShuffle;}

    /**
     *  Read/Write access to number of shuffles
     */
    NaturalType& setShuffleRep() { return mNShuffles;}

    /**
      * Read-only access to number of shuffles
      */
    const NaturalType& setShuffleRep() const {return mNShuffles;}

    /**
     * Set seed variable for RNG.
     */
    void setSeed(spare::RealType seed)
    {
        mSeed = seed;
        generator.seed(mSeed);
    }


    //******

private:

    /**
     * Constant insertion cost (vertices)
     */
    RealType mVertexInsDelCost;

    /**
     * Substitution parameter (vertices)
     */
    RealType mPVertexSubstitution;

    /**
     * Insertion parameter (vertices)
     */
    RealType mPVertexInsDel;

    /**
     * Constant insertion cost (edges)
     */
    RealType mEdgeInsertionCost;

    /**
     * Constant deletion cost (edges)
     */
    RealType mEdgeDeletionCost;

    /**
     * Constant insertion/deletion cost (edges)
     */
    RealType mEdgeInsDelCost;

    /**
     * Substitution parameter (edges)
     */
    RealType mPEdgeSubstitution;

    /**
     * Insertion/deletion parameter (edges)
     */
    RealType mPEdgeInsDel;

    /**
     * vertices dissimilarity function
     */
    VerticesDiss mVerticesDissimilarity;

    /**
     * edges dissimilarity function
     */
    EdgesDiss mEdgesDissimilarity;

    /**
     * Normalization flag
     */
    bool mNormalize;

    /**
     * Value used for normalization
     */
    RealType mNormValue;

    /**
      * Shuffle Enabler
      */

    bool isShuffle;

    /**
      * Number of shuffles
      */
    NaturalType mNShuffles;

    /**
     * Seed variable for RNG for repeatability. If not set, default constructor use time(null)
     */
    spare::NaturalType mSeed;
    mutable std::mt19937 generator;

    /**
      * Enable variable if weights sum 1
      */
    bool paramsBounded;

    /**
    *  Vertex/Edge Dissimilarity values
    *  In order to keep Diss() const, this variables are declared mutable letting Diss() to assign the evaluated cost
    *  Added by Luca Baldini
    */
    mutable RealType mVertexDissValue;
    mutable RealType mEdgeDissValue;

    /**
     * Plain GED Dissimiliarity function.
     */
    template<class BoostGraphType >
    RealType _Diss(const BoostGraphType& g1, const BoostGraphType& g2) const;



};


//IMPLEMENTATION


template <class VerticesDiss, class EdgesDiss>
template <class BoostGraphType >
RealType BMF<VerticesDiss, EdgesDiss>::Diss(const BoostGraphType& g1, const BoostGraphType& g2) const
{


    RealType dissMinTot=std::numeric_limits<RealType>::max();
    NaturalType shuffleCount=0;

    do{

        //do-while for performing at least one time the Diss()
        RealType Diss = _Diss<BoostGraphType>(g1,g2);

        if(Diss<dissMinTot){
            dissMinTot=Diss;
        }

        //No shuffle for graphs with less than 2 nodes TODO: let it set by user?
        if(boost::num_vertices(g1)<2 && boost::num_vertices(g2)<2 )
            break;

        shuffleCount++;

        //repeat diss, shuffling g1 vertex descriptors in _Diss function
    } while(isShuffle == true && shuffleCount < mNShuffles );

    return dissMinTot;
}

template <class VerticesDiss, class EdgesDiss>
template <class BoostGraphType >
RealType BMF<VerticesDiss, EdgesDiss>::_Diss(const BoostGraphType& g1, const BoostGraphType& g2) const
{
    //some boost types
    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;
    typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor BoostVertexDesc;
    typedef typename boost::graph_traits<BoostGraphType>::out_edge_iterator BoostOutEdgeIter;
    typedef typename boost::graph_traits<BoostGraphType>::edge_descriptor BoostEdgeDesc;

    //vars
    RealType totESubsCost= 0., totEInsDelCost=0.,totEDiss=0.,
            totVSubsCost= 0., totVInsDelCost=0.,totVDiss=0.,
            minDiss=0., absOrderDiff, totDiss=0.;
    NaturalType countG1=0, indexG1=0,
            orderG1=boost::num_vertices(g1), orderG2=boost::num_vertices(g2),
            sizeG1=boost::num_edges(g1), sizeG2=boost::num_edges(g2), assignedIndexG2,
            edgesInsertionCount=0, edgesDeletionCount=0, edgesIndex1, edgesIndex2;


    //vertices and iterators/descriptors for graphs
    BoostVertexIter itG2, itG2End;
    BoostVertexDesc v1G1, v2G1, v1G2, v2G2;
    BoostEdgeDesc edgeDescG1, edgeDescG2;

    //vertices assignments
    HashMapType assignments;			//è una mappa associativa chiave-valore (naturali in questo caso)
    HashMapType::iterator assIt, assIt2;
    //hash set for vertices indices
    HashSetType hs;

    //absolute difference between the two orders
    absOrderDiff=std::abs(static_cast<RealType>(orderG1)-static_cast<RealType>(orderG2));

    //Auxiliar container for apply shuffle on vertices
    std::vector<BoostVertexDesc> v1IDs;
    for(BoostVertexDesc vd : make_iterator_range(vertices(g1))){
                v1IDs.push_back(vd);
    }

    //Just to get always the same vertices order if we do not have to shuffle
    if(isShuffle){
        shuffle(v1IDs.begin(), v1IDs.end(), generator);
    }

    typename std::vector<BoostVertexDesc>::iterator itVIDs1=v1IDs.begin(), itVIDs1End=v1IDs.end();
    
    //best-match assignment of vertices
    //Iterate through g1 vertices
    while(itVIDs1!=itVIDs1End)
    {
        //we have assigned all the vertices of G2??
        if(countG1>=orderG2)
            break;

        indexG1=*itVIDs1;
        minDiss=std::numeric_limits<RealType>::max();
        //vertices iterators for the second graph
        itG2=boost::vertices(g2).first;
        itG2End=boost::vertices(g2).second;
        assignedIndexG2=0;

        while(itG2!=itG2End){
            //if NOT already assigned
            if(hs.find((NaturalType)*itG2)==hs.end())
            {
                RealType tmpDiss=mVerticesDissimilarity.Diss(
                            boost::get(v_info_t(), g1, boost::vertex(*itVIDs1, g1)),
                            boost::get(v_info_t(), g2, boost::vertex(*itG2, g2)) );


                //update the best assignment
                if(tmpDiss<minDiss)
                {
                    assignedIndexG2=(NaturalType)*itG2;
                    //create or update the mapping
                    assignments[indexG1]=assignedIndexG2;

                    minDiss=tmpDiss;
                }
            }

            itG2++;
        }

        //save the last assigned vertex of G2 to the current vertex of G1
        hs.insert(assignedIndexG2);
        //inc tot. substitutions cost
        totVSubsCost+=minDiss;

        //counters
        countG1++;
        itVIDs1++;
    }

    //insertions-deletion cost
    totVInsDelCost=mVertexInsDelCost*absOrderDiff;

    //combine and return the total vertices costs
//    totVDiss=(mPVertexSubstitution*totVSubsCost)+(mPVertexInsertion*totVInsertionCost)+(mPVertexDeletion*totVDeletionCost);
    totVDiss=(mPVertexSubstitution*totVSubsCost)+(mPVertexInsDel*totVInsDelCost);
    mVertexDissValue=totVDiss;

    /////////////////////////////
    //induced edit operations on edges
    /////////////////////////////

    assIt=assignments.begin();
    edgesIndex1=0;
    while(assIt!=assignments.end())
    {
        v1G1=boost::vertex((*assIt).first, g1);	//tiro fuori il primo vertice associato da G1 a G2
        v1G2=boost::vertex((*assIt).second, g2);
        assIt2=assignments.begin();
        edgesIndex2=0;

        while(assIt2!=assignments.end())
        {
            bool edgeFoundG2=false, edgeFoundG1=false;

            //check condition
            if(assIt!=assIt2&&edgesIndex2<=edgesIndex1)
            {
                v2G1=boost::vertex((*assIt2).first, g1);	//tiro fuori il secondo vertice associato
                v2G2=boost::vertex((*assIt2).second, g2);

                //check the edge on G1
                if(boost::edge(v1G1, v2G1, g1).second)	//vedo se su G1 i due vertici sono connessi
                {
                    edgeDescG1=boost::edge(v1G1, v2G1, g1).first;
                    edgeFoundG1=true;
                }

                //check the edge on G2
                if(boost::edge(v1G2, v2G2, g2).second)	//vedo se su G2 i due vertici sono connessi
                {
                    edgeDescG2=boost::edge(v1G2, v2G2, g2).first;
                    edgeFoundG2=true;
                }

                //edges operations

                //substitution
                if(edgeFoundG1&&edgeFoundG2)			//se ho match di archi, devo solo sostituire col relativo costo
                {
                    totESubsCost+=mEdgesDissimilarity.Diss(
                                boost::get(e_info_t(), g1, edgeDescG1),
                                boost::get(e_info_t(), g2, edgeDescG2) );
                }
                //insertion								//se non c'è arco in G2 allora lo devo inserire, con relativo costo
                else if(edgeFoundG1)
                    edgesInsertionCount++;
                //deletion								//se non c'è arco in G1 allora lo devo cancellare, con relativo costo
                else if(edgeFoundG2)
                    edgesDeletionCount++;
            }//IF

            assIt2++;
            edgesIndex2++;
        }

        assIt++;
        edgesIndex1++;
    }

    //edges insertions deletion
    totEInsDelCost=mEdgeInsDelCost*(edgesDeletionCount+edgesInsertionCount);


    //bounded parameter
    spare::RealType PEdgeSubstitution=0.;
    if(paramsBounded){
    spare::NaturalType nP=4; // Parameters available
    spare::RealType minX,b = 1.0/nP; // upperbound for parameters in order to have sum to 1
    spare::RealType maxX= 1.0;
    PEdgeSubstitution=1-(mPEdgeInsDel+mPVertexInsDel+mPVertexSubstitution);
    //Scale in [a,b]=[0,1/4] from [minX,maxX]=[1/4,1]
    PEdgeSubstitution= (spare::RealType)(PEdgeSubstitution - minX)*b/(maxX-minX);
    //Check bounds
    assert(mPEdgeInsDel>=0. && mPEdgeInsDel<= 1. &&
            mPEdgeSubstitution>=0. && mPEdgeSubstitution<=1. &&
            mPVertexInsDel >=0. && mPVertexInsDel <=1. &&
            mPVertexSubstitution >=0. && mPVertexSubstitution <=1. && "4-GED params out of bounds" );
    }
    else{
        PEdgeSubstitution= mPEdgeSubstitution;
    }

    //combine and return the total edges costs
    totEDiss=(PEdgeSubstitution*totESubsCost)+(mPEdgeInsDel*totEInsDelCost);
    mEdgeDissValue=totEDiss;
    
    //Total value
    totDiss=totVDiss+totEDiss;

    //normalized or plain dissimilarity value?
    if(mNormalize)
        return totDiss/mNormValue;
    else
        return totDiss;
    }	//DISS

}

#endif /* W4BMF_HPP */
