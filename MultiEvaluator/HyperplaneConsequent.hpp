//  HyperplaneConsequent class, part of the SPARE library.
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

/** @brief File HyperplaneConsequent.hpp, containing the HyperplaneConsequent class.
 *
 * The file contains the HyperplaneConsequent template class, implementing a consequent stage
 * for fuzzy models where the output values are computed as combination of hyperplanes
 * evaluated in the point defined by the input vector. The class is useful for the
 * construction of Sugeno type fuzzy models.
 *
 * @file HyperplaneConsequent.hpp
 * @author Guido Del Vescovo
 */

#ifndef _HyperplaneConsequent_h_
#define _HyperplaneConsequent_h_

// STD INCLUDES
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief Consequent system based on the combination of hyperplane functions.
 *
 * The %HyperplaneConsequent class models the @a MultiEvaluator concept. It can be employed 
 * as the third stage of a fuzzy model. The consequent system is constitued of \f$Q\f$ nodes,
 * representing as many output variables. For each of the \f$Q\f$ nodes, \f$q_i\f$ terms
 * (hyperplanes), are defined, with \f$i=0,1,...,Q-1\f$. Each hyperplane represents the
 * possible consequent of a rule. The output is denoted as the \f$Q\times1\f$ vector
 * \f$\mathbf{y}\f$, containing the output variables. The input is denoted as the
 * \f$P\times1\f$ vector \f$\boldsymbol{\beta}\f$, containing the activation of each
 * hyperplane, where:
 *
 * \f$
 * P=\sum_{i=0}^{Q-1} q_i
 * \f$
 *
 * The input vector components are first ordered by node, then by hyperplane. More precisely,
 * first we have the activations for the first node, from the first to the last hyperplane,
 * then the activations for the second node, and so on. In order to avoid storage
 * redundancies, the hyperplane objects are stored in an array, then only referenced by the
 * data structure holding nodes and terms. In that way, the same hyperplane function can be
 * employed in different terms and nodes without replication of the object in memory.
 * The setup of the class happens in two steps: first the structural setup has to be defined,
 * then the single hyperplane functions can be edited. The structural setup involves the
 * definition of the number of hyperplane functions in use, the number of nodes and number of
 * terms for each node. When the structure is defined, the hyperplane function objects are
 * allocated, and they can be edited thanks to the suited access property.
 */
class HyperplaneConsequent
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
   HyperplaneConsequent()
      : mInputSize(0),
        mHpSize(0)
                                                   { }

// OPERATIONS

   /** Output values evaluation. It is normally preceded by a call to HyperplaneEval.
    *
    * @param[in] rInput Container holding the input vector.
    * @param[out] rOutput Container holding the output vector.
    */
   template <typename SequenceContainer1, typename SequenceContainer2>
   void                 Eval(
                           const SequenceContainer1& rInput,
                           SequenceContainer2&       rOutput) const;

   /** Output values evaluation. It is normally preceded by a call to HyperplaneEval.
    *
    * @param[in] aInput Iterator pair delimiting the input vector.
    * @param[out] aOutput Iterator pair delimiting the output vector.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Eval(
                           std::pair<ForwardIterator1, ForwardIterator1> aInput,
                           std::pair<ForwardIterator2, ForwardIterator2> aOutput) const;

   /** Hyperplane evaluation. Normally, it has to be called before Eval.
    *
    * @param[in] rInput Boost vector containing the input vector.
    */
   void                 HyperplaneEval(const BoostRealVector& rInput)
                           {
                              #if SPARE_DEBUG
                              if (rInput.size() != mHpSize)
                              {
                                 throw SpareLogicError("HyperplaneConsequent, 0, "
                                                       "Hp input of invalid size.");
                              }
                              #endif

                              // Calcolo.
                              HpVector::iterator Hit= mHyperplanes.begin();
                              while (mHyperplanes.end() != Hit)
                              {
                                 (*Hit++).Eval(rInput);
                              }
                           }

   /** Hyperplane evaluation. Normally, it has to be called before Eval.
    *
    * @param[in] rInput Container holding the input vector.
    */
   template <typename SequenceContainer>
   void                 HyperplaneEval(const SequenceContainer& rInput);

   /** Hyperplane evaluation. Normally, it has to be called before Eval.
    *
    * @param[in] aInput Iterator pair delimiting the input vector.
    */
   template <typename ForwardIterator>
   void                 HyperplaneEval(std::pair<ForwardIterator, ForwardIterator> aInput);

// ACCESS

   /** Read only accesso to the input size.
    *
    * @return The input size.
    */
   NaturalType          InputSize() const          { return mInputSize; }

   /** Read only accesso to the output size.
    *
    * @return The output size.
    */
   NaturalType          OutputSize() const         { return boost::numeric::
                                                            converter<NaturalType,
                                                            NodeVector::size_type>::
                                                            convert(mNodes.size()); }

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
    * following: first the input size of hyperplane, then the number of defined hyperplane
    * functions, then the number of nodes, then, for each node a subsequence of values with
    * the following format: first the number of terms, then, for each term, the index of the
    * hyperplane to use for it (index base 0). Example:
    *
    * 3 5 2 3 0 1 2 2 3 4
    *
    * In the example we have hyperplane input size equal to 3, we have 5 hyperplanes, 2
    * nodes. The first node is made of 3 terms, associated with the hyperplanes 0, 1 and 2.
    * The second node is made of 2 terms, associated with the hyperplanes 3 and 4.
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

   /** Hyperplane setup.
    *
    * The hyperplane function is:
    *
    * \f$
    * f(\mathbf{x})=\sum_{i=0}^{M-1} a_i x_i + b
    * \f$
    *
    * The parameters must be passed in the following order: \f$a_0, a_1, ..., a_{M-1}, b\f$.
    * It is worth to remark that \f$M\f$ is the first value of the structural setup sequence.
    *
    * @param[in] aIndex Index of hyperplane to setup (index base 0).
    * @param[in] rParams Container holding the parameter sequence.
    */
   template <typename SequenceContainer>
   void                 HyperplaneSetup(
                                    NaturalType              aIndex,
                                    const SequenceContainer& rParams)
                           {
                              #if SPARE_DEBUG
                              if (aIndex >= mHyperplanes.size())
                              {
                                 throw SpareLogicError("HyperplaneConsequent, 1, "
                                                       "Index out of range.");
                              }
                              #endif

                              mHyperplanes[aIndex].Setup(rParams);
                           }

   /** Hyperplane setup.
    *
    * For more explanation see the overloaded version.
    *
    * @param[in] aIndex Index of hyperplane to setup (index base 0).
    * @param[in] aParams Iterator pair delimiting the parameter sequence.
    */
   template <typename ForwardIterator>
   void                 HyperplaneSetup(
                                    NaturalType                                 aIndex,
                                    std::pair<ForwardIterator, ForwardIterator> aParams)
                           {
                              #if SPARE_DEBUG
                              if (aIndex >= mHyperplanes.size())
                              {
                                 throw SpareLogicError("HyperplaneConsequent, 2, "
                                                       "Index out of range.");
                              }
                              #endif

                              mHyperplanes[aIndex].Setup(aParams);
                           }

private:

   // Iperpiano f = x[0]a[0] + x[1]a[1] + ... + x[N-1]a[N-1] + b
   struct Hyperplane
   {
      // Coefficienti moltiplicativi.
      BoostRealVector      a;

      // Termine noto.
      RealType             b;

      // Valore corrente funzione.
      RealType             f;

      // Inizializzazione con un dato input size.
      void                 Init(BoostRealVector::size_type N)
                              {
                                 BoostRealVector::iterator Vit;

                                 a.resize(N, false);

                                 Vit= a.begin();
                                 while (a.end() != Vit)
                                 {
                                    (*Vit++)= 0.;
                                 }

                                 b= 0.;
                                 f= 0.;
                              }

      // Calcolo f.
      void                 Eval(const BoostRealVector& rInput)
                              {
                                 f= b + inner_prod(a, rInput);
                              }

      // Setup coefficienti (f non viene aggiornato).
      template <typename SequenceContainer>
      void                 Setup(const SequenceContainer& rParams)
                              {
                                 #if SPARE_DEBUG
                                 if (rParams.size() != (a.size() + 1))
                                 {
                                    throw SpareLogicError("HyperplaneConsequent, 3, "
                                                          "Invalid number of "
                                                          "hyperplane parameters.");
                                 }
                                 #endif

                                 typename SequenceContainer::const_iterator
                                                             Pit= rParams.begin();
                                 BoostRealVector::iterator   Ait= a.begin();
                                 while (a.end() != Ait)
                                 {
                                    (*Ait++)= static_cast<RealType>(*Pit++);
                                 }
                                 b= static_cast<RealType>(*Pit);
                              }

      template <typename ForwardIterator>
      void                 Setup(std::pair<ForwardIterator, ForwardIterator> aParams)
                              {
                                 #if SPARE_DEBUG
                                 if (std::distance(aParams.first, aParams.second) !=
                                     boost::numeric::converter<
                                     typename std::iterator_traits<ForwardIterator>
                                              ::difference_type,
                                     BoostRealVector::size_type>
                                     ::convert((a.size() + 1)))
                                 {
                                    throw SpareLogicError("HyperplaneConsequent, 4, "
                                                          "Invalid number of "
                                                          "hyperplane parameters.");
                                 }
                                 #endif

                                 BoostRealVector::iterator Ait= a.begin();
                                 while (a.end() != Ait)
                                 {
                                    (*Ait++)= static_cast<RealType>(*aParams.first++);
                                 }
                                 b= static_cast<RealType>(*aParams.first);
                              }

      // BOOST SERIALIZATION
      friend class boost::serialization::access;

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
         ar & BOOST_SERIALIZATION_NVP(a);
         ar & BOOST_SERIALIZATION_NVP(b);
         ar & BOOST_SERIALIZATION_NVP(f);
      } // BOOST SERIALIZATION
   };

   // Tipo vettore di iperpiani.
   typedef std::vector<Hyperplane>
                        HpVector;

   // Tipo indice identificativo di un iperpiano.
   typedef HpVector::size_type
                        HpVectorSizeType;

   // Tipo vettore di termini (iperpiani associati ad un input).
   typedef std::vector<HpVectorSizeType>
                        TermVector;

   // Tipo vettore di nodi.
   typedef std::vector<TermVector>
                        NodeVector;

   // Size type vettore nodi.
   typedef NodeVector::size_type
                        NodeVectorSizeType;

   // Funzioni membership.
   HpVector             mHyperplanes;

   // Nodi.
   NodeVector           mNodes;

   // Input size.
   NaturalType          mInputSize;

   // Dimensione input iperpiani.
   BoostRealVector::size_type
                        mHpSize;

   // Stato trigger.
   mutable TriggerStatusVector
                        mTriggerStatus;

   // Funzione reset.
   void                 ClearAll();

   // Valutazione media pesata uscite iperpiani.
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 WmEval(
                           std::pair<ForwardIterator1, ForwardIterator1> aInput,
                           std::pair<ForwardIterator2, ForwardIterator2> aOutput) const;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mHyperplanes);
      ar & BOOST_SERIALIZATION_NVP(mNodes);
      ar & BOOST_SERIALIZATION_NVP(mInputSize);
      ar & BOOST_SERIALIZATION_NVP(mHpSize);
      ar & BOOST_SERIALIZATION_NVP(mTriggerStatus);
   } // BOOST SERIALIZATION

}; // class HyperplaneConsequent

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
void
HyperplaneConsequent::WmEval(
                          std::pair<ForwardIterator1, ForwardIterator1> aInput,
                          std::pair<ForwardIterator2, ForwardIterator2> aOutput) const
{
   // Dichiarazioni.
   typename TermVector::const_iterator    Mit;
   typename NodeVector::const_iterator    Nit;
   typename TriggerStatusVector::iterator Bit;
   RealType                               Num, Den;

   // Ciclo nodi.
   Nit= mNodes.begin();
   Bit= mTriggerStatus.begin();
   while (mNodes.end() != Nit)
   {
      // Ciclo calcolo.
      Num= Den= 0.;
      Mit= Nit->begin();
      while (Nit->end() != Mit)
      {
         Num+= (*aInput.first) * mHyperplanes[*Mit++].f;
         Den+= *aInput.first++;
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

      ++Nit;
   }
}

template <typename SequenceContainer1, typename SequenceContainer2>
void
HyperplaneConsequent::Eval(
                        const SequenceContainer1& rInput,
                        SequenceContainer2&       rOutput) const
{
   #if SPARE_DEBUG
   if (rInput.size() != mInputSize)
   {
      throw SpareLogicError("HyperplaneConsequent, 5, Input of invalid size.");
   }

   if (rOutput.size() != mNodes.size())
   {
      throw SpareLogicError("HyperplaneConsequent, 6, Output of invalid size.");
   }
   #endif

   WmEval(
       std::make_pair(rInput.begin(), rInput.end()),
       std::make_pair(rOutput.begin(), rOutput.end()));
}

template <typename ForwardIterator1, typename ForwardIterator2>
void
HyperplaneConsequent::Eval(
                        std::pair<ForwardIterator1, ForwardIterator1> aInput,
                        std::pair<ForwardIterator2, ForwardIterator2> aOutput) const
{
   #if SPARE_DEBUG
   if (std::distance(aInput.first, aInput.second) !=
       boost::numeric::converter<
       typename std::iterator_traits<ForwardIterator1>::difference_type, NaturalType>
       ::convert(mInputSize))
   {
      throw SpareLogicError("HyperplaneConsequent, 7, Input of invalid size.");
   }

   if (std::distance(aOutput.first, aOutput.second) !=
       boost::numeric::converter<
       typename std::iterator_traits<ForwardIterator2>::difference_type,
       NodeVector::size_type>
       ::convert(mNodes.size()))
   {
      throw SpareLogicError("HyperplaneConsequent, 8, Output of invalid size.");
   }
   #endif

   WmEval(aInput, aOutput);
}

template <typename SequenceContainer>
void
HyperplaneConsequent::HyperplaneEval(const SequenceContainer& rInput)
{
   #if SPARE_DEBUG
   if (rInput.size() != mHpSize)
   {
      throw SpareLogicError("HyperplaneConsequent, 9, Hp input of invalid size.");
   }
   #endif

   // Copio in boost vector.
   BoostRealVector                            Temp(mHpSize);
   BoostRealVector::iterator                  Mit= Temp.begin();
   typename SequenceContainer::const_iterator Iit= rInput.begin();
   while (Temp.end() != Mit)
   {
      (*Mit++)= *Iit++;
   }

   // Calcolo.
   HpVector::iterator Hit= mHyperplanes.begin();
   while (mHyperplanes.end() != Hit)
   {
      (*Hit++).Eval(Temp);
   }
}

template <typename ForwardIterator>
void
HyperplaneConsequent::HyperplaneEval(std::pair<ForwardIterator, ForwardIterator> aInput)
{
   #if SPARE_DEBUG
   if (std::distance(aInput.first, aInput.second) !=
       boost::numeric::converter<
       typename std::iterator_traits<ForwardIterator>::difference_type,
       BoostRealVector::size_type>
       ::convert(mHpSize))
   {
      throw SpareLogicError("HyperplaneConsequent, 10, Hp input of invalid size.");
   }
   #endif

   // Copio in boost vector.
   BoostRealVector           Temp(mHpSize);
   BoostRealVector::iterator Mit= Temp.begin();
   while (Temp.end() != Mit)
   {
      (*Mit++)= *aInput.first++;
   }

   // Calcolo.
   HpVector::iterator Hit= mHyperplanes.begin();
   while (mHyperplanes.end() != Hit)
   {
      (*Hit++).Eval(Temp);
   }
}

template <typename ForwardIterator>
void
HyperplaneConsequent::StructureSetup(
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
   HpVector::iterator            Hit;

   // Variabile ausiliaria.
   NaturalType                   Aux= 0;

   // Controllo dimensione minima sequenza struttura.
   if (Sz < 5)
   {
      throw SpareLogicError("HyperplaneConsequent, 11, Invalid structure sequence.");
   }

   // Controllo input size iperpiani (upper bound approssimato, dovrei usare max size di
   // BoostRealVector).
   if ((*aStructure.first < 1) || (*aStructure.first > mHyperplanes.max_size()))
   {
      throw SpareLogicError("HyperplaneConsequent, 12, Invalid structure sequence.");
   }

   // Imposto input size iperpiani e avanzo su sequenza struttura.
   mHpSize= *aStructure.first++;
   --Sz;

   // Controllo numero iperpiani.
   if ((*aStructure.first < 1) || (*aStructure.first > mHyperplanes.max_size()))
   {
      throw SpareLogicError("HyperplaneConsequent, 13, Invalid structure sequence.");
   }

   // Dimensiono vettore iperpiani e avanzo su sequenza struttura.
   mHyperplanes.resize(*aStructure.first++);
   --Sz;

   // Inizializzo iperpiani.
   Hit= mHyperplanes.begin();
   while (mHyperplanes.end() != Hit)
   {
      (*Hit++).Init(mHpSize);
   }

   // Controllo numero nodi.
   if ((*aStructure.first < 1) || (*aStructure.first > mNodes.max_size()))
   {
      ClearAll();
      throw SpareLogicError("HyperplaneConsequent, 14, Invalid structure sequence.");
   }

   // Dimensiono vettori nodi e trigger, e avanzo su sequenza struttura.
   mNodes.resize(*aStructure.first++);
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
         throw SpareLogicError("HyperplaneConsequent, 15, Invalid structure sequence.");
      }

      // Aggiorno InputSize con check overflow (FIXME il check non rileva tutti i casi).
      Aux= mInputSize;
      mInputSize+= (*aStructure.first);
      if (mInputSize <= Aux)
      {
         ClearAll();
         throw SpareLogicError("HyperplaneConsequent, 16, Input size overflow.");
      }

      // Dimensiono vettore termini e avanzo su sequenza struttura.
      Nit->resize(*aStructure.first++);
      --Sz;

      // Controllo dimensione (FIXME: attenzione al casting).
      if (Sz < static_cast<ForwardIteratorDiffType>(Nit->size()))
      {
         ClearAll();
         throw SpareLogicError("HyperplaneConsequent, 17, Invalid structure sequence.");
      }

      // Ciclo setup termini.
      Mit= Nit->begin();
      while (Nit->end() != Mit)
      {
         // Controllo termine.
         if ((*aStructure.first < 0) || (*aStructure.first >= mHyperplanes.size()))
         {
            ClearAll();
            throw SpareLogicError("HyperplaneConsequent, 18, Invalid structure sequence.");
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
      throw SpareLogicError("HyperplaneConsequent, 19, Invalid structure sequence.");
   }
}

inline void
HyperplaneConsequent::ClearAll()
{
   mHyperplanes.clear();
   mNodes.clear();
   mInputSize= 0;
   mHpSize= 0;
   mTriggerStatus.clear();
}

}  // namespace spare

#endif  // _HyperplaneConsequent_h_
