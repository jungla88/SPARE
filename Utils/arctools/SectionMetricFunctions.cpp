//---------------------------------------------------------------------------
#include "SectionMetricFunctions.h"

prlreal TRealEuclideanDistance :: RealEucl(int Sz, prlreal* A, prlreal* B)
{
 prlreal Res = 0;
 for(int i = 0; i < Sz; i++) Res += std::pow(B[i] - A[i], 2);
 return prlreal( std::sqrt(Res) / std::sqrt(Sz) );
}

prlreal TComplexEuclideanDistance :: ComplexEucl(int Sz, prlreal* A, prlreal* B)
{
 prlreal Res = 0;
 for(int i = 0; i < Sz; i++) Res += std::pow(B[i] - A[i], 2);
 return prlreal( std::sqrt(Res) / (2*std::sqrt(Sz/2)) );
}

prlreal TRealManhattanDistance :: RealManh(int Sz, prlreal* A, prlreal* B)
{
 prlreal Res = 0;
 for(int i = 0; i < Sz; i++) Res += std::fabs(B[i] - A[i]);
 return prlreal( Res / Sz );
}

prlreal TComplexManhattanDistance :: ComplexManh(int Sz, prlreal* A, prlreal* B)
{
 prlreal Res = 0;
 for(int i = 0; i < Sz; i++) Res += std::fabs(B[i] - A[i]);
 return prlreal( Res / Sz );
}

prlreal TControlledCircularManhattanDistance :: CoCircManh(int Sz, prlreal* A, prlreal* B)
{
 prlreal Res = 0;
 for(int i = 0; i < Sz; i = i+2)
    {
     prlreal Aux = std::fabs(B[i] - A[i]);
     if (Aux>0.5) Aux = 1-Aux;
     Aux *= 2;
     Res += Aux*A[i+1]*B[i+1];
    }
 return prlreal( Res / (Sz/2) );
}

//---------------------------------------------------------------------------
