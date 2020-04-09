/*
 * RealScalarParser.hpp
 *
 *  Created on: Mar 19, 2012
 *      Author: Azzurra
 */

#ifndef REALSCALARPARSER_HPP_
#define REALSCALARPARSER_HPP_

/** @brief
 * Parser able recognize the sequencer element read as a real number.
 *
 */

class RealScalarParser
{
public:

    template <typename BasicElementType>
    void Convert(std::string& rawElement, BasicElementType& convertedElement) const
    {
        convertedElement=atof(rawElement.c_str());
    }
};


#endif /* REALSCALARPARSER_HPP_ */
