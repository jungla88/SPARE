//  FuzzyMinSod class, part of the SPARE library.
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

/** @brief File FuzzyMinSod.hpp, containing the FuzzyMinSod class.
 *
 * The file contains the FuzzyMinSod class, implementing a cluster model based on
 * the Sum Of Distances (SOD) minimizing sample. The cluster is actually a fuzzy set whose
 * membership function is defined as a user-defined function of the distance from the MinSOD element of the set.
 *
 * @file FuzzyMinSod.hpp
 * @author Lorenzo Livi
 */

#ifndef _FuzzyMinSod_h_
#define _FuzzyMinSod_h_

// STD INCLUDES
#include <algorithm>
#include <iterator>
#include <limits>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/random.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Cluster model based on Fuzzy Set and Sum Of Distances (SOD) minimization.
 *
 * This class models the @a Representative concept. The set of elements is represented as a fuzzy set,
 * whose membership function is a user-defined function of the distances from the MinSOD element.
 * The user-defined function must be a model implementing the @a Evaluator concept, that is, it must implement
 * the evaluation method.
 * The cluster representation approach is
 * based on the determination of the sample which minimizes the sum of distances from the
 * other samples in the cluster. This allows the usage with a generic data type for samples,
 * only requiring a dissimilarity measure to be defined. A speed up is adopted, based on
 * the tracking of the SOD-minimizing element within a reduced pool of samples (@a cache).
 * @todo Access to the dissimilarity matrix.
 * @todo BatchUpdate implementation.
 * @todo Merge implementation.
 * @todo Random choose between SODs of the same score.
 */
template <typename SampleType, typename Dissimilarity, typename Evaluator>
class FuzzyMinSod
{
public:

// PUBLIC TYPES

   /** Container type for stored samples.
    */
   typedef std::vector<SampleType>
                        SampleVector;

   /** Container type for the SOD associated to each stored sample.
    */
   typedef std::vector<RealType>
                        SodVector;

   //
   typedef typename std::vector<SampleType>::size_type
                         SampleSizeType;

// LIFECYCLE

   /** Default constructor.
    */
   FuzzyMinSod()
      : mSampleDist( 0, std::numeric_limits<BoostRealSymmMatrix::size_type>::max() )
                                                   { Init(20);
                                                     mP=1;
                                                   }

   /** Constructor taking the cache size as argument.
    *
    * @param[in] aM Cache size.
    */
   FuzzyMinSod(NaturalType aM)
      : mSampleDist( 0, std::numeric_limits<BoostRealSymmMatrix::size_type>::max() )
                                                   { Init(aM);
                                                        mP=1;
                                                   }

// OPERATIONS

   /** Update of the representative.
    *
    * @param[in] rSample A reference to the new sample.
    */
   void                 Update(const SampleType& rSample);

   /** Dissimilarity evaluation between sample and representative.
    *
    * @param[in] rSample Reference to the sample.
    * @return The dissimilarity value.
    */
   RealType             Diss(const SampleType& rSample) const
                           {
                              if ( mSamples.empty() )
                              {
                                 throw SpareLogicError("FuzzyMinSod, 0, Uninitialized object.");
                              }

                              return mDissAgent.Diss(
                                                   mSamples[mMinSodIndex],
                                                   rSample);
                           }

   /** Dissimilarity evaluation between two representatives.
    *
    * @param[in] rOther Reference to another representative.
    * @return The dissimilarity value.
    */
   RealType             Diss(const FuzzyMinSod<SampleType, Dissimilarity, Evaluator>& rOther) const
                           {
                              if ( mSamples.empty() )
                              {
                                 throw SpareLogicError("FuzzyMinSod, 1, Uninitialized object.");
                              }

                              return mDissAgent.Diss(
                                                   mSamples[mMinSodIndex],
                                                   rOther.mSamples[rOther.mMinSodIndex]);
                           }

   /** Membership value evaluation between sample and representative.
       *
       * @param[in] rSample Reference to the sample.
       * @return The membership value of the sample.
       */
       RealType            Eval(const SampleType& rSample) const
                             {
                                if ( mSamples.empty() )
                                {
                                   throw SpareLogicError("FuzzyMinSod, 4, Uninitialized object.");
                                }

                                return mMembershipAgent.Eval(
                                                       mDissAgent.Diss(
                                                               mSamples[mMinSodIndex],
                                                               rSample)
                                                               );
                             }


// ACCESS

   /** Read access to the cache size.
    *
    * @return The value of the cache size.
    */
   NaturalType          M() const                  { return mM; }

   /**
    * Read/Write access to the power of the dissimiliarities
    *
    * @return The power to apply
    */
   RealType&            P()                        { return mP; }

   /** Read/write access to the dissimilarity agent.
    *
    * @return A reference to the dissimilarity agent.
    */
   Dissimilarity&       DissAgent()                { return mDissAgent; }

   /** Read only access to the dissimilarity agent.
    *
    * @return A reference to the dissimilarity agent.
    */
   const Dissimilarity& DissAgent() const          { return mDissAgent; }

   /** Read/write access to the membership agent.
    *
    * @return A reference to the membership agent.
    */
   Evaluator&       MembershipAgent()          { return mMembershipAgent; }

   /** Read only access to the membership agent.
    *
    * @return A reference to the membership agent.
    */
   const Evaluator& MembershipAgent() const          { return mMembershipAgent; }

   /** Read access to the container of stored samples.
    *
    * @return A reference to the stored samples.
    */
   const SampleVector&  GetSamples() const         { return mSamples; }

   /** Read access to the SODs of all samples.
    *
    * @return A reference to vector of SODs.
    */
   const SodVector&     GetSods() const            { return mSods; }

   /** Read-only access to the current min sod element index
    *
    * @return A reference to the min sod element index
    */
   const SampleSizeType& GetMinSodIndex() const    { return mMinSodIndex; }

   /** Read access to the membership values of all samples.
    *
    * @return A reference to vector of membership values.
    */
   const std::vector<RealType>&     GetMemberships() const            { return mMembershipValues; }

   /** Read access to the SOD-minimizing sample.
    *
    * @return A reference to the sample.
    */
   const SampleType&    getRepresentativeSample() const
                           {
                              if ( mSamples.empty() )
                              {
                                 throw SpareLogicError("FuzzyMinSod, 2, Uninitialized object.");
                              }

                              return mSamples[mMinSodIndex];
                           }

   /** Read access to the number of samples inserted so far.
    *
    * @return The number of samples.
    */
   NaturalType          GetCount() const           { return mCount; }

// SETUP

   /** Random seed setup.
    *
    * @param[in] aSeed The seed value.
    */
   void                 RandSeedSetup(NaturalType aSeed)
                           {
                              mRng.seed(aSeed);
                           }

private:

   // Potenza da applicare alle distanze
   RealType              mP;

   // Numero massimo campioni immagazzinati.
   NaturalType           mM;

   // Conteggio dei campioni che hanno contribuito al calcolo del rappresentante.
   NaturalType           mCount;

   // Istanza classe misuratrice di dissimilarit&agrave;.
   Dissimilarity         mDissAgent;

   // Istanza classe misuratrice di membership
   Evaluator             mMembershipAgent;

   // Campioni immagazzinati.
   SampleVector          mSamples;

   // Matrice dissimilarità.
   BoostRealSymmMatrix   mDissMatrix;

   // Valori SOD relativi ai campioni immagazzinati.
   SodVector             mSods;

   // valori di membership dei sample
   std::vector<RealType> mMembershipValues;

   // Indice dell'elemento in mSamples che minimizza la SOD.
   SampleSizeType        mMinSodIndex;

   // Indice dell'elemento in mSamples da scartare al prossimo arrivo.
   SampleSizeType        mDiscardIndex;

   // BOOST RANDOM
   // Generatore.
   mutable boost::mt19937
                         mRng;

   // Distribuzione per estrazione campione.
   mutable boost::uniform_int<BoostRealSymmMatrix::size_type>
                         mSampleDist;
   // BOOST RANDOM

   // Inizializzazione, richiamata dai costruttori.
   void                  Init(NaturalType aM);

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      BoostRealSymmMatrix::size_type i, j, k;

      k= boost::numeric::converter<BoostRealSymmMatrix::size_type, NaturalType>
         ::convert(mM);

      ar & mM;
      ar & mCount;
      ar & mDissAgent;
      ar & mSamples;

      if (k != mM)
      {
         mDissMatrix.resize(
                        boost::numeric
                        ::converter<BoostRealSymmMatrix::size_type, NaturalType>
                        ::convert(mM) );
      }

      k= boost::numeric::converter<BoostRealSymmMatrix::size_type, SampleSizeType>
         ::convert( mSamples.size() );

      for (i= 0; i < k; i++)
      {
         for (j= 0; j <= i; j++)
         {
            ar & mDissMatrix(i, j);
         }
      }

      ar & mSods;
      ar & mMinSodIndex;
      ar & mDiscardIndex;
   }  // BOOST SERIALIZATION

}; // class MinSod

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename SampleType, typename Dissimilarity, typename Evaluator>
void
FuzzyMinSod<SampleType, Dissimilarity, Evaluator>::Update(const SampleType& rSample)
{
   // Typedef locali.
   typedef std::iterator_traits<std::vector<RealType>::const_iterator>::difference_type
                        SodDiffType;

   // Variabili.
   typename std::vector<SampleType>::const_iterator   Pit;
   std::vector<RealType>::iterator                    Sit;
   BoostRealSymmMatrix::size_type                     i;
   BoostRealSymmMatrix::size_type                     j;
   BoostRealSymmMatrix::size_type                     k;
   RealType                                           Sod;
   RealType                                           Temp;


   if ( mSamples.size() < boost::numeric::converter<SampleSizeType, NaturalType>
                          ::convert(mM) )
   {
      // In questo caso accresco l'insieme dei campioni immagazzinati,
      // senza scartare nessuno.

      // Aggiungo campione.
      mSamples.push_back(rSample);
      mSods.push_back(0);
      mMembershipValues.push_back(1);

      // Calcolo distanze del nuovo campione dai vecchi.
      Pit= mSamples.begin();
      Sit= mSods.begin();
      Sod= 0;
      i= boost::numeric::converter<BoostRealSymmMatrix::size_type, SampleSizeType>
         ::convert(mSamples.size() - 1);

      // Eseguo aggiornamento.
      for(j= 0; j < i; ++j)
      {
         Temp= std::pow(mDissAgent.Diss(*Pit++, mSamples.back()), mP);

         (*Sit++)+= Temp;
         Sod+= Temp;
         mDissMatrix(i, j)= Temp;
      }

      (*Sit)= Sod;
      mDissMatrix(i, i)= 0;
   }
   else
   {
      // In questo caso scarto il campione da scartare.
      i= boost::numeric::converter<BoostRealSymmMatrix::size_type, SampleSizeType>
         ::convert(mDiscardIndex);

      mSamples[mDiscardIndex]= rSample;

      Pit= mSamples.begin();
      Sit= mSods.begin();
      Sod= 0;

      for(j= 0; j < mDissMatrix.size2(); ++j)
      {
         if (i != j)
         {
            Temp= std::pow(mDissAgent.Diss(*Pit++, mSamples[mDiscardIndex]), mP);
         }
         else
         {
            Temp= 0;
            Pit++;
         }

         (*Sit++)+= ( Temp - mDissMatrix(i, j) );
         Sod+= Temp;
         mDissMatrix(i, j)= Temp;
         //mMembershipValues[j]= mMembershipAgent.Eval(Temp);
      }

      mSods[ boost::numeric::converter<std::vector<RealType>::size_type, SampleSizeType>
             ::convert(mDiscardIndex) ]= Sod;

   }

   // Aggiorno  MinSodIndex.
   Sit= std::min_element(
           mSods.begin(),
           mSods.end() );

   mMinSodIndex= boost::numeric::converter<SampleSizeType, SodDiffType>::convert(
                                                                         std::distance(
                                                                            mSods.begin(),
                                                                            Sit) );

   //membership minsod
   mMembershipValues[mMinSodIndex]= 1.;
   //update samples mvs
   for(unsigned int i=0;i<mSamples.size();i++)
       mMembershipValues[i]= mMembershipAgent.Eval(mDissMatrix(mMinSodIndex, i));


   // Aggiorno  WorstIndex, il peggiore &egrave; il più distante dal rappresentante tra due scelti
   // a caso.
   k= boost::numeric::converter<BoostRealSymmMatrix::size_type, SampleSizeType>
      ::convert(mMinSodIndex);

   BoostRealSymmMatrix::size_type sampleSize=boost::numeric::converter<BoostRealSymmMatrix::size_type, SampleSizeType>
            ::convert( mSamples.size() );

   i= mSampleDist(mRng)%sampleSize;
   j= mSampleDist(mRng)%sampleSize;

      if(i==k&&j==k)
          i=(k+1)%sampleSize;

   if ( mDissMatrix(k, j) > mDissMatrix(k, i) )
   {
      i= j;
   }

   mDiscardIndex= boost::numeric::converter<SampleSizeType, BoostRealSymmMatrix::size_type>
                  ::convert(i);

   mCount++;
}  // Update

////////////////////////////////////// PRIVATE /////////////////////////////////////////////

// Funzione Init()
template <typename SampleType, typename Dissimilarity, typename Evaluator>
void
FuzzyMinSod<SampleType, Dissimilarity, Evaluator>::Init(NaturalType aM)
{
   // Variabili.
   std::vector<RealType>::size_type M_;

   mM= aM;
   mCount= 0;
   M_= boost::numeric::converter<std::vector<RealType>::size_type, NaturalType>
       ::convert(mM);

   // M dev'essere almeno 1.
   if (mM < 1)
   {
      throw SpareLogicError("FuzzyMinSod, 3, M cannot be less than 1.");
   }

   // I vector ce la possono fare ?
   if ( ( boost::numeric::converter<SampleSizeType, NaturalType>
          ::convert(mM) > mSamples.max_size() ) || ( M_ > mSods.max_size() ) )
   {
      throw SpareLogicError("FuzzyMinSod, 4, Too large M value.");
   }

   // Alloco tutto lo spazio che mi può servire (tecnica subottima), su mSamples e mSods
   // uso reserve, così mSamples.size() mi conta quanti campioni ho dentro. Su mDissMatrix
   // faccio direttamente resize, perché reserve non c'è...

   mSamples.reserve(
      boost::numeric::converter<SampleSizeType, NaturalType>::convert(mM) );

   mSods.reserve(M_);

   mMembershipValues.reserve(M_);

   mDissMatrix.resize(
      boost::numeric::converter<BoostRealSymmMatrix::size_type, NaturalType>::convert(mM) );

   // Solo per non lasciarli non inizializzati...
   mMinSodIndex= 0;
   mDiscardIndex= 0;
}  // Init

}  // namespace spare

#endif  // _FuzzyMinSod_h_
