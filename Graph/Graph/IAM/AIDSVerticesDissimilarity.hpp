//  AIDSVerticesDissimilarity class, part of the SPARE library.
//  Copyright (C) 2011 Lorenzo Livi
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

/** @brief File AIDSVerticesDissimilarity.hpp, that contains AIDSDissimilarity class.
 *
 * Contains the declaration of the AIDSVerticesDissimilarity class.
 *
 * @file AIDSVerticesDissimilarity.hpp
 * @author Lorenzo Livi
 */

#ifndef _AIDSVerticesDissimilarity_h_
#define _AIDSVerticesDissimilarity_h_

// STD INCLUDES
#include <vector>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Dissimilarity/Euclidean.hpp>



/** @brief Dissimilarity for vertices labels of an IAM AIDS graph type
 */
class AIDSVerticesDissimilarity
{
public:

    /**
     * Dissimilarity for vertices labels of an IAM AIDS graph type
     */
    inline spare::RealType Diss(const VertexLabelType& aV1, const VertexLabelType& aV2) const
    {
        spare::RealType D= 0;
        std::vector<spare::RealType> v1, v2;
        spare::Euclidean e;

        if(aV1.charge!=aV2.charge)
            D+=1;

        if(aV1.chem!=aV2.chem)
            D+=1;

        v1.push_back(aV1.x);
        v1.push_back(aV1.y);
        v2.push_back(aV2.x);
        v2.push_back(aV2.y);


        D+=e.Diss(v1, v2);

        return D;
    }

};


#endif  // _AIDSVerticesDissimilarity_h_
