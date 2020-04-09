//  Eigenvectors class, part of the SPARE library.
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

/** @brief File Eigenvectors.hpp, that contains the eigenvectors-based matrix representation class.
 *
 * Contains the declaration of the Eigenvectors class.
 * It depends on the Eigen 3 library (tested with version 3.0.4).
 *
 * @file Eigenvectors.hpp
 * @author Lorenzo Livi
 */

#ifndef EIGENVECTORS_HPP
#define EIGENVECTORS_HPP

// STD INCLUDES
#include <stdlib.h>
#include <string>
#include <vector>
#include <set>

// EIGEN INCLUDES
#include <Eigen/Dense>

//BOOST INCLUDES
#include <boost/graph/graph_utility.hpp>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {  // Inclusione in namespace spare.


/** @brief Eigenvectors matrix-based representation for a graph.
 *
 * This class implements a @a MatrixRepresentation concept of the graphs domain.
 * It is a composite model of this concept. Indeed, it uses another @a MatrixRepresentation model, from which it extracts the base of eigenvectors.
 */
template <class MatrixRepresentation>
class Eigenvectors {
public:

    /**
     * Default constructor.
     */
    Eigenvectors()
    {
        mK=1;
    }

	/**
	 * Build the eigenvectors-based representation matrix of the graph, using a specific matrix representation.
	 * @param[in] g The graph
	 * @param[out] m The allocated boost-compliant matrix
	 */
	template <typename BoostGraphType, typename BoostMatrixType>
	void getMatrix(const BoostGraphType& g, BoostMatrixType& m) const;

	// ACCESS

    /**
     * Read/Write access to the matrix representation agent
     */
	MatrixRepresentation& MatrixRepresentationAgent() { return mr; }

    /**
     * Read-only access to the matrix representation agent
     */
    const MatrixRepresentation& MatrixRepresentationAgent() const { return mr; }

    /**
     * Read/Write access to the K parameter
     */
    NaturalType& K() { return mK; }

    /**
     * Read-only access to the K parameter
     */
    const NaturalType& K() const { return mK; }

private:

	/**
	 * Basic matrix representation
	 */
	MatrixRepresentation mr;

	/**
	 * Used to retrieve the first k eigenvectors
	 */
	NaturalType mK;
};

//IMPL.

template <class MatrixRepresentation>
template <typename BoostGraphType, typename BoostMatrixType>
void Eigenvectors<MatrixRepresentation>::getMatrix(const BoostGraphType& g, BoostMatrixType& m) const
{
    //order of the graph
    NaturalType order=boost::num_vertices(g), i=0, u=0;
    int j=0, limit=order-mK;

    //matrix representations
    Eigen::MatrixXf eigenMatrix(order, order);
    BoostMatrixType boostMatrix(order, order);

    //obtain the (boost) matrix representation of the graph
    mr.getMatrix(g, boostMatrix);

    //coversion from boost to Eigen type (it assumes a symmetric matrix)
    for(i=0;i<order;i++)
        for(j=0;j<order;j++)
            eigenMatrix(i, j)=boostMatrix(i, j);

    //get the spectrum
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> eigensolver(eigenMatrix);

    //resize the eigenvectors matrix
    m.resize(order, mK);

    //extract the first k eigenvectors
    for(j=order-1,u=0;j>=limit;j--,u++)
        for(i=0;i<order;i++)
            m(i, u)= (RealType)eigensolver.eigenvectors().col(j)[i];
}

}

#endif /* EIGENVECTORS_HPP */
