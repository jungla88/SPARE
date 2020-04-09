//  FuzzyConsequent class, part of the SPARE library.
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

/** @brief File FuzzyConsequent.hpp, containing the FuzzyConsequent template class.
 *
 * The file contains the FuzzyConsequent template class, implementing a defuzzification
 * system based on numerical processing of scalar membership functions. The class is useful
 * for the construction of Mamdani type fuzzy models.
 *
 * @file FuzzyConsequent.hpp
 * @author Guido Del Vescovo
 */

#ifndef _FuzzyConsequent_h_
#define _FuzzyConsequent_h_

// STD INCLUDES
#include <algorithm>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief Defuzzification system based on scalar membership functions.
 *
 * The %FuzzyConsequent class models the @a MultiEvaluator concept. It can be employed as the
 * third stage of a fuzzy model. The defuzzification system is constitued of \f$Q\f$ nodes,
 * representing as many output variables. For each of the \f$Q\f$ nodes, \f$q_i\f$ terms, are
 * defined, with \f$i=0,1,...,Q-1\f$. Each term represents the possible consequent of a rule.
 * The output is denoted as the \f$Q\times1\f$ vector \f$\mathbf{y}\f$, containing the output
 * variables. The input is denoted as the \f$P\times1\f$ vector \f$\boldsymbol{\beta}\f$,
 * containing the activation of each term, where:
 *
 * \f$
 * P=\sum_{i=0}^{Q-1} q_i
 * \f$
 *
 * The input vector components are first ordered by node, then by term. More precisely,
 * first we have the activations for the first node, from the first to the last term, then
 * the activations for the second node, and so on. In order to avoid storage redundancies,
 * the membership function objects are stored in an array, then only referenced by the data
 * structure holding nodes and terms. In that way, the same membership function can be
 * employed in different terms and nodes without replication of the object in memory.
 * The setup of the class happens in two steps: first the structural setup has to be defined,
 * then the single membership functions can be edited. The structural setup involves the
 * definition of the number of membership functions in use, the number of nodes and number of
 * terms for each node. When the structure is defined, the membership function objects are
 * allocated, and they can be edited thanks to the suited access property. Before using the
 * Eval method, also the sampling setup has to be done. It sets the output interval and
 * sampling step for the defuzzification of each output variable.
 */
template <typename Evaluator>
class FuzzyConsequent
{
public:

// PUBLIC TYPES

   /** Trigger status vector type.
    */
   typedef std::vector<bool>
                        TriggerStatusVector;

// LIFECYCLE

   /** Default constructor.
    */
   FuzzyConsequent()
      : mInputSize(0)
                                                   { }

// OPERATIONS

   /** Output values evaluation.
    *
    * @param[in] rInput Container holding the input vector.
    * @param[out] rOutput Container holding the output vector.
    */
   template <typename SequenceContainer1, typename SequenceContainer2>
   void                 Eval(
                           const SequenceContainer1& rInput,
                           SequenceContainer2&       rOutput) const;

   /** Output values evaluation.
    *
    * @param[in] aInput Iterator pair delimiting the input vector.
    * @param[out] aOutput Iterator pair delimiting the output vector.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Eval(
                           std::pair<ForwardIterator1, ForwardIterator1> aInput,
                           std::pair<ForwardIterator2, ForwardIterator2> aOutput) const;

// ACCESS

   /** Read/write access to membership function.
    *
    * @param[in] aIndex Index of the membership function (index base 0).
    * @return A reference to the membership function.
    */
   Evaluator&           Membership(NaturalType aIndex)
                           {
                              #if SPARE_DEBUG
                              if (aIndex >= mMembs.size())
                              {
                                 throw SpareLogicError("FuzzyConsequent, 0, "
                                                       "Index out of range.");
                              }
                              #endif

                              return mMembs[aIndex];
                           }

   /** Read only access to membership function.
    *
    * @param[in] aIndex Index of the membership function (index base 0).
    * @return A const reference to the membership function.
    */
   const Evaluator&     Membership(NaturalType aIndex) const
                           {
                              #if SPARE_DEBUG
                              if (aIndex >= mMembs.size())
                              {
                                 throw SpareLogicError("FuzzyConsequent, 1, "
                                                       "Index out of range.");
                              }
                              #endif

                              return mMembs[aIndex];
                           }

   /** Read only access to the trigger status vector.
    *
    * The trigger status vector is a \f$Q\f$-dimensional vector of booleans, indicating
    * whether an output node has been triggered during the last Eval call. If the activation
    * value for at least one term in the node is non-zero, the node is regarded as triggered.
    *
    * @return A const reference to the trigger status vector.
    */
   const TriggerStatusVector&
                        GetTriggerStatus() const
                           {
                              return mTriggerStatus;
                           }

// SETUP

   /** Structural setup.
    *
    * The structural setup is specified by a sequence of integers. The format is the
    * following: first the number of defined membership functions, then the number of nodes,
    * then, for each node a subsequence of values with the following format: first the number
    * of terms, then, for each term, the index of the membership to use for it (index base
    * 0). Example:
    *
    * 5 2 3 0 1 2 2 3 4
    *
    * In the example we have 5 membership functions, 2 nodes. The first node is made of 3
    * terms, associated with the membership functions 0, 1 and 2. The second node is made of
    * 2 terms, associated with the membership functions 3 and 4.
    *
    * @param[in] rStructure Container holding the structure sequence.
    */
   template <typename SequenceContainer>
   void                 StructureSetup(const SequenceContainer& rStructure)
                           {
                              StructureSetup(std::make_pair(
                                                        rStructure.begin(),
                                                        rStructure.end()));
                           }

   /** Structural setup.
    *
    * See the overloaded version for more explanations.
    *
    * @param[in] aStructure Iterator pair delimiting the structure sequence.
    */
   template <typename ForwardIterator>
   void                 StructureSetup(
                                   std::pair<ForwardIterator, ForwardIterator> aStructure);

   /** Output node sampling setup.
    *
    * @param[in] aIndex Index of the output node to setup (index base 0).
    * @param[in] aA Start of the active defuzzification interval.
    * @param[in] aB End of the active defuzzification interval.
    * @param[in] aQ Number of sampling points in the interval.
    */
   void                 SamplingSetup(
                                   NaturalType   aIndex,
                                   RealType      aA,
                                   RealType      aB,
                                   NaturalType   aQ);

private:

   // Tipo vettore di funzioni membership.
   typedef std::vector<Evaluator>
                        MembVector;

   // Tipo indice identificativo di una funzione membership.
   typedef typename MembVector::size_type
                        MembVectorSizeType;

   // Tipo vettore di termini (membership associate ad un input).
   typedef std::vector<MembVectorSizeType>
                        TermVector;

   // Tipo vettore di nodi.
   typedef std::vector<TermVector>
                        NodeVector;

   // Size type vettore nodi.
   typedef typename NodeVector::size_type
                        NodeVectorSizeType;

   // Campionamento output.
   typedef std::vector<RealType>
                        OutputSampling;

   // Tipo vettore campionamenti output.
   typedef std::vector<OutputSampling>
                        OutputSamplingVector;

   // Funzioni membership.
   MembVector           mMembs;

   // Nodi.
   NodeVector           mNodes;

   // Vettori campionamento output.
   OutputSamplingVector mSamplings;

   // Input size.
   NaturalType          mInputSize;

   // Stato trigger.
   mutable TriggerStatusVector
                        mTriggerStatus;

   // Funzione reset.
   void                 ClearAll();

   // Funzione Cog.
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 CogEval(
                           std::pair<ForwardIterator1, ForwardIterator1> aInput,
                           std::pair<ForwardIterator2, ForwardIterator2> aOutput) const;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mMembs);
      ar & BOOST_SERIALIZATION_NVP(mNodes);
      ar & BOOST_SERIALIZATION_NVP(mSamplings);
      ar & BOOST_SERIALIZATION_NVP(mInputSize);
      ar & BOOST_SERIALIZATION_NVP(mTriggerStatus);
   } // BOOST SERIALIZATION
}; // class FuzzyConsequent

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Evaluator>
template <typename SequenceContainer1, typename SequenceContainer2>
void
FuzzyConsequent<Evaluator>::Eval(
                               const SequenceContainer1& rInput,
                               SequenceContainer2&       rOutput) const
{
   #if SPARE_DEBUG
   if (rInput.size() != mInputSize)
   {
      throw SpareLogicError("FuzzyConsequent, 2, Input of invalid size.");
   }

   if (rOutput.size() != mNodes.size())
   {
      throw SpareLogicError("FuzzyConsequent, 3, Output of invalid size.");
   }
   #endif

   CogEval(
       std::make_pair(rInput.begin(), rInput.end()),
       std::make_pair(rOutput.begin(), rOutput.end()));
}

template <typename Evaluator>
template <typename ForwardIterator1, typename ForwardIterator2>
void
FuzzyConsequent<Evaluator>::Eval(
                               std::pair<ForwardIterator1, ForwardIterator1> aInput,
                               std::pair<ForwardIterator2, ForwardIterator2> aOutput) const
{
   #if SPARE_DEBUG
   if (std::distance(aInput.first, aInput.second) != mInputSize)
   {
      throw SpareLogicError("FuzzyConsequent, 4, Input of invalid size.");
   }

   if (std::distance(aOutput.first, aOutput.second) != mNodes.size())
   {
      throw SpareLogicError("FuzzyConsequent, 5, Output of invalid size.");
   }
   #endif

   CogEval(aInput, aOutput);
}

template <typename Evaluator>
template <typename ForwardIterator1, typename ForwardIterator2>
void
FuzzyConsequent<Evaluator>::CogEval(
                                 std::pair<ForwardIterator1, ForwardIterator1> aInput,
                                 std::pair<ForwardIterator2, ForwardIterator2> aOutput) const
{
   // Dichiarazioni.
   typename TermVector::const_iterator             Mit;
   typename NodeVector::const_iterator             Nit;
   typename OutputSamplingVector::const_iterator   Sit;
   typename OutputSampling::const_iterator         Yit;
   typename TriggerStatusVector::iterator          Bit;
   ForwardIterator1                                Iit;
   RealType                                        Num, Den, W;

   // Ciclo nodi.
   Nit= mNodes.begin();
   Sit= mSamplings.begin();
   Bit= mTriggerStatus.begin();
   while (mNodes.end() != Nit)
   {
      if (Sit->empty())
      {
         throw SpareLogicError("FuzzyConsequent, 6, Uninitialized sampling.");
      }

      // Ciclo calcolo.
      Num= Den= 0.;
      Yit= Sit->begin();
      while (Sit->end() != Yit)
      {
         Iit= aInput.first;
         Mit= Nit->begin();
         W= 0.;
         while (Nit->end() != Mit)
         {
            W= std::max(std::min(*Iit++, mMembs[*Mit++].Eval(*Yit)), W);
         }

         Num+= (*Yit) * W;
         Den+= W;

         ++Yit;
      }

      // Genero output (WARNING: condizionamento divisione).
      if (Den > std::numeric_limits<RealType>::epsilon())
      {
         (*aOutput.first++)= Num / Den;
         (*Bit++)= true;
      }
      else
      {
         ++aOutput.first;
         (*Bit++)= false;
      }

      aInput.first+= Nit->size();
      ++Nit;
      ++Sit;
   }
}

template <typename Evaluator>
template <typename ForwardIterator>
void
FuzzyConsequent<Evaluator>::StructureSetup(
                                      std::pair<ForwardIterator, ForwardIterator> aStructure)
{
   // Typedef locale.
   typedef typename std::iterator_traits<ForwardIterator>::difference_type
                        ForwardIteratorDiffType;

   // Reset.
   ClearAll();

   // Dimensione sequenza passata.
   ForwardIteratorDiffType Sz= std::distance(aStructure.first, aStructure.second);

   // Iteratori.
   typename TermVector::iterator Mit;
   typename NodeVector::iterator Nit;

   // Variabile ausiliaria.
   NaturalType                   Aux= 0;

   // Controllo dimensione minima sequenza struttura.
   if (Sz < 4)
   {
      throw SpareLogicError("FuzzyConsequent, 7, Invalid structure sequence.");
   }

   // Controllo numero membership.
   if ((*aStructure.first < 1) || (*aStructure.first > mMembs.max_size()))
   {
      throw SpareLogicError("FuzzyConsequent, 8, Invalid structure sequence.");
   }

   // Dimensiono vettore membership e avanzo su sequenza struttura.
   mMembs.resize(*aStructure.first++);
   --Sz;

   // Controllo numero nodi.
   if ((*aStructure.first < 1) || (*aStructure.first > mNodes.max_size()))
   {
      ClearAll();
      throw SpareLogicError("FuzzyConsequent, 9, Invalid structure sequence.");
   }

   // Dimensiono vettori nodi e campionamenti, e avanzo su sequenza struttura.
   mNodes.resize(*aStructure.first++);
   mSamplings.resize(mNodes.size());
   mTriggerStatus.assign(mNodes.size(), false);
   --Sz;

   // Ciclo setup nodi.
   Nit= mNodes.begin();
   while (mNodes.end() != Nit)
   {
      // Controllo numero termini.
      if ((*aStructure.first < 1) || (*aStructure.first > Nit->max_size()))
      {
         ClearAll();
         throw SpareLogicError("FuzzyConsequent, 10, Invalid structure sequence.");
      }

      // Aggiorno InputSize con check overflow (FIXME il check non rileva tutti i casi).
      Aux= mInputSize;
      mInputSize+= (*aStructure.first);
      if (mInputSize <= Aux)
      {
         ClearAll();
         throw SpareLogicError("FuzzyConsequent, 11, Input size overflow.");
      }

      // Dimensiono vettore termini e avanzo su sequenza struttura.
      Nit->resize(*aStructure.first++);
      --Sz;

      // Controllo dimensione (FIXME: attenzione al casting).
      if (Sz < static_cast<ForwardIteratorDiffType>(Nit->size()))
      {
         ClearAll();
         throw SpareLogicError("FuzzyConsequent, 12, Invalid structure sequence.");
      }

      // Ciclo setup termini.
      Mit= Nit->begin();
      while (Nit->end() != Mit)
      {
         // Controllo termine.
         if ((*aStructure.first < 0) || (*aStructure.first >= mMembs.size()))
         {
            ClearAll();
            throw SpareLogicError("FuzzyConsequent, 13, Invalid structure sequence.");
         }

         // Imposto termine e avanzo su sequenza struttura.
         (*Mit++)= *aStructure.first++;
         --Sz;
      }
      ++Nit;
   }

   if (Sz > 0)
   {
      ClearAll();
      throw SpareLogicError("FuzzyConsequent, 14, Invalid structure sequence.");
   }
}

template <typename Evaluator>
void
FuzzyConsequent<Evaluator>::ClearAll()
{
   mMembs.clear();
   mNodes.clear();
   mSamplings.clear();
   mInputSize= 0;
   mTriggerStatus.clear();
}

template <typename Evaluator>
void
FuzzyConsequent<Evaluator>::SamplingSetup(
                                      NaturalType   aIndex,
                                      RealType      aA,
                                      RealType      aB,
                                      NaturalType   aQ)
{
   // Dichiarazioni.
   typename OutputSampling::size_type i; // Contatore.

   // Controlli.
   if (aIndex >= mSamplings.size())
   {
      throw SpareLogicError("FuzzyConsequent, 15, Index out of range.");
   }

   if ((aQ < 2) || (aQ > mSamplings[aIndex].max_size()))
   {
      throw SpareLogicError("FuzzyConsequent, 16, Invalid Q value.");
   }

   if (aA >= aB)
   {
      throw SpareLogicError("FuzzyConsequent, 17, Invalid range.");
   }

   mSamplings[aIndex].resize(aQ);
   for (i= 0; i < aQ; ++i)
   {
      mSamplings[aIndex][i]= aA + (aB - aA) * ( static_cast<RealType>(i) /
                                                static_cast<RealType>(aQ - 1) );
   }
}

}  // namespace spare

#endif  // _FuzzyConsequent_h_
