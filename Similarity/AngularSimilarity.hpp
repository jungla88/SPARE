//  AngularSimilarity class, part of the SPARE library.
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

/** @brief File AngularSimilarity.hpp, that contains AngularSimilarity class.
 *
 * Contains the declaration of the AngularSimilarity class.
 *
 * @file AngularSimilarity.hpp
 * @author Lorenzo Livi
 */

#ifndef _AngularSimilarity_h_
#define _AngularSimilarity_h_

// STD INCLUDES
#include <cmath>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>
#include <spare/Similarity/Cosine.hpp>

// BOOST
#include <boost/math/constants/constants.hpp>

namespace spare {

/** @brief Angular similarity function.
 *
 * This class implements the @a Similarity concept.
 * This class contains static functions for the computation of the angular similarity between real-valued vectors, taken by measuring the normalized angle between them.
 * It is implemented by making use of another similarity function: the cosine similarity.
 */
class AngularSimilarity
{
public:

    /**
     * Angular similarity computation using pairs of iterators.
     *
     * @param[in] aA A pair of iterators of the first vector.
     * @param[in] aB A pair of iterators of the second vector.
     * @return The similarity value.
     */
    template <typename ForwardIterator1, typename ForwardIterator2>
    RealType Sim(std::pair<ForwardIterator1, ForwardIterator1> aA,
            std::pair<ForwardIterator2, ForwardIterator2> aB) const;

    /**
     * Angular similarity computation.
     *
     * @param[in] rA A reference to the container of the first vector.
     * @param[in] rB A reference to the container of the second vector.
     * @return The similarity value.
     */
    template <typename SequenceContainer1, typename SequenceContainer2>
    RealType Sim(const SequenceContainer1& rA, const SequenceContainer2& rB) const;

private:

    /**
     * The cosine similarity function.
     */
    Cosine mCosineSimilarity;
};

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
RealType AngularSimilarity::Sim(std::pair<ForwardIterator1, ForwardIterator1> aA, std::pair<ForwardIterator2, ForwardIterator2> aB) const
{
    // Controllo.
    #if SPARE_DEBUG
    // Typedef locali.
    typedef typename std::iterator_traits<ForwardIterator1>::difference_type
                                             DiffTypeA;

    typedef typename std::iterator_traits<ForwardIterator2>::difference_type
                                             DiffTypeB;

    DiffTypeA DistA= std::distance(aA.first, aA.second);
    DiffTypeB DistB= std::distance(aB.first, aB.second);

    if ( (DistA < 0) || (DistB < 0) )
    {
        throw SpareLogicError("Cosine, 0, Invalid range.");
    }

    if (DistA != DistB)
    {
        throw SpareLogicError("Cosine, 1, Different lengths between inputs.");
    }
    #endif


    RealType cosineSim=mCosineSimilarity.Sim(aA, aB);

    return 1.0 - (std::acos(cosineSim)/boost::math::constants::pi<double>());
}


template <typename SequenceContainer1, typename SequenceContainer2>
RealType AngularSimilarity::Sim(const SequenceContainer1& c1, const SequenceContainer2& c2) const
{
    // Controllo.
    #if SPARE_DEBUG
    if ( c1.size() != c2.size() )
    {
        throw SpareLogicError("Cosine, 3, Different lenghts between inputs.");
    }
    #endif

    return Sim(std::make_pair(c1.begin(), c1.end()), std::make_pair(c2.begin(), c2.end()));
}

}

#endif
