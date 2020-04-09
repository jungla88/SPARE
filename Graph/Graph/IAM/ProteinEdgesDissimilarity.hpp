//  ProteinEdgesDissimilarity class, part of the SPARE library.
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

/** @brief File ProteinEdgesDissimilarity.hpp, that contains ProteinEdgesDissimilarity class.
 *
 * Contains the declaration of the ProteinEdgesDissimilarity class.
 *
 * @file ProteinEdgesDissimilarity.hpp
 * @author Lorenzo Livi
 */

#ifndef _ProteinEdgesDissimilarity_h_
#define _ProteinEdgesDissimilarity_h_

// STD INCLUDES
#include <vector>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Dissimilarity/ModuleDistance.hpp>



/** @brief Dissimilarity for edges labels of an IAM Protein graph type
 */
class ProteinEdgesDissimilarity
{
public:

    /**
     * Dissimilarity for edges labels of an IAM Protein graph type
     */
    inline spare::RealType Diss(const EdgeLabelType& aV1, const EdgeLabelType& aV2) const
    {
        spare::RealType D= 0;
        spare::ModuleDistance dist;

        if((aV1.type0!=aV2.type0)||(aV1.type1!=aV2.type1))
            D=1.0;
        else
            D=(dist.Diss(aV1.distance0, aV2.distance0)/50.0)+(dist.Diss(aV1.distance1, aV2.distance1)/50.0);

        return D;
    }

};


#endif  // _AIDSVerticesDissimilarity_h_
