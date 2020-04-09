//  Minkowski class, part of the SPARE library.
//  Copyright (C) 2011 Lorenzo Livi
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

/** @brief File Minkowski.hpp, that contains Minkowski class.
 *
 * Contains the declaration of the Minkowski class.
 *
 * @file Minkowski.hpp
 * @author Lorenzo Livi
 */

#ifndef _Minkowski_h_
#define _Minkowski_h_

// STD INCLUDES
#include <cmath>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Real %Minkowski distance.
 *
 * This class implements the @a Dissimilarity concept.
 * This class contains functions for the computation of the Minkowski distance between
 * real vectors. It's possible to weight the components of the vectors differently usign a
 * proper weight vector. It uses a parameter p, that by default is set to 2, that is an
 * Euclidean distance.
 */
class Minkowski
{
public:

	/** Unsigned strictly positive integer parameter type.
	 */
	typedef BoundedParameter<NaturalType>
						NaturalParam;

// LIFECYCLE

	/** Default constructor. It sets P=2, that is an Euclidean distance.
	 */
	Minkowski(): mP(1, std::numeric_limits<NaturalType>::max())
	{
		mP=2;
	}


// OPERATIONS

   /** Minkowski distance computation.
    *
    * @param[in] aA A pair of iterators of the first vector.
    * @param[in] aB A pair of iterators of the second vector.
    * @return The distance value.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   RealType             Diss(
                           std::pair<ForwardIterator1, ForwardIterator1> aA,
                           std::pair<ForwardIterator2, ForwardIterator2> aB) const;

   /** Minkowski distance computation.
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

   /** Setup the weight vector.
    *
    * @param[in] aW A pair of iterator for the weight vector.
    */
   template <typename ForwardIterator>
   void                 WeightSetup(std::pair<ForwardIterator, ForwardIterator> aW);

   /** Setup the weight vector.
    *
    * @param[in] rW A reference to the container of the weight vector.
    */
   template <typename SequenceContainer>
   void                 WeightSetup(const SequenceContainer& rW);


// ACCESS

	/** Read/Write access to the order P.
	 *
	 * @return A reference to the P parameter.
	 */
	NaturalParam&        P()                { return mP; }


	/** Read only access to the order P.
	 *
	 * @return A const reference to the P parameter.
	 */
	const NaturalParam&  P() const    		{ return mP; }

private:

   // Distance order
   NaturalParam         mP;

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
      ar & BOOST_SERIALIZATION_NVP(mP);
      ar & BOOST_SERIALIZATION_NVP(mWeights);
   } // BOOST SERIALIZATION

}; // class Minkowski

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
RealType
Minkowski::Diss(
              std::pair<ForwardIterator1, ForwardIterator1> aA,
              std::pair<ForwardIterator2, ForwardIterator2> aB) const
{
   // Variabili.
   RealType                         D= 0;
   WeightVector::const_iterator     Wit;
   RealType p=boost::numeric::converter<RealType, NaturalType>::convert(mP);

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
      throw SpareLogicError("Minkowski, 0, Invalid range.");
   }

   if (DistA != DistB)
   {
      throw SpareLogicError("Minkowski, 1, Different lengths between inputs.");
   }

   if ( !mWeights.empty() )
   {
      if (static_cast<DiffTypeA>( mWeights.size() ) != DistA)
      {
         throw SpareLogicError("Minkowski, 2, Different lenghts between inputs and "
                               "weights.");
      }
   }
   #endif

   if ( mWeights.empty() )
   {
      while (aA.first != aA.second)
      {
         D+= std::pow(
                std::abs(static_cast<RealType>(*aA.first++)
                         - static_cast<RealType>(*aB.first++)),
                p);
      }
   }
   else
   {
      Wit= mWeights.begin();
      while (aA.first != aA.second)
      {
         D+= std::pow(
        		 std::abs(static_cast<RealType>(*aA.first++)
                      - static_cast<RealType>(*aB.first++)),
                p) * (*Wit++);
      }
   }

   return std::pow(D, 1/p);
}  // Diss

template <typename SequenceContainer1, typename SequenceContainer2>
RealType
Minkowski::Diss(
              const SequenceContainer1& rA,
              const SequenceContainer2& rB) const
{
   // Controllo.
   #if SPARE_DEBUG
   if ( rA.size() != rB.size() )
   {
      throw SpareLogicError("Minkowski, 3, Different lenghts between inputs.");
   }

   if ( !mWeights.empty() )
   {
      if ( mWeights.size() != rA.size() )
      {
         throw SpareLogicError("Minkowski, 4, Different lenghts between inputs and "
                               "weights.");
      }
   }
   #endif


   return Diss(std::make_pair(rA.begin(), rA.end()), std::make_pair(rB.begin(), rB.end()));
}  // Diss

template <typename ForwardIterator>
void
Minkowski::WeightSetup(std::pair<ForwardIterator, ForwardIterator> aW)
{
   ForwardIterator Wit= aW.first;

   while (Wit != aW.second)
   {
      if (*Wit++ < 0)
      {
         throw SpareLogicError("Minkowski, 5, Negative weights not allowed.");
      }
   }

   mWeights.assign(
               aW.first,
               aW.second);
}  // WeightSetup

template <typename SequenceContainer>
void
Minkowski::WeightSetup(const SequenceContainer& rW)
{
   typename SequenceContainer::const_iterator Wit= rW.begin();

   while (rW.end() != Wit)
   {
      if (*Wit++ < 0)
      {
         throw SpareLogicError("Minkowski, 6, Negative weights not allowed.");
      }
   }

   mWeights.assign(
               rW.begin(),
               rW.end() );
}  // WeightSetup

}  // namespace spare

#endif  // _Minkowski_h_
