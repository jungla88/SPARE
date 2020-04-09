//  FRenyi class, part of the SPARE library.
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

/** @brief File FRenyi.hpp, that contains FRenyi class.
 *
 * Contains the declaration of the FRenyi class.
 *
 * @file FRenyi.hpp
 * @author Lorenzo Livi
 */

#ifndef FRENYI_HPP_
#define FRENYI_HPP_

#include <cmath>
#include <limits>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief FRenyi divergence of order alpha.
 *
 * This class implements the @a Dissimilarity concept between two fuzzy sets.
 */
class FRenyi
{
public:

    /**
     * Default constructor
     */
    FRenyi()
    {
        mAlpha=2.0;
    }

    /**
     * Main divergence method
     */
    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& c1, const SequenceContainer& c2) const;

    /**
     * Read-Write access to the alpha parameter
     */
    RealType& Alpha() { return mAlpha; }

    /**
     * Read-only access to the alpha parameter
     */
    const RealType& Alpha() const { return mAlpha; }

private:

    /**
     * alpha parameter
     */
    RealType mAlpha;
};


template <typename SequenceContainer>
RealType FRenyi::Diss(const SequenceContainer& c1, const SequenceContainer& c2) const
{
    bool flag=false;
    NaturalType count=0;
    RealType logArg=0, sum=0, r1, r2, x1, x2, x3, x4, d;
    typename SequenceContainer::const_iterator it, it2, itEnd;
    it=c1.begin();
    itEnd=c1.end();
    it2=c2.begin();

    //fetch the membership values defining the two fuzzy sets
    while(it!=itEnd)
    {
        r1=*it;
        r2=*it2;

        x1=(r1==0)?0:std::pow(r1, mAlpha);
        x2=(r2==0)?0:std::pow(r2, 1.0-mAlpha);
        x3=(1.0-r1==0)?0:std::pow(1.0-r1, mAlpha);
        x4=(1.0-r2==0)?0:std::pow(1.0-r2, 1.0-mAlpha);

        logArg=(x1*x2) + (x3*x4);

        if(logArg==0)
            count++;

        if(logArg<=0)
            logArg=1;

        sum+=std::log(logArg);

        it++;
        it2++;
    }

    //if we have always zero in the argumento of the log, then the two fs are maximally divergent
    if(count==c1.size())
        d=std::numeric_limits<RealType>::max();
    else
        d=sum/(mAlpha-1.0);

    return d;
}

}

#endif /* FRENYI_HPP_ */
