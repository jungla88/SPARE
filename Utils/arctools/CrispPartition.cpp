//---------------------------------------------------------------------------
#include "CrispPartition.h"

TCrispPartition::TCrispPartition(int P)
{
 if (P<1) throw std::runtime_error( "Invalid Dimension" );

 TPatternList AllPatterns(P);
 for(int i = 0; i < P; i++) AllPatterns[i] = i;
 Partition.push_back( AllPatterns );
}

TCrispPartition::TCrispPartition(int P, int* Data)
{
 if (P<1) throw std::runtime_error( "Invalid Dimension" );

 std::vector<int> IDs;
 for(int i = 0; i < P; i++)
    {
     bool Stop = false;
     unsigned int j = 0;
     while ( (j < IDs.size()) && (!Stop) )
        {
         if (IDs[j] == Data[i]) Stop = true;
         else j++;
        }

     if (j == IDs.size())
        {
         IDs.push_back(Data[i]);
         TPatternList NewCluster(1, i);
         Partition.push_back( NewCluster );
        }
     else
        {
         Partition[j].push_back(i);
        }
    }
}

TCrispPartition::TCrispPartition(std::vector<int> Data)
{
 if (Data.empty()) throw std::runtime_error( "Invalid Dimension" );
 int P = Data.size();

 std::vector<int> IDs;
 for(int i = 0; i < P; i++)
    {
     bool Stop = false;
     unsigned int j = 0;
     while ( (j < IDs.size()) && (!Stop) )
        {
         if (IDs[j] == Data[i]) Stop = true;
         else j++;
        }

     if (j == IDs.size())
        {
         IDs.push_back(Data[i]);
         TPatternList NewCluster(1, i);
         Partition.push_back( NewCluster );
        }
     else
        {
         Partition[j].push_back(i);
        }
    }
}

TCrispPartition::TCrispPartition(const TCrispPartition& Source)
{
 Partition = Source.Partition;
}

TCrispPartition& TCrispPartition::operator=(const TCrispPartition& Source)
{
 if (this != &Source)
    {
     Partition = Source.Partition;
    }

 return *this;
}

int TCrispPartition::GetNumberOfPatterns() const
{
 int P = 0;
 for(unsigned int i = 0; i < Partition.size(); i++) P += Partition[i].size();
 return P;
}

void TCrispPartition::Merge(int Id1, int Id2)
{
 int Cl = Partition.size();
 if (Id1 < 0 || Id1 >= Cl) throw std::runtime_error( "Invalid Class Id" );
 if (Id2 < 0 || Id2 >= Cl) throw std::runtime_error( "Invalid Class Id" );
 if (Id1 == Id2) throw std::runtime_error( "Invalid Class Id" );

 int LowId = (Id1 < Id2) ? Id1 : Id2;
 int UpId = (Id1 < Id2) ? Id2 : Id1;

 Partition[LowId].insert(Partition[LowId].end(), Partition[UpId].begin(), Partition[UpId].end() );
 std::vector<TPatternList>::iterator it = Partition.begin();
 for(int i = 0; i < UpId; i++) it++;
 Partition.erase(it);
}

int* TCrispPartition::GetMap() const
{
 int P = GetNumberOfPatterns();
 int* Map = new int[P];

 for(unsigned int i = 0; i < Partition.size(); i++)
    for(unsigned int j = 0; j < Partition[i].size(); j++)
       Map[ (Partition[i])[j] ] = i;

 return Map;
}

bool TCrispPartition::IsEqual(TCrispPartition* OtherPartition) const
{
 int P = GetNumberOfPatterns();
 int OtherP = OtherPartition->GetNumberOfPatterns();

 if (P != OtherP) throw std::runtime_error("Different Cardinality");

 int* Map = GetMap();
 int* OtherMap = OtherPartition->GetMap();

 bool temp = true;
 for(int i = 0; i < P; i++)
    if (Map[i] != OtherMap[i]) temp = false;

 delete[] Map;
 delete[] OtherMap;
 return temp;
}
//---------------------------------------------------------------------------
