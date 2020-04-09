//  ProteinVerticesDissimilarity class, part of the SPARE library.
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

/** @brief File ProteinVerticesDissimilarity.hpp, that contains ProteinVerticesDissimilarity class.
 *
 * Contains the declaration of the ProteinVerticesDissimilarity class.
 *
 * @file ProteinVerticesDissimilarity.hpp
 * @author Lorenzo Livi
 */

#ifndef _ProteinVerticesDissimilarity_h_
#define _ProteinVerticesDissimilarity_h_

// STD INCLUDES
#include <vector>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Dissimilarity/Levenshtein.hpp>



/** @brief Dissimilarity for vertices labels of an IAM Protein graph type
 */
class ProteinVerticesDissimilarity
{
public:

    /**
     * Dissimilarity for vertices labels of an IAM Protein graph type
     */
    inline spare::RealType Diss(const VertexLabelType& aV1, const VertexLabelType& aV2) const
    {
        spare::RealType D= 0;
        spare::Levenshtein lev;
        lev.Normalization()="On";

        if(aV1.type!=aV2.type)
            D=1.0;
        else
            D=lev.Diss(aV1.sequence, aV2.sequence);

        return D;
    }

};


#endif  // _AIDSVerticesDissimilarity_h_
