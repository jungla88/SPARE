//  VectorGraphReader class, part of the SPARE library.
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

/** @brief File VectorGraphReader.hpp that contains the reader for the VectorGraph type.
 *
 * @file VectorGraphReader.hpp
 * @author Lorenzo Livi
 */


#ifndef VECTORGRAPHREADER_HPP
#define VECTORGRAPHREADER_HPP

//STD INCLUDES
#include <string>
#include <vector>
#include <fstream>

// SPARE INCLUDES
#include <spare/Graph/Graph/VectorGraph.hpp>
#include <spare/SpareTypes.hpp>

// BOOST INCLUDES
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/algorithm/string/predicate.hpp>


namespace spare {

///////////////////////
// TMP GRAPH DEFINITION
///////////////////////
typedef boost::property < boost::vertex_name_t, std::string,
        boost::property< boost::vertex_attribute_t, std::string
        > > sgVertexInfoProp;
typedef boost::property < boost::edge_name_t, std::string,
        boost::property< boost::edge_weight_t, std::string
        > > sgEdgeInfoProp;
// Graph properties
typedef boost::property < boost::graph_name_t, std::string > sgGraphInfoProp;
//graph type
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, sgVertexInfoProp, sgEdgeInfoProp, sgGraphInfoProp> sgGraph;
//iterators for vertices
typedef boost::graph_traits<sgGraph>::vertex_iterator sgVertexIter;
typedef boost::graph_traits<sgGraph>::vertex_descriptor sgVertexDesc;
typedef boost::property_map<sgGraph, sgVertexInfoProp> sgVertexPropertyMap;
//for edges
typedef boost::graph_traits<sgGraph>::edge_iterator sgEdgeIter;
typedef boost::graph_traits<sgGraph>::out_edge_iterator sgOutEdgeIter;
typedef boost::graph_traits<sgGraph>::edge_descriptor sgEdgeDesc;
typedef std::pair<sgOutEdgeIter, sgOutEdgeIter> sgEdgePair;



/** @brief DOT file reader for VectorGraph graph type.
 */
class VectorGraphReader {
private:

    /**
     * Utility function to obtaine a std::vector from a string
     * @param[in] s The string of type [v1, v2, ..., vn]
     * @param[out] v The std::vector of real-valued components
     */
    void string2vector(std::string& s, std::vector<RealType>& v) const;

public:

    /**
     * DOT file read operation.
     * @param[in] in The input stream
     * @param[out] g A compatible boost graph defined as a labeled graph with std::vector<spare::RealType>-valued labels
     */
    void read(std::ifstream& in, VectorGraph& g) const;

    /**
     * DOT file read operation. Wrapper method using std::string as input file path.
     * @param[in] in The input path to the file
     * @param[out] g A compatible boost graph defined as a labeled graph with std::vector<spare::RealType>-valued labels
     */
    void read(std::string& in, VectorGraph& g) const;
};


void VectorGraphReader::read(std::ifstream& in, VectorGraph& g) const
{
    // Construct an empty graph and prepare the dynamic_property_maps.
    sgGraph tmpGraph(0);
    boost::dynamic_properties dp;
    std::string nodeid="node_id";
    //vertex properties
    boost::property_map<sgGraph, boost::vertex_name_t>::type name = boost::get(boost::vertex_name, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_attribute_t>::type v_weight = boost::get(boost::vertex_attribute, tmpGraph);
    dp.property(nodeid, name);
    dp.property("weight", v_weight);
    //edge properties
    boost::property_map<sgGraph, boost::edge_name_t>::type eid = boost::get(boost::edge_name, tmpGraph);
    boost::property_map<sgGraph, boost::edge_weight_t>::type weight = boost::get(boost::edge_weight, tmpGraph);
    dp.property("_gxl_id", eid);
    dp.property("weight", weight);
    // Use ref_property_map to turn a graph property into a property map
    boost::ref_property_map<sgGraph*,std::string> gname(boost::get_property(tmpGraph, boost::graph_name));
    dp.property("classid", gname);

    //read
    read_graphviz(in, tmpGraph, dp, nodeid);

    //vertices iterators
    sgVertexIter vi=boost::vertices(tmpGraph).first;
    sgVertexIter viEnd=boost::vertices(tmpGraph).second;
    unsigned int vertexCount=0;

    //add vertices...
    while(vi!=viEnd)
    {
        std::vector<RealType> vectorVertexLabel;

        // vertex label...
        std::string tmpWeight=boost::get(boost::vertex_attribute_t(), tmpGraph, (unsigned int)*vi);
        // and conversion/parsing
        string2vector(tmpWeight, vectorVertexLabel);

        //labeled vertex generation
        boost::add_vertex(g);
        sgVertexDesc source=boost::vertex(vertexCount, g);
        boost::get(v_info_t(), g, source)= vectorVertexLabel;

        vi++;
        vertexCount++;
    }


    //edges iterators
    sgEdgeIter ei=boost::edges(tmpGraph).first;
    sgEdgeIter eiEnd=boost::edges(tmpGraph).second;

    //add edges...
    while(ei!=eiEnd)
    {
        std::vector<RealType> vectorEdgeLabel;

        sgVertexDesc source=boost::source(*ei, tmpGraph);
        sgVertexDesc ta=boost::target(*ei, tmpGraph);
        sgEdgeDesc edgeDesc=boost::edge(source, ta, tmpGraph).first;
        //edges label...
        std::string edgeLabel=boost::get(boost::edge_weight_t(), tmpGraph, edgeDesc);
        //and conversion/parsing.
        string2vector(edgeLabel, vectorEdgeLabel);

        //vector graph labeled edge generation
        boost::add_edge(source, ta, g);
        edgeDesc=boost::edge(source, ta, g).first;
        boost::get(e_info_t(), g, edgeDesc)= vectorEdgeLabel;


        ei++;
    }

}

void VectorGraphReader::read(std::string& in, VectorGraph& g) const
{
    std::ifstream stream(in.c_str());
    if(stream.good())
    {
        read(stream, g);
        stream.close();
    }
}


void VectorGraphReader::string2vector(std::string& s, std::vector<RealType>& v) const
{
    int cutAt;
    std::string ss=s.substr(1, s.size()-2);
    while((cutAt=ss.find_first_of(","))!=(int)ss.npos)
    {
        if(cutAt>0)
            v.push_back(atof(ss.substr(0,cutAt).c_str()));
        ss=ss.substr(cutAt+1);
    }

    if(ss.length()>0)
        v.push_back(atof(ss.c_str()));
}

}

#endif

