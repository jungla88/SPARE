//  SeqReader class, part of the SPARE library.
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

/** @brief File SeqReader.hpp, that contains SeqReader class.
 *
 * Contains the declaration of the SeqReader class.
 *
 * @file SeqReader.hpp
 * @author Lorenzo Livi
 */

#ifndef SEQREADER_HPP_
#define SEQREADER_HPP_

//STD
#include <string>
#include <iostream>
#include <fstream>

//BOOST
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

namespace spare {

/** @brief Simple dataset utility for loading sequences of objects from file.
 *
 * The class requires two template arguments. The first one is the specific type for the objects characterizing the sequences (e.g., a string, a vector of real-valued numbers).
 * The second one is the parser for that type of objects, which converts the string representing the object in the file and converts that string into the type defined by the first template argument.
 * This reader generates two containers: one containing sequences of objects and the other containing the (class) labels.
 */
template <class BasicElementType, class ObjectParserType>
class SeqReader
{
public:

    /**
     * Read-write access to the object parser
     * @return A const reference to the parser
     */
    ObjectParserType& Parser() { return mParser; }

    /**
     * Read-only access to the object parser
     * @return A reference to the parser
     */
    const ObjectParserType& Parser() const { return mParser; }


    /**
     * Read the dataset of sequences from file
     * @param[in] path The physical path of the file
     * @param[out] ds The container for the sequences
     * @param[out] labels The container for the class labels of the sequences
     */
    template <typename SequenceContainerType, typename LabelContainerType>
    void load(std::string& path, SequenceContainerType& ds, LabelContainerType& labels)
    {
        typedef typename SequenceContainerType::value_type SequenceType;
        typedef typename LabelContainerType::value_type LabelType;

        std::string line;
        std::ifstream myfile;

        myfile.open(path.c_str(), std::ios::binary);

        if(myfile.is_open())
        {
            //skip first line
            getline(myfile, line);

            //1 line, 1 sequence of objects
            while(myfile.good())
            {
                SequenceType s;
                LabelType l;
                getline(myfile, line);

                boost::char_separator<char> sep(" ");
                boost::tokenizer< boost::char_separator<char> > tokens(line, sep);

                unsigned int i=0;
                BOOST_FOREACH(std::string t, tokens)
                {
                    //label
                    if(i==0)
                        l=t;
                    //objects of the sequence
                    else if(i>=3)
                    {
                        //basic element/object type of the sequence
                        BasicElementType e;
                        mParser.Convert(t, e);
                        s.insert(s.end(), e);
                    }

                    i++;
                }

                //skip empty lines
                if(!line.empty())
                {
                    ds.insert(ds.end(), s);
                    labels.insert(labels.end(), l);
                }
            }

            myfile.close();
          }
          else
              std::cout << "Unable to open dataset at: "<<path<<std::endl;
    }

private:

    /**
     * Parse for the objects characterizing the sequences
     */
    ObjectParserType mParser;

};

}

#endif /* SEQREADER_HPP_ */
