//  MultiGaussianAntecedent class, part of the SPARE library.
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

/** @brief File MultiGaussianAntecedent.hpp, that contains the MultiGaussianAntecedent class.
 *
 * Contain the declaration of the MultiGaussianAntecedent class.
 *
 * @file MultiGaussianAntecedent.hpp
 * @author Guido Del Vescovo
 */

#ifndef _MultiGaussianAntecedent_h_
#define _MultiGaussianAntecedent_h_

// STD INCLUDES
#include <iterator>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/Evaluator/MultiGaussian.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Fuzzifier based on multidimensional Guassian functions.
 *
 * The %MultiGaussianAntecedent class implements the @a MultiEvaluator concept. This 
 * fuzzifier is made of \f$N\f$ terms, represented by the same number of multivariate
 * gaussian functions, defined in a vector space with \f$M\times1\f$ dimensions. The input of 
 * all the multivariate gaussian functions is the real \f$M\times1\f$ vector 
 * \f$\mathbf{x}\f$. The output is represented by the real \f$N\times1\f$ vector 
 * \f$\boldsymbol{\mu}\f$, containing the \f$N\f$ outputs of the multivariate gaussian 
 * functions. The setup of the object is carried out in two steps: first you need to specify 
 * the input/output space size. Next, it's possible to setup the MultiGaussian membership 
 * functions with the proper methods. These params are specified in the MultiGaussian class.
 */
class MultiGaussianAntecedent
{
public:

// LIFECYCLE

   /** Default constructor.
    */
   MultiGaussianAntecedent()
      : mInputSize(0)
                                                   { }

// OPERATIONS

   /** Membership values evaluation.
    *
    * @param[in] rInput Input boost vector.
    * @param[out] rOutput Output container.
    */
   template <typename SequenceContainer>
   void                 Eval(
                           const BoostRealVector& rInput,
                           SequenceContainer&     rOutput) const;

   /** Membership values evaluation.
    *
    * @param[in] rInput Input container.
    * @param[out] rOutput Output container.
    */
   template <typename SequenceContainer1, typename SequenceContainer2>
   void                 Eval(
                           const SequenceContainer1& rInput,
                           SequenceContainer2&       rOutput) const;

   /** Membership values evaluation.
    *
    * @param[in] aInput Pair of iterators that points to the input vector.
    * @param[out] aOutput Pair of iterators that points to the output vector.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Eval(
                           std::pair<ForwardIterator1, ForwardIterator1> aInput,
                           std::pair<ForwardIterator2, ForwardIterator2> aOutput) const;

// ACCESS

   /** Read access to the input size param.
    *
    * @return Input size value.
    */
   NaturalType          InputSize() const          { return mInputSize; }

   /** Read access to the output size param.
    *
    * @return Output size value.
    */
   NaturalType          OutputSize() const         { return boost::numeric::
                                                            converter<NaturalType,
                                                            TermVector::size_type>::
                                                            convert(mTerms.size()); }

// SETUP

   /** Setup multidimensional Guassian membership function.
    *
    * @param[in] aIndex Index about the term to setup.
    * @param[in] rMean Centroid vector.
    * @param[in] rInvCov Inverse covariance matrix.
    */
   void                 MembershipParamSetup(
                                         NaturalType                aIndex,
                                         const BoostRealVector&     rMean,
                                         const BoostRealSymmMatrix& rInvCov)
                           {
                              #if SPARE_DEBUG
                              if (aIndex >= mTerms.size())
                              {
                                 throw SpareLogicError("MultiGaussianAntecedent, 0, "
                                                       "Index out of range.");
                              }
                              #endif

                              mTerms[aIndex].ParamSetup(rMean, rInvCov);
                           }

   /** Setup multidimensional Guassian membership function.
    *
    * @param[in] aIndex Index about the term to setup.
    * @param[in] rParams A reference to the container of the parameters.
    */
   template <typename SequenceContainer>
   void                 MembershipParamSetup(
                                         NaturalType              aIndex,
                                         const SequenceContainer& rParams)
                           {
                              #if SPARE_DEBUG
                              if (aIndex >= mTerms.size())
                              {
                                 throw SpareLogicError("MultiGaussianAntecedent, 1, "
                                                       "Index out of range.");
                              }
                              #endif

                              mTerms[aIndex].ParamSetup(rParams);
                           }

   /** Setup multidimensional Guassian membership function.
    *
    * @param[in] aIndex Index about the term to setup.
    * @param[in] aParams Pair of iterators that point to the parameters.
    */
   template <typename ForwardIterator>
   void                 MembershipParamSetup(
                                         NaturalType                                 aIndex,
                                         std::pair<ForwardIterator, ForwardIterator> aParams)
                           {
                              #if SPARE_DEBUG
                              if (aIndex >= mTerms.size())
                              {
                                 throw SpareLogicError("MultiGaussianAntecedent, 2, "
                                                       "Index out of range.");
                              }
                              #endif

                              mTerms[aIndex].ParamSetup(aParams);
                           }

   /** Structure setup. Input size and number of terms (output size).
    *
    * @param[in] aInputSize Input size.
    * @param[in] aTermNumber Number of terms (output size).
    */
   void                 StructureSetup(
                                   NaturalType   aInputSize,
                                   NaturalType   aTermNumber)
                           {
                              if (aTermNumber > mTerms.max_size())
                              {
                                 throw SpareLogicError("MultiGaussianAntecedent, 3, "
                                                       "Term number is too large.");
                              }

                              mTerms.resize(aTermNumber);
                              TermVector::iterator Mit= mTerms.begin();
                              while (mTerms.end() != Mit)
                              {
                                 (*Mit++).InputSizeSetup(aInputSize);
                              }

                              mInputSize= aInputSize;
                           }

private:

   // Tipo vettore di termini (gaussiane multidimensionali).
   typedef std::vector<MultiGaussian>
                        TermVector;

   // Input size.
   NaturalType          mInputSize;

   // Vettore di termini.
   TermVector           mTerms;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mInputSize);
      ar & BOOST_SERIALIZATION_NVP(mTerms);
   } // BOOST SERIALIZATION

}; // class MultiGaussianAntecedent

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename SequenceContainer>
void
MultiGaussianAntecedent::Eval(
                            const BoostRealVector& rInput,
                            SequenceContainer&     rOutput) const
{
   #if SPARE_DEBUG
   if (rInput.size() != mInputSize)
   {
      throw SpareLogicError("MultiGaussianAntecedent, 4, Input of invalid size.");
   }

   if (rOutput.size() != mTerms.size())
   {
      throw SpareLogicError("MultiGaussianAntecedent, 5, Output of invalid size.");
   }
   #endif

   // Iteratore termini.
   TermVector::const_iterator Mit;

   // Iteratore a container output.
   typename SequenceContainer::iterator
                              Oit= rOutput.begin();

   // Calcolo membership.
   Mit= mTerms.begin();
   while (mTerms.end() != Mit)
   {
      (*Oit++)= (*Mit++).Eval(rInput);
   }
}

template <typename SequenceContainer1, typename SequenceContainer2>
void
MultiGaussianAntecedent::Eval(
                            const SequenceContainer1& rInput,
                            SequenceContainer2&       rOutput) const
{
   #if SPARE_DEBUG
   if (rInput.size() != mInputSize)
   {
      throw SpareLogicError("MultiGaussianAntecedent, 6, Input of invalid size.");
   }

   if (rOutput.size() != mTerms.size())
   {
      throw SpareLogicError("MultiGaussianAntecedent, 7, Output of invalid size.");
   }
   #endif

   // Iteratore termini.
   TermVector::const_iterator Mit;

   // Iteratori a primo container.
   typename SequenceContainer1::const_iterator
                        Sit1b= rInput.begin();

   typename SequenceContainer1::const_iterator
                        Sit1e= rInput.end();

   // Iteratore a secondo container.
   typename SequenceContainer2::iterator
                        Sit2= rOutput.begin();

   // Calcolo membership.
   Mit= mTerms.begin();
   while (mTerms.end() != Mit)
   {
      (*Sit2++)= (*Mit++).Eval(std::make_pair(Sit1b, Sit1e));
   }
}

template <typename ForwardIterator1, typename ForwardIterator2>
void
MultiGaussianAntecedent::Eval(
                            std::pair<ForwardIterator1, ForwardIterator1> aInput,
                            std::pair<ForwardIterator2, ForwardIterator2> aOutput) const
{
   typedef typename std::iterator_traits<ForwardIterator1>::difference_type DiffType1;
   typedef typename std::iterator_traits<ForwardIterator2>::difference_type DiffType2;

   #if SPARE_DEBUG
   if (std::distance(aInput.first, aInput.second) != boost::numeric::
                                                     converter<DiffType1, NaturalType>::
                                                     convert(mInputSize))
   {
      throw SpareLogicError("MultiGaussianAntecedent, 8, Input of invalid size.");
   }

   if (std::distance(aOutput.first, aOutput.second) != boost::numeric::
                                                       converter<DiffType2,
                                                                 TermVector::size_type>::
                                                       convert(mTerms.size()))
   {
      throw SpareLogicError("MultiGaussianAntecedent, 9, Output of invalid size.");
   }
   #endif

   // Iteratore termini.
   TermVector::const_iterator Mit;

   // Calcolo membership.
   Mit= mTerms.begin();
   while (mTerms.end() != Mit)
   {
      (*aOutput.first++)= (*Mit++).Eval(aInput);
   }
}

}  // namespace spare

#endif  // _MultiGaussianAntecedent_h_
