//  WTransition class, part of the SPARE library.
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

/** @brief File WTransition.hpp, that contains the transition matrix representation class.
 *
 * Contains the declaration of the WTransition class.
 *
 * @file WTransition.hpp
 * @author Lorenzo Livi
 */

#ifndef WTRANSITION_HPP
#define WTRANSITION_HPP

//BOOST INCLUDES
#include <boost/graph/graph_utility.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {  // Inclusione in namespace spare.


/** @brief Transition matrix-based representation for a labeled graph with scalar-type weights on the edges.
 *
 * This class implements a @a MatrixRepresentation concept of the graphs domain.
 * The labeled graph is assumed to be labeled with scalar-type weights (e.g., real values) on the edges, so they can used to derive the transition probabilities.
 * A standard transition matrix is calculated and returned.
 */
class WTransition {
public:

	/**
	 * Build the transition matrix from the graph using the weights
	 * @param[in] g The graph (assumed to be weighted with scalar values on edges)
	 * @param[out] m The allocated boost-compliant transition matrix
	 */
	template <typename BoostGraphType, typename BoostMatrixType>
	void getMatrix(const BoostGraphType& g, BoostMatrixType& m) const;

};



template <typename BoostGraphType, typename BoostMatrixType>
void WTransition::getMatrix(const BoostGraphType& g, BoostMatrixType& m) const
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
		RealType sumW=0;
		//edge weight
		RealType w=0;
		//compute the sum
		while(ePair.second!=oei)
		{
			sumW+=boost::get(e_info_t(), g, *oei);;
			oei++;
		}

		oei=ePair.first;
		while(ePair.second!=oei)
		{
			BoostVertexDesc t=boost::target(*oei, g);
			w=boost::get(e_info_t(), g, *oei);
			m(source, t)=(sumW==0)?0:w/sumW;
			oei++;
		}

		vi++;
	}
}

}

#endif /* WTRANSITION_HPP */
