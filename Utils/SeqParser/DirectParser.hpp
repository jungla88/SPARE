//  DirectParser class, part of the SPARE library.
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

/** @brief File DirectParser.hpp, that contains DirectParser class.
 *
 * Contains the declaration of the DirectParser class.
 *
 * @file DirectParser.hpp
 * @author Lorenzo Livi
 */

#ifndef DIRECTPARSER_HPP_
#define DIRECTPARSER_HPP_

namespace spare {

/** @brief Simple utility to convert a raw input element directly into a basic element type expected by the user.
 * To be used by the SeqReader class.
 */
class DirectParser
{
public:

    template <typename BasicElementType>
    void Convert(const std::string& rawElemet, BasicElementType& convertedElement) const
    {
        convertedElement=rawElemet;
    }
};

}

#endif /* DIRECTPARSER_HPP_ */
