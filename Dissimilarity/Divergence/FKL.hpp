/*
 * FKL.hpp
 *
 *  Created on: 2012-04-25
 *      Author: strozzino
 */

#ifndef FKL_HPP_
#define FKL_HPP_


#include <cmath>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief FKL (Fuzzy Kullback-Leibler) divergence of order beta.
 *
 * This class implements the @a Dissimilarity concept between two fuzzy sets.
 */
class FKL
{
public:

    /**
     * Default constructor
     */
    FKL()
    {
        mBeta=2.0;
    }

    /**
     * Main divergence method
     */
    template <typename SequenceContainer>
    RealType Diss(const SequenceContainer& c1, const SequenceContainer& c2) const;

    /**
     * Read-Write access to the beta parameter
     */
    RealType& Beta() { return mBeta; }

    /**
     * Read-only access to the beta parameter
     */
    const RealType& Beta() const { return mBeta; }

private:

    /**
     * Beta parameter
     */
    RealType mBeta;
};


template <typename SequenceContainer>
RealType FKL::Diss(const SequenceContainer& c1, const SequenceContainer& c2) const
{
    RealType sum=0.0, r1, r2, x1, x2, x3, x4, x5;
    typename SequenceContainer::const_iterator it, it2, itEnd;
    it=c1.begin();
    itEnd=c1.end();
    it2=c2.begin();

    //fetch the membership values defining the two fuzzy sets
    while(it!=itEnd)
    {
        r1=*it;
        r2=*it2;

        x1=(r1==0)?0:std::pow(r1, mBeta);
        x2=(r2==0)?0:std::pow(r2, 1.0-mBeta);
        x3=(1.0-r1==0)?0:std::pow(1.0-r1, mBeta);
        x4=(1.0-r2==0)?0:std::pow(1.0-r2, 1.0-mBeta);
        x5=(x1*x2)+(x3*x4);

        sum+=(x5-1.0);

        it++;
        it2++;
    }

    return sum/(mBeta-1.0);
}

}


#endif /* FKL_HPP_ */
