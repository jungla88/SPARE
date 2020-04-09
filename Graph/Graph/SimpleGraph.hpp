//  SimpleGraph class, part of the SPARE library.
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

/** @brief File SimpleGraph.hpp, that contains the definition of a boost::graph based simple graph type.
 * It is a simple graph, that is, undirected and unlabeled.
 *
 * @file SimpleGraph.hpp
 * @author Lorenzo Livi
 */


#ifndef SIMPLEGRAPH_HPP
#define SIMPLEGRAPH_HPP

// BOOST INCLUDES
#include <boost/graph/adjacency_list.hpp>


namespace spare {

//simple graph type
typedef boost::adjacency_list<
                              boost::vecS, 
                              boost::vecS, 
                              boost::undirectedS> SimpleGraph;

}

#endif
