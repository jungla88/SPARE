//  MultiGaussian class, part of the SPARE library.
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

/** @brief File MultiGaussian.hpp, containing the MultiGaussian class.
 *
 * The file contains the MultiGaussian class, implementing a multivariate gaussian function.
 *
 * @file MultiGaussian.hpp
 * @author Guido Del Vescovo
 */

#ifndef _MultiGaussian_h_
#define _MultiGaussian_h_

// STD INCLUDES
#include <cmath>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>

// BOOST INCLUDES
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief Multivariate gaussian function.
 *
 * This class models the @a Evaluator concept. The class implements a multivariate gaussian
 * function in the \f$R^n\f$ domain. The function setup requires the pair \f$\langle
 * \mathbf{x}, C^{-1}\rangle\f$, where \f$\mathbf{x}\f$ is the mean vector and \f$C^{-1}\f$ 
 * is the inverse covariance matrix. The coefficients defining the \f$\langle\mathbf{x}, 
 * C^{-1}\rangle\f$ pair could be estimated from a data set using the Mahalanobis class.
 * Otherwise, the user is in charge to ensure that the \f$C^{-1}\f$ matrix represents the
 * inverse of a covariance matrix, no check is performed.
 */
class MultiGaussian
{
public:

// LIFECYCLE

   /** Default constructor.
    */
   MultiGaussian()
      : mM(0)
                                                   { }

// OPERATIONS

   /** Function evaluation.
    *
    * @param[in] rInput Container holding the input vector.
    * @return The function value.
    */
   template <typename SequenceContainer>
   RealType             Eval(const SequenceContainer& rInput) const;

   /** Functin evaluation.
    *
    * @param[in] aInput Pair of iterators delimiting the input vector.
    * @return The function value.
    */
   template <typename ForwardIterator>
   RealType             Eval(std::pair<ForwardIterator, ForwardIterator> aInput) const;

   /** Function evaluation.
    *
    * @param[in] rInput A boost input vector.
    * @return The function value.
    */
   RealType             Eval(const BoostRealVector& rInput) const;

// SETUP

   /** Input size setup, it must be done before the parameter setup.
    *
    * @param[in] aInputSize Input size.
    */
   void                 InputSizeSetup(NaturalType aInputSize)
                           {
                              if (aInputSize > mMean.max_size())
                              {
                                 throw SpareLogicError("MultiGaussian, 0, "
                                                       "Input size is too large.");
                              }

                              ClearAll();

                              mMean.resize(
                                        boost::numeric
                                        ::converter<BoostRealVector::size_type, NaturalType>
                                        ::convert(aInputSize));

                              mInput.resize(mMean.size());
                              mTemp1.resize(mMean.size());
                              mTemp2.resize(mMean.size());

                              mM= boost::numeric
                                  ::converter<BoostRealSymmMatrix::size_type, NaturalType>
                                  ::convert(aInputSize);

                              mInvCov.resize(mM);
                           }

   /** Multivariate gaussian function setup by boost vector and matrix.
    *
    * @param[in] rMean Mean vector.
    * @param[in] rInvCov Inverse covariance matrix.
    */
   void                 ParamSetup(
                              const BoostRealVector&     rMean,
                              const BoostRealSymmMatrix& rInvCov)
                           {
                              if (rMean.size() != mM)
                              {
                                 throw SpareLogicError("MultiGaussian, 1, Size mismatch.");
                              }

                              if (rInvCov.size1() != mM)
                              {
                                 throw SpareLogicError("MultiGaussian, 2, Size mismatch.");
                              }

                              mMean= rMean;
                              mInvCov= rInvCov;
                           }

   /** Multivariate gaussian function setup by parameter sequence.
    *
    * The parameter sequence format is: first the mean vector \f$\mathbf{x}\f$ then the 
    * distinct coefficients of the inverse covariance matrix \f$C^{-1}\f$, in the order 
    * illustrated by the example figure below:
    *
    * \f$
    * \begin{array}{ccc}
    * 1 & * & * \\
    * 2 & 3 & * \\
    * 4 & 5 & 6
    * \end{array}
    * \f$
    *
    * @param[in] rParams A reference to the container holding the parameter sequence.
    */
   template <typename SequenceContainer>
   void                 ParamSetup(const SequenceContainer& rParams);

   /** Multivariate gaussian function setup by parameter sequence.
    *
    * See the overloaded version for more explanations.
    *
    * @param[in] aParams A pair of iterators delimiting the parameter sequence.
    */
   template <typename ForwardIterator>
   void                 ParamSetup(std::pair<ForwardIterator, ForwardIterator> aParams);

private:

   // Dimensione input.
   BoostRealSymmMatrix::size_type
                        mM;

   // Mean vector.
   BoostRealVector      mMean;

   // Inversa della matrice di covarianza.
   BoostRealSymmMatrix  mInvCov;

   // Vettori ausiliari per calcoli.
   mutable BoostRealVector
                        mInput;
   mutable BoostRealVector
                        mTemp1;
   mutable BoostRealVector
                        mTemp2;

   // Funzione reset.
   void                 ClearAll()
                           {
                              mMean.clear();
                              mInput.clear();
                              mTemp1.clear();
                              mTemp2.clear();
                              mInvCov.clear();
                              mM= 0;
                           }

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void save(Archive & ar, const unsigned int version) const
   {
      ar & BOOST_SERIALIZATION_NVP(mMean);

      std::ostringstream StrBuff;
      StrBuff << mInvCov;
      std::string InvCovDesc= StrBuff.str();

      ar & BOOST_SERIALIZATION_NVP(InvCovDesc);
   }

   template<class Archive>
   void load(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mMean);

      std::string InvCovDesc;
      ar & BOOST_SERIALIZATION_NVP(InvCovDesc);
      std::istringstream StrBuff;
      StrBuff.str(InvCovDesc);
      StrBuff >> mInvCov;

      if (mInvCov.size1() != mMean.size())
      {
         throw SpareLogicError("MultiGaussian, 3, Loaded data is invalid.");
      }

      mInput.resize(mMean.size());
      mTemp1.resize(mMean.size());
      mTemp2.resize(mMean.size());
   }

   BOOST_SERIALIZATION_SPLIT_MEMBER() // BOOST SERIALIZATION

}; // class MultiGaussian

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

inline RealType
MultiGaussian::Eval(const BoostRealVector& rInput) const
{
   #if SPARE_DEBUG
   if (rInput.size() != mM)
   {
      throw SpareLogicError("MultiGaussian, 4, Input of invalid size.");
   }
   #endif

   mTemp1= rInput - mMean;
   mTemp2= prod(mInvCov, mTemp1);

   return std::exp(-0.5 * inner_prod(mTemp1, mTemp2));
}

template <typename SequenceContainer>
RealType
MultiGaussian::Eval(const SequenceContainer& rInput) const
{
   #if SPARE_DEBUG
   if (rInput.size() != mM)
   {
      throw SpareLogicError("MultiGaussian, 5, Input of invalid size.");
   }
   #endif

   BoostRealVector::iterator                  Iit= mInput.begin();
   typename SequenceContainer::const_iterator Cit= rInput.begin();

   while (mInput.end() != Iit)
   {
      (*Iit++)= *Cit++;
   }

   mTemp1= mInput - mMean;
   mTemp2= prod(mInvCov, mTemp1);

   return std::exp(-0.5 * inner_prod(mTemp1, mTemp2));
}

template <typename ForwardIterator>
RealType
MultiGaussian::Eval(std::pair<ForwardIterator, ForwardIterator> aInput) const
{
   #if SPARE_DEBUG
   if (std::distance(aInput.first, aInput.second) !=
       boost::numeric::converter<
       typename std::iterator_traits<ForwardIterator>::difference_type,
       BoostRealSymmMatrix::size_type>
       ::convert(mM))
   {
      throw SpareLogicError("MultiGaussian, 7, Input of invalid size.");
   }
   #endif

   BoostRealVector::iterator Iit= mInput.begin();

   while (mInput.end() != Iit)
   {
      (*Iit++)= *aInput.first++;
   }

   mTemp1= mInput - mMean;
   mTemp2= prod(mInvCov, mTemp1);

   return std::exp(-0.5 * inner_prod(mTemp1, mTemp2));
}

//==================================== SETUP ===============================================

template <typename ForwardIterator>
void
MultiGaussian::ParamSetup(std::pair<ForwardIterator, ForwardIterator> aParams)
{
   // Dichiarazioni.
   BoostRealSymmMatrix::size_type   i, j, k;
   BoostRealVector::iterator        Mit;

   // Numero parametri attesi.
   k= mM + (mM*(mM+1))/2;

   #if SPARE_DEBUG
   if (std::distance(aParams.first, aParams.second) !=
       boost::numeric::converter<IntegerType, BoostRealSymmMatrix::size_type>
       ::convert(k))
   {
      throw SpareLogicError("MultiGaussian, 9, Invalid number of parameters.");
   }
   #endif

   // Leggo mean vector.
   k= mM;
   Mit= mMean.begin();
   while (k--)
   {
      (*Mit++)= static_cast<RealType>(*aParams.first++);
   }

   // Leggo matrice InvCov.
   k= mM;
   for (i= 0; i < k; i++)
   {
      for (j= 0; j <= i; j++)
      {
         mInvCov(i, j)= static_cast<RealType>(*aParams.first++);
      }
   }
}

template <typename SequenceContainer>
void
MultiGaussian::ParamSetup(const SequenceContainer& rParams)
{
   // Dichiarazioni.
   BoostRealSymmMatrix::size_type   i, j, k;
   BoostRealVector::iterator        Mit;
   typename SequenceContainer::const_iterator
                                    Sit= rParams.begin();

   // Numero parametri attesi.
   k= mM + (mM*(mM+1))/2;

   #if SPARE_DEBUG
   if (rParams.size() != k)
   {
      throw SpareLogicError("MultiGaussian, 10, Invalid number of parameters.");
   }
   #endif

   // Leggo mean vector.
   k= mM;
   Mit= mMean.begin();
   while (k--)
   {
      (*Mit++)= static_cast<RealType>(*Sit++);
   }

   // Leggo matrice InvCov.
   k= mM;
   for (i= 0; i < k; i++)
   {
      for (j= 0; j <= i; j++)
      {
         mInvCov(i, j)= static_cast<RealType>(*Sit++);
      }
   }
}

}  // namespace spare

#endif  // _MultiGaussian_h_
