//  GRECVerticesDissimilarity class, part of the SPARE library.
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

/** @brief File GRECVerticesDissimilarity.hpp, that contains GRECVerticesDissimilarity class.
 *
 * Contains the declaration of the GRECVerticesDissimilarity class.
 *
 * @file GRECVerticesDissimilarity.hpp
 * @author Lorenzo Livi
 */

#ifndef _GRECVerticesDissimilarity_h_
#define _GRECVerticesDissimilarity_h_

// STD INCLUDES
#include <vector>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Dissimilarity/Euclidean.hpp>


/** @brief Dissimilarity for vertices labels of an IAM GREC graph type
 */
class GRECVerticesDissimilarity
{
public:

    spare::RealType& Alpha(){
        return alpha;
    }

    const spare::RealType& Alpha() const{
        return alpha;
    }

    GRECVerticesDissimilarity(){
        alpha = 0.7;
    }

    inline spare::RealType Diss(const VertexLabelType& gV1, const VertexLabelType& gV2) const{

        spare::RealType D = 0;
        spare::Euclidean e;
        std::vector<spare::RealType> v1, v2;

        v1.push_back(gV1.x);
        v1.push_back(gV1.y);
        v2.push_back(gV2.x);
        v2.push_back(gV2.y);

        if(gV1.type != gV2.type)
            D+= alpha;

        D+=(1-alpha)*e.Diss(v1, v2);

        return D;
    }

private:

    //weight of difference in type (corber vs intersection) over position
    spare::RealType alpha;
};

#endif
