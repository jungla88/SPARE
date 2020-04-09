//  ModuleDistance class, part of the SPARE library.
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

/** @brief File ModuleDistance.hpp, that contains ModuleDistance class.
 *
 * Contains the declaration of the ModuleDistance class.
 *
 * @file ModuleDistance.hpp
 * @author Lorenzo Livi
 */

#ifndef _ModuleDistance_h_
#define _ModuleDistance_h_

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Module distance between two scalar types.
 *
 * This class implements the @a Dissimilarity concept.
 * This class contains inline functions for the computation of the module distance between any two numeric values.
 */
class ModuleDistance
{
public:

// OPERATIONS

    /** Module distance computation.
     *
     * @param[in] rA A reference to the first element.
     * @param[in] rB A reference to the second element.
     * @return The absolute difference
     */
    template <typename Type1, typename Type2>
    inline RealType Diss(const Type1& rA, const Type2& rB) const
    {
        return std::abs(static_cast<RealType>(rA)-static_cast<RealType>(rB));
    }

private:

}; // class ModuleDistance

}  // namespace spare

#endif  // _ModuleDistance_h_
