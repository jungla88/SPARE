//  Centroid class, part of the SPARE library.
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

/** @brief File Centroid.hpp, that contains the Centroid class.
 *
 * Contains the declaration of a Centroid.
 *
 * @file Centroid.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Centroid_h_
#define _Centroid_h_

// STD INCLUDES
#include <iterator>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Real type centroid.
 *
 * This class implements the @a Representative concept.
 * It's a cluster model based on centroid, that is the mean of the vector in the cluster.
 * The centroid thus is vector of real numbers.
 * The dissimilarity measure used for centroid-centroid and centroid-sample comparison is a template argument.
 * @todo BatchUpdate Implementation.
 * @todo Merge Implementation.
 */
template <typename Dissimilarity>
class Centroid
{
public:

// PUBLIC TYPES

   /** Real centroid.
    */
   typedef std::vector<RealType>
                        CentroidVector;

// LIFECYCLE

   /** Default constructor.
    */
   Centroid()
      : mCount(0)
                                                   { }

// OPERATIONS

   /** Centroid update routine.
    *
    * @param[in] aSample Pair of iterators that delimit the sample.
    */
   template <typename ForwardIterator>
   void                 Update(std::pair<ForwardIterator, ForwardIterator> aSample);

   /** Centroid update routine.
    *
    * @param[in] rSample Reference to the container that store the sample.
    */
   template <typename SequenceContainer>
   void                 Update(const SequenceContainer& rSample);

   /** Calculates the dissimilarity between the sample and the centroid.
    *
    * @param[in] aSample Pair of iterators that delimit the sample.
    * @return The calculated dissimilarity value.
    */
   template <typename ForwardIterator>
   RealType             Diss(std::pair<ForwardIterator, ForwardIterator> aSample) const
                           {
                              if (!mCount)
                              {
                                 throw SpareLogicError("Centroid, 0, Uninitialized "
                                                       "object.");
                              }

                              return mDissAgent.Diss(
                                                 std::make_pair(
                                                    mCentroid.begin(),
                                                    mCentroid.end() ),
                                                 aSample);
                           }

   /** Calculates the dissimilarity between the sample and the centroid.
    *
    * @param[in] rSample Reference to the container that store the sample.
    * @return The calculated dissimilarity value.
    */
   template <typename SequenceContainer>
   RealType             Diss(const SequenceContainer& rSample) const
                           {
                              if (!mCount)
                              {
                                 throw SpareLogicError("Centroid, 1, Uninitialized "
                                                       "object.");
                              }

                              return mDissAgent.Diss(
                                                 mCentroid,
                                                 rSample);
                           }

   /** Calculates the dissimilarity between two centroid.
    *
    * @param[in] rOther Reference to another centroid.
    * @return The calculated dissimilarity.
    */
   RealType             Diss(const Centroid& rOther) const
                           {
                              if (!mCount)
                              {
                                 throw SpareLogicError("Centroid, 2, Uninitialized "
                                                       "object.");
                              }

                              return mDissAgent.Diss(
                                                 mCentroid,
                                                 rOther.mCentroid);
                           }

// ACCESS

   /** Read/Write access to the dissimilarity agent.
    *
    * @return A reference to the dissimilarity agent.
    */
   Dissimilarity&       DissAgent()                { return mDissAgent; }

   /** Read access to the dissimilarity agent.
    *
    * @return A reference to the dissimilarity agent.
    */
   const Dissimilarity& DissAgent() const          { return mDissAgent; }

   /** Read access to the centroid (the real vector).
    *
    * @return A reference to the centroid.
    */
   const CentroidVector& getRepresentativeSample() const        { return mCentroid; }

   /** Read access to the number of samples used so far.
    *
    * @return The number of samples.
    */
   NaturalType          GetCount() const           { return mCount; }

private:

   // Vettore contenente le componenti del centroide.
   CentroidVector       mCentroid;

   // Conteggio dei campioni che hanno contribuito al calcolo del centroide.
   NaturalType          mCount;

   // Istanza classe misuratrice di dissimilarit&agrave;.
   Dissimilarity        mDissAgent;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & mCentroid;
      ar & mCount;
      ar & mDissAgent;
   } // BOOST SERIALIZATION

}; // class Centroid

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Dissimilarity>
template <typename ForwardIterator>
void
Centroid<Dissimilarity>::Update(std::pair<ForwardIterator, ForwardIterator> aSample)
{
   // Typedef locali.
   typedef CentroidVector::size_type
                        CentroidSizeType;

   typedef typename std::iterator_traits<ForwardIterator>::difference_type
                        SampleDiffType;

   // Variabili.
   CentroidVector::iterator
                        Mit;

   // Se è il primo aggiornamento imposto la dimensione.
   if (!mCount)
   {
      mCentroid.resize( boost::numeric::converter<CentroidSizeType, SampleDiffType>
                        ::convert( std::distance(aSample.first, aSample.second) ) );
   }

   // Controllo.
   #if SPARE_DEBUG
   if ( static_cast<SampleDiffType>( mCentroid.size() ) !=
        std::distance(aSample.first, aSample.second) )
   {
      throw SpareLogicError("Centroid, 3, Different lenghts.");
   }
   #endif

   Mit= mCentroid.begin();
   ++mCount;

   while (aSample.first != aSample.second)
   {
      (*Mit)+= ( static_cast<RealType>(*aSample.first++) - *Mit ) /
                 static_cast<RealType>(mCount);
      ++Mit;
   }
}  // Update

template <typename Dissimilarity>
template <typename SequenceContainer>
void
Centroid<Dissimilarity>::Update(const SequenceContainer& rSample)
{
   // Typedef locali.
   typedef CentroidVector::size_type
                        CentroidSizeType;

   typedef typename SequenceContainer::size_type
                        SampleSizeType;

   // Variabili.
   CentroidVector::iterator
                        Mit;

   typename SequenceContainer::const_iterator
                        Sit;

   // Se è il primo aggiornamento imposto la dimensione.
   if (!mCount)
   {
      mCentroid.resize( boost::numeric::converter<CentroidSizeType, SampleSizeType>
                        ::convert( rSample.size() ) );
   }

   // Controllo.
   #if SPARE_DEBUG
   if ( mCentroid.size() != rSample.size() )
   {
      throw SpareLogicError("Centroid, 4, Different lenghts.");
   }
   #endif

   Mit= mCentroid.begin();
   Sit= rSample.begin();
   ++mCount;

   while (rSample.end() != Sit)
   {
      (*Mit)+= ( static_cast<RealType>(*Sit++) - *Mit ) /
                 static_cast<RealType>(mCount);
      ++Mit;
   }
}  // Update

}  // namespace spare

#endif  // _Centroid_h_
