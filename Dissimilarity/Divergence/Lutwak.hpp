//  Lutwak class, part of the SPARE library.
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

/** @brief File FSymRenyi.hpp, that contains Lutwak class.
 *
 * Contains the declaration of the Lutwak class.
 *
 * @file Lutwak.hpp
 * @author Lorenzo Livi
 */

#ifndef LUTWAK_HPP_
#define LUTWAK_HPP_

#include <cmath>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief Lutwak divergence of order alpha.
 *
 * This class implements the @a Dissimilarity concept between two probability distributions.
 */
class Lutwak
{
public:

    Lutwak()
    {
        mAlpha=0.5;
    }

    /**
     * Main divergence method
     */
    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& c1, const SequenceContainer& c2) const;

    /**
     * Read/Write access to the alpha param
     */
    RealType& Alpha() { return mAlpha; }

private:

    /**
     * Alpha parameter
     */
    RealType mAlpha;
};


template <typename SequenceContainer>
RealType Lutwak::Diss(const SequenceContainer& c1, const SequenceContainer& c2) const
{
    RealType r1, r2, x1, x2, x3;
    typename SequenceContainer::const_iterator it, it2, itEnd;
    it=c1.begin();
    itEnd=c1.end();
    it2=c2.begin();

    while(it!=itEnd)
    {
        r1=*it;
        r2=*it2;

        x1+=(r1==0)?0:(std::pow(r2, mAlpha-1.0)*r1);
        x2+=(r2==0)?0:std::pow(r2, mAlpha);
        x3+=(r1==0)?0:std::pow(r1, mAlpha);

        it++;
        it2++;
    }

    x1=std::pow(x1, (spare::RealType)1.0/(1.0-mAlpha));
    x2=std::pow(x2, (spare::RealType)1.0/mAlpha);
    x3=std::pow(x3, (spare::RealType)1.0/(mAlpha*(1.0-mAlpha)));

    return std::log((spare::RealType)x1*x2/x3);
}

}

#endif /* LUTWAK_HPP_ */
