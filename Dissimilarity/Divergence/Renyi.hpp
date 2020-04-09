//  Renyi class, part of the SPARE library.
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

/** @brief File Renyi.hpp, that contains Renyi class.
 *
 * Contains the declaration of the Renyi class.
 *
 * @file Renyi.hpp
 * @author Lorenzo Livi
 */

#ifndef RENYI_HPP_
#define RENYI_HPP_

#include <cmath>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {


/** @brief Renyi divergence of order alpha.
 *
 * This class implements the @a Dissimilarity concept between two (discrete) probability distributions
 * The alpha-order Renyi divergence is implemented in this class.
 */
class Renyi
{
public:

    /**
     * Default constructor
     */
    Renyi()
    {
        mAlpha=0.5;
    }

    /**
     * Main dissimilarity method
     *
     * @param[in] c1 The first probability distribution (expressed in terms of a vector of probability values)
     * @param[in] c2 The second probability distribution (expressed in terms of a vector of probability values)
     * @return The dissimilarity value
     */
    template <typename SequenceContainer>
    const spare::RealType Diss(const SequenceContainer& c1, const SequenceContainer& c2) const;

    /**
     * Read-Write access to the alpha parameter
     */
    spare::RealType& Alpha() { return mAlpha; }

    /**
     * Read-only access to the alpha parameter
     */
    const spare::RealType& Alpha() const { return mAlpha; }

private:

    /**
     * alpha parameter
     */
    spare::RealType mAlpha;
};


template <typename SequenceContainer>
const spare::RealType Renyi::Diss(const SequenceContainer& c1, const SequenceContainer& c2) const
{
    spare::RealType logArg=0, sum=0, r1, r2, x1;
    typename SequenceContainer::const_iterator it, it2, itEnd;
    it=c1.begin();
    itEnd=c1.end();
    it2=c2.begin();

    while(it!=itEnd)
    {
        r1=*it;
        r2=*it2;

        x1=(r2==0)?0:std::pow(r1/r2, mAlpha-1.0);
        sum+=r1*x1;

        it++;
        it2++;
    }

    return std::log(sum)/(mAlpha-1.0);
}

}

#endif /* RENYI_HPP_ */
