//  PolynomialKernel class, part of the SPARE library.
//  Copyright (C) 2011 Lorenzo Livi
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

/** @brief File PolynomialKernel.hpp, that contains Polynomial Kernel class.
 *
 * Contains the declaration of the PolynomialKernel class.
 *
 * @file PolynomialKernel.hpp
 * @author Lorenzo Livi
 */

#ifndef _PolynomialKernel_h_
#define _PolynomialKernel_h_

// STD INCLUDES
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

// BOOST INCLUDES

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusione in namespace spare.

/** @brief Polynomial kernel similarity.
 *
 * This class implements the @a Similarity concept.
 * This class contains static functions for the computation of the polynomial kernel similarity between real vectors.
 *
 * <b>Parameter summary</b>
 *  <table class="contents">
 *  <tr>
 *     <td class="indexkey"><b>Name</b></td>
 *     <td class="indexkey"><b>Domain</b></td>
 *     <td class="indexkey"><b>Description</b></td>
 *     <td class="indexkey"><b>Const</b></td>
 *     <td class="indexkey"><b>Default</b></td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">v</td>
 *     <td class="indexvalue">[0, infinity)</td>
 *     <td class="indexvalue"></td>
 *     <td class="indexvalue">-</td>
 *     <td class="indexvalue">0</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">d</td>
 *     <td class="indexvalue">[0, infinity)</td>
 *     <td class="indexvalue">Degree of the polynomial</td>
 *     <td class="indexvalue">-</td>
 *     <td class="indexvalue">2</td>
 *  </tr>
 *  </table>
 */
class PolynomialKernel
{
public:

    /**
     * Constructor.
     */
    PolynomialKernel()
    {
        v=0;
        d=2;
    }


// OPERATIONS

    /**
     * Polynomial kernel computation.
     *
     * @param[in] aA A pair of iterators of the first vector.
     * @param[in] aB A pair of iterators of the second vector.
     * @return The similarity value.
     */
    template <typename ForwardIterator1, typename ForwardIterator2>
    RealType Sim(std::pair<ForwardIterator1, ForwardIterator1> aA,
            std::pair<ForwardIterator2, ForwardIterator2> aB) const;

    /**
     * Polynomial kernel computation.
     *
     * @param[in] rA A reference to the container of the first vector.
     * @param[in] rB A reference to the container of the second vector.
     * @return The similarity value.
     */
    template <typename SequenceContainer1, typename SequenceContainer2>
    RealType Sim(const SequenceContainer1& rA, const SequenceContainer2& rB) const;

    /**
     * Read/Write access to the v parameter
     *
     * @return A reference to the value of v
     */
    RealType& V() { return v; }

    /**
     * Read-only access to the v parameter
     *
     * @return A reference to the value of v
     */
    const RealType& V() const { return v; }

    /**
     * Read/Write access to the d parameter
     *
     * @return A reference to the value of d
     */
    RealType& D() { return d; }

    /**
     * Read-only access to the d parameter
     *
     * @return A reference to the value of d
     */
    const RealType& D() const { return d; }

private:

    /**
     * V parameter
     */
    RealType v;

    /**
     * D parameter
     */
    RealType d;

}; // class PolynomialKernel

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
RealType
PolynomialKernel::Sim(std::pair<ForwardIterator1, ForwardIterator1> aA,
        std::pair<ForwardIterator2, ForwardIterator2> aB) const
{
    // Variabili.
    RealType                         S= 0, InnerProd= 0;

    // Controllo.
    #if SPARE_DEBUG
    // Typedef locali.
    typedef typename std::iterator_traits<ForwardIterator1>::difference_type
                                             DiffTypeA;

    typedef typename std::iterator_traits<ForwardIterator2>::difference_type
                                             DiffTypeB;

    DiffTypeA DistA= std::distance(aA.first, aA.second);
    DiffTypeB DistB= std::distance(aB.first, aB.second);

    if ( (DistA < 0) || (DistB < 0) )
    {
        throw SpareLogicError("PolynomialKernel, 0, Invalid range.");
    }

    if (DistA != DistB)
    {
        throw SpareLogicError("PolynomialKernel, 1, Different lengths between inputs.");
    }
    #endif


    while (aA.first != aA.second)
    {
        InnerProd+=static_cast<RealType>(*aA.first++)*static_cast<RealType>(*aB.first++);
    }

    S= std::pow(InnerProd+v, d);

    return S;
}  // Sim


template <typename SequenceContainer1, typename SequenceContainer2>
RealType
PolynomialKernel::Sim(const SequenceContainer1& c1, const SequenceContainer2& c2) const
{
    // Controllo.
    #if SPARE_DEBUG
    if ( c1.size() != c1.size() )
    {
        throw SpareLogicError("PolynomialKernel, 3, Different lenghts between inputs.");
    }
       #endif


    return Sim(std::make_pair(c1.begin(), c1.end()), std::make_pair(c2.begin(), c2.end()));
}  // Sim


}  // namespace spare

#endif  // _PolynomialKernel_h_
