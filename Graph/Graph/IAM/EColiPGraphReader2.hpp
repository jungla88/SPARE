#ifndef EColiPGRAPHREADER_HPP
#define EColiPGRAPHREADER_HPP

//STD INCLUDES
#include <string>
#include <fstream>

// SPARE INCLUDES
#include <spare/Graph/Graph/IAM/EColiPGraph.hpp>
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
/**
 * centrality --> chem  //position
 * color  --> symbol	//idem gli altri
 */
typedef boost::property < boost::vertex_name_t, std::string,
            boost::property< boost::vertex_color_t, std::string,
            boost::property< boost::vertex_predecessor_t, std::string
                > > > 
                sgVertexInfoProp;

//edge label:
typedef boost::property < boost::edge_name_t, std::string,
            boost::property< boost::edge_weight_t, std::string,
            boost::property< boost::edge_index_t, int,
            > > > sgEdgeInfoProp;


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
    boost::property_map<sgGraph, boost::vertex_color_t>::type v_symb = boost::get(boost::vertex_color, tmpGraph);
    //boost::property_map<sgGraph, boost::vertex_centrality_t>::type v_position = boost::get(boost::vertex_centrality, tmpGraph);
    //boost::property_map<sgGraph, boost::vertex_index1_t>::type v_x = boost::get(boost::vertex_index1, tmpGraph);
    //boost::property_map<sgGraph, boost::vertex_index2_t>::type v_y = boost::get(boost::vertex_index2, tmpGraph);
    //boost::property_map<sgGraph, boost::vertex_distance_t>::type v_z = boost::get(boost::vertex_distance, tmpGraph);
    //boost::property_map<sgGraph, boost::vertex_attribute_t>::type v_f1 = boost::get(boost::vertex_attribute, tmpGraph);
    //boost::property_map<sgGraph, boost::vertex_discover_time_t>::type v_f2 = boost::get(boost::vertex_discover_time, tmpGraph);
    //boost::property_map<sgGraph, boost::vertex_finish_time_t>::type v_f3 = boost::get(boost::vertex_finish_time, tmpGraph);
    dp.property(nodeid, name);
	dp.property("symbol", v_symb);
    //dp.property("position", v_position);	//inserito io
    //dp.property("x", v_x);
    //dp.property("y", v_y);
    //dp.property("z", v_z);
	//dp.property("f1", v_f1);
	//dp.property("f2", v_f2);
	//dp.property("f3", v_f3);
	
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
		std::string symbol=boost::get(boost::vertex_color_t(), tmpGraph, (unsigned int)*vi);
        /*std::string position=boost::get(boost::vertex_centrality_t(), tmpGraph, (unsigned int)*vi);
        std::string x=boost::get(boost::vertex_index1_t(), tmpGraph, (unsigned int)*vi);
        std::string y=boost::get(boost::vertex_index2_t(), tmpGraph, (unsigned int)*vi);
		std::string z=boost::get(boost::vertex_distance_t(), tmpGraph, (unsigned int)*vi);
		std::string f1=boost::get(boost::vertex_attribute_t(), tmpGraph, (unsigned int)*vi);
		std::string f2=boost::get(boost::vertex_discover_time_t(), tmpGraph, (unsigned int)*vi);
		std::string f3=boost::get(boost::vertex_finish_time_t(), tmpGraph, (unsigned int)*vi);
		*/

        VertexLabelType vl;

		vl.symbol=symbol;
        /*vl.position=atoi(position.c_str());
        vl.x=atof(x.c_str());
        vl.y=atof(y.c_str());
        vl.z=atof(z.c_str());        
		vl.f1=atof(f1.c_str());
		vl.f2=atof(f2.c_str());
		vl.f3=atof(f3.c_str());
		*/
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
        std::string distance=boost::get(boost::edge_weight_t(), tmpGraph, edgeDesc);

		EdgeLabelType el;
        el.distance=atof(distance.c_str());

        //edge generation
        boost::add_edge(source, ta, g);
        edgeDesc=boost::edge(source, ta, g).first;
        boost::get(e_info_t(), g, edgeDesc)= el;


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

