//---------------------------------------------------------------------------
#include "SectionedArray.h"

// TSectionDescriptor

bool operator==(const TSectionDescriptor& A, const TSectionDescriptor& B)
{
 return (A.Name == B.Name && A.Type == B.Type && A.Size == B.Size);
}

bool operator!=(const TSectionDescriptor& A, const TSectionDescriptor& B)
{
 return (A.Name != B.Name || A.Type != B.Type || A.Size != B.Size);
}

bool operator==(const TSectioning& A, const TSectioning& B)
{
 if (A.size() != B.size()) return false;

 bool temp = true;
 int S = A.size();

 for(int s = 0; s < S; s++)
    if (A[s] != B[s]) temp = false;

 return temp;
}

bool operator!=(const TSectioning& A, const TSectioning& B)
{
 return !(A == B);
}

// TSectionedArray

TSectionedArray :: TSectionedArray(const TSectioning& SD)
{
 int S = SD.size();
 // numero di sezioni

 int Comps = 0;
 for(int i = 0; i < S; i++) Comps += SD[i].Size;
 // numero totale componenti

 Size = Comps;
 Data = new prlreal[Size];
 for(int i = 0; i < Size; i++) Data[i] = 0;
}

TSectionedArray :: TSectionedArray(int size)
{
 Size = size;
 Data = new prlreal[Size];
 for(int i = 0; i < Size; i++) Data[i] = 0;
}

TSectionedArray :: TSectionedArray(const TSectionedArray& Source)
{
 Size = Source.Size;
 Data = new prlreal[Size];
 for(int i = 0; i < Size; i++) Data[i] = Source.Data[i];
}

TSectionedArray& TSectionedArray :: operator=(const TSectionedArray& Source)
{
 if (this != &Source)
    {
     delete[] Data;

     Size = Source.Size;
     Data = new prlreal[Size];
     for(int i = 0; i < Size; i++) Data[i] = Source.Data[i];
    }

 return *this;
}

TSectionedArray :: ~TSectionedArray()
{
 delete[] Data;
}

// TSectionMetricManager

TSectionMetricManager :: TSectionMetricManager(int SectionSize, TVectorMetricFunction* metrics)
{
 MetricFunction = metrics->GetFunction();
 FunctionCode = metrics->GetUniqueCode();
 int pn = metrics->GetNumberOfParameters();
 if (pn == pnVectorSize) ParNum = SectionSize;
 else ParNum = pn;

 if (ParNum != pnNoParameters) FunctionParameters = new prlreal[ParNum];
 else FunctionParameters = NULL;

 ParameterInfo = metrics->GetParameterInfo();
}

TSectionMetricManager :: TSectionMetricManager(const TSectionMetricManager& Source)
{
 MetricFunction = Source.MetricFunction;
 FunctionCode = Source.FunctionCode;
 ParNum = Source.ParNum;

 if (ParNum != pnNoParameters)
    {
     FunctionParameters = new prlreal[ParNum];
     for(int i = 0; i < ParNum; i++) FunctionParameters[i] = Source.FunctionParameters[i];
    }
 else FunctionParameters = NULL;

 ParameterInfo = Source.ParameterInfo;
}

TSectionMetricManager& TSectionMetricManager :: operator=(const TSectionMetricManager& Source)
{
 if (this != &Source)
    {
     if (FunctionParameters) delete FunctionParameters;

     MetricFunction = Source.MetricFunction;
     FunctionCode = Source.FunctionCode;
     ParNum = Source.ParNum;

     if (ParNum != pnNoParameters)
        {
         FunctionParameters = new prlreal[ParNum];
         for(int i = 0; i < ParNum; i++) FunctionParameters[i] = Source.FunctionParameters[i];
        }
     else FunctionParameters = NULL;

     ParameterInfo = Source.ParameterInfo;
    }

 return *this;
}

TSectionMetricManager :: ~TSectionMetricManager()
{
 if (FunctionParameters) delete FunctionParameters;
}

//---------------------------------------------------------------------------
