//  Rlrpa class, part of the SPARE library.
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

/** @brief File Rlrpa.hpp, containing the %Rlrpa template class.
 *
 * The file contains the %Rlrpa template class, implementing a recurring pattern analysis
 * algorithm based on reinforcement learning.
 *
 * @file Rlrpa.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Rlrpa_h_
#define _Rlrpa_h_

// STD INCLUDES
#include <fstream>
#include <limits>
#include <list>
#include <utility>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

// Operatore di confronto recettori.
template <typename T>
bool                 
CompareReceptors(const std::pair<RealType, T>& rA, const std::pair<RealType, T>& rB)
{
   return rA.first > rB.first;
}

/** @brief %Rlrpa recurring pattern analysis algorithm.
 *
 * The file contains the %Rlrpa template class, implementing a recurring pattern analysis
 * algorithm based on competitive reinforcement learning.
 * Please refer to the paper "A New Granular Computing Approach for Sequences Representation and Classification, Rizzi et al., IJCNN 2012".
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
 *     <td class="indexvalue">Alpha</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Receptor reinforcement factor.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.001</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Beta</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Receptor forgetting factor.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.01</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Theta</td>
 *     <td class="indexvalue">[0, inf)</td>
 *     <td class="indexvalue">Receptor creation threshold.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.1</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Epsilon</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Receptor vanishing threshold.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.001</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Sigma</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Receptor initial strength.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.1</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Q</td>
 *     <td class="indexvalue">[1, inf)</td>
 *     <td class="indexvalue">Maximum number of receptors to be created.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">100</td>
 *  </tr>
 *  </table>  
 */
template <typename Representative>
class Rlrpa
{
public:

   /** Real parameter.
    */
   typedef BoundedParameter<RealType>
                        RealParam;

   /** Integer parameter.
    */
   typedef BoundedParameter<NaturalType>
                        NaturalParam;

   /** Recurrent pattern receptor.
    */
   typedef std::pair<RealType, Representative>
                        Receptor;

   /** Type of container storing the generated receptors.
    */
   typedef std::list<Receptor>
                        ReceptorList;

   /** Label type.
    */
   typedef typename ReceptorList::size_type  
                        LabelType;

   /** Default constructor.
    */
   Rlrpa()
      : mAlpha(RealType(0), RealType(1)),
        mBeta(RealType(0), RealType(1)),
        mTheta(RealType(0), std::numeric_limits<RealType>::max()),
        mEpsilon(RealType(0), RealType(1)),
        mRecInit(RealParam(RealType(0), RealType(1)), Representative()),
        mQ(1, std::numeric_limits<NaturalType>::max())
                        {
                           mAlpha= 0.01;
                           mBeta= 0.001;
                           mTheta= 0.1;
                           mEpsilon= 0.001;
                           mQ= 100;
                           mRecInit.first= 0.1;
                        }

   /** Learning of a single sample.
    *
    * @param[in] rSample The input sample.
    */
   template <typename SampleType>
   void                 Learn(const SampleType& rSample);


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

   /** Elimination of receptors with a low count.
    *
    * @param[in] aMinCount The minimum count of a receptor to be held.
    */
   void                 FilterOutReceptors(NaturalType aMinCount);

   /** Sorting of receptors by strength.
    */
   void                 SortReceptors()
                        {
                           mReceptors.sort(CompareReceptors<Representative>);
                        }   

   /** Clear all receptors.
    */   
   void                 ClearReceptors()
                        {
                           mReceptors.clear();
                        }
   
   /** Read/write access to the Alpha parameter.
    *
    * @return A reference to the Alpha parameter.
    */
   RealParam&           Alpha()                    { return mAlpha; }

   /** Read only access to the Alpha parameter.
    *
    * @return A const reference to the Alpha parameter.
    */
   const RealParam&     Alpha() const              { return mAlpha; }

   /** Read/write access to the Beta parameter.
    *
    * @return A reference to the Beta parameter.
    */
   RealParam&           Beta()                     { return mBeta; }

   /** Read only access to the Beta parameter.
    *
    * @return A const reference to the Beta parameter.
    */
   const RealParam&     Beta() const               { return mBeta; }

   /** Read/write access to the Epsilon parameter.
    *
    * @return A reference to the Epsilon parameter.
    */
   RealParam&           Epsilon()                  { return mEpsilon; }

   /** Read only access to the Epsilon parameter.
    *
    * @return A const reference to the Epsilon parameter.
    */
   const RealParam&     Epsilon() const            { return mEpsilon; }

   /** Read/write access to the Sigma parameter.
    *
    * @return A reference to the Sigma parameter.
    */
   RealParam&           Sigma()                    { return mRecInit.first; }

   /** Read only access to the Sigma parameter.
    *
    * @return The Sigma parameter.
    */
   const RealParam&     Sigma() const              { return mRecInit.first; }

   /** Read/write access to the Theta parameter.
    *
    * @return A reference to the Theta parameter.
    */
   RealParam&           Theta()                    { return mTheta; }

   /** Read only access to the Theta parameter.
    *
    * @return A const reference to the Theta parameter.
    */
   const RealParam&     Theta() const              { return mTheta; }

   /** Read/write access to the Q parameter.
    *
    * @return A reference to the Q parameter.
    */
   NaturalParam&        Q()                        { return mQ; }

   /** Read only access to the Q parameter.
    *
    * @return A const reference to the Q parameter.
    */
   const NaturalParam&  Q() const                  { return mQ; }

   /** Read/write access to the representative intializer.
    *
    * @return A reference to the instance.
    */
   Representative&      RepInit()                  { return mRecInit.second; }

   /** Read only access to the representative intializer.
    *
    * @return A const reference to the instance.
    */
   const Representative&
                        RepInit() const            { return mRecInit.second; }

   /** Read only access to the receptor list.
    *
    * @return A const reference to the receptor list
    */
   const ReceptorList&  GetReceptors() const       { return mReceptors; }

private:

   // Inizializzatore recettore.
   typedef std::pair<RealParam, Representative>
                        ReceptorInitializer;

   // Fattore di rinforzo.
   RealParam            mAlpha;

   // Fattore di dimenticanza.
   RealParam            mBeta;

   // Soglia nuovo recettore.
   RealParam            mTheta;

   // Soglia cancellazione recettore.
   RealParam            mEpsilon;

   // Inizializzatore recettore.
   ReceptorInitializer  mRecInit;

   // Massimo numero di recettori.
   NaturalParam         mQ;

   // Lista recettori.
   ReceptorList         mReceptors;
   
   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mAlpha);
      ar & BOOST_SERIALIZATION_NVP(mBeta);
      ar & BOOST_SERIALIZATION_NVP(mTheta);
      ar & BOOST_SERIALIZATION_NVP(mEpsilon);
      ar & BOOST_SERIALIZATION_NVP(mRecInit);
      ar & BOOST_SERIALIZATION_NVP(mQ);  
      ar & BOOST_SERIALIZATION_NVP(mReceptors);
   } // BOOST SERIALIZATION
};

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Representative>
template <typename SampleType>
void
Rlrpa<Representative>::Learn(const SampleType& rSample)
{
   if (mReceptors.empty())
   {
      mReceptors.push_front(mRecInit);
      mReceptors.front().second.Update(rSample);
      return;
   }

   typename ReceptorList::iterator Rit= mReceptors.begin();
   typename ReceptorList::iterator Nit= Rit;
   RealType                        CurDiss;
   RealType                        MinDiss= Rit->second.Diss(rSample);

   ++Rit;
   while (mReceptors.end() != Rit)
   {
      CurDiss= Rit->second.Diss(rSample);
      if (CurDiss < MinDiss)
      {
         MinDiss= CurDiss;
         Nit= Rit;
      }
      ++Rit;
   }

   if ((MinDiss > mTheta) && (mReceptors.size() < mQ))
   {
      mReceptors.push_front(mRecInit);
      mReceptors.front().second.Update(rSample);
   }
   else
   {
      if (MinDiss <= mTheta)
      {
         Nit->second.Update(rSample);
         Nit->first+= mAlpha * (RealType(1) - Nit->first);
      }
   }

   Rit= mReceptors.begin();
   while (mReceptors.end() != Rit)
   {
      Rit->first-= mBeta * Rit->first;
      if (Rit->first < mEpsilon)
      {
         Rit= mReceptors.erase(Rit);
      }
      else
      {
         ++Rit;
      }
   }
}

template <typename Representative>
template <typename SampleType>
void
Rlrpa<Representative>::Process(
                               const SampleType& rSample,
                               LabelType&        rLabel) const
{
   if (mReceptors.empty())
   {
      return;
   }

   typename ReceptorList::const_iterator Rit= mReceptors.begin();
   LabelType                             i= 0, Nid= 0;
   RealType                              CurDiss;
   RealType                              MinDiss= Rit->second.Diss(rSample);

   ++Rit;
   ++i;
   while (mReceptors.end() != Rit)
   {
      CurDiss= Rit->second.Diss(rSample);
      if (CurDiss < MinDiss)
      {
         MinDiss= CurDiss;
         Nid= i;
      }
      ++Rit;
      ++i;
   }

   if (MinDiss <= mTheta)
   {
       rLabel= Nid;
   }
   else
   {
       rLabel= i;
   }
}

template <typename Representative>
template <typename ForwardIterator1, typename ForwardIterator2>
void
Rlrpa<Representative>::Process(
                               ForwardIterator1 iSampleBegin,
                               ForwardIterator1 iSampleEnd,
                               ForwardIterator2 iLabelBegin) const
{
    // FIXME: iterator check (usare nel frattempo < potrebbe essere più robusto)
    while (iSampleBegin != iSampleEnd)
    {
        Process(*iSampleBegin++, *iLabelBegin++);
    }
}

template <typename Representative>
template <typename ForwardIterator>
void
Rlrpa<Representative>::Learn(
                             ForwardIterator iSampleBegin,
                             ForwardIterator iSampleEnd)
{
	double totdist = std::distance(iSampleBegin,iSampleEnd);
    // FIXME: iterator check (usare nel frattempo < potrebbe essere più robusto)
    while (iSampleBegin != iSampleEnd)
    {
    	double currdist = std::distance(iSampleBegin,iSampleEnd);
    	if ((int)currdist % 1000 == 0) cout << "Completamento:" << currdist/totdist * 100 << "%" <<endl;
        Learn(*iSampleBegin++);
    }
}

template <typename Representative>
void
Rlrpa<Representative>::FilterOutReceptors(NaturalType aMinCount)
{
   typename ReceptorList::iterator Rit= mReceptors.begin();
   while (mReceptors.end() != Rit)
   {
      if (Rit->second.GetCount() < aMinCount)
      {
         Rit= mReceptors.erase(Rit);
      }
      else
      {
         ++Rit;
      }
   }
}

}  // namespace spare

#endif // _Rlrpa_h_
