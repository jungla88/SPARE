//  ProductKernel class, part of the SPARE library.
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

/** @brief File ProductKernel.hpp, that contains ProductKernel class.
 *
 * Contains the declaration of the ProductKernel class.
 *
 * @file ProductKernel.hpp
 * @author Lorenzo Livi
 */

#ifndef PRODUCTKERNEL_HPP_
#define PRODUCTKERNEL_HPP_

namespace spare {  // Inclusion in namespace spare.

/** @brief Product Kernel Function.
 *
 * This class implements the @a Similarity concept.
 * This class contains static functions for the computation of the product between three valid kernel functions.
 * The class of kernel functions are closed under the product operator.
 * Therefore the composition of three (basic) kernel functions will yield again a valid kernel function.
 */
template <class Kernel1, class Kernel2, class Kernel3>
class ProductKernel {
public:

    /** Composite similarity computation.
     *
     * @param[in] c11 A reference to the container of the first container of the first kernel.
     * @param[in] c12 A reference to the container of the second container of the first kernel.
     * @param[in] c21 A reference to the container of the first container of the second kernel.
     * @param[in] c22 A reference to the container of the second container of the second kernel.
     * @param[in] c31 A reference to the container of the first container of the third kernel.
     * @param[in] c32 A reference to the container of the second container of the third kernel.
     * @return The (composite) similarity value.
     */
    template <typename SequenceContainer1, typename SequenceContainer2, typename SequenceContainer3>
    spare::RealType Sim(const SequenceContainer1& c11, const SequenceContainer1& c12,
            const SequenceContainer2& c21, const SequenceContainer2& c22,
            const SequenceContainer3& c31, const SequenceContainer3& c32) const;

    /**
     * Read/Write access to the first kernel
     *
     * @return A reference to the first kernel
     */
    Kernel1& Kernel1Agent() { return k1; }

    /**
     * Read-only access to the first kernel
     *
     * @return A reference to the first kernel
     */
    const Kernel1& Kernel1Agent() const { return k1; }

    /**
     * Read/Write access to the second kernel
     *
     * @return A reference to the second kernel
     */
    Kernel2& Kernel2Agent() { return k2; }

    /**
     * Read-only access to the first kernel
     *
     * @return A reference to the second kernel
     */
    const Kernel1& Kernel2Agent() const { return k2; }

    /**
     * Read/Write access to the third kernel
     *
     * @return A reference to the third kernel
     */
    Kernel3& Kernel3Agent() { return k3; }

    /**
     * Read-only access to the third kernel
     *
     * @return A reference to the third kernel
     */
    const Kernel3& Kernel3Agent() const { return k3; }

private:

    /**
     * First kernel
     */
    Kernel1 k1;

    /**
     * Second kernel
     */
    Kernel2 k2;

    /**
     * Third kernel
     */
    Kernel3 k3;
};


//IMPL

template <class Kernel1, class Kernel2, class Kernel3>
template <typename SequenceContainer1, typename SequenceContainer2, typename SequenceContainer3>
spare::RealType ProductKernel<Kernel1, Kernel2, Kernel3>::Sim(const SequenceContainer1& c11, const SequenceContainer1& c12,
        const SequenceContainer2& c21, const SequenceContainer2& c22,
        const SequenceContainer3& c31, const SequenceContainer3& c32) const
{
    return k1.Sim(c11, c12)*k2.Sim(c21, c22)*k3.Sim(c31, c32);
}

}

#endif /* PRODUCTKERNEL_HPP_ */
