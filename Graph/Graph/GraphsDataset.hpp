//  GraphDataset class, part of the SPARE library.
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

/** @brief File GraphDataset.hpp, that contains the dataset class for labeled graphs.
 *
 * @file GraphDataset.hpp
 * @author Lorenzo Livi
 */

#ifndef GRAPHSDATASET_HPP
#define GRAPHSDATASET_HPP


//STD INCLUDES
#include <string>
#include <vector>
#include <istream>
#include <sys/types.h>
#include <dirent.h>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/SpareExceptions.hpp>


namespace spare {

/** @brief Dataset utility for labeled graphs (Boost compliant graphs used in SPARE).
 *  The class must be configured with two template arguments.
 *  The type of graph reader (first argument) to be used for the specific type of labeled graph (second argument).
 *  The dataset class accepts a directory path and generates STL containers of labeled graphs and related class labels (if specified) as output.
 *  In addition, this class can take into consideration and identifier that is added by the user to the file containing the graph structures.
 */
template <class GraphReaderType, class GraphType>
class GraphsDataset {

public:

    /**
     * Default constructor.
     */
    GraphsDataset()
    {
        DEFAULT_LABEL="UNLABELED";
        DEFAULT_ID="NID";
    }

    /**
     * Read-only access to the loaded max order
     */
    const NaturalType MaxOrder() const { return mMaxOrder; }

    /**
     * Read-only access to the loaded max size
     */
    const NaturalType MaxSize() const { return mMaxSize; }

    /**
     * Reads the dataset of graphs at the given path.
     * Each sample of the dataset is assumed to be formatted as follows: NAME_ID_CLASSID.dot
     * - NAME: usually equal to "graph"
     * - ID: it is a user-defined identifier
     * - CLASSID: it is the identifier of the class (category) to which the pattern/graph belongs
     * A naming convention is, for example, graph_1_0.dot, meaning a graph with id 1 from class 0.
     * @param[in] datasetPath A reference to the path of the dataset of graphs
     * @param[out] graphsContainer A reference to the specified generic type of container for the graphs
     * @param[out] labels A reference to the specified generic type of container for the labels of the graphs. Note that the labels are assumed to be strings.
     * @return True on success. False if at least one graph has failed to be loaded
     */
    template <typename BoostGraphsContainer, typename LabelsContainer>
    bool readDataset(const std::string& datasetPath, BoostGraphsContainer& graphsContainer, LabelsContainer& labels) const;


    /**
     * Reads the dataset of graphs at the given path. This method reads also the graphs ids.
     * Each sample of the dataset is assumed to be formatted as follows: NAME_ID_CLASSID.dot
     * - NAME: usually equal to "graph"
     * - ID: it is a user-defined identifier
     * - CLASSID: it is the identifier of the class (category) to which the pattern/graph belongs
     * A naming convention is, for example, graph_1_0.dot, meaning a graph with id 1 from class 0.
     * @param[in] datasetPath A reference to the path of the dataset of graphs
     * @param[out] graphsContainer A reference to the specified generic type of container for the graphs
     * @param[out] labels A reference to the specified generic type of container for the labels of the graphs. Note that the labels are assumed to be strings.
     * @param[out] ids A reference to the generic type of container for the graphs ids. Note that also the graphs ids are assumed to be strings.
     * @return True on success. False if at least one graph has failed to be loaded
     */
    template <typename BoostGraphsContainer, typename LabelsContainer, typename IdsContainer>
    bool readDataset(const std::string& datasetPath, BoostGraphsContainer& graphsContainer, LabelsContainer& labels,
            IdsContainer& ids) const;

    /**
     * Reads the dataset of unlabeled graphs at the given path.
     * Each sample of the dataset is assumed to be formatted as follows: NAME_ID.dot
     * - NAME: usually equal to "graph"
     * - ID: it is a user-defined string identifier
     * A naming convention is, for example, graph_1.dot, meaning an unlabeled graph with id 1.
     * @param[in] datasetPath A reference to the path of the dataset of graphs
     * @param[out] graphsContainer A reference to the specified generic type of container for the graphs
     * @param[out] ids A reference to the generic type of container for the graphs ids. Note that graphs ids are assumed to be strings.
     * @return True on success. False if at least one graph has failed to be loaded
     */
    template <typename BoostGraphsContainer, typename IdsContainer>
    bool readUnlabeledDataset(const std::string& datasetPath, BoostGraphsContainer& graphsContainer, IdsContainer& ids) const;

private:

    /**
     * Retrieve the DOT files of the specified directory path
     */
    bool getDirContent(const std::string& dir, std::vector<std::string>& files) const
    {
        DIR *dp;
        struct dirent *dirp;
        size_t found, len;

        if((dp=opendir(dir.c_str()))==NULL)
            return false;

        while ((dirp = readdir(dp)) != NULL)
        {
            std::string s(dirp->d_name);
            found=s.find_last_of(".");
            len=s.length()-found;
            //only .dot files are taken
            if(s!="."&&s!=".."&&s.substr(found+1, len)=="dot")
                files.push_back(s);
        }

        closedir(dp);

        return true;
    }

    /**
     * DOT graph reader
     */
    GraphReaderType graphReader;

    /**
     * Default label for graphs without class label
     */
    std::string DEFAULT_LABEL;

    /**
     * Default id for graphs without id
     */
    std::string DEFAULT_ID;

    /**
     * Max order of the loaded graphs
     */
    mutable NaturalType mMaxOrder;

    /**
     * Max size of the loaded graphs
     */
    mutable NaturalType mMaxSize;
};


// IMPL

template <class GraphReaderType, class GraphType>
template <typename BoostGraphsContainer, typename LabelsContainer>
bool GraphsDataset<GraphReaderType, GraphType>::readDataset(const std::string& datasetPath,
        BoostGraphsContainer& graphsContainer, LabelsContainer& labels) const
{
    //the container of ids is instantiated, but not returned...
    std::vector<std::string> ids(0);
    return readDataset(datasetPath, graphsContainer, labels, ids);
}

template <class GraphReaderType, class GraphType>
template <typename BoostGraphsContainer, typename LabelsContainer, typename IdsContainer>
bool GraphsDataset<GraphReaderType, GraphType>::readDataset(const std::string& datasetPath,
        BoostGraphsContainer& graphsContainer, LabelsContainer& labels, IdsContainer& ids) const
{
    bool test=true;
    std::vector<std::string> files;
    NaturalType maxOrder=0, maxSize=0;


    //obtain the list of .dot files
    getDirContent(datasetPath, files);

    //add graphs
    for(unsigned int i=0;i<files.size();i++)
    {
        GraphType g(0);

        //full path to the file
        std::string label, id, currentFile=files[i];
        std::string filePath=datasetPath+currentFile;

        //label and id identification
        size_t found,found2,len;

        found=currentFile.find_first_of("_");
        found2=currentFile.find_first_of("_", found+1);
        len=found2-(found+1);
        if(len<=0)
            id=DEFAULT_ID;
        else
            id=currentFile.substr(found+1, len);

        ids.insert(ids.begin(), id);

        found=currentFile.find_last_of("_");
        found2=currentFile.find_last_of(".");
        len=found2-(found+1);

        //troubles with the label?
        if(len<=0||found==std::string::npos||found2==std::string::npos)
            label=DEFAULT_LABEL;
        else
            label=currentFile.substr(found+1, len);

        labels.insert(labels.begin(), label);

        //stream
        std::ifstream stream(filePath.c_str(), std::ios::binary);
        if(stream.good())
        {
            graphReader.read(stream, g);
            stream.close();
        }
        else
        {
            test=false;
            std::string errMsg="Unable to load file: "+filePath;
            throw SpareLogicError(errMsg.c_str());
        }

        //updating max order and size
        if(boost::num_vertices(g)>maxOrder)
            maxOrder=boost::num_vertices(g);
        if(boost::num_edges(g)>maxSize)
            maxSize=boost::num_edges(g);

        graphsContainer.insert(graphsContainer.begin(), g);
    }

    mMaxOrder=maxOrder;
    mMaxSize=maxSize;

    return test;
}

template <class GraphReaderType, class GraphType>
template <typename BoostGraphsContainer, typename IdsContainer>
bool GraphsDataset<GraphReaderType, GraphType>::
    readUnlabeledDataset(const std::string& datasetPath, BoostGraphsContainer& graphsContainer, IdsContainer& ids) const
{
    bool test=true;
    std::vector<std::string> files;
    NaturalType maxOrder=0, maxSize=0;

    //obtain the list of .dot files
    getDirContent(datasetPath, files);

    //add graphs
    for(unsigned int i=0;i<files.size();i++)
    {
        GraphType g(0);

        //full path to the file
        std::string label, id, currentFile=files[i];
        std::string filePath=datasetPath+currentFile;

        //label and id identification
        size_t found, found2, len;

        found=currentFile.find_first_of("_");
        found2=currentFile.find_first_of("_", found+1);
        len=found2-(found+1);
        if(len<=0)
            id=DEFAULT_ID;
        else
            id=currentFile.substr(found+1, len);

        ids.insert(ids.begin(), id);

        //stream
        std::ifstream stream(filePath.c_str(), std::ios::binary);
        if(stream.good())
        {
            graphReader.read(stream, g);
            stream.close();
        }
        else
        {
            test=false;
            std::string errMsg="Unable to load file: "+filePath;
            throw SpareLogicError(errMsg.c_str());
        }

        //updating max order and size
        if(boost::num_vertices(g)>maxOrder)
            maxOrder=boost::num_vertices(g);
        if(boost::num_edges(g)>maxSize)
            maxSize=boost::num_edges(g);

        graphsContainer.insert(graphsContainer.begin(), g);
    }

    mMaxOrder=maxOrder;
    mMaxSize=maxSize;

    return test;
}

}

#endif
