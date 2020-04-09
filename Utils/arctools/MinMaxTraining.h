//---------------------------------------------------------------------------

#ifndef MinMaxTrainingH
#define MinMaxTrainingH

#include "MinMaxNetwork.h"
#include <fstream>
#include <list>
#include <map>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

typedef enum {hsPure, hsHybrid, hsEmpty} THyperBoxStatus;
// Empty: vuoto
// Pure: non vuoto e puro
// Hybrid: non vuoto e ibrido

typedef enum {cmCentroids} TCutMode;
// tipo di taglio HyperBox

typedef enum {hdSingle, hdBlock} THBDeletionMode;
// tipo di cancellazione HyperBox

typedef std::map< std::string, std::list<int> > THyperBoxContent;

typedef struct
{
 int m;
 prlreal alpha;
} TCutPoint;

// TTempHyperBox

class TTempHyperBox : public THyperBox
{
 private:

    const TVectorCollection* DataSet;
    THyperBoxStatus Status;
    THyperBoxContent Content;

    void InitVW(const prlreal*);
    // primo pattern

    void UpdateVW(const prlreal*);
    // eventuale espansione

    void AddPattern(int Index);
    // aggiunge all'HyperBox il pattern di DataSet di indice Index
    // l'HyperBox si ridimensiona e cambia stato e classe di conseguenza

    bool SimpleOverlap(prlreal v1, prlreal w1, prlreal v2, prlreal w2) const;
    // test di overlap monodimensionale

 public:

    TTempHyperBox(const TVectorCollection* DS, TMembershipType MT);
    TTempHyperBox(const TTempHyperBox&);
    TTempHyperBox& operator=(const TTempHyperBox&);
    bool operator==(const TTempHyperBox&) const;
    bool operator<(const TTempHyperBox&) const;

    int GetNumberOfPatterns() const;
    void GetPatternIndexes(std::list<int>& PattInd) const;
    // a PattInd verranno appesi gli indici dei pattern contenuti

    THyperBoxStatus GetStatus() const {return Status;};

    void Clear();
    // l'HB viene svouotato del contenuto

    void Fill(std::list<int> PattInd);
    // l'Hyperbox viene svuotato e poi riempito con i pattern indicati

    void TotalFill();
    // l'Hyperbox viene svuotato e poi riempito con tutti i pattern del DataSet

    bool Includes(int PattInd);
    // torna vero se il pattern di indice PattInd è incluso nell'HB

    void Affiliate(int PattInd);
    // il pattern di indice PattInd viene affiliato all'HB, ovvero, incluso
    // nel contenuto ma senza aggiornare nulla: né estremi, né stato, né classe

    int GetNumberOfIntruders() const;
    // numero di pattern contenuti nell'HB, ma di classe diversa dalla classe etichetta

    TCutPoint CalcCutPoint(TCutMode CM) const;

    void Cut(TCutPoint CP, TTempHyperBox& Child1, TTempHyperBox& Child2) const;
    // Child1 e Child2 vengono resettati e poi riempiti con i risultati
    // del taglio

    void Fusion(const TTempHyperBox& Other, TTempHyperBox& Result) const;
    // fonde l'istanza corrente con Other, tornando il risultato in Result
    // (previamente resettato)

    bool OverlapsWith(const TTempHyperBox& Other) const;
    // torna vero se l'istanza corrente si sovrappone con Other

    bool OverlapsWithFusionOf(const TTempHyperBox& Other1, const TTempHyperBox& Other2) const;
    // torna vero se l'istanza corrente si sovrappone con l'ipotetica fusione di Other1 e Other2,
    // la fusione non viene effettivamente realizzata per motivi di efficienza

    void WriteContentDebug(std::string NomeFile) const;   // debug
};

// TCoverage

typedef std::map< std::string, std::list<TTempHyperBox> > TCoverage;

// funzione eliminazione incongruenti

int DeleteIncongruent(TVectorCollection* DataSet);
// elimina gli incongruenti tornando il numero di pattern eliminati

// TArcParcTraining

class TArcParcTraining
{
 private:

    TMembershipType MembType;
    std::vector<prlreal> MembPars;
    // parametri
    // caso mtTrapezoidal o mtSimpson: 1 parametro = gamma

    TCutMode CutMode;

    prlreal Lambda;

    // preprocessing setup
    std::vector<int> ProjectionMask;
    TVectorNormalizationTool NormTool;

    // setup report training
    bool RepEnabled;
    std::string RName; // nome file report

    // validation speed-up flag
    bool ValSpeed; // se vero validation speed-up abilitato

    // modalità cancellazione HBs (solo Parc)
    THBDeletionMode HBDelMode;

 public:

    TArcParcTraining();

    TMembershipType GetMembershipType() const {return MembType;};
    void SetMembershipType(TMembershipType Val);

    int GetMembParNumber() const {return MembPars.size();};
    std::string GetMembParName(int i) const;
    prlreal GetMembPar(int i) const {return MembPars[i];};
    void SetMembPar(int i, prlreal Val){MembPars[i] = Val;};

    TCutMode GetCutMode() const {return CutMode;};
    void SetCutMode(TCutMode Val){CutMode = Val;};

    THBDeletionMode GetDeletionMode() const {return HBDelMode;};
    void SetDeletionMode(THBDeletionMode Val){HBDelMode = Val;};

    prlreal GetLambda() const {return Lambda;};
    void SetLambda(prlreal Val){Lambda = Val;};

    // setup report
    bool ReportEnabled(){return RepEnabled;};
    void EnableReport(){RepEnabled = true;};
    void DisableReport(){RepEnabled = false;};
    void ReportSetup(std::string name){RName = name;};

    // validation speed-up
    bool ValidationSpeedUpEnabled(){return ValSpeed;};
    void EnableValidationSpeedUp(){ValSpeed = true;};
    void DisableValidationSpeedUp(){ValSpeed = false;};

    void PreprocessingSetup(std::vector<int>& PM, TVectorNormalizationTool& NT);

    //void AskUserARCParameters();
    //void AskUserPARCParameters();

    void ArcTrain(TVectorCollection* TrainingSet,          // - potrà essere modificato (preproc e/o incongruenti)
                  bool AlreadyPreproc,                     // - è stato già preprocessato, in accordo al preprocessing setup
                  bool ReturnAll,                          // - ritorna tutte le reti generate, e non solo la migliore
                  std::list<TMinMaxNetwork>& ReturnList);  // - lista in cui vengono ritornate le reti

    // le reti ritornate riportano, nel campo "SortData", un vettore di due componenti: la prima è la funzione costo,
    // la seconda la complessità

    void ArcTrainCopyTR(const TVectorCollection* TrainingSet,    // - non sarà modificato (si agirà su una copia)
                        bool AlreadyPreproc,                     // - vedi sopra
                        bool ReturnAll,                          // - vedi sopra
                        std::list<TMinMaxNetwork>& ReturnList);  // - vedi sopra

    void ParcTrain(TVectorCollection* TrainingSet,          // - potrà essere modificato (preproc e/o incongruenti)
                   bool AlreadyPreproc,                     // - è stato già preprocessato, in accordo al preprocessing setup
                   bool ReturnAll,                          // - ritorna tutte le reti generate, e non solo la migliore
                   std::list<TMinMaxNetwork>& ReturnList);  // - lista in cui vengono ritornate le reti

    // le reti ritornate riportano, nel campo "SortData", un vettore di due componenti: la prima è la funzione costo,
    // la seconda la complessità

    void ParcTrainCopyTR(const TVectorCollection* TrainingSet,    // - non sarà modificato (si agirà su una copia)
                         bool AlreadyPreproc,                     // - vedi sopra
                         bool ReturnAll,                          // - vedi sopra
                         std::list<TMinMaxNetwork>& ReturnList);  // - vedi sopra

    // come ArcTrain, ma versione con incongruenti eliminati on the fly
    void ArcTrainLazyInc(TVectorCollection* TrainingSet,
                         bool AlreadyPreproc,
                         bool ReturnAll,
                         std::list<TMinMaxNetwork>& ReturnList);
    
    // come ParcTrain, ma versione con incongruenti eliminati on the fly                
    void ParcTrainLazyInc(TVectorCollection* TrainingSet,          // - potrà essere modificato (preproc e/o incongruenti)
                          bool AlreadyPreproc,                     // - è stato già preprocessato, in accordo al preprocessing setup
                          bool ReturnAll,                          // - ritorna tutte le reti generate, e non solo la migliore
                          std::list<TMinMaxNetwork>& ReturnList);  // - lista in cui vengono ritornate le reti                         
};


//---------------------------------------------------------------------------
#endif
