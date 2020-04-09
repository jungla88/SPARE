//  SymTransition class, part of the SPARE library.
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

/** @brief File SymTransition.hpp, that contains the symmetric version of the transition matrix representation class.
 * Only adjacencies are considered, even if the graph is labeled with scalar values on the edges.
 *
 * Contains the declaration of the SymTransition class.
 *
 * @file SymTransition.hpp
 * @author Lorenzo Livi
 */

#ifndef SYMTRANSITION_HPP
#define SYMTRANSITION_HPP

//BOOST INCLUDES
#include <boost/graph/graph_utility.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {


/** @brief Symmetric transition matrix-based representation for graphs.
 *
 * This class implements a @a MatrixRepresentation concept of the graphs domain.
 * Only adjacencies are considered, even if the graph is labeled with scalar values on the edges.
 * The symmetric transition matrix is called also the normalized transition matrix.
 */
class SymTransition {
public:

	/**
	 * Build the symmetric transition matrix from the graph
	 * @param[in] g The graph
	 * @param[out] m The allocated boost-compliant symmetric transition matrix
	 */
	template <typename BoostGraphType, typename BoostMatrixType>
	void getMatrix(const BoostGraphType& g, BoostMatrixType& m) const;

};

// IMPL

template <typename BoostGraphType, typename BoostMatrixType>
void SymTransition::getMatrix(const BoostGraphType& g, BoostMatrixType& m) const
{
	typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;
	typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor BoostVertexDesc;
	typedef typename boost::graph_traits<BoostGraphType>::out_edge_iterator BoostOutEdgeIter;
	typedef std::pair<BoostOutEdgeIter, BoostOutEdgeIter> BoostEdgePair;


	//init
	unsigned int order=boost::num_vertices(g);
	//degree and tmp transition matrix
	BoostMatrixType degM(order, order);
	BoostMatrixType adjM(order, order);

	for(unsigned int i=0;i<order;i++)
		for(unsigned int j=0;j<order;j++)
		{
			m(i, j)=0;
			adjM(i, j)=0;
			degM(i, j)=0;
		}

	//vertices iterators
	BoostVertexIter vi=boost::vertices(g).first;
	BoostVertexIter viEnd=boost::vertices(g).second;

	while(vi!=viEnd)
	{
		BoostVertexDesc source=boost::vertex((NaturalType)*vi, g);
		BoostEdgePair ePair=boost::out_edges(source, g);
		unsigned int deg=boost::degree(source, g);

		//set the degree
		degM(source, source)=(deg==0)?RealType(0):std::pow(1.0/deg, 0.5);

		BoostOutEdgeIter oei=ePair.first;

		//fetch the output edges
		RealType sumW=0;
		//edge weight
		RealType w=0;
		//compute the sum
		while(ePair.second!=oei)
		{
			BoostVertexDesc t=boost::target(*oei, g);
			adjM(source, t)=1;
			oei++;
		}

		vi++;
	}

	//normalization
	BoostMatrixType t=boost::numeric::ublas::prod(degM, adjM);

	m=boost::numeric::ublas::prod(t, degM);
}

}

#endif /* SYMTRANSITION_HPP */
