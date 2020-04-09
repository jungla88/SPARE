//  Spare type definitions, part of the SPARE library.
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

/** @brief File SpareTypes.hpp, that contains the main typedef.
 *
 * Here are redefined some types based on modifiers, and others that are useful to be able to indirectly modify
 * the incapsulated type. Here are also defined some safe conversion routines between various type.
 * The SPARE namespace is defined and documented here.
 *
 * @file SpareTypes.hpp
 * @author Guido Del Vescovo
 */

#ifndef _SpareTypes_h_
#define _SpareTypes_h_

// STD INCLUDES
#include <complex>

// BOOST INCLUDES
#include <boost/cstdint.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/vector.hpp>

/** @brief SPARE namespace, that contains all spare definitions.
 *
 * This is the main SPARE namespace, and contains everything is related to the SPARE.
 */
namespace spare {

/** Real data type.
 */
typedef double                   RealType;

/** Complex data type.
 */
typedef std::complex<RealType>   ComplexType;

/** Integer data type.
 */
typedef boost::int32_t           IntegerType;

/** Natural number (>= 0).
 */
typedef boost::uint32_t          NaturalType;

/** Safe conversion from RealType to IntegerType.
*/
typedef boost::numeric::converter<IntegerType, RealType> RealToInteger;

/** Safe conversion from RealType to NaturalType.
*/
typedef boost::numeric::converter<NaturalType, RealType> RealToNatural;

/** Safe conversion from RealType to ComplexType.
*/
typedef boost::numeric::converter<ComplexType, RealType> RealToComplex;

/** Safe conversion from IntegerType to RealType.
*/
typedef boost::numeric::converter<RealType, IntegerType> IntegerToReal;

/** Safe conversion from IntegerType to NaturalType.
*/
typedef boost::numeric::converter<NaturalType, IntegerType> IntegerToNatural;

/** Safe conversion from IntegerType to ComplexType.
*/
typedef boost::numeric::converter<ComplexType, IntegerType> IntegerToComplex;

/** Safe conversion from NaturalType to RealType.
*/
typedef boost::numeric::converter<RealType, NaturalType> NaturalToReal;

/** Safe conversion from NaturalType to IntegerType.
*/
typedef boost::numeric::converter<IntegerType, NaturalType> NaturalToInteger;

/** Safe conversion from NaturalType to ComplexType.
*/
typedef boost::numeric::converter<ComplexType, NaturalType> NaturalToComplex;

/** Boost real vector type.
 */
typedef boost::numeric::ublas::vector<RealType>
                        BoostRealVector;

/** Boost symmetric real matrix type.
 */
typedef boost::numeric::ublas::symmetric_matrix<RealType, boost::numeric::ublas::lower>
                        BoostRealSymmMatrix;
}  // namespace spare

#endif  // _SpareTypes_h_
