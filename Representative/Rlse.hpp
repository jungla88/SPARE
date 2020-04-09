//  Rlse class, part of the SPARE library.
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

/** @brief File Rlse.hpp, containing the Rlse class.
 *
 * The file contains the Rlse class, implementing a cluster model based on an approximating
 * hyperplane.
 *
 * @file Rlse.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Rlse_h_
#define _Rlse_h_

// STD INCLUDES
#include <cmath>
#include <iterator>
#include <utility>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Recursive Least Squares Estimation.
 *
 * This class models the @a Representative concept. This is a cluster model based on linear 
 * regression. It is mainly intended for building fuzzy function approximation models using
 * the HyperplaneConsequent class as the output stage, but it can be used for incremental
 * linear regression problems in general. An \f$n\f$-dimensional input sample is regarded as 
 * a point in the euclidean space \f$R^{n-1} \times R\f$, and it is denoted as \f$(x_0, x_1, 
 * ..., x_{n-2}, y)\f$. A cluster of points is represented by the hyperplane which satisfies, 
 * in the least squares error sense, the relation \f$y=a_0 x_0 + a_1 x_1 + ... + a_{n-2} 
 * x_{n_2} + b\f$. The coefficients \f$a_0, a_1, ...,a_{n-2}\f$ and \f$b\f$ are incrementally 
 * estimated by the insertion of new points in the representative by the Update method. The 
 * dissimilarity measure is based on the distance of the point from the estimated hyperplane.
 * @todo BatchUpdate implementation.
 * @todo Merge implementation.
 */
class Rlse
{
public:

// LIFECYCLE

   /** Default constructor.
    */
   Rlse()
      : mCount(0),
        mLambda(1.),
        mAlpha(1e+6)
                                                   { }

// OPERATIONS

   /** Coefficients update.
    *
    * @param[in] rSample A reference to the boost vector holding the new sample.
    */
   void                 Update(const BoostRealVector& rSample);

   /** Coefficients update.
    *
    * @param[in] aSample Iterator pair delimiting the new sample.
    */
   template <typename ForwardIterator>
   void                 Update(std::pair<ForwardIterator, ForwardIterator> aSample);

   /** Coefficients update.
    *
    * @param[in] rSample A reference to the container holding the new sample.
    */
   template <typename SequenceContainer>
   void                 Update(const SequenceContainer& rSample);

   /** Distance between the point passed as argument and the current hyperplane.
    *
    * @param[in] rSample A reference to the boost vector holding the point.
    * @return The distance value.
    */
   RealType             Diss(const BoostRealVector& rSample) const;

   /** Distance between the point passed as argument and the current hyperplane.
    *
    * @param[in] aSample Iterator pair delimiting the point.
    * @return The distance value.
    */
   template <typename ForwardIterator>
   RealType             Diss(std::pair<ForwardIterator, ForwardIterator> aSample) const;

   /** Distance between the point passed as argument and the current hyperplane.
    *
    * @param[in] rSample A reference to the container holding the sample.
    * @return The distance value.
    */
   template <typename SequenceContainer>
   RealType             Diss(const SequenceContainer& rSample) const;

// ACCESS

   /** Read only access to the coefficients of the hyperplane.
    *
    * @return A const reference to the vector of coefficients.
    */
   const BoostRealVector&
                        GetHyperplane() const      { return mW; }

   /** Read access to the number of samples inserted so far.
    *
    * @return The number of samples.
    */
   NaturalType          GetCount() const           { return mCount; }

private:

   // Coefficienti iperpiano.
   BoostRealVector      mW;

   // Matrice P.
   BoostRealSymmMatrix  mP;

   // Conteggio dei campioni.
   NaturalType          mCount;

   // Lambda.
   RealType             mLambda;

   // Alpha.
   RealType             mAlpha;

   // Ausiliarie.
   mutable BoostRealVector
                        mTemp1;
   mutable BoostRealVector
                        mTemp2;
   mutable BoostRealVector
                        mTemp3;
   mutable BoostRealSymmMatrix
                        mTemp4;

   // Update (rInput ha già l'1 come ultima componente)
   void                 Update(
                           const BoostRealVector& rInput,
                           RealType               aOutput);

   // DissOrtho (rInput ha già l'1 come ultima componente)
   RealType             DissOrtho(
                           const BoostRealVector& rInput,
                           RealType               aOutput) const;

   // DissY (rInput ha già l'1 come ultima componente)
   RealType             DissY(
                           const BoostRealVector& rInput,
                           RealType               aOutput) const;

}; // class Rlse

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

inline void
Rlse::Update(
         const BoostRealVector& rInput,
         RealType               aOutput)
{
   // Dichiarazioni.
   RealType             E, D;
   BoostRealSymmMatrix::size_type
                        i, j;

   // Se è il primo aggiornamento faccio inizializzazioni.
   if (!mCount)
   {
      mW.resize(rInput.size());
      mP.resize(rInput.size());
      mTemp1.resize(rInput.size());
      mTemp3.resize(rInput.size());
      mTemp4.resize(rInput.size());
      for (i= 0; i < mP.size1(); ++i)
      {
         mW[i]= 0.;
         for (j= 0; j <= i; ++j)
         {
            if (i == j)
            {
               mP(i, j)= mAlpha;
            }
            else
            {
               mP(i, j)= 0.;
            }
         }
      }
   }

   // Aggiorno.
   E= aOutput - inner_prod(mW, rInput);
   mTemp1= prod(rInput, mP);
   D= 1. / (mLambda + inner_prod(mTemp1, rInput));
   mTemp3= (mTemp1 * D);
   mTemp4= outer_prod(mTemp3, mTemp1);
   mP-= mTemp4;
   mP/= mLambda;
   mW+= (mTemp3 * E);

   // Incremento count.
   ++mCount;
}

inline void
Rlse::Update(const BoostRealVector& rSample)
{
   RealType Y;

   #if SPARE_DEBUG
   if (!mCount)
   {
      if (rSample.size() < 2)
      {
         throw SpareLogicError("Rlse, 3, Invalid input size.");
      }
   }
   else
   {
      if (rSample.size() != mW.size())
      {
         throw SpareLogicError("Rlse, 3, Different lenghts.");
      }
   }
   #endif

   mTemp2= rSample;
   Y= *mTemp2.rbegin();
   *mTemp2.rbegin()= 1.;

   Update(mTemp2, Y);
}

template <typename ForwardIterator>
void
Rlse::Update(std::pair<ForwardIterator, ForwardIterator> aSample)
{
   typename std::iterator_traits<ForwardIterator>::difference_type
                              Diff= std::distance(aSample.first, aSample.second);

   #if SPARE_DEBUG
   if (!mCount)
   {
      if (Diff < 2)
      {
         throw SpareLogicError("Rlse, 3, Invalid input size.");
      }
   }
   else
   {
      if (Diff != boost::numeric::converter<
                  typename std::iterator_traits<ForwardIterator>::difference_type,
                  BoostRealVector::size_type>
                  ::convert(mW.size()))
      {
         throw SpareLogicError("Rlse, 3, Different lenghts.");
      }
   }
   #endif

   BoostRealVector::size_type Sz= Diff;
   mTemp2.resize(Sz);
   RealType                   Y;
   BoostRealVector::iterator  Mit= mTemp2.begin();

   while (--Sz)
   {
      (*Mit++)= *aSample.first++;
   }
   *Mit= 1.;
   Y= *aSample.first;

   Update(mTemp2, Y);
}

template <typename SequenceContainer>
void
Rlse::Update(const SequenceContainer& rSample)
{
   #if SPARE_DEBUG
   if (!mCount)
   {
      if (rSample.size() < 2)
      {
         throw SpareLogicError("Rlse, 3, Invalid input size.");
      }
   }
   else
   {
      if (rSample.size() != mW.size())
      {
         throw SpareLogicError("Rlse, 3, Different lenghts.");
      }
   }
   #endif

   BoostRealVector::size_type                 Sz= rSample.size();
   mTemp2.resize(Sz);
   RealType                                   Y;
   typename SequenceContainer::const_iterator Sit= rSample.begin();
   BoostRealVector::iterator                  Mit= mTemp2.begin();

   while (--Sz)
   {
      (*Mit++)= *Sit++;
   }
   *Mit= 1.;
   Y= *Sit;

   Update(mTemp2, Y);
}

inline RealType
Rlse::DissOrtho(
       const BoostRealVector& rInput,
       RealType               aOutput) const
{
   // Dichiarazioni.
   RealType                        D, E;
   BoostRealVector::size_type      Sz;
   BoostRealVector::const_iterator Mit;

   E= aOutput - inner_prod(mW, rInput);
   Mit= mW.begin();
   Sz= mW.size();
   D= 1.;
   while (--Sz)
   {
      D+= std::pow(*Mit++, 2);
   }

   return std::fabs(E) / std::sqrt(D);
}

inline RealType
Rlse::DissY(
       const BoostRealVector& rInput,
       RealType               aOutput) const
{
   RealType E= aOutput - inner_prod(mW, rInput);
   return std::fabs(E);
}

inline RealType
Rlse::Diss(const BoostRealVector& rSample) const
{
   RealType Y;

   if (!mCount)
   {
      throw SpareLogicError("Rlse, 3, Uninitialized object.");
   }

   #if SPARE_DEBUG
   if (rSample.size() != mW.size())
   {
      throw SpareLogicError("Rlse, 3, Different lenghts.");
   }
   #endif

   mTemp2= rSample;
   Y= *mTemp2.rbegin();
   *mTemp2.rbegin()= 1.;

   return DissY(mTemp2, Y);
}

template <typename ForwardIterator>
RealType
Rlse::Diss(std::pair<ForwardIterator, ForwardIterator> aSample) const
{
   typename std::iterator_traits<ForwardIterator>::difference_type
                              Diff= std::distance(aSample.first, aSample.second);

   if (!mCount)
   {
      throw SpareLogicError("Rlse, 3, Uninitialized object.");
   }

   #if SPARE_DEBUG
   if (Diff != boost::numeric::converter<
               typename std::iterator_traits<ForwardIterator>::difference_type,
               BoostRealVector::size_type>
               ::convert(mW.size()))
   {
      throw SpareLogicError("Rlse, 3, Different lenghts.");
   }
   #endif

   BoostRealVector::size_type Sz= Diff;
   mTemp2.resize(Sz);
   RealType                   Y;
   BoostRealVector::iterator  Mit= mTemp2.begin();

   while (--Sz)
   {
      (*Mit++)= *aSample.first++;
   }
   *Mit= 1.;
   Y= *aSample.first;

   return DissY(mTemp2, Y);
}

template <typename SequenceContainer>
RealType
Rlse::Diss(const SequenceContainer& rSample) const
{
   if (!mCount)
   {
      throw SpareLogicError("Rlse, 3, Uninitialized object.");
   }

   #if SPARE_DEBUG
   if (rSample.size() != mW.size())
   {
      throw SpareLogicError("Rlse, 3, Different lenghts.");
   }
   #endif

   BoostRealVector::size_type                 Sz= rSample.size();
   mTemp2.resize(Sz);
   RealType                                   Y;
   typename SequenceContainer::const_iterator Sit= rSample.begin();
   BoostRealVector::iterator                  Mit= mTemp2.begin();

   while (--Sz)
   {
      (*Mit++)= *Sit++;
   }
   *Mit= 1.;
   Y= *Sit;

   return DissY(mTemp2, Y);
}

}  // namespace spare

#endif  // _Rlse_h_
