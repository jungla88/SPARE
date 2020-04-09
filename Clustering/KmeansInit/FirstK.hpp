//  FirstK class, part of the SPARE library.
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

/** @brief File FirstK.hpp
 *
 * The file contains the kmeans initialization that selects the first K input samples as representatives
 *
 * @file FirstK.hpp
 * @author Lorenzo Livi
 */

#ifndef FIRSTK_HPP_
#define FIRSTK_HPP_

#include <spare/SpareTypes.hpp>

namespace spare {

/** @brief FirstK initialization algorithm for kmeans clustering algorithm.
 *
 *   This basic initialization strategy selects the first K input samples as initial representatives.
 */
class FirstK
{
public:

    /**
     * Main representatives initialization method.
     * @param[in] K The K parameter of the kmeans algorithm
     * @param[in] itS Iterator pointing at the beginning of the samples container
     * @param[in] itE Iterator pointing at the end of the samples container
     * @param[out] representativeVector Reference to the vector of the kmeans representatives. Note that this container must be assumed to be already resized to contain the K representatives.
     */
    template <typename SamplesITType, typename RepVectorType>
    void Initialize(const NaturalType& K, const SamplesITType& itS, const SamplesITType& itE, RepVectorType& representativeVector) const
    {
        SamplesITType it=itS;
        NaturalType i=0;

        while(i<K && it!=itE)
        {
            representativeVector[i].Update(*it);

            i++;
            it++;
        }
    }
};

}

#endif /* FIRSTK_HPP_ */
