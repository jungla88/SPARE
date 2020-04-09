//  RealScalarParser class, part of the SPARE library.
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

/** @brief File RealScalarParser.hpp, that contains RealScalarParser class.
 *
 * Contains the declaration of the RealScalarParser class.
 *
 * @file RealScalarParser.hpp
 * @author Lorenzo Livi
 */

#ifndef REALSCALARPARSER_HPP_
#define REALSCALARPARSER_HPP_

namespace spare {

/** @brief RealScalarParser utility to convert a raw input element as real-value number.
 * To be used by the SeqReader class.
 */
class RealScalarParser
{
public:

    template <typename BasicElementType>
    void Convert(const std::string& rawElemet, BasicElementType& convertedElement) const
    {
        convertedElement=atof(rawElemet.c_str());
    }
};

}

#endif /* REALSCALARPARSER_HPP_ */
