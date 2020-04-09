//  Scbc class, part of the SPARE library.
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

/** @brief File Scbc.hpp, containing the Scbc class.
 *
 * The file contains the Scbc class, implementing a generic clustering based supervised
 * classification system.
 *
 * @file Scbc.hpp
 * @author Guido Del Vescovo
 */
#ifndef _Scbc_h_
#define _Scbc_h_

// STD INCLUDES
#include <iterator>
#include <map>
#include <vector>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief Supervised clustering based classifier.
 *
 * This class models the @a Supervised concept. It is a supervised classification system.
 * The training algorithm first performs an unsupervised clustering step, then the input
 * label information is used to evaluate cluster purity and to label clusters based on the
 * prevalent class. In the test stage, patterns are classified by comparison with the stored
 * clusters.
 */
template <typename Clustering, typename LabelType>
class Scbc
{
public:

// PUBLIC TYPES

   /** Class count map.
    */
   typedef std::map<LabelType, NaturalType>
                        ClassCountMap;

   /** Class count map vector.
    */
   typedef std::vector<ClassCountMap>
                        ClassCountMapVector;

   /** Purity vector type.
    */
   typedef std::vector<RealType>
                        PurityVector;

   /** Label vector type.
    */
   typedef std::vector<LabelType>
                        LabelVector;

   /**
    * Type of the clustering labels
    */
   typedef std::vector<typename Clustering::LabelType> ClusteringLabelsType;

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

   /** Learning of a batch of training samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first input sample.
    * @param[in] iSampleEnd Iterator pointing to one position after the last input sample.
    * @param[in] iLabelBegin Iterator pointing to the first input label.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Learn(
                           ForwardIterator1 iSampleBegin,
                           ForwardIterator1 iSampleEnd,
                           ForwardIterator2 iLabelBegin);

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

   /** Read/write access to the clustering agent.
    *
    * @return A reference to the clustering agent.
    */
   Clustering&          ClustAgent()               { return mClustAgent; }

   /** Read only access to the clustering agent.
    *
    * @return A reference to the clustering agent.
    */
   const Clustering&    ClustAgent() const         { return mClustAgent; }

   /** Read only access to the class count map vector.
    *
    * @return A reference to the class count map vector.
    */
   const ClassCountMapVector&
                        GetClassCounts() const     { return mClassCounts; }

   /** Read only access to the cluster label vector.
    *
    * @return A reference to the cluster label vector.
    */
   const LabelVector&   GetLabels() const          { return mLabels; }

   /** Read only access to the cluster purity vector.
    *
    * @return A reference to the cluster purity vector.
    */
   const PurityVector&  GetPurities() const        { return mPurities; }

   /** Read only access to the internal clustering labels.
    *
    * @return A reference to the internal clustering labels.
    */
   const ClusteringLabelsType& GetClusteringLabels() const {return mClusteringLabels;}

private:

   // Clustering agent instance.
   Clustering           mClustAgent;

   // Class count per cluster.
   ClassCountMapVector  mClassCounts;

   // Supervised cluster labels.
   LabelVector          mLabels;

   // Cluster purities.
   PurityVector         mPurities;

   // Labels of the internal clustering algorithm
   ClusteringLabelsType mClusteringLabels;

   // Cluster labelling and purity evaluation. The iterators iPartitionBegin and
   // iPartitionEnd delimit the label batch generated by the clustering process, while
   // iSupervisedLabelBegins points to the beginning of the supervised input labels.
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 ClusterLabelling(
                                     ForwardIterator1 iPartitionBegin,
                                     ForwardIterator1 iPartitionEnd,
                                     ForwardIterator2 iSupervisedLabelBegin);

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & mClustAgent;
      ar & mClassCounts;
      ar & mLabels;
      ar & mPurities;
   } // BOOST SERIALIZATION

}; // class Scbc

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Clustering, typename LabelType>
template <typename ForwardIterator1, typename ForwardIterator2>
void
Scbc<Clustering, LabelType>::Learn(
                               ForwardIterator1 iSampleBegin,
                               ForwardIterator1 iSampleEnd,
                               ForwardIterator2 iLabelBegin)
{
   // Local typedef
   typedef typename std::iterator_traits<ForwardIterator1>::difference_type SampleDiffType;

   SampleDiffType N;

   N= std::distance(iSampleBegin, iSampleEnd);
   if (N < 0)
   {
      throw SpareLogicError("Scbc, 0, Invalid sample iterators.");
   }
   mClusteringLabels.resize(N);

   mClustAgent.Process(
                   iSampleBegin,
                   iSampleEnd,
                   mClusteringLabels.begin());

   ClusterLabelling(
                mClusteringLabels.begin(),
                mClusteringLabels.end(),
                iLabelBegin);
}

template <typename Clustering, typename LabelType>
template <typename SampleType>
void
Scbc<Clustering, LabelType>::Process(
                                 const SampleType& rSample,
                                 LabelType&        rLabel,
                                 ExtraInfoStruct&  rExtraInfo) const
{
   typename Clustering::RepVector::size_type i, Nearest;
   RealType                                  NearestDiss, CurrDiss;

   if (mClustAgent.GetRepresentatives().empty())
   {
      throw SpareLogicError("Scbc, 1, Uninitialized object.");
   }

   Nearest= 0;
   NearestDiss= mClustAgent.GetRepresentatives()[0].Diss(rSample);
   for (i= 1; i < mClustAgent.GetRepresentatives().size(); ++i)
   {
      CurrDiss= mClustAgent.GetRepresentatives()[i].Diss(rSample);
      if (CurrDiss < NearestDiss)
      {
         NearestDiss= CurrDiss;
         Nearest= i;
      }
   }

   rLabel= mLabels[Nearest];
   rExtraInfo.MinDiss= NearestDiss;
   rExtraInfo.Reliability= mPurities[Nearest];
}

template <typename Clustering, typename LabelType>
template <typename SampleType>
void
Scbc<Clustering, LabelType>::Process(
                                 const SampleType& rSample,
                                 LabelType&        rLabel) const
{
   typename Clustering::RepVector::size_type i, Nearest;
   RealType                                  NearestDiss, CurrDiss;

   if (mClustAgent.GetRepresentatives().empty())
   {
      throw SpareLogicError("Scbc, 2, Uninitialized object.");
   }

   Nearest= 0;
   NearestDiss= mClustAgent.GetRepresentatives()[0].Diss(rSample);
   for (i= 1; i < mClustAgent.GetRepresentatives().size(); ++i)
   {
      CurrDiss= mClustAgent.GetRepresentatives()[i].Diss(rSample);
      if (CurrDiss < NearestDiss)
      {
         NearestDiss= CurrDiss;
         Nearest= i;
      }
   }

   rLabel= mLabels[Nearest];
}

template <typename Clustering, typename LabelType>
template <typename ForwardIterator1, typename ForwardIterator2>
void
Scbc<Clustering, LabelType>::Process(
                                 ForwardIterator1 iSampleBegin,
                                 ForwardIterator1 iSampleEnd,
                                 ForwardIterator2 iLabelBegin) const
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("Scbc, 3, Invalid sample iterators.");
   }

   while (iSampleBegin != iSampleEnd)
   {
      Process(*iSampleBegin++, *iLabelBegin++);
   }
}  // Process

template <typename Clustering, typename LabelType>
template <typename ForwardIterator1, typename ForwardIterator2, typename ForwardIterator3>
void
Scbc<Clustering, LabelType>::Process(
                                 ForwardIterator1 iSampleBegin,
                                 ForwardIterator1 iSampleEnd,
                                 ForwardIterator2 iLabelBegin,
                                 ForwardIterator3 iExtraInfoBegin) const
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("Scbc, 4, Invalid sample iterators.");
   }

   while (iSampleBegin != iSampleEnd)
   {
      Process(*iSampleBegin++, *iLabelBegin++, *iExtraInfoBegin++);
   }
}  // Process

////////////////////////////////////// PRIVATE /////////////////////////////////////////////

template <typename Clustering, typename LabelType>
template <typename ForwardIterator1, typename ForwardIterator2>
void
Scbc<Clustering, LabelType>::ClusterLabelling(
                                          ForwardIterator1 iPartitionBegin,
                                          ForwardIterator1 iPartitionEnd,
                                          ForwardIterator2 iSupervisedLabelBegin)
{
   // Local typedef
   typedef typename Clustering::RepVector::size_type   RepVectorSizeType;
   typedef typename Clustering::LabelType              ClusteringLabelType;
   typedef typename Clustering::LabelVector::size_type LabelVectorSizeType;

   // Declarations
   std::map<ClusteringLabelType, RepVectorSizeType> LabelToRepIndex;
   RepVectorSizeType                                i;

   // Clear/resize previous content.
   mClassCounts.clear();
   mClassCounts.resize(mClustAgent.GetRepresentatives().size()); // FIXME check max_size
   mLabels.resize(mClustAgent.GetRepresentatives().size()); // FIXME check max_size
   mPurities.resize(mClustAgent.GetRepresentatives().size()); // FIXME check max_size

   // Population of LabelToRepIndex
   i= 0;
   for (LabelVectorSizeType j= 0; j < mClustAgent.GetLabels().size(); ++j)
   {
      LabelToRepIndex[mClustAgent.GetLabels()[j]]= i++;
   }

   // Population of mClassCounts
   while (iPartitionBegin != iPartitionEnd)
   {
      ++mClassCounts[LabelToRepIndex[*iPartitionBegin++]][*iSupervisedLabelBegin++];
   }

   // Population of mLabels and mPurities
   for (i= 0; i < mClustAgent.GetRepresentatives().size(); ++i)
   {
      if (mClassCounts[i].size() < 1)
      {
         throw SpareLogicError("Scbc, 5, Unexpected empty cluster.");
      }

      if (mClassCounts[i].size() == 1)
      {
         mLabels[i]= mClassCounts[i].begin()->first;
         mPurities[i]= RealType(1);
      }
      else
      {
         LabelType First, Second, Curr;
         NaturalType FirstNum, SecondNum, CurrNum;
         typename ClassCountMap::const_iterator Mit= mClassCounts[i].begin();

         FirstNum= Mit->second;
         First= Mit->first;

         ++Mit;
         SecondNum= Mit->second;
         Second= Mit->first;

         if (SecondNum > FirstNum)
         {
            Curr= Second;
            CurrNum= SecondNum;

            Second= First;
            SecondNum= FirstNum;

            First= Curr;
            FirstNum= CurrNum;
         }

         ++Mit;
         while (mClassCounts[i].end() != Mit)
         {
            CurrNum= Mit->second;
            Curr= Mit->first;

            if (CurrNum > SecondNum)
            {
               if (CurrNum > FirstNum)
               {
                  Second= First;
                  SecondNum= FirstNum;

                  First= Curr;
                  FirstNum= CurrNum;
               }
               else
               {
                  Second= Curr;
                  SecondNum= CurrNum;
               }
            }

            ++Mit;
         }

         mLabels[i]= First;
         mPurities[i]= RealType(1) - RealType(SecondNum) / RealType(FirstNum);
      }
   }
}

}  // namespace spare

#endif  // _Scbc_h_
