//  sBMF class, part of the SPARE library.
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

/** @brief File sBMF.hpp, that contains the shuffled (vertex) weighted best match first (sBMF) class.
 *
 * Contains the declaration and the implementation of the shuffled weighted BMF class.
 *
 * @file sBMF.hpp
 * @author Lorenzo Livi
 */

#ifndef SBMF_HPP
#define SBMF_HPP

// BOOST INCLUDES
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>

//STD INCLUDES
#include <cmath>
#include <vector>
#include <algorithm>
#include <functional>
#include <ctime>


namespace spare {  // Inclusione in namespace spare.

//generates random number. used in random_shuffle
ptrdiff_t sBMFRandom(ptrdiff_t i)
{
	return rand()%i;
}

/** @brief sBMF dissimilarity function for labeled graphs.
 *
 * This class implements the @a Dissimilarity concept for labeled graphs.
 * This class contains functions for the computation of the dissimilarity of two arbitrary-labeled graphs,
 * following the Graph Edit Distance (GED) algorithmic scheme. The method is called Best Match First (sBMF)
 * because tries to associate first, following a greedy strategy, vertices with higher similarity (derived in turn from the related labels similarity).
 * The edit operations on the edges are induced from the ones performed on the vertices.
 * In addition to the well-known BMF scheme, the algorithm performs a user-defined number of re-ordering of the vertices of the first graphs, perming for each of those configurations the wBMF algorithm.
 * The returned value of the algorithm is the minimum dissimilarity value obtained over these configurations.
 * The matching adopts six weighting parameters: three for the vertex and three for the edge operations.
 * The computational complexity is given by O(k*2*n^2), where n is the lower order of the input graphs, and k is the user-defined number of shuffles (default 5).
 */
template <class VerticesDiss, class EdgesDiss>
class sBMF {
public:

    /**
     * Default constructor.
     */
    sBMF()
    {
        //vertices
        //default weighting factors
        mPVertexSubstitution=1;
        mPVertexInsertion=1;
        mPVertexDeletion=1;
        //default insertion and deletion costs
        mVertexInsertionCost=1;
        mVertexDeletionCost=1;

        //edges
        //default weighting factors
        mPEdgeSubstitution=1;
        mPEdgeInsertion=1;
        mPEdgeDeletion=1;
        //default insertion and deletion costs
        mEdgeInsertionCost=1;
        mEdgeDeletionCost=1;

        //normalization
        mNormalize=false;
        mNormValue=1.0;

        //number of shuffles
        mNShuffles=5;
        //random seed initialization
        srand(time(NULL));
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
     * Hash Map Type
     */
    typedef boost::unordered_map<NaturalType, NaturalType> HashMapType;


    /**
     * Main graph dissimilarity method
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
    RealType& VertexInsertionCost() { return mVertexInsertionCost; }

    /**
     * Read-only access to the vertices insertion cost.
     */
    const RealType& VertexInsertionCost() const { return mVertexInsertionCost; }

    /**
     * Read/Write access to the vertices deletion cost.
     */
    RealType& VertexDeletionCost() { return mVertexDeletionCost; }

    /**
     * Read-only access to the vertices deletion cost.
     */
    const RealType& VertexDeletionCost() const { return mVertexDeletionCost; }

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
    RealType& PVertexInsertion() { return mPVertexInsertion; }

    /**
     * Read-only access to the parameter for vertices insertion weight.
     */
    const RealType& PVertexInsertion() const { return mPVertexInsertion; }

    /**
     * Read/Write access to the parameter for vertices deletion weight.
     */
    RealType& PVertexDeletion() { return mPVertexDeletion; }

    /**
     * Read-only access to the parameter for vertices deletion weight.
     */
    const RealType& PVertexDeletion() const { return mPVertexDeletion; }


    //edges

    /**
     * Read/Write access to the edges insertion cost.
     */
    RealType& EdgeInsertionCost() { return mEdgeInsertionCost; }

    /**
     * Read-only access to the edges insertion cost.
     */
    const RealType& EdgeInsertionCost() const { return mEdgeInsertionCost; }

    /**
     * Read/Write access to the edges deletion cost.
     */
    RealType& EdgeDeletionCost() { return mEdgeDeletionCost; }

    /**
     * Read-only access to the edges deletion cost.
     */
    const RealType& EdgeDeletionCost() const { return mEdgeDeletionCost; }

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
    RealType& PEdgeInsertion() { return mPEdgeInsertion; }

    /**
     * Read-only access to the parameter for vertices insertion weight.
     */
    const RealType& PEdgeInsertion() const { return mPEdgeInsertion; }

    /**
     * Read/Write access to the parameter for vertices deletion weight.
     */
    RealType& PEdgeDeletion() { return mPEdgeDeletion; }

    /**
     * Read-only access to the parameter for vertices deletion weight.
     */
    const RealType& PEdgeDeletion() const { return mPEdgeDeletion; }

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
     * Read-write access to the number of shuffles
     */
    NaturalType& NShuffles() { return mNShuffles; }

    /**
     * Read-only access to the number of shuffles
     */
    const NaturalType& NShuffles() const { return mNShuffles; }

private:

    /**
     * Number of shuffles
     */
    NaturalType mNShuffles;

    /**
     * Constant insertion cost (vertices)
     */
    RealType mVertexInsertionCost;

    /**
     * Constant deletion cost (vertices)
     */
    RealType mVertexDeletionCost;

    /**
     * Substitution parameter (vertices)
     */
    RealType mPVertexSubstitution;

    /**
     * Insertion parameter (vertices)
     */
    RealType mPVertexInsertion;

    /**
     * Deleteion parameter (vertices)
     */
    RealType mPVertexDeletion;

    /**
     * Constant insertion cost (edges)
     */
    RealType mEdgeInsertionCost;

    /**
     * Constant deletion cost (edges)
     */
    RealType mEdgeDeletionCost;

    /**
     * Substitution parameter (edges)
     */
    RealType mPEdgeSubstitution;

    /**
     * Insertion parameter (edges)
     */
    RealType mPEdgeInsertion;

    /**
     * Deleteion parameter (edges)
     */
    RealType mPEdgeDeletion;

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
     * User-define value used for normalization
     */
    RealType mNormValue;
};


//IMPLEMENTATION


template <class VerticesDiss, class EdgesDiss>
template <class BoostGraphType >
RealType sBMF<VerticesDiss, EdgesDiss>::Diss(const BoostGraphType& g1, const BoostGraphType& g2) const
{
    //some boost types
    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;
    typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor BoostVertexDesc;
    typedef typename boost::graph_traits<BoostGraphType>::out_edge_iterator BoostOutEdgeIter;
    typedef typename boost::graph_traits<BoostGraphType>::edge_descriptor BoostEdgeDesc;

    NaturalType orderG1=boost::num_vertices(g1), orderG2=boost::num_vertices(g2), sizeG1=boost::num_edges(g1), sizeG2=boost::num_edges(g2);
    RealType absOrderDiff, dissMinTot=std::numeric_limits<RealType>::max();

    //vertices and iterators/descriptors for graphs
    BoostVertexIter itG1=boost::vertices(g1).first;
    BoostVertexIter itG1End=boost::vertices(g1).second;

    //function for random numbers
    ptrdiff_t (*p_sBMFRandom)(ptrdiff_t) = sBMFRandom;

    //get the vertex ids
    std::vector<NaturalType> vIDs;
    while(itG1!=itG1End)
    {
    	vIDs.push_back((NaturalType)*itG1);
    	itG1++;
    }

    //absolute difference between the two orders
    absOrderDiff=std::abs(static_cast<RealType>(orderG1)-static_cast<RealType>(orderG2));

    //number of shuffles
    for(NaturalType s=0;s<mNShuffles;s++)
    {
        //vars
        RealType totESubsCost= 0., totEInsertionCost=0., totEDeletionCost=0., totEDiss=0.,
                totVSubsCost= 0., totVInsertionCost=0., totVDeletionCost=0., totVDiss=0.,
                minDiss=0., totDiss=0.;
        NaturalType countG1=0, indexG1=0, assignedIndexG2,
                    edgesInsertionCount=0, edgesDeletionCount=0, edgesIndex1, edgesIndex2;


        BoostVertexIter itG2, itG2End;
        BoostVertexDesc v1G1, v2G1, v1G2, v2G2;
        BoostEdgeDesc edgeDescG1, edgeDescG2;

        //vertices assignments
        HashMapType assignments;
        HashMapType::iterator assIt, assIt2;
        //hash set for vertices indices
        HashSetType hs;

    	//apply the shuffle to the vertices
    	random_shuffle(vIDs.begin(), vIDs.end(), p_sBMFRandom);
    	std::vector<NaturalType>::iterator itVIDs1=vIDs.begin(), itVIDs1End=vIDs.end();

		//best-match assignment of vertices
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

			while(itG2!=itG2End)
			{
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
						//create the mapping
						if((assIt=assignments.find(indexG1))==assignments.end())
							assignments[indexG1]=assignedIndexG2;
						//or update the assigned vertex
						else
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

		//insertions
		if(orderG1>orderG2)
			totVInsertionCost=mVertexInsertionCost*absOrderDiff;
		//deletions
		else
			totVDeletionCost=mVertexDeletionCost*absOrderDiff;

		//combine and return the total vertices costs
		totVDiss=(mPVertexSubstitution*totVSubsCost)+(mPVertexInsertion*totVInsertionCost)+(mPVertexDeletion*totVDeletionCost);


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
					}
					//insertion
					else if(edgeFoundG1)
						edgesInsertionCount++;
					//deletion
					else if(edgeFoundG2)
						edgesDeletionCount++;
				}//IF

				assIt2++;
				edgesIndex2++;
			}

			assIt++;
			edgesIndex1++;
		}

		//edges insertions
		totEInsertionCost=mEdgeInsertionCost*edgesInsertionCount;
		//edges deletions
		totEDeletionCost=mEdgeDeletionCost*edgesDeletionCount;

		//combine and return the total edges costs
		totEDiss=(mPEdgeSubstitution*totESubsCost)+(mPEdgeInsertion*totEInsertionCost)+(mPEdgeDeletion*totEDeletionCost);
		totDiss=totVDiss+totEDiss;

		if(totDiss<dissMinTot)
			dissMinTot=totDiss;
    }

    //normalized or plain dissimilarity value?
    if(mNormalize)
        return dissMinTot/mNormValue;
    else
        return dissMinTot;
}

}

#endif /* SBMF_HPP */
