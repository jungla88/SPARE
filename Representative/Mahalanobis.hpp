//  Mahalanobis class, part of the SPARE library.
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

/** @brief File Mahalanobis.hpp, containing the Mahalanobis class.
 *
 * The file contains the Mahalanobis class, implementing a cluster model based on a
 * multivariate gaussian function.
 *
 * @file Mahalanobis.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Mahalanobis_h_
#define _Mahalanobis_h_

// STD INCLUDES
#include <cmath>
#include <iterator>
#include <utility>

// BOOST INCLUDES
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief Cluster model based on a multivariate gaussian function.
 *
 * This class models the @a Representative concept. It is a cluster model working in the
 * \f$R^n\f$ domain. The parameters of an \f$n\f$-dimensional multivariate Gaussian are
 * estimated based on the inserted samples. The dissimilarity between a generic point and
 * the representative is the %Mahalanobis distance. The multivariate gaussian function is
 * internally represented as the pair \f$\langle\mathbf{x}, C^{-1}\rangle\f$, where 
 * \f$\mathbf{x}\f$ is the mean vector and \f$C^{-1}\f$ is the inverse covariance matrix. The
 * estimated \f$\langle\mathbf{x}, C^{-1}\rangle\f$ parameters could be used to perform the
 * setup of a MultiGaussian object.
 * @todo BatchUpdate impelementation.
 * @todo Merge impelementation.
 */
class Mahalanobis
{
public:

// LIFECYCLE

   /** Default constructor.
    */
   Mahalanobis()
      : mCount(0),
        mAlpha(1.),
        mBeta(0.99),
        mGamma(0.)
                                                   { }

// OPERATIONS

   /** Update of the representative using the boost vector interface.
    *
    * @param[in] rSample A reference to the boost vector holding the new sample.
    */
   void                 Update(const BoostRealVector& rSample);

   /** Update of the representative using the iterator pair interface.
    *
    * @param[in] aSample Iterator pair delimiting the new sample.
    */
   template <typename ForwardIterator>
   void                 Update(std::pair<ForwardIterator, ForwardIterator> aSample);

   /** Update of the representative using the sequence container interface.
    *
    * @param[in] rSample A reference to the container holding the new sample.
    */
   template <typename SequenceContainer>
   void                 Update(const SequenceContainer& rSample);

   /** Mahalanobis distance between sample and representative, boost vector interface.
    *
    * @param[in] rSample A reference to the boost vector holding the sample.
    * @return The dissimilarity value.
    */
   RealType             Diss(const BoostRealVector& rSample) const;

   /** Mahalanobis distance between sample and representative, iterator pair interface.
    *
    * @param[in] aSample Iterator pair delimiting the new sample.
    * @return The dissimilarity value.
    */
   template <typename ForwardIterator>
   RealType             Diss(std::pair<ForwardIterator, ForwardIterator> aSample) const;

   /** Mahalanobis distance between sample and representative, sequence container interface.
    *
    * @param[in] rSample A reference to the container holding the sample.
    * @return The dissimilarity value.
    */
   template <typename SequenceContainer>
   RealType             Diss(const SequenceContainer& rSample) const;

// ACCESS

   /** Read only access to the mean vector.
    *
    * @return A const reference to the mean vector.
    */
   const BoostRealVector&
                        getRepresentativeSample() const        { return mCentroid; }

   /** Read only access to the inverse covariance matrix.
    *
    * @return A const reference to the inverse covariance matrix.
    */
   const BoostRealSymmMatrix&
                        GetInvCov() const          { return mInvCov; }

   /** Read access to the number of samples inserted so far.
    *
    * @return The number of samples.
    */
   NaturalType          GetCount() const           { return mCount; }

private:

   // Centroide.
   BoostRealVector      mCentroid;

   // Matrice P modificata.
   BoostRealSymmMatrix  mP;

   // Matrice identica.
   BoostRealSymmMatrix  mI;

   // Inversa matrice di covarianza.
   BoostRealSymmMatrix  mInvCov;

   // Conteggio dei campioni.
   NaturalType          mCount;

   // Alpha.
   RealType             mAlpha;

   // Beta.
   RealType             mBeta;

   // Gamma.
   RealType             mGamma;

   // Ausiliarie.
   mutable BoostRealVector
                        mTemp1;
   mutable BoostRealSymmMatrix
                        mTemp2;
   mutable BoostRealVector
                        mTemp3;

}; // class Mahalanobis

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

inline void
Mahalanobis::Update(const BoostRealVector& rInput)
{
   // Dichiarazioni.
   RealType             D;
   BoostRealSymmMatrix::size_type
                        i, j;

   // Se è il primo aggiornamento faccio inizializzazioni.
   if (!mCount)
   {
      mCentroid.resize(rInput.size());
      mP.resize(rInput.size());
      mI.resize(rInput.size());
      mInvCov.resize(rInput.size());
      mTemp1.resize(rInput.size());
      mTemp2.resize(rInput.size());
      mTemp3.resize(rInput.size());
      for (i= 0; i < mP.size1(); ++i)
      {
         for (j= 0; j <= i; ++j)
         {
            if (i == j)
            {
               mP(i, j)= mAlpha;
               mInvCov(i, j)= 1.;
               mI(i, j)= 1.;
            }
            else
            {
               mP(i, j)= 0.;
               mInvCov(i, j)= 0.;
               mI(i, j)= 0.;
            }
         }
      }
   }

   // Controllo.
   #if SPARE_DEBUG
   if (mCentroid.size() != rInput.size())
   {
      throw SpareLogicError("Mahalanobis, 3, Different lenghts.");
   }
   #endif

   // Aggiorno centroide.
   mCentroid= (static_cast<RealType>(mCount)*mCentroid + rInput) /
              (static_cast<RealType>(mCount)+1.);

   // Incremento count.
   ++mCount;

   // Aggiorno matrice P.
   mTemp3= rInput - mCentroid;
   mTemp1= prod(mP, mTemp3);
   mTemp2= outer_prod(mTemp1, mTemp1);
   D= 1. + inner_prod(mTemp1, mTemp3);
   mTemp2/= D;
   mP-= mTemp2;

   // Aggiorno inversa matrice covarianza.
   mInvCov= mGamma * mI + (1. - mGamma) * mP * static_cast<RealType>(mCount);
   mGamma*= mBeta;
}

template <typename ForwardIterator>
void
Mahalanobis::Update(std::pair<ForwardIterator, ForwardIterator> aSample)
{
   typename std::iterator_traits<ForwardIterator>::difference_type
                             Diff= std::distance(aSample.first, aSample.second);

   #if SPARE_DEBUG
   if (Diff < 0)
   {
      throw SpareLogicError("Mahalanobis, 3, Invalid range.");
   }
   #endif

   BoostRealVector           Input(Diff);
   BoostRealVector::iterator Iit= Input.begin();

   while (Input.end() != Iit)
   {
      (*Iit++)= *aSample.first++;
   }

   Update(Input);
}

template <typename SequenceContainer>
void
Mahalanobis::Update(const SequenceContainer& rSample)
{
   BoostRealVector                            Input(rSample.size());
   BoostRealVector::iterator                  Iit= Input.begin();
   typename SequenceContainer::const_iterator Sit= rSample.begin();

   while (Input.end() != Iit)
   {
      (*Iit++)= *Sit++;
   }

   Update(Input);
}

inline RealType
Mahalanobis::Diss(const BoostRealVector& rSample) const
{
   if (!mCount)
   {
      throw SpareLogicError("Mahalanobis, 0, Uninitialized object.");
   }

   #if SPARE_DEBUG
   if (rSample.size() != mCentroid.size())
   {
      throw SpareLogicError("Mahalanobis, 3, Different lenghts.");
   }
   #endif

   RealType D;

   mTemp1= rSample - mCentroid;
   mTemp3= prod(mInvCov, mTemp1);
   D= inner_prod(mTemp1, mTemp3);

   return std::sqrt(D);
}

template <typename ForwardIterator>
RealType
Mahalanobis::Diss(std::pair<ForwardIterator, ForwardIterator> aSample) const
{
   typename std::iterator_traits<ForwardIterator>::difference_type
                             Diff= std::distance(aSample.first, aSample.second);

   #if SPARE_DEBUG
   if (Diff < 0)
   {
      throw SpareLogicError("Mahalanobis, 3, Invalid range.");
   }
   #endif

   BoostRealVector           Input(Diff);
   BoostRealVector::iterator Iit= Input.begin();

   while (Input.end() != Iit)
   {
      (*Iit++)= *aSample.first++;
   }

   return Diss(Input);
}

template <typename SequenceContainer>
RealType
Mahalanobis::Diss(const SequenceContainer& rSample) const
{
   BoostRealVector                            Input(rSample.size());
   BoostRealVector::iterator                  Iit= Input.begin();
   typename SequenceContainer::const_iterator Sit= rSample.begin();

   while (Input.end() != Iit)
   {
      (*Iit++)= *Sit++;
   }

   return Diss(Input);
}

}  // namespace spare

#endif  // _Mahalanobis_h_
