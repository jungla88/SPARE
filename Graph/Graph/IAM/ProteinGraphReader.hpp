#ifndef PROTEINGRAPHREADER_HPP
#define PROTEINGRAPHREADER_HPP

//STD INCLUDES
#include <string>
#include <fstream>

// SPARE INCLUDES
#include <spare/Graph/Graph/IAM/ProteinGraph.hpp>
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
// TMP IAM PROTEIN GRAPH DEFINITION
///////////////////////

//vertex label
/**
 * attribute --> aaLength
 * centrality --> sequence
 * color --> type
 */
typedef boost::property < boost::vertex_name_t, std::string,
            boost::property< boost::vertex_attribute_t, std::string,
            boost::property< boost::vertex_centrality_t, std::string,
            boost::property< boost::vertex_color_t, std::string
                > > > > sgVertexInfoProp;

//edge label
typedef boost::property < boost::edge_name_t, std::string,
            boost::property< boost::edge_weight_t, std::string,
            boost::property< boost::edge_owner_t, std::string,
            boost::property< boost::edge_weight2_t, std::string,
            boost::property< boost::edge_update_t, std::string,
            boost::property< boost::edge_reverse_t, std::string
                > > > > > > sgEdgeInfoProp;

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



/** @brief DOT file reader for IAM Protein graph.
 */
class ProteinGraphReader {

public:

    /**
     * DOT file read operation.
     * @param[in] in The input stream
     * @param[out] g A compatible boost graph defined as a labeled graph with std::vector<spare::RealType>-valued labels
     */
    void read(std::ifstream& in, ProteinGraph& g) const;

    /**
     * DOT file read operation. Wrapper method using std::string as input file path.
     * @param[in] in The input path to the file
     * @param[out] g A compatible boost graph defined as a labeled graph with std::vector<spare::RealType>-valued labels
     */
    void read(std::string& in, ProteinGraph& g) const;
};

//IMPL.

void ProteinGraphReader::read(std::ifstream& in, ProteinGraph& g) const
{
    // Construct an empty graph and prepare the dynamic_property_maps.
    sgGraph tmpGraph(0);
    boost::dynamic_properties dp;
    std::string nodeid="node_id";
    //vertex properties

    boost::property_map<sgGraph, boost::vertex_name_t>::type name = boost::get(boost::vertex_name, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_attribute_t>::type aaLength= boost::get(boost::vertex_attribute, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_centrality_t>::type sequence = boost::get(boost::vertex_centrality, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_color_t>::type type = boost::get(boost::vertex_color, tmpGraph);
    dp.property(nodeid, name);
    dp.property("aaLength", aaLength);
    dp.property("sequence", sequence);
    dp.property("type", type);

    //edge props
    boost::property_map<sgGraph, boost::edge_weight_t>::type distance0 = boost::get(boost::edge_weight, tmpGraph);
    boost::property_map<sgGraph, boost::edge_owner_t>::type distance1 = boost::get(boost::edge_owner, tmpGraph);
    boost::property_map<sgGraph, boost::edge_weight2_t>::type frequency = boost::get(boost::edge_weight2, tmpGraph);
    boost::property_map<sgGraph, boost::edge_update_t>::type type0 = boost::get(boost::edge_update, tmpGraph);
    boost::property_map<sgGraph, boost::edge_reverse_t>::type type1 = boost::get(boost::edge_reverse, tmpGraph);
    dp.property("distance0", distance0);
    dp.property("distance1", distance1);
    dp.property("frequency", frequency);
    dp.property("type0", type0);
    dp.property("type1", type1);

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
        std::string aaLength=boost::get(boost::vertex_attribute_t(), tmpGraph, (unsigned int)*vi);
        std::string sequence=boost::get(boost::vertex_centrality_t(), tmpGraph, (unsigned int)*vi);
        std::string type=boost::get(boost::vertex_color_t(), tmpGraph, (unsigned int)*vi);

        VertexLabelType vl;

        vl.aaLength=atoi(aaLength.c_str());
        vl.sequence=sequence;
        vl.type=atoi(type.c_str());

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
        std::string distance0=boost::get(boost::edge_weight_t(), tmpGraph, edgeDesc);
        std::string distance1=boost::get(boost::edge_owner_t(), tmpGraph, edgeDesc);
        std::string frequency=boost::get(boost::edge_weight2_t(), tmpGraph, edgeDesc);
        std::string type0=boost::get(boost::edge_update_t(), tmpGraph, edgeDesc);
        std::string type1=boost::get(boost::edge_reverse_t(), tmpGraph, edgeDesc);

        EdgeLabelType el;
        el.distance0=atof(distance0.c_str());
        el.distance1=atof(distance1.c_str());
        el.frequency=atoi(frequency.c_str());
        el.type0=atoi(type0.c_str());
        el.type1=atoi(type1.c_str());

        //edge generation
        boost::add_edge(source, ta, g);
        edgeDesc=boost::edge(source, ta, g).first;
        boost::get(e_info_t(), g, edgeDesc)= el;

        ei++;
    }

}

void ProteinGraphReader::read(std::string& in, ProteinGraph& g) const
{
    std::ifstream stream(in.c_str());
    if(stream.good())
    {
        read(stream, g);
        stream.close();
    }
}


#endif

