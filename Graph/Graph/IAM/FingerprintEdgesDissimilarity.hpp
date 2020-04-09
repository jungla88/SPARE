//  FingerprintEdgesDissimilarity class, part of the SPARE library.
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

/** @brief File FingerprintEdgesDissimilarity.hpp, that contains FingerprintEdgesDissimilarity class.
 *
 * Contains the declaration of the FingerprintEdgesDissimilarity class.
 *
 * @file FingerprintEdgesDissimilarity.hpp
 * @author Lorenzo Busta de Piscio Livi
 */

#ifndef _FingerprintEdgesDissimilarity_h_
#define _FingerprintEdgesDissimilarity_h_

// STD INCLUDES
#include <vector>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Dissimilarity/ModuleDistance.hpp>



/** @brief Dissimilarity for edges labels of an IAM Fingerprint graph type
 */
class FingerprintEdgesDissimilarity
{
public:

    /**
     *
     */
    spare::RealType& Alpha() { return mAlpha; }

    /**
     *
     */
    const spare::RealType& Alpha()const { return mAlpha; }


    /**
     * Default constructor
     */
    FingerprintEdgesDissimilarity()
    {
        mAlpha=0.5;
    }

    /**
     * Dissimilarity for edges labels of an IAM Fingerprint graph type
     */
    inline spare::RealType Diss(const EdgeLabelType& aV1, const EdgeLabelType& aV2) const
    {
        spare::RealType D = 0,d1,d2;
        spare::ModuleDistance dist;

        d1=mAlpha*dist.Diss(aV1.angle1, aV2.angle1)+(1.0-mAlpha)*dist.Diss(aV1.orient1, aV2.orient1);
        d2=mAlpha*dist.Diss(aV1.angle2, aV2.angle2)+(1.0-mAlpha)*dist.Diss(aV1.orient2, aV2.orient2);
        D=1/2*(d1+d2);

        return D;
    }

private:

    spare::RealType mAlpha;

};

#endif
