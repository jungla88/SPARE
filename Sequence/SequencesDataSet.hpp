//  SequencesDataSet class, part of the SPARE library.
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

/** @brief File SequencesDataSet.hpp, that contains the definition of a general dataset if sequence of objects.
 *
 * @file SequencesDataSet.hpp
 * @author Francesca Possemato
*/

#ifndef SEQUENCESDATASET_HPP_
#define SEQUENCESDATASET_HPP_

#include <spare/Sequence/Sequence.hpp>

namespace spare
{

	template<typename AtomicElement>
	using SequencesDataSet = std::vector<Sequence<AtomicElement> >;

}



#endif  SEQUENCESDATASET_HPP_
