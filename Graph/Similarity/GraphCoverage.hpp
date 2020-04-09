//  GraphCoverage class, part of the SPARE library.
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

/** @brief File GraphCoverage.hpp, that contains the graph coverage class.
 *
 * Contains the declaration of the GraphCoverage class.
 *
 * @file GraphCoverage.hpp
 * @author Lorenzo Livi
 */

#ifndef GRAPHCOVERAGE_HPP
#define GRAPHCOVERAGE_HPP

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief GraphCoverage kernel function for labeled graphs.
 *
 * This class implements both @a Similarity and @a Dissimilarity concepts.
 * This class contains functions for the computation of the (dis)similarity of two arbitrary-labeled graphs.
 * It computes three tensor products and, using the properties of the derived weights, extracts the final similarity value.
 */
template <class GraphProduct >
class GraphCoverage {
public:

    /**
     * Main similarity method
     * @param[in] g1 The first graph
     * @param[in] g2 The second graph
     * @return The similarity value
     */
    template<class BoostGraphType >
    RealType Sim(const BoostGraphType& g1, const BoostGraphType& g2) const;

    /**
     * Main dissimilarity method
     * @param[in] g1 The first graph
     * @param[in] g2 The second graph
     * @return The dissimilarity value
     */
    template<class BoostGraphType >
    RealType Diss(const BoostGraphType& g1, const BoostGraphType& g2) const
    {
        return RealType(1.0) - Sim(g1, g2);
    }

    /**
     * Read/Write access to the product graph
     */
    GraphProduct& ProductAgent() { return pg; }

    /**
     * Read only access to the product graph
     */
    const GraphProduct& ProductAgent() const { return pg; }

private:

    /**
     * Type of graph product
     */
    GraphProduct pg;
};


//IMPL


template <class GraphProduct >
template <class BoostGraphType >
RealType GraphCoverage<GraphProduct >::Sim(const BoostGraphType& g1, const BoostGraphType& g2) const
{
    RealType sum=0., sum2=0., sum3=0.;

    //tensor products
    sum=pg.product(g1, g2);
    sum2=pg.product(g1, g1);
    sum3=pg.product(g2, g2);

    //gc
    if(sum2>=sum3)
        return (sum2==0)?0.:sum/sum2;
    else
        return (sum3==0)?0.:sum/sum3;
}

}

#endif /* GRAPHCOVERAGE_HPP */
