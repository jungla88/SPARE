//  FSymKL class, part of the SPARE library.
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

/** @brief File FSymKL.hpp, that contains FSymRenyi class.
 *
 * Contains the declaration of the FSymKL class.
 *
 * @file FSymKL.hpp
 * @author Lorenzo Livi
 */

#ifndef FSYMKL_HPP_
#define FSYMKL_HPP_

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Dissimilarity/Divergence/FKL.hpp>


namespace spare {

/** @brief FSymKL implements a symmetric KL fuzzy divergence of order beta.
 *
 * This class implements the @a Dissimilarity concept between two fuzzy sets.
 */
class FSymKL
{
public:

    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& c1, const SequenceContainer& c2) const;

    FKL& getKL() { return mKLDiv; }

    const FKL& getKL() const { return mKLDiv; }

private:

    FKL mKLDiv;
};


template <typename SequenceContainer>
RealType FSymKL::Diss(const SequenceContainer& c1, const SequenceContainer& c2) const
{
    return (mKLDiv.Diss(c1, c2)+mKLDiv.Diss(c2, c1))/2.0;
}

}

#endif /* SYMKL_HPP_ */
