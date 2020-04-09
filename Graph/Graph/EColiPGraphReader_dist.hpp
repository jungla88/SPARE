#ifndef ECOLIPGRAPHREADER_HPP
#define ECOLIPGRAPHREADER_HPP

//STD INCLUDES
#include <string>
#include <fstream>

// SPARE INCLUDES
#include <spare/Graph/Graph/EColiPGraph.hpp>
#include <spare/SpareTypes.hpp>

// BOOST INCLUDES
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/algorithm/string/predicate.hpp>



///////////////////////
// TMP EColiP GRAPH DEFINITION
///////////////////////
//vertex label
typedef boost::property < boost::vertex_name_t, std::string,
            boost::property< boost::vertex_attribute_t, std::string
                > > sgVertexInfoProp;
//edge label
typedef boost::property < boost::edge_name_t, std::string,
            boost::property< boost::edge_weight_t, std::string
            > > sgEdgeInfoProp;

//graph type
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
        sgVertexInfoProp, sgEdgeInfoProp> sgGraph;
//iterators for vertices
typedef boost::graph_traits<sgGraph>::vertex_iterator sgVertexIter;
typedef boost::graph_traits<sgGraph>::vertex_descriptor sgVertexDesc;
typedef boost::property_map<sgGraph, sgVertexInfoProp> sgVertexPropertyMap;
//for edges
typedef boost::graph_traits<sgGraph>::edge_iterator sgEdgeIter;
typedef boost::graph_traits<sgGraph>::out_edge_iterator sgOutEdgeIter;
typedef boost::graph_traits<sgGraph>::edge_descriptor sgEdgeDesc;
typedef std::pair<sgOutEdgeIter, sgOutEdgeIter> sgEdgePair;



/** @brief DOT file reader for EColiP graph.
 */
class EColiPGraphReader {

public:

    /**
     * DOT file read operation.
     * @param[in] in The input stream
     * @param[out] g A compatible boost graph defined as a labeled graph with std::vector<spare::RealType>-valued labels
     */
    void read(std::ifstream& in, EColiPGraph& g) const;

    /**
     * DOT file read operation. Wrapper method using std::string as input file path.
     * @param[in] in The input path to the file
     * @param[out] g A compatible boost graph defined as a labeled graph with std::vector<spare::RealType>-valued labels
     */
    void read(std::string& in, EColiPGraph& g) const;
};

//IMPL.

void EColiPGraphReader::read(std::ifstream& in, EColiPGraph& g) const
{
    // Construct an empty graph and prepare the dynamic_property_maps.
    sgGraph tmpGraph(0);
    boost::dynamic_properties dp;
    std::string nodeid="node_id";
    //vertex properties
    boost::property_map<sgGraph, boost::vertex_name_t>::type name = boost::get(boost::vertex_name, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_attribute_t>::type v_symb = boost::get(boost::vertex_attribute, tmpGraph);
    dp.property(nodeid, name);
    dp.property("symbol", v_symb);
    //edge props
    boost::property_map<sgGraph, boost::edge_weight_t>::type weight = boost::get(boost::edge_weight, tmpGraph);
    dp.property("distance", weight);

    //read
    read_graphviz(in, tmpGraph, dp, nodeid);


    //vertices iterators
    sgVertexIter vi=boost::vertices(tmpGraph).first;
    sgVertexIter viEnd=boost::vertices(tmpGraph).second;
    unsigned int vertexCount=0;


    //add vertices...
    while(vi!=viEnd)
    {
        // vertex label...
        std::string chem=boost::get(boost::vertex_attribute_t(), tmpGraph, (unsigned int)*vi);

        //labeled vertex generation
        boost::add_vertex(g);
        sgVertexDesc source=boost::vertex(vertexCount, g);
        boost::get(v_info_t(), g, source)= chem;

        vi++;
        vertexCount++;
    }


    //edges iterators
    sgEdgeIter ei=boost::edges(tmpGraph).first;
    sgEdgeIter eiEnd=boost::edges(tmpGraph).second;
    unsigned int edgeCount=0;

    //add edges...
    while(ei!=eiEnd)
    {
        sgVertexDesc source=boost::source(*ei, tmpGraph);
        sgVertexDesc ta=boost::target(*ei, tmpGraph);
        sgEdgeDesc edgeDesc=boost::edge(source, ta, tmpGraph).first;
        //edges label...
        std::string edgeLabel=boost::get(boost::edge_weight_t(), tmpGraph, edgeDesc);

        //edge generation
        boost::add_edge(source, ta, g);
        edgeDesc=boost::edge(source, ta, g).first;
        boost::get(e_info_t(), g, edgeDesc)= atoi(edgeLabel.c_str());


        ei++;
    }

}

void EColiPGraphReader::read(std::string& in, EColiPGraph& g) const
{
    std::ifstream stream(in.c_str());
    if(stream.good())
    {
        read(stream, g);
        stream.close();
    }
}


#endif

