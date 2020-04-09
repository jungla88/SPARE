//  Delta class, part of the SPARE library.
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

/** @brief File Delta.hpp, that contains Delta class.
 *
 * Contains the declaration of the Delta class.
 *
 * @file Delta.hpp
 * @author Lorenzo Livi
 */

#ifndef _Delta_h_
#define _Delta_h_

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Kronecker delta distance.
 *
 * This class implements the @a Dissimilarity concept.
 * This class contains inline functions for the computation of the Kronecker delta distance between compatible data types.
 * It returns 1 if the two objects are diverse, and 0 otherwise.
 * For compatibility with the other dissimilarities, the return value is a spare::RealType.
 * Any data type can be processed by this dissimilarity measure. The only requirement is the definition of the != operator.
 */
class Delta
{
public:

// OPERATIONS

    /** Kronecker Delta distance computation.
     *
     * @param[in] rA A reference to the first element.
     * @param[in] rB A reference to the second element.
     * @return The distance value in {0, 1}.
     */
    template <typename Type1, typename Type2>
    inline RealType Diss(const Type1& rA, const Type2& rB) const
    {
        RealType D= 0;

        if(rA!=rB)
            D= 1;

        return D;
    }

private:

}; // class Delta

}  // namespace spare

#endif  // _Delta_h_
