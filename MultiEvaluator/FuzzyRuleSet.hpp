//  FuzzyRuleSet class, part of the SPARE library.
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

/** @brief File FuzzyRuleSet.hpp, containing the FuzzyRuleSet class.
 *
 * The file contains the FuzzyRuleSet class, implementing the core of a rule based fuzzy
 * reasoning system.
 *
 * @file FuzzyRuleSet.hpp
 * @author Guido Del Vescovo
 */

#ifndef _FuzzyRuleSet_h_
#define _FuzzyRuleSet_h_

// STD INCLUDES
#include <algorithm>
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

namespace spare {  // Inclusion in namespace spare.

/** @brief A rule system implementing a Max-Min or Max-Product fuzzy inference core.
 *
 * The %FuzzyRuleSet class models the @a MultiEvaluator concept. The input is denoted as the
 * \f$N\times1\f$ vector \f$\boldsymbol{\mu}\f$ generated as output by an antecedent system,
 * containing fuzzy membership values referring to simple antecedents (like "x is A", to make
 * things clearer). The output is denoted as the \f$P\times1\f$ vector
 * \f$\boldsymbol{\beta}\f$ taken as input by a consequent system, containing activation
 * values for the terms in the output stage of the fuzzy model (i.e. the weight of terms like
 * "y is B" in the final output computation). The rules are only defined in terms of the
 * components of \f$\boldsymbol{\mu}\f$ (always expected to be in the range [0, 1]) and
 * \f$\boldsymbol{\beta}\f$, the actual meaning of these components depend on the object
 * which produces \f$\boldsymbol{\mu}\f$ as output and on the object which receives
 * \f$\boldsymbol{\beta}\f$ as input. The %FuzzyRuleSet contains a vector of \f$K\f$ rules.
 * Each rule holds information concerning the \f$\boldsymbol{\mu}\f$ components (i.e. the
 * atomic antecedents) to combine, the \f$\boldsymbol{\beta}\f$ components to act on, a
 * linguistic hedghe for each input, the type of connector to use to combine the inputs (Min
 * or Product), the weight of the rule itself (in the range [0, 1]). The setup of the class
 * happens in two steps: first the structural setup has to be defined (number of rules,
 * number of inputs, number of outputs), then the single rules can be edited.
 */
class FuzzyRuleSet
{
public:

// PUBLIC TYPES

   /** Hedge tag.
    */
   typedef enum
   {
      /** No hedge.
       */
      hgNone= 0,

      /** "Not" hedge.
       */
      hgNot= 1,

      /** "Strictly" hedge.
       */
      hgStrictly= 2,

      /** "Loosely" hedge.
       */
      hgLoosely= 3
   }
   HedgeTag;

   /** Connector tag.
    */
   typedef enum
   {
      /** Min connector.
       */
      cnMin= 0,

      /** Product connector.
       */
      cnProduct= 1
   }
   ConnectorTag;

// LIFECYCLE

   /** Default constructor.
    */
   FuzzyRuleSet()
      : mInputSize(0),
        mOutputSize(0)
                                                   { }

// OPERATIONS

   /** Output evaluation.
    *
    * @param[in] rInput Container holding the \f$\boldsymbol{\mu}\f$ vector.
    * @param[out] rOutput Container holding the \f$\boldsymbol{\beta}\f$ vector.
    */
   template <typename SequenceContainer1, typename SequenceContainer2>
   void                 Eval(
                           const SequenceContainer1& rInput,
                           SequenceContainer2&       rOutput) const;

   /** Output evaluation.
    *
    * @param[in] aInput Iterator pair delimiting the \f$\boldsymbol{\mu}\f$ vector.
    * @param[out] aOutput Iterator pair delimiting the \f$\boldsymbol{\beta}\f$ vector.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Eval(
                           std::pair<ForwardIterator1, ForwardIterator1> aInput,
                           std::pair<ForwardIterator2, ForwardIterator2> aOutput) const;

// SETUP

   /** Structure setup.
    *
    * @param[in] aInputSize Input size (size of the \f$\boldsymbol{\mu}\f$ vector).
    * @param[in] aOutputSize Output size (size of the \f$\boldsymbol{\beta}\f$ vector).
    * @param[in] aRuleNum Number of rules.
    */
   void                 StructureSetup(
                                   NaturalType   aInputSize,
                                   NaturalType   aOutputSize,
                                   NaturalType   aRuleNum)
                           {
                              if ( (aInputSize < 1) || (aOutputSize < 1) ||
                                   (aRuleNum < 1) || (aRuleNum > mRules.max_size()) )
                              {
                                 throw SpareLogicError("FuzzyRuleSet, 0, Invalid "
                                                       "structure data.");
                              }

                              mInputSize= aInputSize;
                              mOutputSize= aOutputSize;
                              mRules.clear();
                              mRules.resize(aRuleNum);
                           }

   /** Rule setup.
    *
    * The setup requires the weight of the rule (in the range [0, 1]) and the structure
    * sequence. The structure sequence is an integer sequence, the format is the following:
    * first the connector type id, then the number of atomic antecedents to combine, then,
    * for each antecedents a couple of values representing the index of the antecedent in the
    * \f$\boldsymbol{\mu}\f$ vector and the linguistic hedge id, then the number of
    * consequents, then the index of each consequent in the \f$\boldsymbol{\beta}\f$ vector
    * (the index base is always 0). Example:
    *
    * 0 2 0 0 3 1 1 2
    *
    * In this example we have Min type connector, 2 antecedents. The first antecedent is
    * the first component of the \f$\boldsymbol{\mu}\f$ vector, without hedge, the second
    * antecedent is the fourth component of the \f$\boldsymbol{\mu}\f$ vector, with Not
    * hedge. There is only one consequent, which index in the \f$\boldsymbol{\beta}\f$ vector
    * is 2.
    *
    * @param[in] aIndex Rule index (index base 0).
    * @param[in] aWeight Rule weight (in the range [0, 1]).
    * @param[in] rStructure Container holding the structure sequence.
    */
   template <typename SequenceContainer>
   void                 RuleSetup(
                              NaturalType              aIndex,
                              RealType                 aWeight,
                              const SequenceContainer& rStructure)
                           {
                              RuleSetup(
                                      aIndex,
                                      aWeight,
                                      std::make_pair(
                                                 rStructure.begin(),
                                                 rStructure.end()));
                           }

   /** Rule setup.
    *
    * For more explanations see the overloaded version.
    *
    * @param[in] aIndex Rule index (index base 0).
    * @param[in] aWeight Rule weight (in the range [0, 1]).
    * @param[in] aStructure Iterator pair delimiting the structure sequence.
    */
   template <typename ForwardIterator>
   void                 RuleSetup(
                              NaturalType                                 aIndex,
                              RealType                                    aWeight,
                              std::pair<ForwardIterator, ForwardIterator> aStructure);

private:

   // Tipo vettore indici.
   typedef std::vector<NaturalType>
                        IdVector;

   // Tipo struttura dati regola.
   struct RuleDataStruct
   {
      // Indici dei valori di fulfillment in input da elaborare (antecedenti).
      IdVector          Antecs;

      // Modificatori.
      IdVector          Hedges;

      // Tipo di connettore degli antecedenti.
      NaturalType       Connector;

      // Peso della regola.
      RealType          Weight;

      // Indici dei valori di fulfillment in output su cui agire (conseguenti).
      IdVector          Conseqs;

      // Costruttore di default.
      RuleDataStruct()
          : Connector(cnMin),
            Weight(1.)
             {
                Antecs.assign(1, 0);
                Conseqs.assign(1, 0);
             }

      // BOOST SERIALIZATION
      friend class boost::serialization::access;

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
         ar & BOOST_SERIALIZATION_NVP(Antecs);
         ar & BOOST_SERIALIZATION_NVP(Hedges);
         ar & BOOST_SERIALIZATION_NVP(Connector);
         ar & BOOST_SERIALIZATION_NVP(Weight);
         ar & BOOST_SERIALIZATION_NVP(Conseqs);
      } // BOOST SERIALIZATION
   };

   // Vettore di regole.
   typedef std::vector<RuleDataStruct>
                        RuleVector;

   // Regole.
   RuleVector           mRules;

   // Input size.
   NaturalType          mInputSize;

   // Output size.
   NaturalType          mOutputSize;

   // Resetta regola.
   void                 ClearRule(NaturalType aIndex);

   // Valutazione regole.
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 RuleEval(
                              std::pair<ForwardIterator1, ForwardIterator1> aInput,
                              std::pair<ForwardIterator2, ForwardIterator2> aOutput) const;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mRules);
      ar & BOOST_SERIALIZATION_NVP(mInputSize);
      ar & BOOST_SERIALIZATION_NVP(mOutputSize);
   } // BOOST SERIALIZATION
}; // class FuzzyRuleSet

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== SETUP ===============================================

template <typename ForwardIterator>
void
FuzzyRuleSet::RuleSetup(
                    NaturalType                                 aIndex,
                    RealType                                    aWeight,
                    std::pair<ForwardIterator, ForwardIterator> aStructure)
{
   // Controllo indice.
   if (aIndex >= mRules.size())
   {
      throw SpareLogicError("FuzzyRuleSet, 1, Index out of range.");
   }

   // Typedef locale.
   typedef typename std::iterator_traits<ForwardIterator>::difference_type
                        ForwardIteratorDiffType;

   // Resetto regola.
   ClearRule(aIndex);

   // Imposto peso, dopo averlo controllato.
   if ((aWeight < 0.) || (aWeight > 1.))
   {
      throw SpareLogicError("FuzzyRuleSet, 2, Weight out of range.");
   }
   mRules[aIndex].Weight= aWeight;

   // Dichiarazioni.
   IdVector                TmpHedges;
   IdVector::iterator      Ait;
   IdVector::iterator      Hit;

   // Dimensione sequenza passata.
   ForwardIteratorDiffType Sz= std::distance(aStructure.first, aStructure.second);

   // Controllo dimensione minima sequenza struttura.
   if (Sz < 6)
   {
      throw SpareLogicError("FuzzyRuleSet, 3, Invalid structure sequence.");
   }

   // Controllo connettore.
   if ((*aStructure.first < 0) || (*aStructure.first > 1))
   {
      throw SpareLogicError("FuzzyRuleSet, 4, Invalid structure sequence.");
   }

   // Imposto connettore e avanzo su sequenza struttura.
   mRules[aIndex].Connector= *aStructure.first++;
   --Sz;

   // Controllo numero antecedenti.
   if (*aStructure.first < 1)
   {
      ClearRule(aIndex);
      throw SpareLogicError("FuzzyRuleSet, 5, Invalid structure sequence.");
   }

   // Dimensiono vettore antecedenti e temp modificatori, e avanzo su sequenza struttura.
   mRules[aIndex].Antecs.resize(*aStructure.first++);
   TmpHedges.resize(mRules[aIndex].Antecs.size());
   --Sz;

   // Controllo dimensione (FIXME: attenzione al casting).
   if (Sz < static_cast<ForwardIteratorDiffType>(2*mRules[aIndex].Antecs.size()))
   {
      ClearRule(aIndex);
      throw SpareLogicError("FuzzyRuleSet, 6, Invalid structure sequence.");
   }

   // Leggo antecedenti e modificatori.
   Ait= mRules[aIndex].Antecs.begin();
   Hit= TmpHedges.begin();
   while (mRules[aIndex].Antecs.end() != Ait)
   {
      if ((*aStructure.first < 0) || (*aStructure.first >= mInputSize))
      {
         ClearRule(aIndex);
         throw SpareLogicError("FuzzyRuleSet, 7, Invalid structure sequence.");
      }

      (*Ait++)= *aStructure.first++;
      --Sz;

      if ((*aStructure.first < 0) || (*aStructure.first > 3))
      {
         ClearRule(aIndex);
         throw SpareLogicError("FuzzyRuleSet, 8, Invalid structure sequence.");
      }

      (*Hit++)= *aStructure.first++;
      --Sz;
   }

   // Assegno modificatori, se almeno uno è diverso da hgNone (FIXME: attenzione a casting).
   if (std::count(TmpHedges.begin(), TmpHedges.end(), hgNone) <
       static_cast<IntegerType>(TmpHedges.size()))
   {
      mRules[aIndex].Hedges= TmpHedges;
   }

   // Controllo dimensione.
   if (Sz < 1)
   {
      ClearRule(aIndex);
      throw SpareLogicError("FuzzyRuleSet, 9, Invalid structure sequence.");
   }

   // Controllo numero conseguenti.
   if (*aStructure.first < 1)
   {
      ClearRule(aIndex);
      throw SpareLogicError("FuzzyRuleSet, 10, Invalid structure sequence.");
   }

   // Dimensiono vettore conseguenti e avanzo su sequenza struttura.
   mRules[aIndex].Conseqs.resize(*aStructure.first++);
   --Sz;

   // Controllo dimensione (FIXME: attenzione al casting).
   if (Sz < static_cast<ForwardIteratorDiffType>(mRules[aIndex].Conseqs.size()))
   {
      ClearRule(aIndex);
      throw SpareLogicError("FuzzyRuleSet, 11, Invalid structure sequence.");
   }

   // Leggo conseguenti.
   Ait= mRules[aIndex].Conseqs.begin();
   while (mRules[aIndex].Conseqs.end() != Ait)
   {
      if ((*aStructure.first < 0) || (*aStructure.first >= mOutputSize))
      {
         ClearRule(aIndex);
         throw SpareLogicError("FuzzyRuleSet, 12, Invalid structure sequence.");
      }

      (*Ait++)= *aStructure.first++;
      --Sz;
   }

   if (Sz > 0)
   {
      ClearRule(aIndex);
      throw SpareLogicError("FuzzyRuleSet, 13, Invalid structure sequence.");
   }
}

//==================================== OPERATIONS ==========================================

template <typename SequenceContainer1, typename SequenceContainer2>
void
FuzzyRuleSet::Eval(
                 const SequenceContainer1& rInput,
                 SequenceContainer2&       rOutput) const
{
   #if SPARE_DEBUG
   if (rInput.size() != mInputSize)
   {
      throw SpareLogicError("FuzzyRuleSet, 14, Input of invalid size.");
   }

   if (rOutput.size() != mOutputSize)
   {
      throw SpareLogicError("FuzzyRuleSet, 15, Output of invalid size.");
   }
   #endif

   RuleEval(
        std::make_pair(rInput.begin(), rInput.end()),
        std::make_pair(rOutput.begin(), rOutput.end()));
}

template <typename ForwardIterator1, typename ForwardIterator2>
void
FuzzyRuleSet::Eval(
                 std::pair<ForwardIterator1, ForwardIterator1> aInput,
                 std::pair<ForwardIterator2, ForwardIterator2> aOutput) const
{
   #if SPARE_DEBUG
   if (std::distance(aInput.first, aInput.second) != mInputSize)
   {
      throw SpareLogicError("FuzzyRuleSet, 16, Input of invalid size.");
   }

   if (std::distance(aOutput.first, aOutput.second) != mOutputSize)
   {
      throw SpareLogicError("FuzzyRuleSet, 17, Output of invalid size.");
   }
   #endif

   RuleEval(aInput, aOutput);
}

////////////////////////////////////// PRIVATE /////////////////////////////////////////////

template <typename ForwardIterator1, typename ForwardIterator2>
void
FuzzyRuleSet::RuleEval(
                     std::pair<ForwardIterator1, ForwardIterator1> aInput,
                     std::pair<ForwardIterator2, ForwardIterator2> aOutput) const
{
   // Dichiarazioni.
   RuleVector::const_iterator   Rit;
   IdVector::const_iterator     Fit;
   IdVector::const_iterator     Mit;
   RealType                     Beta, Aux;
   bool                         ModFlag;

   // Inizializzo l'output a 0.
   std::fill(aOutput.first, aOutput.second, 0.);

   // Processo regole
   Rit= mRules.begin();
   while (mRules.end() != Rit)
   {
      switch (Rit->Connector)
      {
         case cnMin: Beta= 1;
         break;

         case cnProduct: Beta= 1;
         break;
      }
      ModFlag= !Rit->Hedges.empty();

      Fit= Rit->Antecs.begin();
      if (ModFlag)
      {
         Mit= Rit->Hedges.begin();
      }
      while (Rit->Antecs.end() != Fit)
      {
         Aux= *(aInput.first + (*Fit++));
         if (ModFlag)
         {
            switch (*Mit++)
            {
               case hgNot: Aux= RealType(1) - Aux;
            	break;

               case hgStrictly: Aux*= Aux;
            	break;

               case hgLoosely: Aux= std::sqrt(Aux);
            	break;

               case hgNone:
            	break;
            }
         }

         switch (Rit->Connector)
         {
            case cnMin: Beta= std::min(Aux, Beta);
            break;

            case cnProduct: Beta*= Aux;
            break;
         }
      }

      Beta*= Rit->Weight;

      Fit= Rit->Conseqs.begin();
      while (Rit->Conseqs.end() != Fit)
      {
         Aux= *(aOutput.first + *Fit);
         *(aOutput.first + *Fit++)= std::max(Aux, Beta);
      }

      ++Rit;
   }
}

inline
void
FuzzyRuleSet::ClearRule(NaturalType aIndex)
{
   mRules[aIndex].Antecs.assign(1, 0);
   mRules[aIndex].Hedges.clear();
   mRules[aIndex].Connector= cnMin;
   mRules[aIndex].Weight= 1.;
   mRules[aIndex].Conseqs.assign(1, 0);
}

}  // namespace spare

#endif  // _FuzzyRuleSet_h_
