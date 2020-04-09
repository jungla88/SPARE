//  Gaussian class, part of the SPARE library.
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

/** @brief File Gaussian.hpp, that contains the Gaussian class.
 *
 * Contains the declaration of the Gaussian class.
 *
 * @file Gaussian.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Gaussian_h_
#define _Gaussian_h_

// STD INCLUDES
#include <cmath>
#include <iterator>
#include <limits>
#include <utility>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Monodimensional gaussian function.
 *
 * Monodimensional gaussian function (without the 1/(sqrt(2*pi)*stdev) factor), that implements the @a Evaluator concept.
 */
class Gaussian
{
public:

// LIFECYCLE

   /** Default constructor for a zero mean function.
    */
   Gaussian()
      : mMean(0.),
        m2Var(2.)
                                                   { }

// OPERATIONS

   /** Function evaluation.
    *
    * @param[in] aX The real input value.
    * @return The function output.
    */
   RealType             Eval(RealType aX) const
                           {
                              return std::exp(-std::pow(aX - mMean, RealType(2.)) / m2Var);
                           }

// SETUP

   /** Guassian function setup.
    *
    * @param[in] aMean Mean value.
    * @param[in] aStdDev Standard deviation.
    */
   void                 ParamSetup(
                               RealType aMean,
                               RealType aStdDev);

   /**
    * Read-write access to the standard deviation
    *
    * @return The standard deviation
    */
   RealType& StdDev() { return mStdDev; }

   /**
    * Read-only access to the standard deviation
    *
    * @return The standard deviation
    */
   const RealType& StdDev() const { return mStdDev; }

private:

   // Posizione del centro.
   RealType             mMean;

   // Varianza moltiplicata per due.
   RealType             m2Var;

   // Standard Deviation
   RealType             mStdDev;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mMean);
      ar & BOOST_SERIALIZATION_NVP(m2Var);
   } // BOOST SERIALIZATION

}; // class Gaussian

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== SETUP ===============================================

void
Gaussian::ParamSetup(
                 RealType aMean,
                 RealType aStdDev)
{
   mMean= aMean;

   if (aStdDev < std::numeric_limits<RealType>::epsilon())
   {
      throw SpareLogicError("Gaussian, 0, Invalid standard deviation parameter.");
   }

   mStdDev= aStdDev;
   m2Var= RealType(2.) * std::pow(aStdDev, RealType(2.));
}

}  // namespace spare

#endif // _Gaussian_h_
