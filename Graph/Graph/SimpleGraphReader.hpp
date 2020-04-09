//  SimpleGraphReader class, part of the SPARE library.
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

/** @brief DOT file reader for the SimpleGraph graph type.
 *
 * @file SimpleGrapReaderh.hpp
 * @author Lorenzo Livi
 */


#ifndef SIMPLEGRAPHREADER_HPP
#define SIMPLEGRAPHREADER_HPP

//STD INCLUDES
#include <string>
#include <fstream>

// SPARE INCLUDES
#include <spare/Graph/Graph/SimpleGraph.hpp>
#include <spare/SpareTypes.hpp>

// BOOST INCLUDES
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>


namespace spare {

///////////////////////
// TMP GRAPH DEFINITION
///////////////////////
typedef boost::property < boost::vertex_name_t, std::string > sgVertexInfoProp;
//graph type
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, sgVertexInfoProp> sgGraph;
//iterators for vertices and edges
typedef boost::graph_traits<sgGraph>::vertex_iterator sgVertexIter;
typedef boost::graph_traits<sgGraph>::edge_iterator sgEdgeIter;



/** @brief DOT file simple unlabeled graphs.
 */
class SimpleGraphReader {
public:

    /**
     * DOT file read operation.
     * @param[in] in The input stream
     * @param[out] g A compatible boost graph defined as a simple unlabeled graph
     */
    void read(std::ifstream& in, SimpleGraph& g) const;

    /**
     * DOT file read operation. Wrapper method using std::string as input file path.
     * @param[in] in The input stream
     * @param[out] g A compatible boost graph defined as a simple unlabeled graph
     */
    void read(std::string& in, SimpleGraph& g) const;
};

// IMPL.

void SimpleGraphReader::read(std::ifstream& in, SimpleGraph& g) const
{
    // Construct an empty graph and prepare the dynamic_property_maps.
    sgGraph tmpGraph(0);
    boost::dynamic_properties dp;
    std::string nodeid="node_id";
    //vertex properties
    boost::property_map<sgGraph, boost::vertex_name_t>::type name = get(boost::vertex_name, tmpGraph);
    dp.property(nodeid, name);

    //read the dot file
    read_graphviz(in, tmpGraph, dp, nodeid);


    //vertices iterators
    sgVertexIter vi=boost::vertices(tmpGraph).first;
    sgVertexIter viEnd=boost::vertices(tmpGraph).second;
    unsigned int vertexCount=0;


    while(vi!=viEnd)
    {
        boost::add_vertex(g);
        vi++;
    }

    //edges iterators on the tmp loaded simple graph
    sgEdgeIter ei=boost::edges(tmpGraph).first;
    sgEdgeIter eiEnd=boost::edges(tmpGraph).second;

    //add edges...
    while(ei!=eiEnd)
    {
        //string graph edge label generation
        boost::add_edge(boost::source(*ei, tmpGraph), boost::target(*ei, tmpGraph), g);

        ei++;
    }
}


void SimpleGraphReader::read(std::string& in, SimpleGraph& g) const
{
    std::ifstream stream(in.c_str());
    if(stream.good())
    {
        read(stream, g);
        stream.close();
    }
}

}

#endif

