//  FSymRenyi class, part of the SPARE library.
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

/** @brief File FSymRenyi.hpp, that contains FSymRenyi class.
 *
 * Contains the declaration of the FSymRenyi class.
 *
 * @file FSymRenyi.hpp
 * @author Lorenzo Livi
 */

#ifndef FSYMRENYI_HPP_
#define FSYMRENYI_HPP_

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Dissimilarity/Divergence/FRenyi.hpp>


namespace spare {

/** @brief FSymRenyi implements a symmetric Renyi fuzzy divergence of order alpha.
 *
 * This class implements the @a Dissimilarity concept between two fuzzy sets.
 */
class FSymRenyi
{
public:

    /**
     * Symmetric dissimilarity computation
     * @param[in] c1 A reference to the first container of membership values
     * @param[in] c2 A reference to the second container of membership values
     * @return The symmetric dissimilarity value
     */
    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& c1, const SequenceContainer& c2) const;

    /**
     * Read-write access to the fuzzy Renyi entropy measure
     * @return A reference to the object
     */
    FRenyi& getRenyi() { return mRenyiDiv; }

    /**
     * Read-only access to the fuzzy Renyi entropy measure
     * @return A const reference to the object
     */
    const FRenyi& getRenyi() const { return mRenyiDiv; }

private:

    FRenyi mRenyiDiv;
};


template <typename SequenceContainer>
RealType FSymRenyi::Diss(const SequenceContainer& c1, const SequenceContainer& c2) const
{
    return (mRenyiDiv.Diss(c1, c2)+mRenyiDiv.Diss(c2, c1))/2.0;
}

}

#endif /* SYMRENYI_HPP_ */
