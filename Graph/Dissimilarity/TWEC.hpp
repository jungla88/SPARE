//  TWEC class, part of the SPARE library.
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

/** @brief File TWEC.hpp, that contains the Triple Weighted (vertex) best match first (BMF) class.
 *
 * Contains the declaration and the implementation of the TWEC class.
 *
 * @file TWEC.hpp
 * @author Lorenzo Livi
 */

#ifndef TWEC_HPP
#define TWEC_HPP

// BOOST INCLUDES
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>

//STD INCLUDES
#include <cmath>


namespace spare {  // Inclusione in namespace spare.

/** @brief TWEC dissimilarity function for labeled graphs.
 *
 * This class implements the @a Dissimilarity concept for labeled graphs.
 * This class contains functions for the computation of the dissimilarity of two arbitrary-labeled graphs,
 * following the Graph Edit Distance (GED) algorithmic scheme. The method is called Del Vescovo-Rizzi Best Match First (BMF)
 * because tries to associate first, following a greedy strategy, vertices with higher similarity (derived in turn from the related labels similarity).
 * This measure is naturally normalized into the interval [0, 1], and adopts three weighting parameters to manage the importance of vertices and edges substitutions and constant edit operations (i.e., insertions and deletions).
 * The edit operations on the edges are, as usual, induced from the ones performed on the vertices.
 * The computational complexity is given by O(2*n^2), where n is the lower order of the input graphs.
 */
template <class VerticesDiss, class EdgesDiss>
class TWEC {
public:

    /**
     * Default constructor.
     */
    TWEC()
    {
        //vertices
        //default weighting factors
        mPAlpha=0.5;
        mPBeta=0.5;
        mPGamma=0.5;
        //default vertex operation cost (insertion or deletion)
        mVertexOPCost=1;
        mEdgeOPCost=1;
    }

    /**
     * Hash Set Type
     */
    typedef boost::unordered_set<NaturalType> HashSetType;

    /**
     * Map Type
     */
    typedef boost::unordered_map<NaturalType, NaturalType> HashMapType;

    /**
     * Vertices dissimilarity type
     */
    typedef VerticesDiss VerticesDissType;

    /**
     * Edges dissimilarity type
     */
    typedef EdgesDiss EdgesDissType;


    /**
     * Main graph dissimilarity method
     * @param[in] g1 The first graph
     * @param[in] g2 The second graph
     * @return The dissimilarity value
     */
    template<class BoostGraphType >
    RealType Diss(const BoostGraphType& g1, const BoostGraphType& g2) const;


//ACCESS

    /**
     * Real/Write access to the alpha weighting parameter.
     */
    RealType& PAlpha() { return mPAlpha; }

    /**
     * Real-only access to the alpha weighting parameter.
     */
    const RealType& PAlpha() const { return mPAlpha; }

    /**
     * Real/Write access to the beta weighting parameter.
     */
    RealType& PBeta() { return mPBeta; }

    /**
     * Real-only access to the beta weighting parameter.
     */
    const RealType& PBeta() const { return mPBeta; }

    /**
     * Real/Write access to the gamma weighting parameter.
     */
    RealType& PGamma() { return mPGamma; }

    /**
     * Real-only access to the gamma weighting parameter.
     */
    const RealType& PGamma() const { return mPGamma; }

    /**
     * Real/Write access to the vertices dissimilarity agent.
     */
    VerticesDiss& VerticesDissAgent() { return mVerticesDissimilarity; }

    /**
     * Real-only access to the vertices dissimilarity agent.
     */
    const VerticesDiss& VerticesDissAgent() const { return mVerticesDissimilarity; }

    /**
     * Real/Write access to the edges dissimilarity agent.
     */
    EdgesDiss& EdgesDissAgent() { return mEdgesDissimilarity; }

    /**
     * Real-only access to the edges dissimilarity agent.
     */
    const EdgesDiss& EdgesDissAgent() const { return mEdgesDissimilarity; }


private:

    /**
     * Constant operation cost for vertices (this value is fixed)
     */
    RealType mVertexOPCost;

    /**
     * Constant operation cost for edges (this value is fixed)
     */
    RealType mEdgeOPCost;

    /**
     * Alpha weighting parameter
     */
    RealType mPAlpha;

    /**
     * Beta weighting parameter
     */
    RealType mPBeta;

    /**
     * Gamma weighting parameter
     */
    RealType mPGamma;

    /**
     * vertices dissimilarity function
     */
    VerticesDiss mVerticesDissimilarity;

    /**
     * edges dissimilarity function
     */
    EdgesDiss mEdgesDissimilarity;

};


//IMPLEMENTATION


template <class VerticesDiss, class EdgesDiss>
template <class BoostGraphType >
RealType TWEC<VerticesDiss, EdgesDiss>::Diss(const BoostGraphType& g1, const BoostGraphType& g2) const
{
    //some boost types
    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;
    typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor BoostVertexDesc;
    typedef typename boost::graph_traits<BoostGraphType>::out_edge_iterator BoostOutEdgeIter;
    typedef typename boost::graph_traits<BoostGraphType>::edge_descriptor BoostEdgeDesc;

    //vars
    RealType totESubsCost= 0., totEOPCost=0., totVSubsCost= 0., totVOPCost=0., CND=0., CN=0., CED=0., CE=0.,
            minDiss=0., absOrderDiff;
    NaturalType countG1=0, indexG1=0,
                orderG1=boost::num_vertices(g1), orderG2=boost::num_vertices(g2),
                sizeG1=boost::num_edges(g1), sizeG2=boost::num_edges(g2), assignedIndexG2,
                edgesOPCount=0, ESubsCount=0, ETotOPCount=0, edgesIndex1, edgesIndex2;


    //vertices and iterators/descriptors for graphs
    BoostVertexIter itG1=boost::vertices(g1).first;
    BoostVertexIter itG1End=boost::vertices(g1).second;
    BoostVertexIter itG2, itG2End;
    BoostVertexDesc v1G1, v2G1, v1G2, v2G2;
    BoostEdgeDesc edgeDescG1, edgeDescG2;

    //vertices assignments
    HashMapType assignments;
    HashMapType::iterator assIt, assIt2;
    //hash set for vertices indices
    HashSetType hs;

    //absolute difference between the two orders
    absOrderDiff=std::abs(static_cast<RealType>(orderG1)-static_cast<RealType>(orderG2));

    //best-match assignment of vertices
    while(itG1!=itG1End)
    {
        //have we assigned all the vertices of G2??
        if(countG1>=orderG2)
            break;

        indexG1=(NaturalType)*itG1;
        minDiss=std::numeric_limits<RealType>::max();
        //vertices iterators for the second graph
        itG2=boost::vertices(g2).first;
        itG2End=boost::vertices(g2).second;
        assignedIndexG2=0;

        while(itG2!=itG2End)
        {
            //if NOT already assigned
            if(hs.find((NaturalType)*itG2)==hs.end())
            {
                RealType tmpDiss=mVerticesDissimilarity.Diss(
                                         boost::get(v_info_t(), g1, boost::vertex(*itG1, g1)),
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
        itG1++;
    }

    //additional constant operations
    totVOPCost=mVertexOPCost*absOrderDiff;

    //normalized vertex substitutions cost
    CN=totVSubsCost/(RealType)std::min(orderG1, orderG2);
    //normalized vertex operation costs (the input graphs are assumed to contain vertices)
    CND=( (orderG1+orderG2)-(2*std::min(orderG1, orderG2)) )/(orderG1+orderG2);


    /////////////////////////////
    //induced edit operations on edges
    /////////////////////////////

    assIt=assignments.begin();
    edgesIndex1=0;
    while(assIt!=assignments.end())
    {
        v1G1=boost::vertex((*assIt).first, g1);
        v1G2=boost::vertex((*assIt).second, g2);
        assIt2=assignments.begin();
        edgesIndex2=0;

        while(assIt2!=assignments.end())
        {
            bool edgeFoundG2=false, edgeFoundG1=false;

            //check condition
            if(assIt!=assIt2&&edgesIndex2<=edgesIndex1)
            {
                v2G1=boost::vertex((*assIt2).first, g1);
                v2G2=boost::vertex((*assIt2).second, g2);

                //check the edge on G1
                if(boost::edge(v1G1, v2G1, g1).second)
                {
                    edgeDescG1=boost::edge(v1G1, v2G1, g1).first;
                    edgeFoundG1=true;
                }

                //check the edge on G2
                if(boost::edge(v1G2, v2G2, g2).second)
                {
                    edgeDescG2=boost::edge(v1G2, v2G2, g2).first;
                    edgeFoundG2=true;
                }

                //edges operations

                //substitution
                if(edgeFoundG1&&edgeFoundG2)
                {
                    totESubsCost+=mEdgesDissimilarity.Diss(
                            boost::get(e_info_t(), g1, edgeDescG1),
                                boost::get(e_info_t(), g2, edgeDescG2) );
                    ESubsCount++;
                }
                //edge operation (insertion/deletion)
                else if(edgeFoundG1||edgeFoundG2)
                {
                    edgesOPCount++;
                    ETotOPCount++;
                }
            }//IF

            assIt2++;
            edgesIndex2++;
        }

        assIt++;
        edgesIndex1++;
    }

    //edges operation
    totEOPCost=mEdgeOPCost*edgesOPCount;

    //normalized edge operations and substitution costs
    //CE: edge substitution cost
    CE=(ESubsCount==0)?0:totESubsCost/(RealType)ESubsCount;
    RealType sizeTot=sizeG1+sizeG2;
    //CED: edge insertion/deletion cost
    CED=(sizeTot==0)?0:(RealType)ETotOPCount/sizeTot;

    //return the final dissimilarity value
    return (1.0-mPAlpha)*( ((1.0-mPBeta)*CN) + ( mPBeta*(( (1.0-mPGamma)*CE) + (mPGamma*CED))) )+mPAlpha*CND;
}

}

#endif /* TWEC_HPP */
