//---------------------------------------------------------------------------
#ifndef SectionMetricFunctionsH
#define SectionMetricFunctionsH

#include "PrlDefinitions.h"

#include <cmath>
#include <string>

enum {pnVectorSize = -1, pnNoParameters = 0};

typedef prlreal(*SMF)(int, prlreal*, prlreal*);

class TVectorMetricFunction
{
 public:
 virtual SMF GetFunction() = 0;
 virtual std::string GetUniqueCode() = 0;
 virtual int GetNumberOfParameters() = 0;
 virtual std::string GetParameterInfo() = 0;
};

// Distanza euclidea
class TRealEuclideanDistance : public TVectorMetricFunction
{
 private:

    static prlreal RealEucl(int Size, prlreal* A, prlreal* B);

 public:

    SMF GetFunction(){return RealEucl;};
    std::string GetUniqueCode(){return "RED";};
    int GetNumberOfParameters(){return pnNoParameters;};
    std::string GetParameterInfo(){return "No parameters";};
};

class TComplexEuclideanDistance : public TVectorMetricFunction
{
 private:

    static prlreal ComplexEucl(int Size, prlreal* A, prlreal* B);

 public:

    SMF GetFunction(){return ComplexEucl;};
    std::string GetUniqueCode(){return "CED";};
    int GetNumberOfParameters(){return pnNoParameters;};
    std::string GetParameterInfo(){return "No parameters";};
};

// Distanza Manhattan
class TRealManhattanDistance : public TVectorMetricFunction
{
 private:

    static prlreal RealManh(int Size, prlreal* A, prlreal* B);

 public:

    SMF GetFunction(){return RealManh;};
    std::string GetUniqueCode(){return "RMD";};
    int GetNumberOfParameters(){return pnNoParameters;};
    std::string GetParameterInfo(){return "No parameters";};
};

class TComplexManhattanDistance : public TVectorMetricFunction
{
 private:

    static prlreal ComplexManh(int Size, prlreal* A, prlreal* B);

 public:

    SMF GetFunction(){return ComplexManh;};
    std::string GetUniqueCode(){return "CMD";};
    int GetNumberOfParameters(){return pnNoParameters;};
    std::string GetParameterInfo(){return "No parameters";};
};

class TControlledCircularManhattanDistance : public TVectorMetricFunction
{
 private:

    static prlreal CoCircManh(int Size, prlreal* A, prlreal* B);

 public:

    SMF GetFunction(){return CoCircManh;};
    std::string GetUniqueCode(){return "CCMD";};
    int GetNumberOfParameters(){return pnNoParameters;};
    std::string GetParameterInfo(){return "No parameters";};
};
//---------------------------------------------------------------------------
#endif
