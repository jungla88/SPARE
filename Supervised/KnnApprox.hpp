//  KnnApprox class, part of the SPARE library.
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

/** @brief File KnnApprox.hpp, that contains the KnnApprox.
 *
 * Contains the declaration of the KnnApprox class.
 *
 * @file KnnApprox.hpp
 * @author Guido Del Vescovo
 */

#ifndef _KnnApprox_h_
#define _KnnApprox_h_

// STD INCLUDES
#include <iterator>
#include <limits>
#include <list>
#include <set>
#include <utility>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/list.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>
#include <spare/SwitchParameter.hpp>

namespace spare {  // Inclusione in namespace spare.

// Dati per costruzione parametro switch.
static const std::string KNNA_NSVAL[]= {"Off", "On"};
static const size_t      KNNA_NSVAL_SZ= 2;

/** @brief Knn approximator.
 *
 * This class models the @a Supervised concept. It is a version of the well known K-nn 
 * algorithm adapted for function approximation.
 * @todo Aggiunta parametro Incremental.
 * @todo Aggiunta metodo ClearModel.
 */
template <typename SampleType,typename Dissimilarity,typename LabelType,typename Evaluator>
class KnnApprox
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

   /** Integer param.
    */
   typedef BoundedParameter<NaturalType>
                        NaturalParam;

   /** Switch param.
    */
   typedef SwitchParameter<std::string>
                        StringParam;

// LIFECYCLE

   /** Default constructor.
    */
   KnnApprox()
      : mK( 1, std::numeric_limits<NaturalType>::max() ),
        mIncremental(KNNA_NSVAL,
                     KNNA_NSVAL + KNNA_NSVAL_SZ)
                           {
                              mK= 5;
                           }

// OPERATIONS

   /** Learning for a single sample.
    *
    * @param[in] rSample A reference to the sample.
    * @param[in] rLabel A reference to the label.
    */
   void                 Learn(
                           const SampleType&   rSample,
                           const LabelType&    rLabel)
                           {
                              mSamples.push_back(rSample);
                              mLabels.push_back(rLabel);
                           }

   /** Learning of a batch of samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first sample.
    * @param[in] iSampleEnd Iterator pointing to the first position after the last sample.
    * @param[in] iLabelBegin Iterator pointing to the first label.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Learn(
                           ForwardIterator1      iSampleBegin,
                           ForwardIterator1      iSampleEnd,
                           ForwardIterator2      iLabelBegin);

   /** Classification for a single sample.
    *
    * @param[in] rSample Reference to the sample.
    * @param[out] rLabel Reference to the label.
    */
   void                 Process(
                           const SampleType&   rSample,
                           LabelType&          rLabel) const;

   /** Classify a batch of samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first sample.
    * @param[in] iSampleEnd Iterator pointing to the first position after the last sample.
    * @param[out] iLabelBegin Iterator pointing to the first label.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Process(
                           ForwardIterator1    iSampleBegin,
                           ForwardIterator1    iSampleEnd,
                           ForwardIterator2    iLabelBegin) const;

// ACCESS

   /** Read/Write access to the value of K.
    *
    * @return A reference to the value of K.
    */
   NaturalParam&        K()                        { return mK; }

   /** Read access to the value of K.
    *
    * @return A reference to the value of K.
    */
   const NaturalParam&  K() const                  { return mK; }

   /** Read/Write access to Incremental, a switch for the incremental modality.
    *
    * @return A reference to the value of Incremental.
    */
   StringParam&         Incremental()              { return mIncremental; }

   /** Read access to Incremental, a switch for the incremental modality.
    *
    * @return A reference to the value of Incremental.
    */
   const StringParam&   Incremental() const        { return mIncremental; }

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

   /** Read/Write access to the weight function.
    *
    * @return A reference the weight function.
    */
   Evaluator&           WeightAgent()              { return mWeightAgent; }

   /** Read access to the weight function.
    *
    * @return A reference to the weight function.
    */
   const Evaluator&     WeightAgent() const        { return mWeightAgent; }

   /** Read access to the stored samples.
    *
    * @return A reference to the container of samples.
    */
   const SampleList&    GetSamples() const         { return mSamples; }

   /** Read access to the stored labels.
    *
    * @return A reference to the container of labels.
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

   // Istanza classe funzione peso.
   Evaluator            mWeightAgent;

   // Campioni immagazzinati.
   SampleList           mSamples;

   // Etichette immagazzinate.
   LabelList            mLabels;

   // Set coppie (dissimilarità, etichetta), per uso interno alle funzioni.
   mutable DissLabelPairSet
                        mDlSet;

   // Funzione trova vicini.
   void                 FindNeighbors(const SampleType& rSample) const;

   // Funzione regressione.
   void                 DissWeightedRegression(LabelType& rLabel) const;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & mK;
      ar & mIncremental;
      ar & mDissAgent;
      ar & mWeightAgent;
      ar & mSamples;
      ar & mLabels;
   } // BOOST SERIALIZATION

}; // class KnnApprox

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename SampleType,typename Dissimilarity,typename LabelType,typename Evaluator>
template <typename ForwardIterator1, typename ForwardIterator2>
void
KnnApprox<SampleType, Dissimilarity, LabelType, Evaluator>
::Learn(
    ForwardIterator1  iSampleBegin,
    ForwardIterator1  iSampleEnd,
    ForwardIterator2  iLabelBegin)
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("KnnApprox, 0, Invalid sample iterators.");
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

template <typename SampleType,typename Dissimilarity,typename LabelType,typename Evaluator>
void
KnnApprox<SampleType, Dissimilarity, LabelType, Evaluator>
::Process(
      const SampleType& rSample,
      LabelType&        rLabel) const
{
   FindNeighbors(rSample);
   DissWeightedRegression(rLabel);
}  // Process

template <typename SampleType,typename Dissimilarity,typename LabelType,typename Evaluator>
template <typename ForwardIterator1, typename ForwardIterator2>
void
KnnApprox<SampleType, Dissimilarity, LabelType, Evaluator>
::Process(
     ForwardIterator1  iSampleBegin,
     ForwardIterator1  iSampleEnd,
     ForwardIterator2  iLabelBegin) const
{
   if (std::distance(iSampleBegin, iSampleEnd) < 0)
   {
      throw SpareLogicError("KnnApprox, 1, Invalid sample iterators.");
   }

   while (iSampleBegin != iSampleEnd)
   {
      Process(
         *iSampleBegin++,
         *iLabelBegin++);
   }
}  // Process

//==================================== STREAM ==============================================

////////////////////////////////////// PRIVATE /////////////////////////////////////////////

template <typename SampleType,typename Dissimilarity,typename LabelType,typename Evaluator>
void
KnnApprox<SampleType, Dissimilarity, LabelType, Evaluator>
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
      throw SpareLogicError("KnnApprox, 2, No knowledge.");
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

template <typename SampleType,typename Dissimilarity,typename LabelType,typename Evaluator>
void
KnnApprox<SampleType, Dissimilarity, LabelType, Evaluator>
::DissWeightedRegression(LabelType& rLabel) const
{
   // Variabili.
   DissLabelPairSetIterator     Sit;
   LabelType                    Output;
   RealType                     Weight;
   RealType                     WeightSum;

   Output= 0;
   WeightSum= 0;
   Sit= mDlSet.begin();
   while (mDlSet.end() != Sit)
   {
      Weight= mWeightAgent.Eval(Sit->first);
      Output+= (*Sit++).second * Weight;
      WeightSum+= Weight;
   }

   if (std::numeric_limits<RealType>::epsilon() <= WeightSum)
   {
      Output/= WeightSum;
   }

   // Output.
   rLabel= Output;
}  // DissWeighted

}  // namespace spare

#endif  // _KnnApprox_h_
