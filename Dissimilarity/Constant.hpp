//  Constant class, part of the SPARE library.
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

/** @brief File Delta.hpp, that contains Constant class.
 *
 * Contains the declaration of the Constant class.
 *
 * @file Constant.hpp
 * @author Lorenzo Livi
 */

#ifndef _Constant_h_
#define _Constant_h_

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Constant distance.
 *
 * This class implements the @a Dissimilarity concept.
 * This class contains an inline function that returns a constant user-defined dissimilarity value.
 * The default dissimilarity value is 0.
 */
class Constant
{
public:

    /**
     * Default constructor
     */
    Constant()
    {
        mValue=0;
    }

// OPERATIONS

    /** Constant distance.
     *
     * @param[in] rA A reference to the first element
     * @param[in] rB A reference to the second element
     * @return The user-defined constant dissimilarity value.
     */
    template <typename Type1, typename Type2>
    inline RealType Diss(const Type1& rA, const Type2& rB) const
    {
        return mValue;
    }

    /**
     * Read/write access to the user-defined constant dissimilarity value.
     */
    RealType& Value() { return mValue; }

    /**
     * Read-only access to the user-defined constant dissimilarity value.
     */
    const RealType& Value() const { return mValue; }

private:

    /**
     * Constant dissimilarity value
     */
    RealType mValue;

}; // class Constant

}  // namespace spare

#endif  // _Constant_h_
