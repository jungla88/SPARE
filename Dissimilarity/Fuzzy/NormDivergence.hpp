//  NormDivergence class, part of the SPARE library.
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

/** @brief File NormDivergence.hpp, that contains NormDivergence class.
 *
 * Contains the declaration of the NormDivergence class.
 *
 * @file NormDivergence.hpp
 * @author Lorenzo Livi
 */

#ifndef NORMDIVERGENCE_HPP_
#define NORMDIVERGENCE_HPP_


// SPARE INCLUDES
#include <spare/SpareTypes.hpp>

namespace spare {

/** @brief NormDivergence dissimilarity function between T1FSs
 *
 * This class implements the @a Dissimilarity concept.
 * This class is a regularizer for the fuzzy divergence. It normalizes the output value using a real-valued number.
 */
template <class FuzzyDivergenceType>
class NormDivergence
{

public:

    /**
     * Default constructor
     */
    NormDivergence()
    {
        mNormValue=1.0;
    }

    /**
     * Read-write access to the normalizing value
     */
    RealType& NormValue() { return mNormValue; }

    /**
     * Read-write access to the (symmetric) fuzzy divergence function
     */
    FuzzyDivergenceType& FuzzyDivergence() { return mFuzzyDivergence; }

    /**
     * Main divergence method
     *
     * @param[in] The first fuzzy set
     * @param[in] The second fuzzy set
     * @return The normalized divergence
     */
    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& c1, const SequenceContainer& c2) const;


private:

    /**
     * The basic fuzzy divergence
     */
    FuzzyDivergenceType mFuzzyDivergence;

    /**
     * The normalizing factor
     */
    RealType mNormValue;
};


template <class FuzzyDivergenceType>
template <typename SequenceContainer>
RealType NormDivergence<FuzzyDivergenceType>::Diss(const SequenceContainer& c1, const SequenceContainer& c2) const
{
    RealType div=mFuzzyDivergence.Diss(c1, c2);

    if(div<0)
        div*=-1.0;

    return div/mNormValue;
}

}

#endif /* NORMDIVERGENCE_HPP_ */
