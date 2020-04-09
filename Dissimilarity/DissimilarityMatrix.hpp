//  DissimilarityMatrix, part of the SPARE library.
//  Copyright (C) 2011 Azzurra Sebastiani
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

/** @brief File DissimilarityMatrix.hpp, that contains Delta class.
 *
 * Contains the declaration of the DissimilarityMatrix class.
 *
 * @file DissimilarityMatrix.hpp
 * @author Azzurra Sebastiani
 */

#ifndef _DissimilarityMatrix_h_
#define _DissimilarityMatrix_h_

// BOOST INCLUDES
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

// STD INCLUDES
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>



namespace spare {  // Inclusione in namespace spare.

/** @brief Dissimalarity Matrix.
 *
 * This class implements the @a Dissimilarity concept.
 * This class contains the implementation of the dissimilarity matrix of a set of indexed patterns.
 * Each pattern is indexed with a string id, corresponding one-to-one with an index position in the dissimilarity matrix (both rows and columns).
 * The dissimilarities are assumed always pre-computed, so the dissimilarity is actually an access to the loaded matrix.
 */
class DissimilarityMatrix
{
public:

    /**
     * string - index hash mapping type
     */
    typedef boost::unordered_map<std::string, NaturalType> HashMapType;

    /**
     * Dissimilarity matrix type
     */
    typedef boost::numeric::ublas::matrix<RealType> DissMatrixType;

    /**
     *
     */
    typedef std::vector<std::string> SeqIndicesType;


    /**
     * Read-write access to the dissimilarity matrix
     */
    DissMatrixType& DissMatrix() { return mDissMatrix; }

    /**
     * Read-only access to the dissimilarity matrix
     */
    const DissMatrixType& DissMatrix() const { return mDissMatrix; }
    
    /**
     * Read-write access to the hash map (index-position)
     */
    HashMapType& HashMap() { return mHashMap; }

    /**
     * Read-only access to the hash map
     */
    const HashMapType& HashMap() const { return mHashMap; }

    /**
     * Read-write access to the indices
     */
    SeqIndicesType& Indices() { return mIndices; }

    /**
     * Read-only access to the indices
     */
    const SeqIndicesType& Indices() const { return mIndices; }


// OPERATIONS

    /** DissimilarityMatrix distance computation.
    *
    * @param[in] rA A reference to the first element.
    * @param[in] rB A reference to the second element.
    * @return The pre-computed dissimilarity value.
    */
    RealType Diss(const std::string& rA, const std::string& rB) const;


    /** Set the parameters needed to use this kind of dissimilarity.
    *
    * @param[in] M: A reference to the first element, it represents the dissimilarity matrix.
    * @param[in] H: A reference to the second element, it represents the hash map between strings and matrix indexes.
    */
    void SetMatrix(boost::numeric::ublas::matrix<RealType>& M, HashMapType& H)
    {
        mDissMatrix=M;
        mHashMap=H;
    }


    /** Parameters extraction from a file of a pre-computed dissimilarity matrix.
     *
     * @param[in] MatFile: A reference to the file.
     */
    void LoadMatrix(std::istream& MatFile)
    {
        std::string line;
        char *lineTok;
        char *ptr;
        int row, col, i;

        if (!MatFile.good())
        {
            throw SpareLogicError("Input stream trouble.");
        }

        /*----------Read file --------------*/

        //Skip comments, check if the first char of the line is #, skip blank lines
        getline (MatFile, line, '\n' );
        while((line[0]=='#' || (strlen(line.c_str())==0)) && getline (MatFile, line, '\n' ));
        //      while ((line[0]=='#')&& getline (MatFile, line, '\n' ));

        // Read first line with characters, string extraction from it
        HashIndex=0;

        char ptr0[strlen(line.c_str())];
        strcpy(ptr0,line.c_str());

        ptr = strtok(ptr0, " ,\n");
        while (ptr != NULL)
        {
            if (strcspn(ptr, " ") == strlen(ptr))
            {
                mIndices.push_back(ptr);
                //set correspondence between string extracted and the index in the matrix
                mHashMap[mIndices[HashIndex]]=HashIndex;
                HashIndex++;
            }
            ptr = strtok(NULL, " ,\n");
        }

        MatSize=mIndices.size();
        mDissMatrix=boost::numeric::ublas::matrix<RealType>(MatSize, MatSize);

        /*-------Get the data values now an build the matrix ------------*/

        // Read each line of the file and extract each score value
        row=0;

        while (getline (MatFile, line, '\n' ))
        {
            char line1[strlen(line.c_str())];
            strcpy(line1,line.c_str());

            if(strlen(line.c_str()) > 1 && row < MatSize)
            {
                if (row >= 0 && row < MatSize)
                {
                    col=0;
                    lineTok = strtok(line1, " ,\n");
                    while (lineTok != NULL)
                    {
                        if (strspn(lineTok, "+-.0123456789") == strlen(lineTok))
                        {
                            if (col >= 0 && col < MatSize)
                                mDissMatrix(row,col) = atof(lineTok);
                            col++;
                        }
                        lineTok = strtok(NULL, " ,\n");
                    }
                }

                row++;
            }
        }
    }

private:

    /**
     * boost type dissimilarity matrix
     */
    DissMatrixType mDissMatrix;

    /**
     * hash map (from string to index)
     */
    HashMapType mHashMap;

    /**
     * Sequence of Indices
     */
    SeqIndicesType mIndices;

    /**
     * hash index
     */
    NaturalType HashIndex;

    /**
     * dissimilarity matrix size
     */
    NaturalType MatSize;

};

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================


RealType DissimilarityMatrix::Diss(const std::string& rA, const std::string& rB) const
{
    //TODO: add bound check for indices
    return mDissMatrix(mHashMap.at(rA), mHashMap.at(rB));
}

}  // namespace spare

#endif  // _DissimilarityMatrix_h_
