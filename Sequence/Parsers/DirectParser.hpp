/*
 * DirectParser.hpp
 *
 *  Created on: Mar 10, 2012
 *      Author: Azzurra
 */

#ifndef DIRECTPARSER_HPP_
#define DIRECTPARSER_HPP_

/** @brief
 * Parser able recognize the sequencer element read as a string or character.
 *
 */

class DirectParser
{
public:

    template <typename BasicElementType>
    void Convert(std::string& rawElemet, BasicElementType& convertedElement) const
    {
        convertedElement=rawElemet;
    }
};



#endif /* DIRECTPARSER_HPP_ */
