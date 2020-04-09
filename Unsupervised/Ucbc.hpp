//  Ucbc class, part of the SPARE library.
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

/** @brief File Ucbc.hpp, containing the Ucbc class.
 *
 * The file contains the Ucbc class, implementing a generic clustering based unsupervised
 * classification system.
 *
 * @file Ucbc.hpp
 * @author Guido Del Vescovo
 */
#ifndef _Ucbc_h_
#define _Ucbc_h_

// STD INCLUDES
#include <iterator>
#include <limits>
#include <vector>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief Unsupervised clustering based classifier.
 *
 * This class models the @a Unsupervised concept. It is an unsupervised classification
 * system, that is, the algorithm itself is in charge of discovering the underlying classes.
 * In order to accomplish this task, the algorithm performs a custom clustering procedure
 * during the learning stage, generating and storing labeled clusters. In the subsequent test
 * stage the classifier assigns to each pattern the label associated with the nearest stored
 * cluster.
 */
template <typename Clustering>
class Ucbc
{
public:

// PUBLIC TYPES

   /** Label type.
    */
   typedef typename Clustering::LabelType
                        LabelType;

   /** @brief Extra information about classification.
    *
    * This struct contains two values related to the classification performed on an incoming
    * sample.
    */
   struct ExtraInfoStruct
   {
      /** An estimation of the classification reliability, in the range [0, 1].
       */
      RealType          Reliability;

      /** Minimum dissimilarity between stored clusters and the sample under classification.
       */
      RealType          MinDiss;
   };

// OPERATIONS

   /** Learning of a batch of samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first sample.
    * @param[in] iSampleEnd Iterator pointing to the first position after the last sample.
    */
   template <typename ForwardIterator>
   void                 Learn(
                           ForwardIterator iSampleBegin,
                           ForwardIterator iSampleEnd);

   /** Classification of a single sample.
    *
    * @param[in] rSample The input sample.
    * @param[out] rLabel The output label.
    */
   template <typename SampleType>
   void                 Process(
                           const SampleType& rSample,
                           LabelType&        rLabel) const;

   /** Classification of a single sample with additional information.
    *
    * @param[in] rSample The input sample.
    * @param[out] rLabel The output label.
    * @param[out] rExtraInfo Structure receiving the extra information.
    */
   template <typename SampleType>
   void                 Process(
                           const SampleType& rSample,
                           LabelType&        rLabel,
                           ExtraInfoStruct&  rExtraInfo) const;

   /** Classification of a batch of samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first sample.
    * @param[in] iSampleEnd Iterator pointing to the first position after the last sample.
    * @param[out] iLabelBegin Iterator pointing to the first output label.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Process(
                           ForwardIterator1 iSampleBegin,
                           ForwardIterator1 iSampleEnd,
                           ForwardIterator2 iLabelBegin) const;

   /** Classification of a batch of samples with additional information.
    *
    * @param[in] iSampleBegin Iterator pointing to the first input sample.
    * @param[in] iSampleEnd Iterator pointing to one position after the last input sample.
    * @param[out] iLabelBegin Iterator pointing the first output label.
    * @param[out] iExtraInfoBegin Iterator pointing to the first structure for extra info.
    */
   template <typename ForwardIterator1, typename ForwardIterator2,
             typename ForwardIterator3>
   void                 Process(
                           ForwardIterator1 iSampleBegin,
                           ForwardIterator1 iSampleEnd,
                           ForwardIterator2 iLabelBegin,
                           ForwardIterator3 iExtraInfoBegin) const;

// ACCESS

   /** Read/Write access to the clustering agent.
    *
    * @return A reference to the clustering agent.
    */
   Clustering&          ClustAgent()               { return mClustAgent; }

   /** Read access to the clustering agent.
    *
    * @return A reference to the clustering agent.
    */
   const Clustering&    ClustAgent() const         { return mClustAgent; }

private:

   // Clustering agent instance.
   Clustering           mClustAgent;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & mClustAgent;
   } // BOOST SERIALIZATION

}; // class Ucbc

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Clustering>
template <typename ForwardIterator>
void
Ucbc<Clustering>::Learn(
                     ForwardIterator iSampleBegin,
                     ForwardIterator iSampleEnd)
{
   // Local typedef
   typedef typename std::iterator_traits<ForwardIterator>::difference_type
                        SampleDiffType;

   SampleDiffType                              N;
   std::vector<typename Clustering::LabelType> Labels;

   N= std::distance(iSampleBegin, iSampleEnd);
   if (N < 0)
   {
      throw SpareLogicError("Ucbc, 0, Invalid sample iterators.");
   }
   Labels.resize(N);

   mClustAgent.Process(
                   iSampleBegin,
                   iSampleEnd,
                   Labels.begin());
}

template <typename Clustering>
template <typename SampleType>
void
Ucbc<Clustering>::Process(
                     const SampleType& rSample,
                     LabelType&        rLabel,
                     ExtraInfoStruct&  rExtraInfo) const

{
   // Local typedef
   typedef typename Clustering::RepVector::size_type RepVectorSizeType;

   if (mClustAgent.GetRepresentatives().size() < 1)
   {
      return;
   }

   if (mClustAgent.GetRepresentatives().size() == 1)
   {
      rLabel= mClustAgent.GetLabels()[0];
      return;
   }

   LabelType First, Second, Curr;
   RealType FirstDiss, SecondDiss, CurrDiss;

   FirstDiss= mClustAgent.GetRepresentatives()[0].Diss(rSample);
   First= mClustAgent.GetLabels()[0];

   SecondDiss= mClustAgent.GetRepresentatives()[1].Diss(rSample);
   Second= mClustAgent.GetLabels()[1];

   if (SecondDiss < FirstDiss)
   {
      Curr= Second;
      CurrDiss= SecondDiss;

      Second= First;
      SecondDiss= FirstDiss;

      First= Curr;
      FirstDiss= CurrDiss;
   }

   for (RepVectorSizeType i= 2; i < mClustAgent.GetRepresentatives().size(); ++i)
   {
      CurrDiss= mClustAgent.GetRepresentatives()[i].Diss(rSample);
      Curr= mClustAgent.GetLabels()[i];

      if (CurrDiss < SecondDiss)
      {
         if (CurrDiss < FirstDiss)
         {
            Second= First;
            SecondDiss= FirstDiss;

            First= Curr;
            FirstDiss= CurrDiss;
         }
         else
         {
            Second= Curr;
            SecondDiss= CurrDiss;
         }
      }
   }

   rLabel= First;
   rExtraInfo.MinDiss= FirstDiss;
   if (SecondDiss > std::numeric_limits<RealType>::epsilon())
   {
      rExtraInfo.Reliability= RealType(1) - FirstDiss / SecondDiss;
   }
   else
   {
      rExtraInfo.Reliability= RealType(0);
   }
}

template <typename Clustering>
template <typename SampleType>
void
Ucbc<Clustering>::Process(
                     const SampleType& rSample,
                     LabelType&        rLabel) const

{
   // Local typedef
   typedef typename Clustering::RepVector::size_type RepVectorSizeType;

   if (mClustAgent.GetRepresentatives().size() < 1)
   {
      return;
   }

   LabelType First, Curr;
   RealType FirstDiss, CurrDiss;

   FirstDiss= mClustAgent.GetRepresentatives()[0].Diss(rSample);
   First= mClustAgent.GetLabels()[0];

   for (RepVectorSizeType i= 1; i < mClustAgent.GetRepresentatives().size(); ++i)
   {
      CurrDiss= mClustAgent.GetRepresentatives()[i].Diss(rSample);
      Curr= mClustAgent.GetLabels()[i];

      if (CurrDiss < FirstDiss)
      {
         First= Curr;
         FirstDiss= CurrDiss;
      }
   }

   rLabel= First;
}

template <typename Clustering>
template <typename ForwardIterator1, typename ForwardIterator2>
void
Ucbc<Clustering>::Process(
                     ForwardIterator1 iSampleBegin,
                     ForwardIterator1 iSampleEnd,
                     ForwardIterator2 iLabelBegin) const
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("Ucbc, 1, Invalid sample iterators.");
   }

   while (iSampleBegin != iSampleEnd)
   {
      Process(*iSampleBegin++, *iLabelBegin++);
   }
}  // Process

template <typename Clustering>
template <typename ForwardIterator1, typename ForwardIterator2, typename ForwardIterator3>
void
Ucbc<Clustering>::Process(
                     ForwardIterator1 iSampleBegin,
                     ForwardIterator1 iSampleEnd,
                     ForwardIterator2 iLabelBegin,
                     ForwardIterator3 iExtraInfoBegin) const
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("Ucbc, 2, Invalid sample iterators.");
   }

   while (iSampleBegin != iSampleEnd)
   {
      Process(*iSampleBegin++, *iLabelBegin++, *iExtraInfoBegin++);
   }
}  // Process

}  // namespace spare

#endif  // _Ucbc_h_
