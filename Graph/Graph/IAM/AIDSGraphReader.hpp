#ifndef AIDSGRAPHREADER_HPP
#define AIDSGRAPHREADER_HPP

//STD INCLUDES
#include <string>
#include <fstream>

// SPARE INCLUDES
#include <spare/Graph/Graph/IAM/AIDSGraph.hpp>
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
// TMP IAM AIDS GRAPH DEFINITION
///////////////////////
//vertex label
/**
 * attribute --> charge
 * centrality --> chem
 * color --> symbol
 * index1 --> x
 * index2 --> y
 */
typedef boost::property < boost::vertex_name_t, std::string,
            boost::property< boost::vertex_attribute_t, std::string,
            boost::property< boost::vertex_centrality_t, std::string,
            boost::property< boost::vertex_color_t, std::string,
            boost::property< boost::vertex_index1_t, std::string,
            boost::property< boost::vertex_index2_t, std::string
                > > > > > > sgVertexInfoProp;

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



/** @brief DOT file reader for IAM AIDS graph.
 */
class AIDSGraphReader {

public:

    /**
     * DOT file read operation.
     * @param[in] in The input stream
     * @param[out] g A compatible boost graph defined as a labeled graph with std::vector<spare::RealType>-valued labels
     */
    void read(std::ifstream& in, AIDSGraph& g) const;

    /**
     * DOT file read operation. Wrapper method using std::string as input file path.
     * @param[in] in The input path to the file
     * @param[out] g A compatible boost graph defined as a labeled graph with std::vector<spare::RealType>-valued labels
     */
    void read(std::string& in, AIDSGraph& g) const;
};

//IMPL.

void AIDSGraphReader::read(std::ifstream& in, AIDSGraph& g) const
{
    // Construct an empty graph and prepare the dynamic_property_maps.
    sgGraph tmpGraph(0);
    boost::dynamic_properties dp;
    std::string nodeid="node_id";
    //vertex properties
    boost::property_map<sgGraph, boost::vertex_name_t>::type name = boost::get(boost::vertex_name, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_attribute_t>::type v_charge= boost::get(boost::vertex_attribute, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_centrality_t>::type v_chem = boost::get(boost::vertex_centrality, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_color_t>::type v_symb = boost::get(boost::vertex_color, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_index1_t>::type v_x = boost::get(boost::vertex_index1, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_index2_t>::type v_y = boost::get(boost::vertex_index2, tmpGraph);
    dp.property(nodeid, name);

    dp.property("charge", v_charge);
    dp.property("chem", v_chem);
    dp.property("symbol", v_symb);
    dp.property("x", v_x);
    dp.property("y", v_y);

    //edge props
    boost::property_map<sgGraph, boost::edge_weight_t>::type weight = boost::get(boost::edge_weight, tmpGraph);
    dp.property("valence", weight);

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
        std::string charge=boost::get(boost::vertex_attribute_t(), tmpGraph, (unsigned int)*vi);
        std::string chem=boost::get(boost::vertex_centrality_t(), tmpGraph, (unsigned int)*vi);
        std::string symbol=boost::get(boost::vertex_color_t(), tmpGraph, (unsigned int)*vi);
        std::string x=boost::get(boost::vertex_index1_t(), tmpGraph, (unsigned int)*vi);
        std::string y=boost::get(boost::vertex_index2_t(), tmpGraph, (unsigned int)*vi);

        VertexLabelType vl;

        vl.charge=atoi(charge.c_str());
        vl.chem=atoi(chem.c_str());
        vl.symbol=symbol;
        vl.x=atof(x.c_str());
        vl.y=atof(y.c_str());

        //labeled vertex generation
        boost::add_vertex(g);
        sgVertexDesc source=boost::vertex(vertexCount, g);
        boost::get(v_info_t(), g, source)= vl;

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

void AIDSGraphReader::read(std::string& in, AIDSGraph& g) const
{
    std::ifstream stream(in.c_str());
    if(stream.good())
    {
        read(stream, g);
        stream.close();
    }
}


#endif

