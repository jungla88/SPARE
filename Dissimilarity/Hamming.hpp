//  Hamming class, part of the SPARE library.
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

/** @brief File Hamming.hpp, that contains the Hamming class.
 *
 * Contains the declaration of the Hamming class.
 *
 * @file Hamming.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Hamming_h_
#define _Hamming_h_

// STD INCLUDES
#include <iterator>
#include <utility>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief %Hamming distance.
 *
 * This class implements the @a Dissimilarity concept.
 * It offers static functions for the computation of the Hamming distance between vector of generic objects
 * that implements the != operator.
 */
class Hamming
{
public:

// OPERATIONS

   /** Hamming distance computation.
    *
    * @param[in] aA A pair of iterators for the first vector.
    * @param[in] aB A pair of iterators for the second vector.
    * @return The distance value.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   static RealType      Diss(
                           std::pair<ForwardIterator1, ForwardIterator1> aA,
                           std::pair<ForwardIterator2, ForwardIterator2> aB);

   /** Hamming distance computation.
    *
    * @param[in] rA A reference to the first container.
    * @param[in] rB A reference to the second container.
    * @return The distance value.
    */
   template <typename SequenceContainer1, typename SequenceContainer2>
   static RealType      Diss(
                           const SequenceContainer1& rA,
                           const SequenceContainer2& rB);

private:

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
   } // BOOST SERIALIZATION

}; // class Hamming


/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
RealType
Hamming::Diss(
            std::pair<ForwardIterator1, ForwardIterator1> aA,
            std::pair<ForwardIterator2, ForwardIterator2> aB)
{
   // Variabili.
   RealType          D= 0;

   // Controllo.
   #if SPARE_DEBUG
   if ( std::distance(aA.first, aA.second) < 0 )
   {
      throw SpareLogicError("Hamming, 0, Invalid range.");
   }

   if ( std::distance(aA.first, aA.second) != std::distance(aB.first, aB.second) )
   {
      throw SpareLogicError("Hamming, 1, Different lenghts.");
   }
   #endif

   while (aA.first != aA.second)
   {
      if ( (*aA.first++) != (*aB.first++) )
      {
         D++;
      }
   }

   return D;
}  // Diss

template <typename SequenceContainer1, typename SequenceContainer2>
RealType
Hamming::Diss(
            const SequenceContainer1& rA,
            const SequenceContainer2& rB)
{
   // Controllo.
   #if SPARE_DEBUG
   if ( rA.size() != rB.size() )
   {
      throw SpareLogicError("Hamming, 2, Different lenghts.");
   }
   #endif

   return Diss(std::make_pair(rA.begin(), rA.end()), std::make_pair(rB.begin(), rB.end()));
}  // Diss

}  // namespace spare

#endif  // _Hamming_h_
