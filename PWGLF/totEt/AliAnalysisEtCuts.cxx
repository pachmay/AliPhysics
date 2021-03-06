//_________________________________________________________________________
//  Utility Class for transverse energy studies
//  Base class for ESD & MC analysis
//  - cuts for reconstruction and MonteCarlo 
// implementation file
//  
//*-- Authors: Oystein Djuvsland (Bergen), David Silvermyr (ORNL)
//_________________________________________________________________________

#include "AliAnalysisEtCuts.h"

#include <iostream>
#include "TNamed.h"

using namespace std;

ClassImp(AliAnalysisEtCuts);


AliAnalysisEtCuts::AliAnalysisEtCuts() : 
  TNamed()
				    //
  ,fCommonEtaCut(0.5)
  ,fCommonClusterEnergyCut(0.15)
  ,fCommonTrackPtCut(0.0)
  ,fCommonSingleCell(1)
  ,fEmcalTrackDistanceCut(15.0)
  ,fEmcalTrackDxCut(5.0)
  ,fEmcalTrackDzCut(5.0)
  ,fPhosTrackDistanceCut(10.0)  
  ,fPhosTrackDxCut(8.0)
  ,fPhosTrackDzCut(3.0)
  ,fPhosTrackRCut(5.0)
  ,fPhosBadDistanceCut(3.0)
  
  ,fGeometryPhosEtaAccCut(0.1)//gives wiggle room
  ,fGeometryPhosPhiAccMinCut(-100)//260.0)
  ,fGeometryPhosPhiAccMaxCut(-40)//320.0)
  ,fGeometryPhosDetectorRadius(460.0)
				    //
  ,fGeometryEmcalEtaAccCut(0.6)
  ,fGeometryEmcalPhiAccMinCut(80.0) // rad 1.4
  ,fGeometryEmcalPhiAccMaxCut(120.0) // rad 2.1
				    //,fGeometryEmcalPhiAccMaxCut(180.0) // rad 3.14
  ,fGeometryEmcalDetectorRadius(440.0)
				    //
  ,fReconstructedVertexXCut(0.5)
  ,fReconstructedVertexYCut(0.5)
  ,fReconstructedVertexZCut(12.0)
  ,fReconstructedIPxyCut(1.5)
  ,fReconstructedIPzCut(1.5)
  ,fReconstructedNTpcClustersCut(30)
  ,fReconstructedNItsClustersCut(3)
  ,fReconstructedPidCut(0.0)
				    //
  ,fReconstructedPhosClusterType(-1)
  ,fReconstructedPhosClusterEnergyCut(0.25)// GeV
  ,fReconstructedPhosSingleCellEnergyCut(0.5)
  ,fReconstructedPhosTrackDistanceTightCut(3.0)
  ,fReconstructedPhosTrackDistanceMediumCut(5.0)
  ,fReconstructedPhosTrackDistanceLooseCut(15.0)
				    //
  ,fReconstructedEmcalClusterType(1)
  ,fReconstructedEmcalClusterEnergyCut(0.30) // GeV
  ,fReconstructedEmcalSingleCellEnergyCut(0.5)
  ,fReconstructedEmcalTrackDistanceTightCut(5.0)
  ,fReconstructedEmcalTrackDistanceMediumCut(10.0)
  ,fReconstructedEmcalTrackDistanceLooseCut(15.0)
  
  ,fMonteCarloSingleChargedParticle(3)
  ,fMonteCarloNeutralParticle(0)

  ,fHistMakeTree(kFALSE)
  ,fHistMakeTreeDeposit(kFALSE)
  ,fHistNbinsMult(2000)
  ,fHistMinMult(-0.5)
  ,fHistMaxMult(1999.5)
  ,fHistNbinsTotEt(10000)
  ,fHistMinTotEt(0.000)
  ,fHistMaxTotEt(1000)
  ,fHistNbinsParticleEt(5000)
  ,fHistMinParticleEt(0)
  ,fHistMaxParticleEt(500)
  ,fHistNbinsParticlePt(200) 
  ,fHistMinParticlePt(0)
  ,fHistMaxParticlePt(20)
  
  ,fPrimaryVertexCutXY(4.0)
  ,fPrimaryVertexCutZ(20.0)
{ // ctor
}

AliAnalysisEtCuts::~AliAnalysisEtCuts()
{ // dtor
}

void AliAnalysisEtCuts::SetPbPbDefaults()
{ // just increase seom histogram max values for now
  // enough to multiply conservative p+p defaults by a factor 100?
  fHistMaxMult = 20000;
  fHistMaxTotEt = 10000;
}


