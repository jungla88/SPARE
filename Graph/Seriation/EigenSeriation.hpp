//  EigenSeriation class, part of the SPARE library.
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

/** @brief File EigenSeriation.hpp, that contains the graph seriation algorithm based on the first eigenvector of its matrix representation.
 *
 * Contains the declaration of the EigenSeriation class.
 *
 * @file EigenSeriation.hpp
 * @author Lorenzo Livi
 */

#ifndef EIGENSERIATION_HPP
#define EIGENSERIATION_HPP


// STD INCLUDES
#include <stdlib.h>
#include <string>
#include <vector>
#include <set>

//BOOST INCLUDES
#include <boost/graph/graph_utility.hpp>
#include <boost/numeric/ublas/matrix.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {


/** @brief Seriation method based on a walk-like algorithm on the graph using the first eigenvector information of the chosen representation matrix of the input graph.
 *
 * This class implements a @a Seriation concept of the graphs domain in SPARE.
 * The algorithm is taken from "Graph Edit Distance from Spectral Seriation, Antonio Robles-Kelly and Edwin R. Hancock" paper.
 * It generates an output container which contains the ordered list of vertex labels, according to the information retrieved from the eigendecomposition of the graph matrix.
 */
template <class MatrixRepresentation>
class EigenSeriation {
public:

	/**
	 * Seriates the graph using the first eigenvector of the chosen representation matrix.
	 * @param[in] g A spare-compatible graph
	 * @param[out] sequence The sequence or vertices labels of type VertexLabelType
	 */
	template <typename BoostGraphType, typename SequenceContainer >
	void Seriate(const BoostGraphType& g, SequenceContainer& sequence) const;

	/**
     * Read/Write access to the Matrix Representation
     */
	MatrixRepresentation& MatrixRepresentationAgent() { return mr; }

    /**
     * Read only access to the Matrix Representation
     */
    const MatrixRepresentation& MatrixRepresentationAgent() const { return mr; }

private:

	/**
     * Obtains the next vertex to visit during the walk.
     * @param[in] g A spare-compatible graph
     * @param[in] firstEigenvector The leading eigenvector of the chosen representation matrix
     * @param[in] allVSet The set of all vertices identifiers
     * @param[in] vertex The label (id) of the current vertex
     * @param[in/out] visitedVertices The set of current visited vertices identifiers
     * @param[in/out] found A flag
     * @return The next vertex identifier to visit
     */
    template <typename BoostGraphType >
    unsigned int getNBmax(
            const BoostGraphType& g,
            const std::vector<RealType>& firstEigenvector, const std::set<unsigned int>& allVSet,
            const std::set<unsigned int>& visitedVertices, unsigned int vertex, bool& found) const;

	/**
	 * Matrix representation for the graph
	 */
	MatrixRepresentation mr;
};


//IMPLEMENTATION

template <class MatrixRepresentation>
template <typename BoostGraphType, typename SequenceContainer>
void EigenSeriation<MatrixRepresentation>::Seriate(const BoostGraphType& g,
        SequenceContainer& sequence) const
{
    //typedef
    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;

    bool found, isFirst=true;
	unsigned int order, maxVertex=0, currMaxVertex=0, i, j, count=0;
	//first eigenvector
    std::vector<RealType> ev;
    std::set<unsigned int> visitedVertices;
    std::set<unsigned int> allVSet;

    //retrive the order of the graph
	order=boost::num_vertices(g);

	//matrix representations
	boost::numeric::ublas::matrix<RealType> boostMatrix(order, order);

	//take only the first eigenvector
	mr.getMatrix(g, boostMatrix);

	//extract the first eigenvector as a std::vector
	for(i=0;i<order;i++)
	{
		RealType f=boostMatrix(i, 0);
		if(f<0)
			f*=-1;

		ev.push_back(f);
	}

	//vertices iterators
    BoostVertexIter vi=boost::vertices(g).first;
    BoostVertexIter viEnd=boost::vertices(g).second;
    //get the set of all vertices
    while(vi!=viEnd)
    {
        allVSet.insert((unsigned int)*vi);

        vi++;
    }


	do
	{
		if(count==order)
			break;

		if(isFirst)
		{
		    found=false;
		    isFirst=false;
		}
		else
		    found=true;

		//find the next vertex in the nb
		currMaxVertex=getNBmax(g, ev, allVSet, visitedVertices, maxVertex, found);

		//found??
		if(found)
		{
		    maxVertex=currMaxVertex;
		    //add at the end of the container
			sequence.insert(sequence.end(), boost::get(v_info_t(), g, boost::vertex(maxVertex, g)));
			//update visisted vertices identifiers set
			visitedVertices.insert(maxVertex);

			count++;
		}

	}while(true);

}


template <class MatrixRepresentation>
template <typename BoostGraphType >
unsigned int EigenSeriation<MatrixRepresentation>::getNBmax(const BoostGraphType& g,
        const std::vector<RealType>& firstEigenvector, const std::set<unsigned int>& allVSet,
        const std::set<unsigned int>& visitedVertices, unsigned int vertex, bool& found) const
{
    //typedefs
    typedef typename boost::graph_traits<BoostGraphType>::vertex_iterator BoostVertexIter;
    typedef typename boost::graph_traits<BoostGraphType>::vertex_descriptor BoostVertexDesc;
    typedef typename boost::graph_traits<BoostGraphType>::out_edge_iterator BoostOutEdgeIter;
    typedef std::pair<BoostOutEdgeIter, BoostOutEdgeIter> BoostEdgePair;

    //vars
	unsigned int max=0;
	//reduced and effective NB sets
	std::set<unsigned int> nb;
	std::set<unsigned int> fullNB;
	//iterators
	std::set<unsigned int>::iterator it;
	std::set<unsigned int>::iterator end;
	std::set<unsigned int>::iterator test;
	std::set<unsigned int>::iterator itNB;
	std::set<unsigned int>::iterator itNBEnd;

	if(!found)
	{
	    //get the nb of vertex
        BoostVertexIter vi=boost::vertices(g).first;
        BoostVertexIter viEnd=boost::vertices(g).second;
        unsigned int i=0;

        while(vi!=viEnd)
        {
            BoostVertexDesc source=boost::vertex(i, g);
            BoostEdgePair ePair=boost::out_edges(source, g);
            BoostOutEdgeIter oei=ePair.first;

            //fetch the output edges
            while(ePair.second!=oei)
            {
                BoostVertexDesc t=boost::target(*oei, g);
                fullNB.insert((unsigned int)t);
                oei++;
            }

            vi++;
            i++;
        }
	}
	else
		fullNB=allVSet;

	//iterators
	it=fullNB.begin();
	end=fullNB.end();

	while(it!=end)
	{
		if((test=visitedVertices.find(*it))==visitedVertices.end())
			nb.insert(*it);

		it++;
	}

	itNB=nb.begin();
	itNBEnd=nb.end();

	//get the max of the NB
	RealType currMax=0;
	while(itNB!=itNBEnd)
	{
		NaturalType pos=*itNB;

		if(firstEigenvector[pos]>=currMax)
		{
		    found=true;
			max=*itNB;
			currMax=firstEigenvector[pos];
		}

		itNB++;
	}

	return max;
}

}

#endif /* EIGENSERIATION_HPP */
