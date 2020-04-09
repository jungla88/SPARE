//  Euclidean class, part of the SPARE library.
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

/** @brief File Euclidean.hpp, that contains Euclidean class.
 *
 * Contains the declaration of the Euclidean class.
 *
 * @file Euclidean.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Euclidean_h_
#define _Euclidean_h_

// STD INCLUDES
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Real euclidean distance.
 *
 * This class implements the @a Dissimilarity concept.
 * This class contains functions for the computation of the Euclidean distance between real vectors.
 * It's possible to weight the components of the vectors differently usign a proper weight vector.
 */
class Euclidean
{
public:

// OPERATIONS

   /** Euclidean distance computation.
    *
    * @param[in] aA A pair of iterators of the first vector.
    * @param[in] aB A pair of iterators of the second vector.
    * @return The distance value.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   RealType             Diss(
                           std::pair<ForwardIterator1, ForwardIterator1> aA,
                           std::pair<ForwardIterator2, ForwardIterator2> aB) const;

   /** Euclidean distance computation.
    *
    * @param[in] rA A reference to the container of the first vector.
    * @param[in] rB A reference to the container of the second vector.
    * @return The distance value.
    */
   template <typename SequenceContainer1, typename SequenceContainer2>
   RealType             Diss(
                           const SequenceContainer1& rA,
                           const SequenceContainer2& rB) const;

// SETUP

   /** Setup of the weight vector.
    *
    * @param[in] aW A pair of iterators for the weight vector.
    */
   template <typename ForwardIterator>
   void                 WeightSetup(std::pair<ForwardIterator, ForwardIterator> aW);

   /** Setup of the weight vector.
    *
    * @param[in] rW The container of the weights.
    */
   template <typename SequenceContainer>
   void                 WeightSetup(const SequenceContainer& rW);

private:

   // Tipo vettore pesi.
   typedef std::vector<RealType>
                        WeightVector;

   // Vettore contenente i pesi.
   WeightVector         mWeights;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mWeights);
   } // BOOST SERIALIZATION

}; // class Euclidean

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
RealType
Euclidean::Diss(
              std::pair<ForwardIterator1, ForwardIterator1> aA,
              std::pair<ForwardIterator2, ForwardIterator2> aB) const
{
   // Variabili.
   RealType                         D= 0;
   WeightVector::const_iterator     Wit;

   // Controllo.
   #if SPARE_DEBUG
   // Typedef locali.
   typedef typename std::iterator_traits<ForwardIterator1>::difference_type
                                             DiffTypeA;

   typedef typename std::iterator_traits<ForwardIterator2>::difference_type
                                             DiffTypeB;

   DiffTypeA DistA= std::distance(aA.first, aA.second);
   DiffTypeB DistB= std::distance(aB.first, aB.second);

   if ( (DistA < 0) || (DistB < 0) )
   {
      throw SpareLogicError("Euclidean, 0, Invalid range.");
   }

   if (DistA != DistB)
   {
      throw SpareLogicError("Euclidean, 1, Different lengths between inputs.");
   }

   if ( !mWeights.empty() )
   {
      if (static_cast<DiffTypeA>( mWeights.size() ) != DistA)
      {
         throw SpareLogicError("Euclidean, 2, Different lenghts between inputs and "
                               "weights.");
      }
   }
   #endif

   if ( mWeights.empty() )
   {
      while (aA.first != aA.second)
      {
         D+= std::pow(
                static_cast<RealType>(*aA.first++) - static_cast<RealType>(*aB.first++),
                2);
      }
   }
   else
   {
      Wit= mWeights.begin();
      while (aA.first != aA.second)
      {
         D+= std::pow(
                static_cast<RealType>(*aA.first++) - static_cast<RealType>(*aB.first++),
                2) * (*Wit++);
      }
   }

   return std::sqrt(D);
}  // Diss

template <typename SequenceContainer1, typename SequenceContainer2>
RealType
Euclidean::Diss(
              const SequenceContainer1& rA,
              const SequenceContainer2& rB) const
{
   // Controllo.
   #if SPARE_DEBUG
   if ( rA.size() != rB.size() )
   {
      throw SpareLogicError("Euclidean, 3, Different lenghts between inputs.");
   }

   if ( !mWeights.empty() )
   {
      if ( mWeights.size() != rA.size() )
      {
         throw SpareLogicError("Euclidean, 4, Different lenghts between inputs and "
                               "weights.");
      }
   }
   #endif


   return Diss(std::make_pair(rA.begin(), rA.end()), std::make_pair(rB.begin(), rB.end()));

}  // Diss

template <typename ForwardIterator>
void
Euclidean::WeightSetup(std::pair<ForwardIterator, ForwardIterator> aW)
{
   ForwardIterator Wit= aW.first;

   while (Wit != aW.second)
   {
      if (*Wit++ < 0)
      {
         throw SpareLogicError("Euclidean, 5, Negative weights not allowed.");
      }
   }

   mWeights.assign(
               aW.first,
               aW.second);
}  // WeightSetup

template <typename SequenceContainer>
void
Euclidean::WeightSetup(const SequenceContainer& rW)
{
   typename SequenceContainer::const_iterator Wit= rW.begin();

   while (rW.end() != Wit)
   {
      if (*Wit++ < 0)
      {
         throw SpareLogicError("Euclidean, 6, Negative weights not allowed.");
      }
   }

   mWeights.assign(
               rW.begin(),
               rW.end() );
}  // WeightSetup

}  // namespace spare

#endif  // _Euclidean_h_
