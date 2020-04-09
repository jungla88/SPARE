//  Cosine class, part of the SPARE library.
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

/** @brief File Cosine.hpp, that contains Cosine class.
 *
 * Contains the declaration of the Cosine class.
 *
 * @file Cosine.hpp
 * @author Lorenzo Livi
 */

#ifndef _Cosine_h_
#define _Cosine_h_

// STD INCLUDES
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

// BOOST INCLUDES

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Cosine similarity.
 *
 * This class implements the @a Similarity concept.
 * This class contains static functions for the computation of the cosine similarity between real vectors, by measuring the cosine of the angle between them.
 */
class Cosine
{
public:

// OPERATIONS

    /**
     * Cosine similarity computation using input iterators.
     *
     * @param[in] aA A pair of iterators of the first vector.
     * @param[in] aB A pair of iterators of the second vector.
     * @return The similarity value.
     */
    template <typename ForwardIterator1, typename ForwardIterator2>
    RealType Sim(std::pair<ForwardIterator1, ForwardIterator1> aA,
            std::pair<ForwardIterator2, ForwardIterator2> aB) const;

    /**
     * Cosine similarity computation.
     *
     * @param[in] rA A reference to the container of the first vector.
     * @param[in] rB A reference to the container of the second vector.
     * @return The similarity value.
     */
    template <typename SequenceContainer1, typename SequenceContainer2>
    RealType Sim(const SequenceContainer1& rA, const SequenceContainer2& rB) const;

}; // class Cosine

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
RealType Cosine::Sim(std::pair<ForwardIterator1, ForwardIterator1> aA, std::pair<ForwardIterator2, ForwardIterator2> aB) const
{
    // Variabili.
    RealType InnerProd=0, SumA=0, SumB=0, tmpA, tmpB;

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


    while(aA.first != aA.second)
    {
        tmpA=static_cast<RealType>(*aA.first++);
        tmpB=static_cast<RealType>(*aB.first++);

        InnerProd+=tmpA*tmpB;
        SumA+=std::pow(tmpA, 2.0);
        SumB+=std::pow(tmpB, 2.0);
    }

    return InnerProd/(std::sqrt(SumA)*std::sqrt(SumB));
}


template <typename SequenceContainer1, typename SequenceContainer2>
RealType
Cosine::Sim(const SequenceContainer1& c1, const SequenceContainer2& c2) const
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
