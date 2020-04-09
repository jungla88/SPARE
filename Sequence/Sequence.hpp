//  Sequence class, part of the SPARE library.
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

/** @brief File Sequence.hpp, that contains the definition of a general sequence of objects.
 *
 * @file Sequence.hpp
 * @author Francesca Possemato
 */
#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

namespace spare
{

	template<typename AtomicElement>
	using Sequence = std::vector<AtomicElement>;

}

#endif
