//  InducedTransition class, part of the SPARE library.
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

/** @brief File InducedTransition.hpp, that contains the induced transition matrix representation class.
 *
 * Contains the declaration of the InducedTransition class.
 *
 * @file InducedTransition.hpp
 * @author Lorenzo Livi
 */

#ifndef INDUCEDTRANSITION_HPP
#define INDUCEDTRANSITION_HPP

//BOOST INCLUDES
#include <boost/graph/graph_utility.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>

//STD INCLUDES
#include <string>
#include <vector>
#include <cmath>


namespace spare {  // Inclusione in namespace spare.

/** @brief Induced transition matrix-based representation for a graph.
 *
 * This class implements a @a MatrixRepresentation concept of the graphs domain.
 * This class assumes that the input graph is labeled with real vectors on the edges.
 * The induced transition matrix is developed analyzing those real vectors.
 * The assumption is that the longer is the norm of this vector, the stronger is the relationship.
 * Therefore, the norm of the vector is considered as a weight and, consequently, the standard transition matrix can be derived accordingly.
 */
class InducedTransition {
private:

	/**
	 * Utility function to compute the norm of the vector
	 */
	inline RealType getNorm(const std::vector<RealType>& v) const;

public:

	/**
	 * Compute the vector-norm induced transition matrix from the graph
	 * @param[in] g The graph (assumed to be weighted with real-valued vectors on edges)
	 * @param[out] m The allocated boost-compliant induced transition matrix
	 */
	template <typename BoostGraphType, typename BoostMatrixType>
	void getMatrix(const BoostGraphType& g, BoostMatrixType& m) const;

};



inline RealType InducedTransition::getNorm(const std::vector<RealType>& v) const
{
	RealType sum=0.;
	for(unsigned int i=0;i<v.size();i++)
		sum+=v[i]*v[i];

	return std::pow(sum, 0.5);
}


template <typename BoostGraphType, typename BoostMatrixType>
void InducedTransition::getMatrix(const BoostGraphType& g, BoostMatrixType& m) const
{
    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;
    typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor BoostVertexDesc;
    typedef typename boost::graph_traits<BoostGraphType>::out_edge_iterator BoostOutEdgeIter;
    typedef std::pair<BoostOutEdgeIter, BoostOutEdgeIter> BoostEdgePair;

	//init
    unsigned int order=boost::num_vertices(g);
	for(unsigned int i=0;i<order;i++)
		for(unsigned int j=0;j<order;j++)
			m(i, j)=0;

	//vertices iterators
	BoostVertexIter vi=boost::vertices(g).first;
	BoostVertexIter viEnd=boost::vertices(g).second;

	while(vi!=viEnd)
	{
		BoostVertexDesc source=boost::vertex((NaturalType)*vi, g);
		BoostEdgePair ePair=boost::out_edges(source, g);
		BoostOutEdgeIter oei=ePair.first;

		//fetch the output edges
		RealType sumW=0.;
		while(ePair.second!=oei)
		{
			sumW+=getNorm(boost::get(e_info_t(), g, *oei));
			oei++;
		}

		oei=ePair.first;
		RealType w=0.;
		while(ePair.second!=oei)
		{
			BoostVertexDesc t=boost::target(*oei, g);
			w=getNorm(boost::get(e_info_t(), g, *oei));
			m(source, t)=w/sumW;
			oei++;
		}

		vi++;
	}

}

}

#endif /* INDUCEDTRANSITION_HPP */
