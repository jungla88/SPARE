//  HyperbolicTangentKernel class, part of the SPARE library.
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

/** @brief File HyperbolicTangentKernel.hpp, that contains HyperbolicTangentKernel class.
 *
 * Contains the declaration of the HyperbolicTangentKernel class.
 *
 * @file HyperbolicTangentKernel.hpp
 * @author Lorenzo Livi
 */

#ifndef HYPERBOLICTANGENTKERNEL_H_
#define HYPERBOLICTANGENTKERNEL_H_

#include <cmath>

#include <spare/SpareTypes.hpp>


namespace spare {  // Inclusion in namespace spare.

/** @brief Hyperbolic Tangent (Sigmoid) Kernel Function.
 *
 * This class implements the @a Similarity concept.
 * This class contains static functions for the computation of the similarity between real vectors using the hyperbolic tangent (sigmoid) kernel function.
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
 *     <td class="indexvalue">alpha</td>
 *     <td class="indexvalue">(0, infinity)</td>
 *     <td class="indexvalue">Slope</td>
 *     <td class="indexvalue">-</td>
 *     <td class="indexvalue">2</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">beta</td>
 *     <td class="indexvalue">[0, infinity)</td>
 *     <td class="indexvalue">Margin</td>
 *     <td class="indexvalue">-</td>
 *     <td class="indexvalue">1</td>
 *  </tr>
 *  </table>
 */
class HyperbolicTangentKernel {
public:

	/**
	 * Default constructor.
	 */
	HyperbolicTangentKernel()
	{
		alpha=2;
		beta=1;
	}

	/**
	 * Constructor.
	 * @param[in] a The value for the alpha parameter.
	 * @param[in] b The value for the beta parameter.
	 */
	HyperbolicTangentKernel(RealType a, RealType b)
	{
		alpha=a;
		beta=b;
	}

	/**
	 * Similarity computation using input iterators.
	 *
	 * @param[in] aA A pair of iterators of the first vector.
	 * @param[in] aB A pair of iterators of the second vector.
	 * @return The similarity value.
	 */
	template <typename ForwardIterator1, typename ForwardIterator2>
	RealType Sim(std::pair<ForwardIterator1, ForwardIterator1> aA,
	        std::pair<ForwardIterator2, ForwardIterator2> aB) const;


	/** Similarity computation.
	 *
	 * @param[in] c1 A reference to the container of the first container.
	 * @param[in] c2 A reference to the container of the second container.
	 * @return The similarity value.
	 */
	template <typename SequenceContainer1, typename SequenceContainer2>
	RealType Sim(const SequenceContainer1& c1, const SequenceContainer2& c2) const;

	/**
	 * Read/write access to the alpha parameter.
	 *
	 * @return A reference to the value of alpha
	 */
	RealType& Alpha() { return alpha; }

	/**
	 * Read-only access to the alpha parameter.
	 * @return A reference to the value of alpha
	 */
	const RealType& Alpha() const { return alpha; }

	/**
	 * Read/write access to the beta parameter.
	 *
	 * @return A reference to the value of beta
	 */
	RealType& Beta() { return beta; }

	/**
	 * Read-only access to the beta parameter.
	 * @return A reference to the value of beta
	 */
	const RealType& Beta() const { return beta; }

private:

	/**
	 * Slope parameter
	 */
	RealType alpha;

	/**
	 * Margin parameter
	 */
	RealType beta;
};


//IMPLEMENTATION


template <typename ForwardIterator1, typename ForwardIterator2>
RealType
HyperbolicTangentKernel::Sim(std::pair<ForwardIterator1, ForwardIterator1> aA,
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
		throw SpareLogicError("HyperbolicTangentKernel, 0, Invalid range.");
	}

	if (DistA != DistB)
	{
		throw SpareLogicError("HyperbolicTangentKernel, 1, Different lengths between inputs.");
	}
	#endif


	while (aA.first != aA.second)
	{
		InnerProd+=static_cast<RealType>(*aA.first++)*static_cast<RealType>(*aB.first++);
	}

	return std::tanh(alpha*InnerProd+beta);
}


template <typename SequenceContainer1, typename SequenceContainer2>
RealType HyperbolicTangentKernel::Sim(const SequenceContainer1& c1, const SequenceContainer2& c2) const
{
	// Controllo.
	#if SPARE_DEBUG
	if ( c1.size() != c2.size() )
	{
		throw SpareLogicError("HyperbolicTangentKernel, 3, Different lenghts between inputs.");
	}
	#endif

	return Sim(std::make_pair(c1.begin(), c1.end()), std::make_pair(c2.begin(), c2.end()));
}


} // namespace

#endif /* HYPERBOLICTANGENTKERNEL_H_ */
