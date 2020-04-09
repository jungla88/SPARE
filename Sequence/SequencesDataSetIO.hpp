//  SequencesDataset class, part of the SPARE library.
//  Copyright (C) 2014 Francesca Possemato
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

/** @brief File SequencesDataset.hpp, that contains the dataset class for labeled sequences.
 *
 * @file SequencesDataset.hpp
 * @author Francesca Possemato
 */

#ifndef SEQUENCESDATASETIO_HPP
#define SEQUENCESSDATASETIO_HPP


//STD INCLUDES
#include <string>
#include <vector>
#include <istream>
#include <sys/types.h>
#include <dirent.h>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/Sequence/SequenceIO.hpp>
#include <spare/Sequence/SequencesDataSet.hpp>


namespace spare {

/** @brief Dataset utility for labeled sequences.
 *  The class must be configured with two template arguments.
 *  The type of sequence reader (first argument) to be used for the specific type of element of labeled sequence (second argument).
 *  The dataset class accepts a directory path and generates STL containers of labeled sequences and related class labels (if specified) as output.
 */
template <typename SequenceReaderType, typename AtomicElement>
class SequencesDatasetIO {
public:

    /**
     * Reads the dataset of sequences at the given path.
     * Read examples for the standard format used.
     * @param[in] datasetPath A reference to the path of the dataset of sequences
     * @param[out] sequencesContainer A reference to the specified generic type of container for the sequences
     * @param[out] labels A reference to the specified generic type of container for the labels of the sequences. Note that the labels are assumed to be strings.
     * @return True on success. False if at least one sequence has failed to be loaded
     */
    template <typename LabelsContainer>
    bool readDataset(const std::string& datasetPath, SequencesDataSet<AtomicElement>& dataset, LabelsContainer& labels) const;

    // DA COMPLETARE
    template <typename LabelsContainer>
    bool writeDataset(const std::string& datasetPath, SequencesDataSet<AtomicElement>& dataset, LabelsContainer& labels) const;

    template <typename LabelsContainer>
    bool readFeaturesDataset(const std::string& datasetPath, SequencesDataSet<AtomicElement>& sequencesContainer, LabelsContainer& labels) const;

    void writeFeaturesDataset(const std::string& datasetPath, SequencesDataSet<AtomicElement>& sequencesContainer) const;

    bool readAlphabet(const std::string& alphabetPath, SequencesDataSet<AtomicElement>& alphabet) const;

    void writeAlphabet(const std::string& alphabetPath, SequencesDataSet<AtomicElement>& alphabet) const;



};

// IMPL
template <class SequenceReaderType, class AtomicElement>
template <typename LabelsContainer>
bool SequencesDatasetIO<SequenceReaderType, AtomicElement>::readDataset(const std::string& datasetPath, SequencesDataSet<AtomicElement>& dataset, LabelsContainer& labels) const
{
	typename spare::SequencesDataSet<AtomicElement>::size_type i, N;
	std::string Buf;
	SequenceIO sIO;
	SequenceReaderType sequenceReader;
	//stream
	std::ifstream seqFile(datasetPath.c_str(), std::ios::binary);

	if(!seqFile.good())
	{
		std::string errMsg="Unable to load file: "+ datasetPath + " (1)";
		throw SpareLogicError(errMsg.c_str());
	}

	seqFile >> N;

	if(!seqFile.good())
	{
		std::string errMsg="Unable to load file: "+ datasetPath + " (2)";
		throw SpareLogicError(errMsg.c_str());
	}

	seqFile >> Buf;

	if(!seqFile.good() || Buf!= ":")
	{
		std::string errMsg="Unable to load file: "+ datasetPath + " (3)";
		throw SpareLogicError(errMsg.c_str());
	}

	if((N > dataset.max_size()) || (N > labels.max_size()))
	{
		std::string errMsg="Unable to load file: "+ datasetPath + " (4)";
		throw SpareLogicError(errMsg.c_str());
	}

	dataset.resize(N);
	labels.resize(N);

	for(i=0; i<N; ++i)
	{
		seqFile >> labels[i];
		if(!seqFile.good())
		{
			std::string errMsg="Unable to load file: "+ datasetPath + " (5)";
			throw SpareLogicError(errMsg.c_str());
		}
		sIO.readSequence(seqFile, sequenceReader, dataset[i]);

	}
	seqFile.close();

}

template <class SequenceReaderType, class AtomicElement>
template <typename LabelsContainer>
bool SequencesDatasetIO<SequenceReaderType, AtomicElement>::writeDataset(const std::string& datasetPath, SequencesDataSet<AtomicElement>& dataset, LabelsContainer& labels) const
{
	//stream
	std::ofstream seqFile(datasetPath.c_str(), std::ios::binary);
	typename spare::SequencesDataSet<AtomicElement>::size_type i, N;
	SequenceIO sIO;

	N = dataset.size();
	std::string Buf;

	if(!seqFile.good())
	{
		std::string errMsg="Unable to load file: "+ datasetPath + " (1)";
		throw SpareLogicError(errMsg.c_str());
	}

	seqFile << N;

	if(!seqFile.good())
	{
		std::string errMsg="Unable to load file: "+ datasetPath + " (2)";
		throw SpareLogicError(errMsg.c_str());
	}

	seqFile << ":" << endl;

	for(i=0; i<N; ++i)
	{
		seqFile << labels[i] << " ";

	}

}

template <class SequenceReaderType, class AtomicElement>
template <typename LabelsContainer>
bool SequencesDatasetIO<SequenceReaderType, AtomicElement>::readFeaturesDataset(const std::string& datasetPath, SequencesDataSet<AtomicElement>& sequencesContainer, LabelsContainer& labels) const
{
	//stream
	std::ifstream seqFile(datasetPath.c_str(), std::ios::binary);
	SequenceIO sIO;
	Sequence<AtomicElement> seq;
	SequenceReaderType sequenceReader;

	if (!seqFile.good())
	{
		std::string errMsg="Unable to load file: "+ datasetPath;
		throw SpareLogicError(errMsg.c_str());
	}

	while(seqFile.good())
	{
		if (!seqFile.good())
			{
				std::string errMsg="Unable to load portion of file: "+ datasetPath;
				throw SpareLogicError(errMsg.c_str());
			}
		sIO.readFeaturesSequence(seqFile, sequenceReader, seq);

		if(seqFile.good())
		{
			//cout << "etichetta: " << seq[seq.size()-1] << endl;
			labels.push_back(seq[seq.size()-1]);
			seq.pop_back();
			sequencesContainer.push_back(seq);
		}
	}

	return true;

}

template <class SequenceReaderType, class AtomicElement>
void SequencesDatasetIO<SequenceReaderType, AtomicElement>::writeFeaturesDataset(const std::string& datasetPath, SequencesDataSet<AtomicElement>& sequencesContainer) const
{
	//stream
	std::ofstream seqFile(datasetPath.c_str(), std::ios::binary);
	SequenceIO sIO;

	if (!seqFile.good())
	{
		std::string errMsg="Unable to load file: "+ datasetPath;
		throw SpareLogicError(errMsg.c_str());
	}

	typename SequencesDataSet<AtomicElement>::const_iterator         Sit;
	Sit= sequencesContainer.begin();

	for(unsigned int i=0; i<sequencesContainer.size(); i++)
	{
		sIO.writeFeaturesSequence(seqFile, sequencesContainer[i]);
		seqFile << endl;
	}

}

template <class SequenceReaderType, class AtomicElement>
bool SequencesDatasetIO<SequenceReaderType, AtomicElement>::readAlphabet(const std::string& alphabetPath, SequencesDataSet<AtomicElement>& alphabet) const
{
	//stream
	std::ifstream seqFile(alphabetPath.c_str(), std::ios::binary);

	if(!seqFile.good())
	{
		std::string errMsg="Unable to load file: "+ alphabetPath + " (1)";
		throw SpareLogicError(errMsg.c_str());
	}

	alphabet.clear();

	while(!seqFile.eof())
	{
		std::string sLine = "";
		std::getline(seqFile,sLine);
		alphabet.push_back(sLine);
	}
	seqFile.close();

}

template <class SequenceReaderType, class AtomicElement>
void SequencesDatasetIO<SequenceReaderType, AtomicElement>::writeAlphabet(const std::string& alphabetPath, SequencesDataSet<AtomicElement>& alphabet) const
{
	//stream
	std::ofstream seqFile(alphabetPath.c_str(), std::ios::binary);
	SequenceIO sIO;

	if (!seqFile.good())
	{
		std::string errMsg="Unable to load file: "+ alphabetPath;
		throw SpareLogicError(errMsg.c_str());
	}

	for (int i = 0; i < alphabet.size(); i++)
	{
		for (int j = 0; j < alphabet[i].size(); j++)
		{
			seqFile << alphabet[i][j];
		}
		seqFile <<std::endl;
	}

}

}

#endif

