//  MaxIntersection class, part of the SPARE library.
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

/** @brief File MaxIntersection.hpp, that contains MaxIntersection class.
 *
 * Contains the declaration of the MaxIntersection class.
 *
 * @file MaxIntersection.hpp
 * @author Lorenzo Livi
 */

#ifndef MAXINTERSECTION_HPP_
#define MAXINTERSECTION_HPP_

#include <spare/SpareTypes.hpp>

namespace spare {

/** @brief MaxIntersection dissimilarity function between T1FSs
 *
 * This class implements the @a Dissimilarity concept.
 * This class contains functions for the computation of the dissimilarity value between T1FSs, taken as the head of the intersection (using the min)
 */
class MaxIntersection
{
public:

    /**
     * Main dissimilarity method
     *
     * @param[in] fs1 The first fuzzy set represented as a sequence of membership values
     * @param[in] fs2 The second fuzzy set represented as a sequence of membership values
     * @return The dissimilarity value
     */
    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& fs1, const SequenceContainer& fs2) const
    {
        RealType max=0.0, min;
        typedef SequenceContainer ContainerType;
        typedef typename SequenceContainer::const_iterator ItType;

        ItType it1=fs1.begin(), it2=fs2.begin();

        while(it1!=fs1.end())
        {
            min=std::min(*it1, *it2);
            if(min>max)
                max=min;

            it1++;
            it2++;
        }

        return 1.0-max;
    }
};

}

#endif /* MAXINTERSECTION_HPP_ */
