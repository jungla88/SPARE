//  Transition class, part of the SPARE library.
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

/** @brief File Transition.hpp, that contains the transition matrix representation class.
 *
 * Contains the declaration of the Transition class.
 *
 * @file Transition.hpp
 * @author Lorenzo Livi
 */

#ifndef TRANSITION_HPP
#define TRANSITION_HPP

//BOOST INCLUDES
#include <boost/graph/graph_utility.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {  // Inclusione in namespace spare.


/** @brief Transition matrix-based representation for a graph.
 *
 * This class implements a @a MatrixRepresentation concept of the graphs domain.
 * The matrix representation encodes standard transition probabilities derived analyzing only the topology of the graph.
 */
class Transition {
public:

	/**
	 * Build the transition matrix from the graph
	 * @param[in] g The graph
	 * @param[out] m The allocated boost-compliant transition matrix
	 */
	template <typename BoostGraphType, typename BoostMatrixType>
	void getMatrix(const BoostGraphType& g, BoostMatrixType& m) const;

};



template <typename BoostGraphType, typename BoostMatrixType>
void Transition::getMatrix(const BoostGraphType& g, BoostMatrixType& m) const
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
			sumW+=1.0;
			oei++;
		}

		oei=ePair.first;
		while(ePair.second!=oei)
		{
			BoostVertexDesc t=boost::target(*oei, g);
			m(source, t)=(sumW==0)?0:1.0/sumW;
			oei++;
		}

		vi++;
	}
}

}

#endif /* TRANSITION_HPP */
