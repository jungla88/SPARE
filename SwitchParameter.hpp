//  SwitchParameter class, part of the SPARE library.
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

/** @brief File SwitchParameter.hpp, that contains the SwitchParameter class.
 *
 * Contains the declaration of the SwitchParameter class.
 *
 * @file SwitchParameter.hpp
 * @author Guido Del Vescovo
 */

#ifndef _SwitchParameter_h_
#define _SwitchParameter_h_

// STD INCLUDES
#include <set>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Switch parameter class.
 *
 * A switch paramter is an object capable of switch between a set to allowed user-defined values of ValueType type.
 */
template <typename ValueType>
class SwitchParameter
{
public:

// PUBLIC TYPES

   /** Container type for allowed values.
    */
   typedef std::set<ValueType>
                        ValueSet;

// LIFECYCLE

   /** Constructor that takes the allowed values from a container.
    *
    * @param[in] iValueBegin Iterator pointing to the first element in the container.
    * @param[in] iValueEnd Iterator pointing to the first position after the last element in the container.
    */
   template <typename ForwardIterator>
   SwitchParameter(
      ForwardIterator iValueBegin,
      ForwardIterator iValueEnd);

// OPERATORS

   /** Assignment operator.
    *
    * @param[in] rValue A reference to the ValueType to assign.
    * @return A reference to the current instance.
    */
   template <typename ValueType_>
   SwitchParameter<ValueType>&
                        operator=(const ValueType_& rValue);

   /** Assignment operator.
    *
    * @param[in] rSource Value to assign.
    * @return A reference to the current instance.
    */
   SwitchParameter<ValueType>&
                        operator=(const SwitchParameter<ValueType>& rSource)
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

   /** Conversion routine to ValueType.
    *
    * @return The parameter value.
    */
   operator             ValueType() const          { return mValue; }

// ACCESS

   /** Read access to the set of allowed values.
    *
    * @return A reference to the set of allowed values.
    */
   const ValueSet&      SwitchValues() const       { return mSwitchValues; }

private:

   // Vettore contenente i possibili valori assunti.
   const ValueSet       mSwitchValues;

   // Valore corrente.
   ValueType            mValue;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mValue);
   } // BOOST SERIALIZATION

}; // class SwitchParameter

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== LIFECYCLE ===========================================

template <typename ValueType>
template <typename ForwardIterator>
SwitchParameter<ValueType>::SwitchParameter(
                                        ForwardIterator iValueBegin,
                                        ForwardIterator iValueEnd)
                                        : mSwitchValues(iValueBegin, iValueEnd)
{
   if ( !mSwitchValues.empty() )
   {
      mValue= *mSwitchValues.begin();
   }
   else
   {
      throw SpareLogicError("SwitchParameter, 0, Empty value container.");
   }
}  // SwitchParameter

//==================================== OPERATORS ===========================================

template <typename ValueType>
template <typename ValueType_>
SwitchParameter<ValueType>&
SwitchParameter<ValueType>::operator=(const ValueType_& rValue)
{
   if ( mSwitchValues.find( static_cast<ValueType>(rValue) ) == mSwitchValues.end() )
   {
      throw SpareLogicError("SwitchParameter, 1, Invalid parameter value (not in the set "
                            "of possible values).");
   }

   mValue= static_cast<ValueType>(rValue);

   return *this;
}  // operator=

}  // namespace spare

#endif  // _SwitchParameter_h_
