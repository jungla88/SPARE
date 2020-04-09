#ifndef GRECGRAPHREADER_HPP
#define GRECGRAPHREADER_HPP

//STD INCLUDES
#include <string>
#include <vector>
#include <fstream>

// SPARE INCLUDES
#include <spare/Graph/Graph/IAM/GRECGraph.hpp>
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
// TMP IAM COILDEL GRAPH DEFINITION
///////////////////////

//vertex label
/**
 * attribute ---> type
 * rank ---> x
 * priority ---> y
 */
typedef boost::property < boost::vertex_name_t, std::string,
            boost::property< boost::vertex_attribute_t, std::string,
                boost::property< boost::vertex_rank_t, std::string,
                    boost::property< boost::vertex_priority_t, std::string
                    > > > > sgVertexInfoProp;
//edge label
/**
 * angle0 ---> weight
 * angle1 ---> weight2
 * frequency ---> update
 * type0 ---> owner
 * type1 ---> reverse
 */
typedef boost::property < boost::edge_name_t, std::string,
            boost::property< boost::edge_weight_t, std::string,
            boost::property< boost::edge_weight2_t, std::string,
                boost::property< boost::edge_update_t, std::string,
                    boost::property< boost::edge_owner_t, std::string,
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



/** @brief DOT file reader for IAM COILDEL graph.
 */
class GRECGraphReader {

public:

    /**
     * DOT file read operation.
     * @param[in] in The input stream
     * @param[out] g A compatible boost graph defined as a labeled graph
     */
    void read(std::ifstream& in, GRECGraph& g) const;

    /**
     * DOT file read operation. Wrapper method using std::string as input file path.
     * @param[in] in The input path to the file
     * @param[out] g A compatible boost graph defined as a labeled graph
     */
    void read(std::string& in, GRECGraph& g) const;
};


void GRECGraphReader::read(std::ifstream& in, GRECGraph& g) const
{
    // Construct an empty graph and prepare the dynamic_property_maps.
    sgGraph tmpGraph(0);
    boost::dynamic_properties dp;
    std::string nodeid="node_id";
    //vertex properties
    boost::property_map<sgGraph, boost::vertex_name_t>::type name = boost::get(boost::vertex_name, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_attribute_t>::type v_type = boost::get(boost::vertex_attribute, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_rank_t>::type v_x = boost::get(boost::vertex_rank, tmpGraph);
    boost::property_map<sgGraph, boost::vertex_priority_t>::type v_y = boost::get(boost::vertex_priority, tmpGraph);
    dp.property(nodeid, name);
    dp.property("type", v_type);
    dp.property("x", v_x);
    dp.property("y", v_y);

    //edge props
    boost::property_map<sgGraph, boost::edge_weight_t>::type weight = boost::get(boost::edge_weight, tmpGraph);
    boost::property_map<sgGraph, boost::edge_weight2_t>::type weight2 = boost::get(boost::edge_weight2, tmpGraph);
    boost::property_map<sgGraph, boost::edge_update_t>::type update = boost::get(boost::edge_update, tmpGraph);
    boost::property_map<sgGraph, boost::edge_owner_t>::type owner = boost::get(boost::edge_owner, tmpGraph);
    boost::property_map<sgGraph, boost::edge_reverse_t>::type reverse = boost::get(boost::edge_reverse, tmpGraph);
    dp.property("angle0", weight);
    dp.property("angle1", weight2);
    dp.property("frequency", update);
    dp.property("type0", owner);
    dp.property("type1", reverse);

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
        std::string x=boost::get(boost::vertex_rank_t(), tmpGraph, (unsigned int)*vi);
        std::string y=boost::get(boost::vertex_priority_t(), tmpGraph, (unsigned int)*vi);
        std::string type=boost::get(boost::vertex_attribute_t(), tmpGraph, (unsigned int)*vi);

        //labeled vertex generation
        boost::add_vertex(g);
        sgVertexDesc source=boost::vertex(vertexCount, g);

        GRECVertexLabel vl;
        vl.x=atof(x.c_str());
        vl.y=atof(y.c_str());
        vl.type=type;
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
        std::string lineType0=boost::get(boost::edge_owner_t(), tmpGraph, edgeDesc);
        std::string angle0=boost::get(boost::edge_weight_t(), tmpGraph, edgeDesc);
        std::string lineType1=boost::get(boost::edge_reverse_t(), tmpGraph, edgeDesc);
        std::string angle1=boost::get(boost::edge_weight2_t(), tmpGraph, edgeDesc);
        std::string frequency=boost::get(boost::edge_update_t(), tmpGraph, edgeDesc);

        //edge generation
        GRECEdgeLabel el;
        el.type0=lineType0;
        el.angle0=atof(angle0.c_str());
        el.type1=lineType1;
        el.angle1=atof(angle1.c_str());
        el.frequency=atoi(frequency.c_str());

        boost::add_edge(source, ta, g);
        edgeDesc=boost::edge(source, ta, g).first;
        boost::get(e_info_t(), g, edgeDesc)= el;

        ei++;
    }

}

void GRECGraphReader::read(std::string& in, GRECGraph& g) const
{
    std::ifstream stream(in.c_str());
    if(stream.good())
    {
        read(stream, g);
        stream.close();
    }
}


#endif

