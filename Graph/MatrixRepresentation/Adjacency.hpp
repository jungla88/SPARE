//  Adjacency class, part of the SPARE library.
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

/** @brief File Adjacency.hpp, that contains the adjacency matrix representation class.
 *
 * Contains the declaration of the Adjacency class.
 *
 * @file Adjacency.hpp
 * @author Lorenzo Livi
 */

#ifndef ADJACENCY_HPP
#define ADJACENCY_HPP

//BOOST INCLUDES
#include <boost/graph/graph_utility.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief Adjacency matrix-based representation for a graph.
 *
 * This class implements a @a MatrixRepresentation concept of the graph domain.
 * It generates a boost matrix denoting the topology of the input graph.
 */
class Adjacency {
public:

	/**
	 * Build the adjacency matrix of the graph
	 * @param[in] g The BGT graph
	 * @param[out] m The allocated boost-compliant matrix
	 */
	template <typename BoostGraphType, typename BoostMatrixType>
	void getMatrix(const BoostGraphType& g, BoostMatrixType& m) const;
};

//IMPL.

template <typename BoostGraphType, typename BoostMatrixType>
void Adjacency::getMatrix(const BoostGraphType& g, BoostMatrixType& m) const
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
		while(ePair.second!=oei)
		{
			BoostVertexDesc t=boost::target(*oei, g);
			m(source, t)=1;
			oei++;
		}

		vi++;
	}
}

}

#endif /* ADJACENCY_HPP */
