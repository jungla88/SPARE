//  EColiPGraph, part of the SPARE library.
//  Copyright (C) 2017 Ettore Di Micco
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

/** @brief File EColiPGraph.hpp,
 *
 * Contains the declaration of the EColiPGraph type.
 *
 * @file EColiPGraph.hpp
 * @author Ettore Di Micco
 */

#ifndef EColiPGRAPH_HPP
#define EColiPGRAPH_HPP

// STD INCLUDES
#include <string>

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

/**
 * vertex label structure
 */
class EColiPVertexLabel
{
public:
	/**
     * Default constructor
     */
    EColiPVertexLabel()
    {
        /*position=0;
        x=0.0;
        y=0.0;
        z=0.0;
        f1=0.0;
        f2=0.0;
        f3=0.0;*/
    }
    
	//spare::NaturalType position;	
    std::string symbol;			
    /*spare::RealType x;			
    spare::RealType y;			
	spare::RealType z;
	spare::RealType f1;	
	spare::RealType f2;	
	spare::RealType f3;*/
};

/**
 * edge label structure
 */
/*class EColiPEdgeLabel
{
public:
    EColiPEdgeLabel()
    {
        distance=0.0;
    }
    
	spare::RealType distance;
};*/

// vertices and edges labels types
typedef EColiPVertexLabel VertexLabelType;
//typedef EColiPEdgeLabel EdgeLabelType;

// These two typedefs set the types of vertex and edge labels
typedef boost::property<v_info_t, VertexLabelType > VertexInfoProp;
typedef boost::property<edge_index_t, int > EdgeInfoProp;

// Letter graph type
typedef boost::adjacency_list<
                              boost::vecS, 
                              boost::vecS, 
                              boost::undirectedS,
                              VertexInfoProp,
                              EdgeInfoProp> EColiPGraph;

#endif
