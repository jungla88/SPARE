QT -= gui

TEMPLATE = lib

DEFINES += SPARE_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/../

#OBJECTS_DIR += arctools

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    BoundedParameter.hpp \
    Clustering/Bsas.hpp \
    Clustering/Ensembler/BsasPartitions.hpp \
    Clustering/Kmeans.hpp \
    Clustering/KmeansInit/Dbcrimes2Init.hpp \
    Clustering/KmeansInit/DbcrimesInit.hpp \
    Clustering/KmeansInit/FirstK.hpp \
    Clustering/KmeansInit/ProbabilisticDiss.hpp \
    Clustering/KmeansInit/RandomK.hpp \
    Clustering/KmeansInit/SamplingSeeding.hpp \
    Clustering/MTBsas.hpp \
    Dissimilarity/CBMF.hpp \
    Dissimilarity/Constant.hpp \
    Dissimilarity/Converter/Complement.hpp \
    Dissimilarity/Delta.hpp \
    Dissimilarity/DissimilarityMatrix.hpp \
    Dissimilarity/Divergence/FKL.hpp \
    Dissimilarity/Divergence/FRenyi.hpp \
    Dissimilarity/Divergence/FSymKL.hpp \
    Dissimilarity/Divergence/FSymRenyi.hpp \
    Dissimilarity/Divergence/Lutwak.hpp \
    Dissimilarity/Divergence/Renyi.hpp \
    Dissimilarity/Divergence/SymLutwak.hpp \
    Dissimilarity/Dtw.hpp \
    Dissimilarity/Euclidean.hpp \
    Dissimilarity/Fuzzy/MaxIntersection.hpp \
    Dissimilarity/Fuzzy/NormDivergence.hpp \
    Dissimilarity/Fuzzy/Subsethood.hpp \
    Dissimilarity/Hamming.hpp \
    Dissimilarity/Levenshtein.hpp \
    Dissimilarity/Minkowski.hpp \
    Dissimilarity/ModuleDistance.hpp \
    Environment/DiscreteCode.hpp \
    Evaluator/Gaussian.hpp \
    Evaluator/MultiGaussian.hpp \
    Evaluator/PiecewiseLinear.hpp \
    Graph/Dissimilarity/4WBMF.hpp \
    Graph/Dissimilarity/BMF.hpp \
    Graph/Dissimilarity/HGED.hpp \
    Graph/Dissimilarity/PGED.hpp \
    Graph/Dissimilarity/TWEC.hpp \
    Graph/Dissimilarity/nBMF.hpp \
    Graph/Dissimilarity/sBMF.hpp \
    Graph/Dissimilarity/~sBMF.hpp \
    Graph/Graph/EColiPGraph.hpp \
    Graph/Graph/EColiPGraphReader.hpp \
    Graph/Graph/EColiPGraphReader_dist.hpp \
    Graph/Graph/EColiPGraph_dist.hpp \
    Graph/Graph/GraphsDataset.hpp \
    Graph/Graph/IAM/AIDSGraph.hpp \
    Graph/Graph/IAM/AIDSGraphReader.hpp \
    Graph/Graph/IAM/AIDSVerticesDissimilarity.hpp \
    Graph/Graph/IAM/COILDELGraph.hpp \
    Graph/Graph/IAM/COILDELGraphReader.hpp \
    Graph/Graph/IAM/EColiPGraph.hpp \
    Graph/Graph/IAM/EColiPGraph2.hpp \
    Graph/Graph/IAM/EColiPGraphReader.hpp \
    Graph/Graph/IAM/EColiPGraphReader2.hpp \
    Graph/Graph/IAM/EColiPGraphReader_OLD.hpp \
    Graph/Graph/IAM/EColiPGraph_OLD.hpp \
    Graph/Graph/IAM/FingerprintEdgesDissimilarity.hpp \
    Graph/Graph/IAM/FingerprintGraph.hpp \
    Graph/Graph/IAM/FingerprintGraphReader.hpp \
    Graph/Graph/IAM/GRECEdgesDissimilarity.hpp \
    Graph/Graph/IAM/GRECGraph.hpp \
    Graph/Graph/IAM/GRECGraphReader.hpp \
    Graph/Graph/IAM/GRECVerticesDissimilarity.hpp \
    Graph/Graph/IAM/LetterGraph.hpp \
    Graph/Graph/IAM/LetterGraphReader.hpp \
    Graph/Graph/IAM/MutagenicityGraph.hpp \
    Graph/Graph/IAM/MutagenicityGraphReader.hpp \
    Graph/Graph/IAM/ProteinEdgesDissimilarity.hpp \
    Graph/Graph/IAM/ProteinGraph.hpp \
    Graph/Graph/IAM/ProteinGraphReader.hpp \
    Graph/Graph/IAM/ProteinVerticesDissimilarity.hpp \
    Graph/Graph/IdGraph.hpp \
    Graph/Graph/IdGraphReader.hpp \
    Graph/Graph/IdVectorGraph.hpp \
    Graph/Graph/IdVectorGraphReader.hpp \
    Graph/Graph/ScalarRGraph.hpp \
    Graph/Graph/ScalarRGraphReader.hpp \
    Graph/Graph/SimpleGraph.hpp \
    Graph/Graph/SimpleGraphReader.hpp \
    Graph/Graph/StringGraph.hpp \
    Graph/Graph/StringGraphReader.hpp \
    Graph/Graph/VectorGraph.hpp \
    Graph/Graph/VectorGraphReader.hpp \
    Graph/MatrixRepresentation/Adjacency.hpp \
    Graph/MatrixRepresentation/Eigenvectors.hpp \
    Graph/MatrixRepresentation/InducedTransition.hpp \
    Graph/MatrixRepresentation/Laplacian.hpp \
    Graph/MatrixRepresentation/NormalizedLaplacian.hpp \
    Graph/MatrixRepresentation/SymTransition.hpp \
    Graph/MatrixRepresentation/Transition.hpp \
    Graph/MatrixRepresentation/WTransition.hpp \
    Graph/Operator/MCTensorProduct.hpp \
    Graph/Operator/SymTensorProduct.hpp \
    Graph/Operator/TensorProduct.hpp \
    Graph/Seriation/EigenSeriation.hpp \
    Graph/Similarity/GraphCoverage.hpp \
    MultiEvaluator/FuzzyAntecedent.hpp \
    MultiEvaluator/FuzzyConsequent.hpp \
    MultiEvaluator/FuzzyRuleSet.hpp \
    MultiEvaluator/HyperplaneConsequent.hpp \
    MultiEvaluator/MultiGaussianAntecedent.hpp \
    Optimizer/Genetic.hpp \
    Optimizer/PGenetic.hpp \
    Representation/DissimilarityRepr.hpp \
    Representation/SymbolicHistograms.hpp \
    Representation/SymbolicHistograms_original.hpp \
    Representative/Centroid.hpp \
    Representative/FuzzyHyperbox.hpp \
    Representative/FuzzyMinSod.hpp \
    Representative/Mahalanobis.hpp \
    Representative/MinSod.hpp \
    Representative/PFuzzyMinSod.hpp \
    Representative/RFFuzzyMinSod.hpp \
    Representative/Rlse.hpp \
    Sequence/AE_RealN.hpp \
    Sequence/AE_String.hpp \
    Sequence/Parsers/DirectParser.hpp \
    Sequence/Parsers/RealScalarParser.hpp \
    Sequence/Parsers/VectorOfRealParser.hpp \
    Sequence/Sequence.hpp \
    Sequence/SequenceIO.hpp \
    Sequence/SequencesDataSet.hpp \
    Sequence/SequencesDataSetIO.hpp \
    Sequence/SubSequence.hpp \
    Similarity/AngularSimilarity.hpp \
    Similarity/AvgKernel.hpp \
    Similarity/Cosine.hpp \
    Similarity/ExponentialKernel.hpp \
    Similarity/GTSKernel.hpp \
    Similarity/HyperbolicTangentKernel.hpp \
    Similarity/LaplacianKernel.hpp \
    Similarity/MinKernel.hpp \
    Similarity/PolynomialKernel.hpp \
    Similarity/ProductKernel.hpp \
    Similarity/RBFKernel.hpp \
    Similarity/RationalQuadraticKernel.hpp \
    SpareExceptions.hpp \
    SpareTypes.hpp \
    Supervised/DlibSvmClass.hpp \
    Supervised/KnnApprox.hpp \
    Supervised/KnnClass.hpp \
    Supervised/MMNet.hpp \
    Supervised/MTKnnClass.hpp \
    Supervised/Scbc.hpp \
    Supervised/SvmApprox.hpp \
    Supervised/SvmClass.hpp \
    Supervised/SvmCommon.hpp \
    SwitchParameter.hpp \
    Unsupervised/Rlrpa.hpp \
    Unsupervised/Ucbc.hpp \
    Utils/SeqParser/DirectParser.hpp \
    Utils/SeqParser/RealScalarParser.hpp \
    Utils/SeqParser/VectorParser.hpp \
    Utils/SeqReader.hpp \
    Utils/arctools/MinMaxNetwork.h \
    Utils/arctools/MinMaxTraining.h \

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
