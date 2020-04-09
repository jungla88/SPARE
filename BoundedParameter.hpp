//  BoundedParameter class, part of the SPARE library.
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

/** @brief File BoundedParameter.hpp, containing the BoundedParameter class.
 *
 * The file contains the BoundedParameter template class, implementing a bounded numerical
 * value.
 *
 * @file BoundedParameter.hpp
 * @author Guido Del Vescovo
 */

#ifndef _BoundedParameter_h_
#define _BoundedParameter_h_

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief %BoundedParameter class, implementing a bounded numerical value.
 *
 * This is a simple wrapper class to a numerical value which lies into an interval. It is
 * mainly intended for use as an algorithm parameter. The main operators are overloaded and
 * a conversion operator to the ValueType is also provided, such that the object can be used
 * like a ValueType object in a transparent way. An exception is thrown on assignment of a
 * value which falls outside the bounds defined at construction time.
 */
template <typename ValueType>
class BoundedParameter
{
public:

// LIFECYCLE

   /** Constructor taking lower and upper bounds.
    *
    * @param[in] rMinValue Lower bound of the parameter.
    * @param[in] rMaxValue Upper bound of the parameter.
    */
   BoundedParameter(
      const ValueType& rMinValue,
      const ValueType& rMaxValue)
         : mMIN_VALUE(rMinValue),
           mMAX_VALUE(rMaxValue)
            {
               mValue= mMIN_VALUE;

               if (mMAX_VALUE < mMIN_VALUE)
               {
                  throw SpareLogicError("BoundedParameter, 0, Invalid parameter range.");
               }
            }

// OPERATORS

   /** Assignment operator.
    *
    * @param[in] rValue The value to assign.
    * @return A reference to the current instance.
    */
   template <typename ValueType_>
   BoundedParameter<ValueType>&
                        operator=(const ValueType_& rValue)
         {
            if ( ( mMIN_VALUE > static_cast<ValueType>(rValue) ) ||
                 ( mMAX_VALUE < static_cast<ValueType>(rValue) ) )
            {
               throw SpareLogicError("BoundedParameter, 1, Invalid parameter value (out of "
                                     "range).");
            }

            mValue= rValue;
            return *this;
         }

   /** Assignment operator.
    *
    * @param[in] rSource Object instance to assign.
    * @return A reference to the current instance.
    */
   BoundedParameter<ValueType>&
                        operator=(const BoundedParameter<ValueType>& rSource)
         {
            *this= rSource.mValue;
            return *this;
         }

   /** == operator
    */
   template <typename ValueType_>
   bool                 operator==(const ValueType_& rValue) const
                                                   { return (mValue == rValue); }

   /** != operator
    */
   template <typename ValueType_>
   bool                 operator!=(const ValueType_& rValue) const
                                                   { return (mValue != rValue); }

   /** > operator
    */
   template <typename ValueType_>
   bool                 operator>(const ValueType_& rValue) const
                                                   { return (mValue > rValue); }

   /** < operator
    */
   template <typename ValueType_>
   bool                 operator<(const ValueType_& rValue) const
                                                   { return (mValue < rValue); }

   /** >= operator
    */
   template <typename ValueType_>
   bool                 operator>=(const ValueType_& rValue) const
                                                   { return (mValue >= rValue); }

   /** <= operator
    */
   template <typename ValueType_>
   bool                 operator<=(const ValueType_& rValue) const
                                                   { return (mValue <= rValue); }

   /** Conversion operator to ValueType.
    *
    * @return The value of the parameter.
    */
   operator             ValueType() const          { return mValue; }

// ACCESS

   /** Read access to the lower bound value.
    *
    * @return A reference to the value.
    */
   const ValueType&     MinValue() const           { return mMIN_VALUE; }

   /** Read access to the upper bound value.
    *
    * @return A reference to the value.
    */
   const ValueType&     MaxValue() const           { return mMAX_VALUE; }

private:

   // Valore minimo.
   const ValueType      mMIN_VALUE;

   // Valore massimo.
   const ValueType      mMAX_VALUE;

   // Valore corrente.
   ValueType            mValue;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mValue);
   } // BOOST SERIALIZATION

}; // class BoundedParameter

}  // namespace spare

#endif  // _BoundedParameter_h_
