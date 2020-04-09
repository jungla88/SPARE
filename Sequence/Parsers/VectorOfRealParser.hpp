/*
 * VectorOfRealParser.hpp
 *
 *  Created on: 27/apr/2012
 *      Author: Azzurra
 */

#ifndef VECTOROFREALPARSER_HPP_
#define VECTOROFREALPARSER_HPP_

/** @brief
 * Parser able recognize the sequencer element read as a vector of real numbers.
 *
 */

class VectorOfRealParser
{
public:

    template <typename BasicElementType>
    void Convert(std::string& rawElemet, BasicElementType& convertedElement)
    {
        string2vectorOfReal(rawElemet, convertedElement);
    }

private:

    void string2vectorOfReal(std::string& sequenceEl, std::vector<spare::RealType>& sequenceVec)
    {
        // skip the first character ([) and the last character (]) of the vector represented by the string s
        std::string element=sequenceEl.substr(1, sequenceEl.size()-2);

        // the "," is the separator character between two elements of the vector
        char_separator<char> sep(",");
        tokenizer< char_separator<char> > tokens(element, sep);

        BOOST_FOREACH(std::string s, tokens)
        {
//            std::cout<<s<<std::endl;
            sequenceVec.push_back(atof(s.c_str()));

        }
    }

};


#endif /* VECTOROFREALPARSER_HPP_ */
