//  Complement class, part of the SPARE library.
//  Copyright (C) 2011 Guido Del Vescovo
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

/** @brief File Complement.hpp, that contains Complement class.
 *
 * Contains the declaration of the Complement class.
 *
 * @file Complement.hpp
 * @author Lorenzo Livi
 */

#ifndef Complement_HPP_
#define Complement_HPP_

#include <spare/SpareTypes.hpp>

namespace spare {

/** @brief Converter dissimilarity function between patterns defined as feature values.
 *
 * This class implements the @a Dissimilarity concept.
 * It computes the complement value of the outcome of the basic (bounded) similarity function.
 * The complement is computed using a user-defined maximum value (default value is 1).
 */
template <class SimilarityType>
class Complement
{
public:

    /**
     * Default constructor
     */
    Complement()
    {
        mMaxValue=1.0;
    }


    /**
     * Read-write access to the max value term
     */
    RealType& MaxValue() { return mMaxValue; }

    /**
     * Read-only access to the max value term
     */
    const RealType& MaxValue() const { return mMaxValue; }

    /**
     * Read-write access to the basic similarity function
     */
    SimilarityType& Similarity() { return mSimilarity; }

    /**
     * Read-only access to the basic similarity function
     */
    const SimilarityType& Similarity() const { return mSimilarity; }

    /**
     * Dissimilarity method
     *
     * @param[in] s1 The first sample
     * @param[in] s2 The second sample
     * @return The dissimilarity value
     */
    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& s1, const SequenceContainer& s2) const;

private:

    /**
     * The maximum value
     */
    RealType mMaxValue;

    /**
     * (Bounded) Similarity function
     */
    SimilarityType mSimilarity;
};


template <class SimilarityType>
template <typename SequenceContainer>
RealType Complement<SimilarityType>::Diss(const SequenceContainer& s1, const SequenceContainer& s2) const
{
    return mMaxValue - mSimilarity.Sim(s1, s2);
}

}

#endif
