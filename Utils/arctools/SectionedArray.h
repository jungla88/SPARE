//---------------------------------------------------------------------------
#ifndef SectionedArrayH
#define SectionedArrayH

#include "SectionMetricFunctions.h"
#include <string>
#include <vector>

typedef enum {stSwitch, stInteger, stReal, stComplex, stControlledCircular} TSectionType;

typedef struct
{
 std::string Name;
 TSectionType Type;
 int Size;
} TSectionDescriptor;

bool operator==(const TSectionDescriptor&, const TSectionDescriptor&);
bool operator!=(const TSectionDescriptor&, const TSectionDescriptor&);

typedef std::vector<TSectionDescriptor> TSectioning;
bool operator==(const TSectioning&, const TSectioning&);
bool operator!=(const TSectioning&, const TSectioning&);

class TSectionedArray
{
 friend class TVectorCollection;
 friend class TVectorGraphCollection;
 friend class TVectorDistanceTool;
 friend class TVectorGraphDistanceTool;
 friend class TVectorGraphCentroidTool;
 friend class TVectorNormalizationTool;
 friend class TVectorGraphNormalizationTool;
 friend class TVectorCentroidTool;

 private:

    int Size;
    prlreal* Data;

 public:

    TSectionedArray(const TSectioning&);
    TSectionedArray(int);
    TSectionedArray(const TSectionedArray&);
    TSectionedArray& operator=(const TSectionedArray&);
    ~TSectionedArray();
};

// metrica

class TSectionMetricManager
{
 friend class TVectorCollection;
 friend class TVectorGraphCollection;
 friend class TSectionedArray;
 friend class TVectorDistanceTool;
 friend class TVectorGraphDistanceTool;
 private:

    SMF MetricFunction;
    std::string FunctionCode;
    int ParNum;
    prlreal* FunctionParameters;
    std::string ParameterInfo;

 public:

    TSectionMetricManager(int, TVectorMetricFunction*);
    TSectionMetricManager(const TSectionMetricManager&);
    TSectionMetricManager& operator=(const TSectionMetricManager&);
    ~TSectionMetricManager();
};

typedef std::vector<TSectionMetricManager> TVectorMetricManager;

//---------------------------------------------------------------------------
#endif
