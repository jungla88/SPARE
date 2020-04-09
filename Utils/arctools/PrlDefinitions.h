//---------------------------------------------------------------------------
#ifndef PrlDefinitionsH
#define PrlDefinitionsH

#define ZT 1e-20 // Zero Threshold

// precisione reali

typedef double prlreal;

inline prlreal prl_min(prlreal a, prlreal b){return (a < b) ? a : b;};
inline prlreal prl_max(prlreal a, prlreal b){return (a > b) ? a : b;};

// normalizzazione

typedef enum {nmFeatureWise, nmSectionWise, nmNone} TNormalizationMode;

//const prlreal ZT = 1e-20; // Zero Threshold

typedef struct
{
 prlreal R; // radium
}
TComplexPlaneCircle;

typedef struct
{
 prlreal Min; // start
 prlreal Max; // end
}
TRealAxisInterval;

typedef struct
{
 TRealAxisInterval S; // source
 TRealAxisInterval D; // destination
}
TRealFeatureNormalizationPair;

typedef struct
{
 TComplexPlaneCircle S; // source
 TComplexPlaneCircle D; // destination
}
TComplexFeatureNormalizationPair;

// varie

typedef enum {MIN, MEAN} TPatternClusterDistanceType;
typedef enum {BASIC, COOPERATIVE} TGeneticAlgorithmType;
typedef enum {saLexicographic, saCentroidLexicographic,
              saWatershed, saKMeanSgm, saThreeStep} TSegmentationAlgorithm;

typedef enum {ngHalves, ngRouletteWheel} TNextGenAlgo;

typedef struct
{
 unsigned int N;
 unsigned int MaxGen;
 prlreal InitialThreshold;
 prlreal ThresholdReductionStep;
 prlreal ThresholdRiseFactor;
 prlreal CrossoverRate;
 prlreal MutationRate;
 TNextGenAlgo NextGenAlgo;
} TBasicGeneticSearchParameters;

typedef enum {NODE_BMF, NODE_LEXIC, PHASES} TCouplingSchemeType;

typedef enum {maArc, maParc} TMinMaxTrainingAlgo;

typedef struct
{
 int Index;
 prlreal Distance;
} TDistData;

//---------------------------------------------------------------------------
#endif
