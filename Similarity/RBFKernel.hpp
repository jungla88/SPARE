//  RBFKernel class, part of the SPARE library.
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

/** @brief File RBFKernel.hpp, that contains RBFKernel class.
 *
 * Contains the declaration of the Gaussian RBFKernel class.
 *
 * @file RBFKernel.hpp
 * @author Lorenzo Livi
 */

#ifndef RBFKERNEL_H_
#define RBFKERNEL_H_

//STD INCLUDES
#include <cmath>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {  // Inclusion in namespace spare.

/** @brief Radial Basis Gaussian Kernel Function.
 *
 * This class implements the @a Similarity concept.
 * This class contains functions for the computation of the similarity between two objects using a kernel derived from the Gaussian Radial Basis Function (RBF).
 * It is a template kernel function that is based on @a Dissimilarity tailored for the input data.
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
 *     <td class="indexvalue">sigma</td>
 *     <td class="indexvalue">[0, infinity)</td>
 *     <td class="indexvalue">kernel size.</td>
 *     <td class="indexvalue">-</td>
 *     <td class="indexvalue">1</td>
 *  </tr>
 *  </table>
 */
template <class Dissimilarity>
class RBFKernel {
public:

    /**
     * Default constructor.
     */
    RBFKernel()
    {
        mSigma=1.0;
    }

    /**
     * Constructor.
     * @param[in] s The value for the sigma parameter.
     */
    RBFKernel(RealType s)
    {
        mSigma=s;
    }


    /** Similarity computation.
     *
     * @param[in] o1 A reference to the first generic object.
     * @param[in] o2 A reference to the second generic object.
     * @return The similarity value.
     */
    template <typename Type>
    RealType Sim(const Type& o1, const Type& o2) const
    {
        return std::exp(-(std::pow(mDiss.Diss(o1, o2), 2.0)/(2.0*mSigma*mSigma)));
    }

    /**
     * Read/write access to the sigma parameter.
     *
     * @return A reference to the value of sigma
     */
    RealType& Sigma(){ return mSigma; }

    /**
     * Read-only access to the sigma parameter.
     * @return A reference to the value of sigma
     */
    const RealType& Sigma() const { return mSigma; }

    /**
     * Read/write access to the Dissimilarity agent.
     * @return A reference to the dissimilarity agent
     */
    Dissimilarity& DissimilarityAgent() { return mDiss; }

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
     * RBF Kernel parameter
     */
    RealType mSigma;
};


} // namespace

#endif /* RBFKERNEL_H_ */
