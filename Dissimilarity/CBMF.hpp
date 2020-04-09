//  CBMF class, part of the SPARE library.
//  Copyright (C) 2011 Guido Del Vescovo
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

/** @brief File CBMF.hpp, that contains CBMF class.
 *
 * Contains the declaration of the CBMF class.
 *
 * @file CBMF.hpp
 * @author Lorenzo Livi
 */

#ifndef _CBMF_h_
#define _CBMF_h_

// STD INCLUDES
#include <iterator>

// BOOST INCLUDES
#include <boost/unordered_set.hpp>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief Container-based Best Match First dissimilarity.
 *
 * This class implements the @a Dissimilarity concept.
 * This class contains functions for the computation of the Container-based Best Match First dissimilarity.
 * The class implements a greedy solution to the assignment problem, generalized between two sets of arbitrary objects.
 */
template <class Dissimilarity>
class CBMF
{
public:

    /**
     * hash set type
     */
    typedef boost::unordered_set<NaturalType> HashSetType;


    /**
     * Default constructor
     */
    CBMF()
    {
        //default weighting factors
        mPSubstitution=0.8;
        mPInsertion=0.1;
        mPDeletion=0.1;
        //default insertion and deletion costs
        mInsertionCost=1;
        mDeletionCost=1;
    }

// OPERATIONS

   /** Container-based Best Match First dissimilarity computation.
    *
    * @param[in] aC1 A reference to the first container.
    * @param[in] aC2 A reference to the second container.
    * @return The dissimilarity value.
    */
   template <typename Container1, typename Container2>
   RealType Diss(const Container1& aC1, const Container2& aC2) const;

// SETUP

   /**
    * Read/Write access to the dissimilarity agent.
    */
   Dissimilarity& DissimilarityAgent() { return mDiss; }

   /**
    * Read-only access to the dissimilarity agent.
    */
   const Dissimilarity& DissimilarityAgent() const { return mDiss; }

   /**
    * Read/Write access to the insertion cost.
    */
   RealType& InsertionCost() { return mInsertionCost; }

   /**
    * Read-only access to the insertion cost.
    */
   const RealType& InsertionCost() const { return mInsertionCost; }

   /**
    * Read/Write access to the deletion cost.
    */
   RealType& DeletionCost() { return mDeletionCost; }

   /**
    * Read-only access to the deletion cost.
    */
   const RealType& DeletionCost() const { return mDeletionCost; }

   /**
    * Read/Write access to the substitution weighting parameter.
    */
   RealType& PSubstitution() { return mPSubstitution; }

   /**
    * Read-only access to the substitution weighting parameter.
    */
   const RealType& PSubstitution() const { return mPSubstitution; }

   /**
    * Read/Write access to the insertion weighting parameter.
    */
   RealType& PInsertion() { return mPInsertion; }

   /**
    * Read-only access to the insertion weighting parameter.
    */
   const RealType& PInsertion() const { return mPInsertion; }

   /**
    * Read/Write access to the deletion weighting parameter.
    */
   RealType& PDeletion() { return mPDeletion; }

   /**
    * Read-only access to the deletion weighting parameter.
    */
   const RealType& PDeletion() const { return mPDeletion; }

private:

   /**
    * Constant insertion cost
    */
   RealType mInsertionCost;

   /**
    * Constant deletion cost
    */
   RealType mDeletionCost;

   /**
    * Substitution parameter
    */
   RealType mPSubstitution;

   /**
    * Insertion parameter
    */
   RealType mPInsertion;

   /**
    * Deleteion parameter
    */
   RealType mPDeletion;

   /**
    * Dissimilarity measure for the elements of the containers
    */
   Dissimilarity mDiss;

};


//IMPL.


template <class Dissimilarity>
template <typename Container1, typename Container2>
RealType CBMF<Dissimilarity>::Diss(const Container1& aC1, const Container2& aC2) const
{
    //Vars
    NaturalType countC1=0, countC2=0;
    typename Container1::size_type sizeC1=aC1.size();
    typename Container2::size_type sizeC2=aC2.size();
    RealType totSubsCost= 0., totInsertionCost=0., totDeletionCost=0., absSizeDiff;
    typename Container1::const_iterator it1;
    typename Container2::const_iterator it2;
    HashSetType hs;

    absSizeDiff=std::abs(static_cast<RealType>(sizeC1)-static_cast<RealType>(sizeC2));

    it1=aC1.begin();
    while(aC1.end()!=it1)
    {
        //
        if(countC1>=sizeC2)
            break;

        it2=aC2.begin();
        RealType minDiss=std::numeric_limits<RealType>::max();
        NaturalType indexC2=0;
        countC2=0;

        while(aC2.end()!=it2)
        {
            //if NOT already assigned
            if(hs.find(countC2)==hs.end())
            {
                RealType tmpDiss=mDiss.Diss(*it1, *it2);

                if(tmpDiss<minDiss)
                {
                    indexC2=countC2;
                    minDiss=tmpDiss;
                }
            }

            countC2++;
            it2++;
        }

        //save the last assigned element of C2 to the current element of C1
        hs.insert(indexC2);
        //inc tot. substitutions cost
        totSubsCost+=minDiss;

        countC1++;
        it1++;
    }

    //insertions
    if(sizeC1>sizeC2)
        totInsertionCost=mInsertionCost*absSizeDiff;
    //deletions
    else
        totDeletionCost=mDeletionCost*absSizeDiff;

    //weighted sum of the costs
    return (mPSubstitution*totSubsCost)+(mPInsertion*totInsertionCost)+(mPDeletion*totDeletionCost);
}

}  // namespace spare

#endif  // _CBMF_h_
