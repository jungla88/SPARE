//---------------------------------------------------------------------------
#ifndef CrispPartitionH
#define CrispPartitionH

#include <stdexcept>
#include <vector>

typedef std::vector<int> TPatternList;

class TCrispPartition
{
 private:

    std::vector<TPatternList> Partition;

 public:

    TCrispPartition(){};
    TCrispPartition(int);
    TCrispPartition(int, int*);
    TCrispPartition(std::vector<int>);
    TCrispPartition(const TCrispPartition&);
    TCrispPartition& operator=(const TCrispPartition&);

    int GetNumberOfPatterns() const;
    int GetNumberOfClasses() const {return Partition.size();};
    int GetClassCardinality(int Id) const {return Partition[Id].size();};
    int GetPattern(int Id, int Index) const {return (Partition[Id])[Index];};
    TPatternList GetPatternList(int Id) const {return Partition[Id];};

    // inizio stub
    //int GetClassId(int Index) const {return 0;};
    //void SetClassId(int Index, int Id) {};
    // fine stub

    void Merge(int Id1, int Id2);

    int* GetMap() const;

    bool IsEqual(TCrispPartition* OtherPartition) const;
};

//---------------------------------------------------------------------------
#endif
