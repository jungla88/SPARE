//  SequenceIO class, part of the SPARE library.
//  Copyright (C) 2014 Francesca Possemato
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

/** @brief General Sequence reader/writer.
 *
 * @file SequenceIO.hpp
 * @author Francesca Possemato
 */


#ifndef SEQUENCEIO_HPP
#define SEQUENCEIO_HPP

//STD INCLUDES
#include <string>
#include <fstream>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/Sequence/Sequence.hpp>
#include <spare/SpareExceptions.hpp>

// BOOST INCLUDES
#include <boost/config.hpp>

namespace spare {

/** @brief 
 */
class SequenceIO {
public:

    /**
     * Sequence file read operation.
     * @param[in] in The input stream
     * @param[out] g A compatible boost graph defined as a simple unlabeled graph
     */
     template<typename ParserContainer, typename AtomicElement>
     void readSequence(std::ifstream&    rStrm, ParserContainer& parser, Sequence<AtomicElement>& rSequence);

     template<typename ParserContainer, typename AtomicElement>
     void readFeaturesSequence(std::ifstream&    rStrm, ParserContainer& parser, Sequence<AtomicElement>& rSequence);

     template<typename AtomicElement>
     void writeFeaturesSequence(std::ofstream&  rStrm, Sequence<AtomicElement>& rSequence);

};

// IMPL.

template<typename ParserContainer, typename AtomicElement>
void SequenceIO::readSequence(std::ifstream&    rStrm, ParserContainer& parser, Sequence<AtomicElement>& rSequence)

{
   std::string              Buf;
   typedef typename Sequence<AtomicElement>::size_type   index, Size;

   index i;
   Size Sz;

   if (!rStrm.good())
   {
      throw std::runtime_error("Input stream trouble.");
   }

   rStrm >> Sz;

   if (!rStrm.good())
   {
      throw std::runtime_error("Input stream trouble.");
   }

   rStrm >> Buf;

   if (!rStrm.good() || (Buf != ":"))
   {
      throw std::runtime_error("Input stream trouble.");
   }

   rSequence.resize(Sz);

   for (i= 0; i < Sz; ++i)
   {
       // convert token to the atomic symbol defined for the type of sequence
       AtomicElement el;
       std::string stringOfSeq;

       rStrm >> stringOfSeq;
       parser.Convert(stringOfSeq, el);
       rSequence[i]=el;

   }

   if (rStrm.bad())
   {
      throw std::runtime_error("Input stream trouble.");
   }
}

template<typename ParserContainer, typename AtomicElement>
void SequenceIO::readFeaturesSequence(std::ifstream&  rStrm, ParserContainer& parser, Sequence<AtomicElement>& rSequence)
{
	rSequence.clear();

	std::string stringOfSeq;
	std::string convString;
	rStrm >> stringOfSeq;
	if (rStrm.good())
	{
		std::stringstream sStrm(stringOfSeq);
		std::string item;
		while (std::getline(sStrm, item, ','))
		{
			parser.Convert(item, convString);
			rSequence.push_back(convString);
		}
	}


}

template<typename AtomicElement>
void SequenceIO::writeFeaturesSequence(std::ofstream&  rStrm, Sequence<AtomicElement>& rSequence)
{
	typedef typename Sequence<AtomicElement>::const_iterator SeqIt;

	if (!rStrm.good())
	{
		throw std::runtime_error("Output stream trouble.");
	}

	SeqIt Sit= rSequence.begin();
	while (rSequence.end()-1 != Sit)
	{
	   rStrm << *Sit++ << ',';
	}
	rStrm << *Sit++;
}


}

#endif
