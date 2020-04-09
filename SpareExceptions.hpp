//  Spare exception classes, part of the SPARE library.
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

/** @brief File SpareExceptions.hpp, that contains the SPARE exceptions.
 *
 * Contains the SPARE exceptions.
 *
 * @file SpareExceptions.hpp
 * @author Guido Del Vescovo
 */

#ifndef _SpareExceptions_h_
#define _SpareExceptions_h_

// STD INCLUDES
#include <stdexcept>
#include <string>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Logic error class.
 *
 * Derived from the standard logic error class.
 */
class SpareLogicError : public std::logic_error
{
public:

   explicit SpareLogicError(const std::string& rWhat) : std::logic_error(rWhat) { };
};

/** @brief Runtime error class.
 *
 * Derived from the standard runtime error class.
 */
class SpareRuntimeError : public std::runtime_error
{
public:

   explicit SpareRuntimeError(const std::string& rWhat) : std::runtime_error(rWhat) { };
};

}  // namespace spare

#endif  // _SpareExceptions_h_
