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

#ifndef VECTORPARSER_HPP_
#define VECTORPARSER_HPP_

namespace spare {

/** @brief VectorParser utility to convert a raw input element as a container (push_back() must be defined) of real-value numbers.
 * To be used by the SeqReader class.
 */
class VectorParser
{
public:

    template <typename BasicElementType>
    void Convert(const std::string& rawElemet, BasicElementType& convertedElement)
    {
        int cutAt;
        std::string ss=rawElemet.substr(1, rawElemet.size()-2);
        while((cutAt=ss.find_first_of(","))!=(int)ss.npos)
        {
            if(cutAt>0)
                convertedElement.push_back(atof(ss.substr(0,cutAt).c_str()));
            ss=ss.substr(cutAt+1);
        }

        if(ss.length()>0)
            convertedElement.push_back(atof(ss.c_str()));
    }
};

}

#endif
