//  SubSequence class, part of the SPARE library.
//  Copyright (C) 2014 Francesca Possemato
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

/** @brief File SubSequence.hpp, that contains the definition of a general class for subsequences objects
 *
 * @file SubSequence.hpp
 * @author Francesca Possemato
 */
#ifndef SUBSEQUENCE_HPP_
#define SUBSEQUENCE_HPP_

#include <spare/Sequence/Sequence.hpp>
#include <spare/Sequence/SequencesDataSet.hpp>

namespace spare
{
	template<typename AtomicElement>
	class SubSequence
	{
	private:

		   // Iterators
		   typename Sequence<AtomicElement>::const_iterator	mBegin;

		   typename Sequence<AtomicElement>::const_iterator    mEnd;

		   // Original sequence (index)
		   typename SequencesDataSet<AtomicElement>::size_type mSeqIndex;

		   // Subsequence position in original sequence (index)
		   typename SequencesDataSet<AtomicElement>::size_type mPosInSeqIndex;

		   // Length of subsequence
		   typename SequencesDataSet<AtomicElement>::size_type mSubseqSize;


	public:
		/*
		* Default constructor
		*/
		SubSequence() { }

		/*
		* Constructor
		*/
		SubSequence(Sequence<AtomicElement>& rBatch, typename Sequence<AtomicElement>::size_type aStart, typename Sequence<AtomicElement>::size_type aSize)
		{
			// Initialization
			mPosInSeqIndex=aStart;
			mSubseqSize=aSize;
			mBegin= rBatch.begin() + aStart;
			mEnd= mBegin + aSize;
		}

		SubSequence(const SequencesDataSet<AtomicElement>& rBatch, typename SequencesDataSet<AtomicElement>::size_type aSeqIndex,
				typename Sequence<AtomicElement>::size_type aStart,
				typename Sequence<AtomicElement>::size_type aSize)
		{
			if (rBatch.size() <= aSeqIndex)
			{
				throw std::logic_error("SeqIndex out of range.");
			}
			if (rBatch[aSeqIndex].size() < (aStart + aSize))
			{
				throw std::logic_error("Subsequence limits out of range.");
			}
			// Initialization
			mSeqIndex= aSeqIndex;
			mPosInSeqIndex=aStart;
			mSubseqSize=aSize;
			mBegin= rBatch[mSeqIndex].begin() + aStart;
			mEnd= mBegin + aSize;
		}

	   // Sequence iterator definition
		typename Sequence<AtomicElement>::const_iterator begin()  const	{ return mBegin; }
		typename Sequence<AtomicElement>::const_iterator end()  const   { return mEnd; }

		// Sequence from which subsequence is extracted
		typename SequencesDataSet<AtomicElement>::size_type SeqIndex() const   { return mSeqIndex; }

	    // Subsequence position in original sequence
		typename SequencesDataSet<AtomicElement>::size_type PosInSeqIndex() const   { return mPosInSeqIndex; }

	    // Length of subsequence
		typename SequencesDataSet<AtomicElement>::size_type size() 	const    { return mSubseqSize; }
	};
}


#endif /* GDSSUBSEQUENCE_HPP_ */
