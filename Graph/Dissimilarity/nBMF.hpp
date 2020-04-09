//  nBMF class, part of the SPARE library.
//  Copyright (C) 2011
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

/** @brief File nBMF.hpp, that contains the normalized (vertex) weighted best match first (nBMF) class.
 *
 * Contains the declaration and the implementation of the normalized weighted BMF class.
 *
 * @file nBMF.hpp
 * @author Lorenzo Livi
 */

#ifndef NBMF_HPP
#define NBMF_HPP

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/SpareTypes.hpp>
#ifdef SBMF
    #include <spare/Graph/Dissimilarity/sBMF.hpp>
#else
//    #include <spare/Graph/Dissimilarity/BMF.hpp>
    #include <spare/Graph/Dissimilarity/4WBMF.hpp>

#endif


namespace spare {  // Inclusione in namespace spare.

/** @brief normalized BMF dissimilarity function for labeled graphs.
 *
 * This class implements the @a Dissimilarity concepts.
 * This class contains functions for the computation of the dissimilarity of two arbitrary-labeled graphs,
 * following the Graph Edit Distance (GED) scheme. The method is called normalized Best Match First (BMF)
 * because it is basically a standard BMF computation, normalizing the value outcome considering the two specific input graphs instances.
 * The main assumption here is that any substitution cost, for both vertices and edges labels, is upper bounded by the value of 1.
 * Of course, also insertion and deletion costs of both vertices and edges must be at most 1.
 * Consequently, the maximum edit distance cost, used for the normalization, is given by MAX_O+MIN_O*(MIN_O-1)/2, where MAX_O and MIN_O are, respectively, the maximum and minimum order of the two input graphs.
 */
template <class VerticesDiss, class EdgesDiss>
class nBMF {
public:

    /**
     * Main dissimilarity method
     * @param[in] g1 The first graph
     * @param[in] g2 The second graph
     * @return The normalized dissimilarity value in [0, 1]
     */
    template<typename BoostGraphType >
    RealType Diss(const BoostGraphType& g1, const BoostGraphType& g2) const ;


//ACCESS

    /**
     * Read/Write access to the BMF graph dissimilarity algorithm
     */
    BMF<VerticesDiss, EdgesDiss>& BMFAgent() { return mBMFDiss; }

    /**
     * Read-only access to the BMF graph dissimilarity algorithm
     */
    const BMF<VerticesDiss, EdgesDiss>& BMFAgent() const { return mBMFDiss; }

private:

    /**
     * Standard BMF algorithm
     */
    BMF<VerticesDiss, EdgesDiss> mBMFDiss;
};


//IMPLEMENTATION


template <class VerticesDiss, class EdgesDiss>
template <typename BoostGraphType >
RealType nBMF<VerticesDiss, EdgesDiss>::Diss(const BoostGraphType& g1, const BoostGraphType& g2) const
{
    //upper bound
    NaturalType o1=boost::num_vertices(g1), o2=boost::num_vertices(g2);
    
    mBMFDiss.Diss(g1, g2);

    RealType result= 0.0;
    RealType  d1 = mBMFDiss.getVertexDissValue();
    RealType d2 = mBMFDiss.getEdgeDissValue();
    NaturalType min=std::min(o1, o2);
    RealType u=(min*(min-1)/2);

    if(u!=0){
    result = ((d1/std::max(o1,o2)) + d2/(RealType)u)/2;
    }
    else result = d1/std::max(o1,o2);
    return result;


    // PREVIOUS DISSIMILARITY
	/*
    NaturalType min=std::min(o1, o2), u=std::max(o1, o2)+(min*(min-1)/2);
    return mBMFDiss.Diss(g1, g2)/(RealType)u;
	*/
}

}

#endif /* BMF_HPP */
