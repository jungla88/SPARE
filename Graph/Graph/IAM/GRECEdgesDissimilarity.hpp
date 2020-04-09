//  GRECEdgesDissimilarity class, part of the SPARE library.
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

/** @brief File GRECEdgesDissimilarity.hpp, that contains GRECEdgesDissimilarity class.
 *
 * Contains the declaration of the GRECEdgesDissimilarity class.
 *
 * @file GRECEdgesDissimilarity.hpp
 * @author Lorenzo Livi
 */

#ifndef _GRECEdgesDissimilarity_h_
#define _GRECEdgesDissimilarity_h_

// STD INCLUDES
#include <vector>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>


/** @brief Dissimilarity for edges labels of an IAM GREC graph type
 */
class GRECEdgesDissimilarity{
public:

    spare::RealType& Alpha(){
        return alpha;
    }

    const spare::RealType& Alpha() const{
        return alpha;
    }

    spare::RealType& Beta(){
        return beta;
    }

    const spare::RealType& Beta() const{
        return beta;
    }

    spare::RealType& Gamma(){
        return gamma;
    }

    const spare::RealType& Gamma() const{
        return gamma;
    }

    spare::RealType& Delta(){
        return delta;
    }

    const spare::RealType& Delta() const{
        return delta;
    }

    GRECEdgesDissimilarity(){
        alpha = 1;
        beta = 1;
        gamma = 1;
        delta = 1;
    }

    inline spare::RealType Diss(const EdgeLabelType& gE1, const EdgeLabelType& gE2) const{

        spare::RealType D = 0;
        spare::NaturalType f1 = gE1.frequency, f2 = gE2.frequency;

        if(f1==f2)
        {
            if(f1==1){
                if(gE1.type0.compare("line")==0 && gE2.type0.compare("line")==0){
                    D= alpha*(std::abs(gE1.angle0 - gE2.angle0) + 1.575)/3.15;
                }else if(gE1.type0.compare("arc")==0 && gE2.type0.compare("arc")==0){
                    D= beta*std::abs(gE1.angle0 - gE2.angle0);
                }else
                    D=gamma;
            }
            else
                if(gE1.type0.compare("line")==0 && gE2.type0.compare("line")==0){
                    D= alpha*(std::abs(gE1.angle0 - gE2.angle0) + 1.575)/(2*3.15) + beta*std::abs(gE1.angle1 - gE2.angle1);
                }else if(gE1.type0.compare("arc")==0 && gE2.type0.compare("arc")==0){
                    D= alpha*(std::abs(gE1.angle1 - gE2.angle1) + 1.575)/(2*3.15) + beta*std::abs(gE1.angle0 - gE2.angle0);
                }else
                    D=gamma;
        }else
            D=delta;

        return D;
    }

private:

    //weights
    spare::RealType alpha;
    spare::RealType beta;
    spare::RealType gamma;
    spare::RealType delta;
};

#endif
