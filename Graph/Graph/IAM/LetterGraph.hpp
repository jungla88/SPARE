//  LetterGraph, part of the SPARE library.
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

/** @brief File LetterGraph.hpp,
 *
 * Contains the declaration of the IAM LetterGraph type.
 *
 * @file LetterGraph.hpp
 * @author Lorenzo Livi
 */

#ifndef LETTERGRAPH_HPP
#define LETTERGRAPH_HPP

// STD INCLUDES
#include <vector>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>

// BOOST INCLUDES
#include <boost/graph/adjacency_list.hpp>

// DEFINITIONS
struct v_info_t
{
   typedef boost::vertex_property_tag kind;
};

struct e_info_t
{
   typedef boost::edge_property_tag kind;
};

// vertices and edges labels types
typedef std::vector<spare::RealType> VertexLabelType;
typedef spare::RealType EdgeLabelType;

// These two typedefs set the types of vertex and edge labels
typedef boost::property<v_info_t, VertexLabelType > VertexInfoProp;
typedef boost::property<e_info_t, EdgeLabelType > EdgeInfoProp;

// IAM Letter graph type
typedef boost::adjacency_list<
                              boost::setS, //boost::setS 
                              boost::vecS, 
                              boost::undirectedS,
                              VertexInfoProp,
                              EdgeInfoProp> LetterGraph;

#endif
