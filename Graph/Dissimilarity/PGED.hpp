//  PGED class, part of the SPARE library.
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

/** @brief File PGED.hpp that contains the Perfect graph edit distance (PGED) class.
 *
 * Contains the declaration and the implementation of the Perfect GED class.
 *
 * @file PGED.hpp
 * @author Filippo Maria Bianchi
 */

#ifndef PGED_HPP_
#define PGED_HPP_

#include <list>
#include <vector>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief Perfect Graph Edit Distance (PGED) dissimilarity measure for labeled graphs.
 *
 * This class implements the @a Dissimilarity concept for labeled graphs.
 * This class contains functions for the computation of the dissimilarity of two arbitrary-labeled graphs,
 * following the Graph Edit Distance (GED) algorithmic scheme. It implements the perfect GED, that is, it computes the exact minimum cost of alignment between two labeled graph.
 * The edit distance is weighted using four weights to adapt the importance of the operations for vertices and edges.
 * In this measure, only substitutions and replacement (i.e., insertions and deletions) costs are considered.
 * The measure can be normalized using a user-defined factor.
 * The computational complexity is given by O(m^n * n^2 + m^(n+1) - 1), where n<m are the orders of the two input graphs.
 */
template <class VerticesDiss, class EdgesDiss>
class PGED
{
public:

	/**
	 * Read-write access to the normalization factor
	 */
	RealType& NormalizationFactor() { return mNormalizationFactor; }

	/**
	 * Read-only access to the normalization factor
	 */
	const RealType& NormalizationFactor() const { return mNormalizationFactor; }

	/**
	 * Read-write access to the vertices substitution weight
	 */
	RealType& VSub(){
		return mVSub;
	}

	/**
	 * Read-only access to the vertices substitution weight
	 */
	const RealType& VSub() const{
		return mVSub;
	}

	/**
	 * Read-write access to the edges substitution weight
	 */
	RealType& ESub(){
		return mESub;
	}

	/**
	 * Read-only access to the vertices substitution weight
	 */
	const RealType& ESub() const{
		return mESub;
	}

	/**
	 * Read-write access to the vertices insertion/deletion weight
	 */
	RealType& VRep(){
		return mVRep;
	}

	/**
	 * Read-only access to the vertices insertion/deletion weight
	 */
	const RealType& VRep() const{
		return mVRep;
	}

	/**
	 * Read-write access to the edges insertion/deletion weight
	 */
	RealType& ERep(){
		return mERep;
	}

	/**
	 * Read-only access to the edges insertion/deletion weight
	 */
	const RealType& ERep() const{
		return mERep;
	}

	/**
	 * Read-write access to the vertices dissimilarity measure agent
	 */
	VerticesDiss& VerticesDissAgent() { return mVerticesDiss; }

	/**
	 * Read-only access to the vertices dissimilarity measure agent
	 */
	const VerticesDiss& VerticesDissAgent() const { return mVerticesDiss; }

	/**
	 * Read-write access to the edges dissimilarity measure agent
	 */
	EdgesDiss& EdgesDissAgent() { return mEdgesDiss; }

	/**
	 * Read-only access to the edges dissimilarity measure agent
	 */
	const EdgesDiss& EdgesDissAgent() const { return mEdgesDiss; }


	/**
	 * Default constructor
	 */
	PGED()
	{
		mNormalizationFactor = 1.0;
		mVSub = 1.0;
		mESub = 1.0;
		mVRep = 1.0;
		mERep = 1.0;
	}

	/**
	 * Main graph dissimilarity method
	 * @param[in] g1 The instance of the first graph
	 * @param[in] g2 The instance of the second graph
	 * @return The dissimilarity value
	 */
    template<class BoostGraphType >
    RealType Diss(const BoostGraphType& g1, const BoostGraphType& g2) const;

private:

    /**
     * Normalization factor
     */
    RealType mNormalizationFactor;

	/**
	 * vertices substitution weight
	 */
	RealType mVSub;

	/**
	 * vertices insertion/deletion weight
	 */
	RealType mVRep;

	/**
	 * edges substitution weight
	 */
	RealType mESub;

	/**
	 * edges insertion/deletion weight
	 */
	RealType mERep;

	/**
	 * Dissimilarity measure between vertices
	 */
	VerticesDiss mVerticesDiss;

	/**
	 * Dissimilarity measure between edges
	 */
	EdgesDiss mEdgesDiss;


	/**
	 * Internal method that implements the recursive researh
	 */
	template<typename BoostGraphType, typename VertexDesc>
	RealType rsearch(const BoostGraphType gmin, const BoostGraphType gMax, std::list<VertexDesc> minVC, std::list<VertexDesc> maxVC,
			   	   	   	    std::vector<std::pair<VertexDesc, VertexDesc> > Vmatchings) const;
};

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>> IMPLEMENTATION <<<<<<<<<<
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template <class VerticesDiss, class EdgesDiss>
template<typename BoostGraphType, typename VertexDesc>
RealType PGED<VerticesDiss, EdgesDiss>::rsearch(const BoostGraphType gmin, const BoostGraphType gMax,
													   std::list<VertexDesc> minVC, std::list<VertexDesc> maxVC,
													   std::vector<std::pair<VertexDesc, VertexDesc> > Vmatchings) const{

	typedef typename boost::graph_traits<BoostGraphType>::edge_descriptor BoostEdgeDesc;

	//ci sono ancora vertici da matchare in minVC
	if(!minVC.empty())
	{
		RealType minDist = std::numeric_limits<RealType>::max();

		VertexDesc minID = *minVC.begin();
		minVC.erase(minVC.begin());

		std::list<VertexDesc> auxList;
		typename std::list<VertexDesc>::iterator auxListIT;
		for(NaturalType i=0; i < maxVC.size(); i++)
		{
			auxList = maxVC;
			auxListIT = auxList.begin();
			advance(auxListIT, i);
			std::vector<std::pair<VertexDesc, VertexDesc> > Auxmatchings = Vmatchings;
			Auxmatchings.push_back( std::make_pair(minID, *auxListIT) );
			auxList.erase(auxListIT);

			RealType dist = rsearch(gmin, gMax, minVC, auxList, Auxmatchings);

			if(dist < minDist)
				minDist = dist;
		}
		return minDist;
	}
	else
	{
		RealType dist=0;

		//********* VERTEX COST *************
		//vertex substitution cost
		for(NaturalType i=0; i < Vmatchings.size(); i++){
			VertexDesc IDgmin = Vmatchings[i].first;
			VertexDesc IDgMax = Vmatchings[i].second;

			RealType d = mVerticesDiss.Diss(boost::get(v_info_t(), gmin, IDgmin), boost::get(v_info_t(), gMax, IDgMax))*mVSub;
			dist+= d;
		}
		//vertex replacement cost
		dist+= (boost::num_vertices(gMax) - boost::num_vertices(gmin))*mVRep;;

		//********* EDGE COST *************
		for(NaturalType i=0; i < Vmatchings.size(); i++){
			VertexDesc IDgmin1 = Vmatchings[i].first;
			VertexDesc IDgMax1 = Vmatchings[i].second;

			for(NaturalType j=i+1; j < Vmatchings.size(); j++){
				VertexDesc IDgmin2 = Vmatchings[j].first;
				VertexDesc IDgMax2 = Vmatchings[j].second;
				//edge replacement cost
				if(boost::edge(IDgmin1, IDgmin2, gmin).second == 1 && boost::edge(IDgMax1, IDgMax2, gMax).second == 1){

					BoostEdgeDesc edmin = boost::edge(IDgmin1, IDgmin2, gmin).first;
					BoostEdgeDesc edMax = boost::edge(IDgMax1, IDgMax2, gMax).first;

					dist+= mEdgesDiss.Diss(boost::get(e_info_t(), gmin, edmin), boost::get(e_info_t(), gMax, edMax))*mESub;

				}else if(boost::edge(IDgmin1, IDgmin2, gmin).second == 0 && boost::edge(IDgMax1, IDgMax2, gMax).second == 0){
					//do nothing
				//edge substitution cost
				}else{
					dist+= mERep;
				}
			}//forj
		}//fori

		return dist;


	}//else
}

template <class VerticesDiss, class EdgesDiss>
template <class BoostGraphType >
RealType PGED<VerticesDiss, EdgesDiss>::Diss(const BoostGraphType& g1, const BoostGraphType& g2) const
{
    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;
    typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor BoostVertexDesc;

    NaturalType orderG1=boost::num_vertices(g1), orderG2=boost::num_vertices(g2);
    BoostGraphType gmin, gMax;

    if(orderG1 <= orderG2){
    	gmin = g1;
    	gMax = g2;
    }else{
    	gmin = g2;
    	gMax = g1;
    }

    std::list<BoostVertexDesc> VCmin, VCmax;

    BoostVertexIter vminIT = boost::vertices(gmin).first;
    BoostVertexIter vminITend = boost::vertices(gmin).second;
    while(vminIT != vminITend){
    	VCmin.push_back(boost::vertex(*vminIT++, gmin));
    }
    BoostVertexIter vmaxIT = boost::vertices(gMax).first;
    BoostVertexIter vmaxITend = boost::vertices(gMax).second;
    while(vmaxIT != vmaxITend){
    	VCmax.push_back(boost::vertex(*vmaxIT++, gMax));
    }

    std::vector<std::pair<BoostVertexDesc, BoostVertexDesc> > vmatchs;

    RealType res = rsearch(gmin, gMax, VCmin, VCmax, vmatchs);

    return res/mNormalizationFactor;
}

}

#endif /* PGED_HPP_ */
