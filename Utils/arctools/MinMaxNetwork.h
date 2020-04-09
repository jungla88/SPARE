//---------------------------------------------------------------------------
#ifndef MinMaxNetworkH
#define MinMaxNetworkH

#include "VectorNormalizationTool.h"
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

const std::string HybClass = "HYBC";

typedef enum {mtSimpson, mtTrapezoidal} TMembershipType;

// HYPERBOX

class THyperBox
{
 protected:

    std::vector<prlreal> V;
    std::vector<prlreal> W;
    std::string Class;

    TMembershipType MembType;
    std::vector<prlreal> MembPars;
    // parametri
    // caso mtTrapezoidal: 1 parametro = gamma

 public:

    THyperBox(int N, TMembershipType MT);
    THyperBox(const THyperBox&);
    THyperBox& operator=(const THyperBox&);
    bool operator==(const THyperBox&) const;
    bool operator<(const THyperBox&) const;

    int GetInputSize() const {return V.size();};
    prlreal GetV(int i) const {return V[i];};
    void SetV(int i, prlreal Val){V[i] = Val;};
    prlreal GetW(int i) const {return W[i];};
    void SetW(int i, prlreal Val){W[i] = Val;};

    std::string GetClass() const {return Class;};
    void SetClass(std::string Val){Class = Val;};

    TMembershipType GetMembershipType() const {return MembType;};

    int GetMembParNumber() const {return MembPars.size();};
    prlreal GetMembPar(int i) const {return MembPars[i];};
    void SetMembPar(int i, prlreal Val){MembPars[i] = Val;};

    prlreal Membership(const prlreal* X) const;
    // X dev'essere della stessa dimensione
    // di V e W

    void WriteToFile(std::ofstream& Fp) const;

    void ReadFromFile(std::ifstream& Fp);
    // può causare la modifica di tutte le informazioni, compreso input size
};

// NETWORK

class TMinMaxNetwork
{
 private:

    std::string Name;
    std::vector<THyperBox> HyperBoxes;

    // preprocessing setup
    std::vector<int> ProjectionMask;
    TVectorNormalizationTool NormTool;

    // dati per ordinamento
    std::vector<prlreal> SortData;

    // report
    bool RepEnabled;
    std::string RName;

 public:

    TMinMaxNetwork(){Name = "DefaultMinMaxNetwork";
                     RepEnabled = false;
                     RName = "MinMaxNetworkClassificationReport.txt";};

    TMinMaxNetwork(std::string InitName,
                   std::vector<THyperBox>& InitHBs,
                   std::vector<int>& ProjMask,
                   TVectorNormalizationTool& InitNT);

    TMinMaxNetwork(std::string InitName,
                   std::vector<THyperBox>& InitHBs);

    TMinMaxNetwork(const TMinMaxNetwork&);
    TMinMaxNetwork& operator=(const TMinMaxNetwork&);
    bool operator<(const TMinMaxNetwork&) const;

    std::string GetName() const {return Name;};
    void SetName(std::string name) {Name = name;};

    int GetNumberOfHyperBoxes() const {return HyperBoxes.size();};
    std::vector<THyperBox> GetHyperBoxes() const {return HyperBoxes;};
    std::string GetHyperBoxClass(int Index) const {return HyperBoxes.at(Index).GetClass();};

    int GetInterfaceInputSize() const;
    int GetCoreInputSize() const;

    std::vector<int> GetProjectionMask() const {return ProjectionMask;};
    TVectorNormalizationTool GetNormalizationTool() const {return NormTool;};

    std::vector<prlreal> GetSortData() const {return SortData;};
    void SetSortData(std::vector<prlreal>& Val){SortData = Val;};

    // setup report
    bool ReportEnabled() const {return RepEnabled;};
    void EnableReport(){RepEnabled = true;};
    void DisableReport(){RepEnabled = false;};
    void ReportSetup(std::string name){RName = name;};

    std::vector<std::string> Classify(const TVectorCollection* TestSet) const;

    std::string RawClassify(const prlreal* X) const;
    // il preprocessing non viene effettuato, il pattern deve avere dimensione
    // pari a GetCoreInputSize

    std::string RawClassify(const prlreal* X, std::map<std::string, prlreal>& ExtraData) const;
    // come sopra, ma torna anche la mappa con l'appartenenza per ciascuna classe

    void WriteToFile(std::string NomeFile) const;
    void LoadFromFile(std::string NomeFile);
};

//---------------------------------------------------------------------------
#endif
