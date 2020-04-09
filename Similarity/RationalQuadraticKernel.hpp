//  RationalQuadraticKernel class, part of the SPARE library.
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

/** @brief File RationalQuadraticKernel.hpp, that contains RationalQuadraticKernel class.
 *
 * Contains the declaration of the Rational Quadratic Kernel class.
 *
 * @file RationalQuadraticKernel.hpp
 * @author Lorenzo Livi
 */

#ifndef RationalQuadraticKERNEL_H_
#define RationalQuadraticKERNEL_H_

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief Rational Quadratic Kernel Function.
 *
 * This class implements the @a Similarity concept.
 * This class contains functions for the computation of the similarity between two objets using a Rational Quadratic kernel function.
 * It is a template kernel function that is based on @a Dissimilarity function between the input data.
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
 *     <td class="indexvalue">c</td>
 *     <td class="indexvalue">(0, infinity)</td>
 *     <td class="indexvalue">Kernel size parameter</td>
 *     <td class="indexvalue">-</td>
 *     <td class="indexvalue">1</td>
 *  </tr>
 *  </table>
 */
template <class Dissimilarity>
class RationalQuadraticKernel {
public:

    /**
     * Default constructor.
     */
    RationalQuadraticKernel()
    {
        mSigma=1;
    }


    /** Similarity computation.
     *
     * @param[in] o1 A reference to the first generic object.
     * @param[in] o2 A reference to the second generic object.
     * @return The similarity value.
     */
    template <typename Type>
    RealType Sim(const Type& o1, const Type& o2) const;

    /**
     * Read/write access to the c parameter (kernel size).
     *
     * @return A reference to the value of c
     */
    RealType& Sigma(){ return mSigma; }

    /**
     * Read-only access to the c parameter (kernel size).
     * @return A reference to the value of c
     */
    const RealType& Sigma() const { return mSigma; }

    /**
     * Read/write access to the Dissimilarity agent.
     * @return A reference to the dissimilarity agent
     */
    Dissimilarity& DissimilarityAgent(){ return mDiss; }

    /**
     * Read-only access to the Dissimilarity agent.
     * @return A reference to the dissimilarity agent
     */
    const Dissimilarity& DissimilarityAgent() const { return mDiss; }

private:

    /**
     * Dissimilarity measure
     */
    Dissimilarity mDiss;

    /**
     * Sigma parameter
     */
    RealType mSigma;
};


//IMPLEMENTATION


template <class Dissimilarity>
template <typename Type>
RealType
RationalQuadraticKernel<Dissimilarity>::Sim(const Type& o1, const Type& o2) const
{
    RealType d, dd;

    d=mDiss.Diss(o1, o2);
    dd=d*d;

    return 1.0-(dd/(dd+mSigma));
}


} // namespace

#endif /* RationalQuadraticKERNEL_H_ */
