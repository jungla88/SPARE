//  SymLutwak class, part of the SPARE library.
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

/** @brief File SymLutwak.hpp, that contains SymLutwak class.
 *
 * Contains the declaration of the SymLutwak class.
 *
 * @file SymLutwak.hpp
 * @author Lorenzo Livi
 */

#ifndef SYMLUTWAK_HPP_
#define SYMLUTWAK_HPP_

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Dissimilarity/Divergence/Lutwak.hpp>


namespace spare {

/** @brief Symmetric Lutwak divergence of order alpha.
 *
 * This class implements the @a Dissimilarity concept between two probability distributions.
 */
class SymLutwak
{
public:

    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& c1, const SequenceContainer& c2) const;

    Lutwak& getLutwak() { return mLutwakDiv; }

    const Lutwak& getLutwak() const { return mLutwakDiv; }

private:

    Lutwak mLutwakDiv;
};


template <typename SequenceContainer>
RealType SymLutwak::Diss(const SequenceContainer& c1, const SequenceContainer& c2) const
{
    return mLutwakDiv.Diss(c1, c2)+mLutwakDiv.Diss(c2, c1);
}

}

#endif
