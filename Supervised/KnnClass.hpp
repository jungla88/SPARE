//  KnnClass class, part of the SPARE library.
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

/** @brief File KnnClass.hpp, containing the template class KnnClass.
 *
 * The file contains the template class KnnClass, implementing a K-nn classifier.
 *
 * @file KnnClass.hpp
 * @author Guido Del Vescovo
 */

#ifndef _KnnClass_h_
#define _KnnClass_h_

// STD INCLUDES
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <utility>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/nvp.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>
#include <spare/SwitchParameter.hpp>

namespace spare {  // Inclusion in namespace spare.

// Data for switch parameter construction.
static const std::string KNNC_NSVAL[]= {"Off", "On"};
static const size_t      KNNC_NSVAL_SZ= 2;

/** @brief K-nn classifier.
 *
 * This class models the @a Supervised concept. It is a K-nn classifier with custom
 * sample type, label type and dissimilarity measure. The template arguments SampleType and
 * LabelType can be any type provided with the basic operators. The chosen dissimilarity 
 * agent must accept the SampleType as argument to the Diss method.
 *
 * <b>Parameter summary</b>
 *  <table class="contents">
 *  <tr>
 *     <td class="indexkey"><b>Name</b></td>
 *     <td class="indexkey"><b>Domain</b></td>
 *     <td class="indexkey"><b>Description</b></td>
 *     <td class="indexkey"><b>Const</b></td>
 *     <td class="indexkey"><b>Default</b></td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">K</td>
 *     <td class="indexvalue">[1, inf)</td>
 *     <td class="indexvalue">Number of nearest neighbors used for classification.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">5</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Incremental</td>
 *     <td class="indexvalue">{On, Off}</td>
 *     <td class="indexvalue">On Learn method call, the previously stored samples are kept if 
 *                            set to "On", deleted otherwise.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">Off</td>
 *  </tr>
 *  </table>  
 * 
 * @todo Add ClearModel method.
 */
template <typename SampleType, typename Dissimilarity, typename LabelType>
class KnnClass
{
public:

// PUBLIC TYPES

   /** Container type for samples.
    */
   typedef std::list<SampleType>
                        SampleList;

   /** Container type for labels.
    */
   typedef std::list<LabelType>
                        LabelList;

   //Extra information about classification.
   //This struct contains two values related to the classification performed on an incoming sample.
   struct ExtraInfoStruct
   {
      /** An estimation of the classification reliability, in the range [0, 1].
       */
      RealType          Reliability;

      /** Minimum dissimilarity between the stored samples and the one under classification.
       */
      RealType          MinDiss;
   };

   /** Integer parameter.
    */
   typedef BoundedParameter<NaturalType>
                        NaturalParam;

   /** Switch parameter.
    */
   typedef SwitchParameter<std::string>
                        StringParam;

// LIFECYCLE

   /** Default constructor.
    */
   KnnClass()
      : mK( 1, std::numeric_limits<NaturalType>::max() ),
        mIncremental(KNNC_NSVAL,
                     KNNC_NSVAL + KNNC_NSVAL_SZ)
                                                   { 
                                                      mK= 5;
                                                      mIncremental= "Off"; 
                                                   }

// OPERATIONS

   /** Learning of a single training sample.
    *
    * @param[in] rSample The input sample.
    * @param[in] rLabel The input label.
    */
   void                 Learn(
                           const SampleType&   rSample,
                           const LabelType&    rLabel)
                           {
                              mSamples.push_back(rSample);
                              mLabels.push_back(rLabel);
                           }

   /** Learning of a batch of training samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first input sample.
    * @param[in] iSampleEnd Iterator pointing to one position after the last input sample.
    * @param[in] iLabelBegin Iterator pointing to the first input label.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Learn(
                           ForwardIterator1      iSampleBegin,
                           ForwardIterator1      iSampleEnd,
                           ForwardIterator2      iLabelBegin);

   /** Classification of a single sample.
    *
    * @param[in] rSample The input sample.
    * @param[out] rLabel The output label.
    */
   void                 Process(
                           const SampleType&   rSample,
                           LabelType&          rLabel) const;

   /** Classification of a single sample with additional information.
    *
    * @param[in] rSample The input sample.
    * @param[out] rLabel The output label.
    * @param[out] rExtraInfo Structure receiving the extra information.
    */
   void                 Process(
                           const SampleType&   rSample,
                           LabelType&          rLabel,
                           ExtraInfoStruct&    rExtraInfo) const;

   /** Classification of a batch of samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first input sample.
    * @param[in] iSampleEnd Iterator pointing to one position after the last input sample.
    * @param[out] iLabelBegin Iterator pointing to the first output label.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Process(
                           ForwardIterator1    iSampleBegin,
                           ForwardIterator1    iSampleEnd,
                           ForwardIterator2    iLabelBegin) const;

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
                           ForwardIterator1    iSampleBegin,
                           ForwardIterator1    iSampleEnd,
                           ForwardIterator2    iLabelBegin,
                           ForwardIterator3    iExtraInfoBegin) const;

// ACCESS

   /** Read/write access to the K parameter.
    *
    * @return A reference to the value of K.
    */
   NaturalParam&        K()                        { return mK; }

   /** Read only access to the K parameter.
    *
    * @return A const reference to the value of K.
    */
   const NaturalParam&  K() const                  { return mK; }

   /** Read/write access to the Incremental parameter.
    *
    * @return A reference to the value of Incremental.
    */
   StringParam&         Incremental()              { return mIncremental; }

   /** Read only access to the Incremental parameter.
    *
    * @return A const reference to the value of Incremental.
    */
   const StringParam&   Incremental() const        { return mIncremental; }

   /** Read/write access to the dissimilarity agent.
    *
    * @return A reference to the dissimilarity agent.
    */
   Dissimilarity&       DissAgent()                { return mDissAgent; }

   /** Read only access to the dissimilarity agent.
    *
    * @return A const reference to the dissimilarity agent.
    */
   const Dissimilarity& DissAgent() const          { return mDissAgent; }

   /** Read access to the stored samples.
    *
    * @return A const reference to the container of samples.
    */
   const SampleList&    GetSamples() const         { return mSamples; }

   /** Read access to the stored labels.
    *
    * @return A const reference to the container of labels.
    */
   const LabelList&     GetLabels() const          { return mLabels; }

private:

   // Typedef privati.
   typedef std::multiset<std::pair<RealType, LabelType> >
                        DissLabelPairSet;

   typedef typename DissLabelPairSet::size_type
                        DissLabelPairSetSizeType;

   typedef typename DissLabelPairSet::const_iterator
                        DissLabelPairSetIterator;

   typedef std::map<LabelType, NaturalType>
                        LabelCountMap;

   typedef typename LabelCountMap::const_iterator
                        LabelCountMapIterator;

   typedef typename SampleList::const_iterator
                        SampleIterator;

   typedef typename LabelList::const_iterator
                        LabelIterator;

   // Valore K (numero di vicini usati per l'inferenza).
   NaturalParam         mK;

   // Switch modalità incrementale.
   StringParam          mIncremental;

   // Istanza classe misuratrice di dissimilarità.
   Dissimilarity        mDissAgent;

   // Campioni immagazzinati.
   SampleList           mSamples;

   // Etichette immagazzinate.
   LabelList            mLabels;

   // Set coppie (dissimilarit&agrave;, etichetta), per uso interno alle funzioni.
   mutable DissLabelPairSet
                        mDlSet;

   // Conteggio etichette, per uso interno alle funzioni.
   mutable LabelCountMap
                        mLcMap;

   // Funzione trova vicini.
   void                 FindNeighbors(const SampleType& rSample) const;

   // Funzione classificazione.
   void                 Classification(LabelType& rLabel) const;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mK);
      ar & BOOST_SERIALIZATION_NVP(mIncremental);
      ar & BOOST_SERIALIZATION_NVP(mDissAgent);
      ar & BOOST_SERIALIZATION_NVP(mSamples);
      ar & BOOST_SERIALIZATION_NVP(mLabels);
   } // BOOST SERIALIZATION

}; // class KnnClass

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename SampleType, typename Dissimilarity, typename LabelType>
template <typename ForwardIterator1, typename ForwardIterator2>
void
KnnClass<SampleType, Dissimilarity, LabelType>::Learn(
                                                  ForwardIterator1  iSampleBegin,
                                                  ForwardIterator1  iSampleEnd,
                                                  ForwardIterator2  iLabelBegin)
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("KnnClass, 0, Invalid sample iterators.");
   }

   if (mIncremental == "Off")
   {
      mSamples.clear();
      mLabels.clear();
   }

   while (iSampleBegin != iSampleEnd)
   {
      mSamples.push_back(*iSampleBegin++);
      mLabels.push_back(*iLabelBegin++);
   }
}  // Learn

template <typename SampleType, typename Dissimilarity, typename LabelType>
void
KnnClass<SampleType, Dissimilarity, LabelType>::Process(
                                                    const SampleType& rSample,
                                                    LabelType&        rLabel) const
{
   FindNeighbors(rSample);
   Classification(rLabel);
}  // Process

template <typename SampleType, typename Dissimilarity, typename LabelType>
void
KnnClass<SampleType, Dissimilarity, LabelType>::Process(
                                                    const SampleType& rSample,
                                                    LabelType&        rLabel,
                                                    ExtraInfoStruct&  rExtraInfo) const
{
   LabelCountMapIterator Mit;
   NaturalType           WinnerNum, SecondNum;
   LabelType             Winner;
   bool                  First;

   FindNeighbors(rSample);
   Classification(rLabel);

   // Calcolo affidabilità.
   if (mLcMap.size() == 1)
   {
      rExtraInfo.Reliability= RealType(1.);
   }
   else
   {
      Mit= mLcMap.begin();
      WinnerNum= Mit->second;
      Winner= (*Mit++).first;
      while (mLcMap.end() != Mit)
      {
         if (Mit->second > WinnerNum)
         {
            WinnerNum= Mit->second;
            Winner= (*Mit++).first;
         }
         else
         {
            ++Mit;
         }
      }
      Mit= mLcMap.begin();
      First= true;
      while (mLcMap.end() != Mit)
      {
         if (Mit->first != Winner)
         {
            if (First)
            {
               SecondNum= Mit->second;
               First= false;
            }
            else
            {
               if (Mit->second > SecondNum)
               {
                  SecondNum= Mit->second;
               }
            }
         }
         ++Mit;
      }
      rExtraInfo.Reliability= RealType(1.) - RealType(SecondNum) / RealType(WinnerNum);
   }

   // Calcolo dissimilarità minima.
   rExtraInfo.MinDiss= mDlSet.begin()->first;

}  // Process

template <typename SampleType, typename Dissimilarity, typename LabelType>
template <typename ForwardIterator1, typename ForwardIterator2>
void
KnnClass<SampleType, Dissimilarity, LabelType>::Process(
                                                    ForwardIterator1  iSampleBegin,
                                                    ForwardIterator1  iSampleEnd,
                                                    ForwardIterator2  iLabelBegin) const
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("KnnClass, 1, Invalid sample iterators.");
   }

   while (iSampleBegin != iSampleEnd)
   {
      Process(
         *iSampleBegin++,
         *iLabelBegin++);
   }
}  // Process

template <typename SampleType, typename Dissimilarity, typename LabelType>
template <typename ForwardIterator1, typename ForwardIterator2, typename ForwardIterator3>
void
KnnClass<SampleType, Dissimilarity, LabelType>::Process(
                                                    ForwardIterator1  iSampleBegin,
                                                    ForwardIterator1  iSampleEnd,
                                                    ForwardIterator2  iLabelBegin,
                                                    ForwardIterator3  iExtraInfoBegin) const
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("KnnClass, 1, Invalid sample iterators.");
   }

   while (iSampleBegin != iSampleEnd)
   {
      Process(
         *iSampleBegin++,
         *iLabelBegin++,
         *iExtraInfoBegin++);
   }
}  // Process

////////////////////////////////////// PRIVATE /////////////////////////////////////////////

template <typename SampleType, typename Dissimilarity, typename LabelType>
void
KnnClass<SampleType, Dissimilarity, LabelType>
::FindNeighbors(const SampleType& rSample) const
{
   // Variabili.
   SampleIterator                Sit;
   LabelIterator                 Lit;
   RealType                      DissBuff;
   DissLabelPairSetSizeType      K_;

   // Controllo se ho qualcosa nella base-esempi.
   if ( mSamples.empty() )
   {
      throw SpareLogicError("KnnClass, 2, No knowledge.");
   }

   // Inizializzo.
   mDlSet.clear();
   Sit= mSamples.begin();
   Lit= mLabels.begin();
   K_= boost::numeric::converter<DissLabelPairSetSizeType, NaturalType>::convert(mK);

   // Primo elemento.
   DissBuff= mDissAgent.Diss(rSample, *Sit++);
   mDlSet.insert( std::make_pair(DissBuff, *Lit++) );

   // Ciclo principale.
   while (mSamples.end() != Sit)
   {
      DissBuff= mDissAgent.Diss(rSample, *Sit++);

      if (mDlSet.size() < K_)
      {
         mDlSet.insert( std::make_pair(DissBuff, *Lit++) );
      }
      else
      {
         if (mDlSet.rbegin()->first >= DissBuff)
         {
            mDlSet.erase( --mDlSet.end() );
            mDlSet.insert( std::make_pair(DissBuff, *Lit++) );
         }
         else
         {
            Lit++;
         }
      }
   }
}  // FindNeighbors

template <typename SampleType, typename Dissimilarity, typename LabelType>
void
KnnClass<SampleType, Dissimilarity, LabelType>::Classification(LabelType& rLabel) const
{
   // Variabili.
   DissLabelPairSetIterator   Sit;
   LabelCountMapIterator      Mit;
   bool                       HaveWinner; // Flag per vedere se la classe è determinata.
   NaturalType                WinnerNum;
   LabelType                  Winner;

   // Conto numero vicini per le varie classi.
   mLcMap.clear();
   Sit= mDlSet.begin();
   while (mDlSet.end() != Sit)
   {
      mLcMap[ (*Sit++).second ]++;
   }

   // Cerco eventuale classe vincitrice.
   Mit= mLcMap.begin();
   WinnerNum= Mit->second;
   Winner= (*Mit++).first;
   HaveWinner= true;
   while (mLcMap.end() != Mit)
   {
      if (Mit->second > WinnerNum)
      {
         WinnerNum= Mit->second;
         Winner= (*Mit++).first;
         HaveWinner= true;
      }
      else
      {
         if ( (Mit->second == WinnerNum) && (Mit->first != Winner) )
         {
            Mit++;
            HaveWinner= false;
         }
         else
         {
            Mit++;
         }
      }
   }

   // Eventuale Output.
   if (HaveWinner)
   {
      rLabel= Winner;
   }
}  // Classification

}  // namespace spare

#endif  // _KnnClass_h_
