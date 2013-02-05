#include "AliLog.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TAxis.h"
#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "TMath.h"
#include "TString.h"

#include "AliAnalysisTaskMuonHadronCorrelations.h"
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"
#include "AliEventPoolManager.h"

ClassImp(AliAnalysisTaskMuonHadronCorrelations)

//====================================================================================================================================================

AliAnalysisTaskMuonHadronCorrelations::AliAnalysisTaskMuonHadronCorrelations() : 
  AliAnalysisTaskSE(), 
  fAOD(0x0),
  fPoolMgr(0x0),
  fTrackCB(0x0),
  fTrackMA(0x0),
  fFilterBitCentralBarrel(0),
  fMaxEtaCentralBarrel(1.0),
  fMaxChi2Muon(9999999999.), 
  fMinRAbsMuon(0), 
  fMaxRAbsMuon(9999999999.),
  fTriggerMatchLevelMuon(0),
  fNbinsCent(1), 
  fNbinsPt(1),
  fCentAxis(0x0), 
  fPtAxis(0x0),
  fHistV0Multiplicity(0x0), 
  fHistITSMultiplicity(0x0),
  fHistCentrality(0x0),
  fCentMethod(0),
  fOutputList(0x0)
{

  // Default constructor

  for (Int_t iCent=0; iCent<fNMaxBinsCentrality; iCent++) {
    for (Int_t iPtBinCB=0; iPtBinCB<fNMaxBinsPt; iPtBinCB++) {
      for (Int_t iPtBinMA=0; iPtBinMA<fNMaxBinsPt; iPtBinMA++) {
	fHistDeltaPhi[iCent][iPtBinCB][iPtBinMA]    = NULL;
	fHistDeltaPhiMix[iCent][iPtBinCB][iPtBinMA] = NULL;
      }
    }
    fHistNTracksCB_vs_NTracksMA[iCent]  = NULL;
  }  
  
}


//====================================================================================================================================================

AliAnalysisTaskMuonHadronCorrelations::AliAnalysisTaskMuonHadronCorrelations(const char *name) : 
  AliAnalysisTaskSE(name), 
  fAOD(0x0),
  fPoolMgr(0x0),
  fTrackCB(0x0),
  fTrackMA(0x0),
  fFilterBitCentralBarrel(0),
  fMaxEtaCentralBarrel(1.0),
  fMaxChi2Muon(9999999999.), 
  fMinRAbsMuon(0), 
  fMaxRAbsMuon(9999999999.),
  fTriggerMatchLevelMuon(0),
  fNbinsCent(1), 
  fNbinsPt(1),
  fCentAxis(0x0), 
  fPtAxis(0x0),
  fHistV0Multiplicity(0x0), 
  fHistITSMultiplicity(0x0),
  fHistCentrality(0x0),
  fCentMethod(0),
  fOutputList(0x0)
{

  // Constructor

  for (Int_t iCent=0; iCent<fNMaxBinsCentrality; iCent++) {
    for (Int_t iPtBinCB=0; iPtBinCB<fNMaxBinsPt; iPtBinCB++) {
      for (Int_t iPtBinMA=0; iPtBinMA<fNMaxBinsPt; iPtBinMA++) {
	fHistDeltaPhi[iCent][iPtBinCB][iPtBinMA]    = NULL;
	fHistDeltaPhiMix[iCent][iPtBinCB][iPtBinMA] = NULL;
      }
    }
    fHistNTracksCB_vs_NTracksMA[iCent]  = NULL;
  }  
  
  // Define input and output slots here
  DefineOutput(1, TList::Class());
  
}

//====================================================================================================================================================

AliAnalysisTaskMuonHadronCorrelations::~AliAnalysisTaskMuonHadronCorrelations() {
  
  delete fCentAxis;
  delete fPtAxis;

  if (fOutputList  && !AliAnalysisManager::GetAnalysisManager()->IsProofMode()) 
    delete fOutputList;
}

//====================================================================================================================================================

void AliAnalysisTaskMuonHadronCorrelations::UserCreateOutputObjects() {
  
  fOutputList = new TList();
  fOutputList->SetOwner(kTRUE);

  for (Int_t iCent=0; iCent<fNbinsCent; iCent++) {
    for (Int_t iPtBinCB=0; iPtBinCB<fNbinsPt; iPtBinCB++) {
      for (Int_t iPtBinMA=0; iPtBinMA<fNbinsPt; iPtBinMA++) {
	
	fHistDeltaPhi[iCent][iPtBinCB][iPtBinMA]    = new TH1D(Form("fHistDeltaPhi_Cent%02d_PtBin%02d_%02d",iCent,iPtBinCB,iPtBinMA), 
							       Form("%d-%d %%, %3.1f<p_{T}^{TPC}<%3.1f, %3.1f<p_{T}^{Muon}<%3.1f",
								    Int_t(fCentAxis->GetBinLowEdge(iCent+1)),
								    Int_t(fCentAxis->GetBinUpEdge(iCent+1)),
								    fPtAxis->GetBinLowEdge(iPtBinCB+1),
								    fPtAxis->GetBinUpEdge(iPtBinCB+1),
								    fPtAxis->GetBinLowEdge(iPtBinMA+1),
								    fPtAxis->GetBinUpEdge(iPtBinMA+1)),
							       100, -0.5*TMath::RadToDeg()*TMath::Pi(), 1.5*TMath::RadToDeg()*TMath::Pi());
	
	fHistDeltaPhiMix[iCent][iPtBinCB][iPtBinMA]    = new TH1D(Form("fHistDeltaPhiMix_Cent%02d_PtBin%02d_%02d",iCent,iPtBinCB,iPtBinMA), 
								  Form("%d-%d %%, %3.1f<p_{T}^{TPC}<%3.1f, %3.1f<p_{T}^{Muon}<%3.1f MIXED",
								       Int_t(fCentAxis->GetBinLowEdge(iCent+1)),
								       Int_t(fCentAxis->GetBinUpEdge(iCent+1)),
								       fPtAxis->GetBinLowEdge(iPtBinCB+1),
								       fPtAxis->GetBinUpEdge(iPtBinCB+1),
								       fPtAxis->GetBinLowEdge(iPtBinMA+1),
								       fPtAxis->GetBinUpEdge(iPtBinMA+1)),
								  100, -0.5*TMath::RadToDeg()*TMath::Pi(), 1.5*TMath::RadToDeg()*TMath::Pi());
	
	fHistDeltaPhi[iCent][iPtBinCB][iPtBinMA]    -> SetXTitle("#Delta#varphi  [degrees]");
	fHistDeltaPhiMix[iCent][iPtBinCB][iPtBinMA] -> SetXTitle("#Delta#varphi  [degrees]");

	fHistDeltaPhi[iCent][iPtBinCB][iPtBinMA]    -> Sumw2();
	fHistDeltaPhiMix[iCent][iPtBinCB][iPtBinMA] -> Sumw2();
	
	fOutputList -> Add(fHistDeltaPhi[iCent][iPtBinCB][iPtBinMA]);
	fOutputList -> Add(fHistDeltaPhiMix[iCent][iPtBinCB][iPtBinMA]);

      }
    }

    fHistNTracksCB_vs_NTracksMA[iCent] = new TH2D(Form("fHistNTracksCB_vs_NTracksMA_Cent%02d",iCent),
						  Form("%d-%d %%",Int_t(fCentAxis->GetBinLowEdge(iCent+1)),Int_t(fCentAxis->GetBinUpEdge(iCent+1))),
						  100, 0, 1000, 100, 0, 100);
    fHistNTracksCB_vs_NTracksMA[iCent] -> SetXTitle("N_{tracks} Central Barrel");
    fHistNTracksCB_vs_NTracksMA[iCent] -> SetYTitle("N_{tracks} Muon Arm");

    fHistNTracksCB_vs_NTracksMA[iCent] -> Sumw2();

    fOutputList -> Add(fHistNTracksCB_vs_NTracksMA[iCent]);
    
  }
  
  fHistV0Multiplicity = new TH1D("fHistV0Multiplicity", "V0 Multiplicity", 500, 0, 1000);
  fHistV0Multiplicity -> SetXTitle("Multiplicity");
  fHistV0Multiplicity -> Sumw2();

  fHistITSMultiplicity = new TH1D("fHistITSMultiplicity", "ITS Multiplicity", 500, 0, 500);
  fHistITSMultiplicity -> SetXTitle("N_{Clusters}");
  fHistITSMultiplicity -> Sumw2();

  fHistCentrality = new TH1D("fHistCentrality", Form("%s Centrality",fCentMethod.Data()), 300, -100, 200);
  fHistCentrality -> SetXTitle("Centrality  [%]");
  fHistCentrality -> Sumw2();

  fOutputList -> Add(fHistV0Multiplicity);
  fOutputList -> Add(fHistITSMultiplicity);
  fOutputList -> Add(fHistCentrality);

  const Int_t kNZvtxBins  = 10;
  // bins for further buffers are shifted by 100 cm
  Double_t vertexBins[kNZvtxBins+1] = { -10,   -8,  -6,  -4,  -2,   0,   2,   4,   6,   8,  10 };
  Int_t nZvtxBins  = kNZvtxBins;
  Double_t* zvtxbin = vertexBins;

  fPoolMgr = new AliEventPoolManager(1000, 20000, fNbinsCent, (Double_t*)fCentAxis->GetXbins()->GetArray(), nZvtxBins, zvtxbin);

  PostData(1, fOutputList); 

}

//====================================================================================================================================================

void AliAnalysisTaskMuonHadronCorrelations::UserExec(Option_t *) {

  fAOD = dynamic_cast<AliAODEvent *>(InputEvent());  
  if (!fAOD) return;  
  
  // Trigger selection
  if (!(IsTriggerFired())) return;
  
  fHistV0Multiplicity  -> Fill(GetV0Multiplicity());
  fHistITSMultiplicity -> Fill(GetITSMultiplicity());

  Int_t centBin = GetCentBin();
  if (centBin<0) return;
  Double_t percentile = fAOD->GetCentrality()->GetCentralityPercentile(fCentMethod.Data());
  fHistCentrality->Fill(percentile);

  const AliVVertex* vertex = fAOD->GetPrimaryVertex();
  Double_t zVtx = vertex->GetZ();
  if (TMath::Abs(zVtx) > 10.) return;
  
  TObjArray *tracksMuonArm = GetAcceptedTracksMuonArm(fAOD);
  if (tracksMuonArm->GetEntriesFast() == 0) {
    delete tracksMuonArm;
    return;
  }
  TObjArray *tracksCentralBarrel = GetAcceptedTracksCentralBarrel(fAOD);
  
  fHistNTracksCB_vs_NTracksMA[centBin] -> Fill(tracksCentralBarrel->GetEntries(), tracksMuonArm->GetEntries());

  AliDebug(1, Form("Single Event analysis : nTracksCB = %4d, nTracksMA = %4d", tracksCentralBarrel->GetEntries(), tracksMuonArm->GetEntries()));

  // Same event  
  for (Int_t iTrMA=0; iTrMA<tracksMuonArm->GetEntriesFast(); iTrMA++) {
    fTrackMA = (AliAODTrack*) tracksMuonArm->At(iTrMA);
    for (Int_t iTrCB=0; iTrCB<tracksCentralBarrel->GetEntriesFast(); iTrCB++) {
      fTrackCB = (AliAODTrack*) tracksCentralBarrel -> At(iTrCB);
      FillHistograms(centBin, kSingleEvent);
    }
  }

  // Mixed event
  {
    AliEventPool* pool = fPoolMgr->GetEventPool(percentile, zVtx);
    //pool->PrintInfo();
    if (pool->IsReady() || pool->NTracksInPool() > 2000 || pool->GetCurrentNEvents() >= 5) 
      for (Int_t jMix=0; jMix<pool->GetCurrentNEvents(); jMix++) {
	for (Int_t iTrMA=0; iTrMA<tracksMuonArm->GetEntriesFast(); iTrMA++) {
	  fTrackMA = (AliAODTrack*) tracksMuonArm->At(iTrMA);
	  TObjArray *mixedTracks = pool->GetEvent(jMix);
	  for (Int_t iTrCB=0; iTrCB<mixedTracks->GetEntriesFast(); iTrCB++) {
	    fTrackCB = (AliAODTrack*) mixedTracks -> At(iTrCB);
	    FillHistograms(centBin, kMixedEvent);
	  }
	}
      }
    pool->UpdatePool(tracksCentralBarrel);
  }

  delete tracksMuonArm;

  PostData(1, fOutputList); 

}

//====================================================================================================================================================

void AliAnalysisTaskMuonHadronCorrelations::FillHistograms(Int_t centrality, Int_t option) {

  Int_t ptBinTrackCB = fPtAxis -> FindBin(fTrackCB->Pt());
  Int_t ptBinTrackMA = fPtAxis -> FindBin(fTrackMA->Pt());

  if (ptBinTrackCB<1 || ptBinTrackCB>fNbinsPt || ptBinTrackMA<1 || ptBinTrackMA>fNbinsPt) return;

  Double_t deltaPhi = fTrackCB->Phi() - fTrackMA->Phi();
  if (deltaPhi >  1.5*TMath::Pi()) deltaPhi -= TMath::TwoPi();
  if (deltaPhi < -0.5*TMath::Pi()) deltaPhi += TMath::TwoPi();

  if (option==kSingleEvent)     fHistDeltaPhi[centrality][ptBinTrackCB-1][ptBinTrackMA-1]    -> Fill(TMath::RadToDeg()*deltaPhi);
  else if (option==kMixedEvent) fHistDeltaPhiMix[centrality][ptBinTrackCB-1][ptBinTrackMA-1] -> Fill(TMath::RadToDeg()*deltaPhi);

}

//====================================================================================================================================================

Bool_t AliAnalysisTaskMuonHadronCorrelations::IsTriggerFired() {
  
  Bool_t isSelected = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected() & AliVEvent::kINT7); 

  return isSelected;
}

//====================================================================================================================================================

Float_t AliAnalysisTaskMuonHadronCorrelations::GetV0Multiplicity() {
  
  Float_t multiplicity=0;
  for (Int_t iChannel=0; iChannel<64; iChannel++) multiplicity += fAOD->GetVZEROData()->GetMultiplicity(iChannel);
  return multiplicity;

}

//====================================================================================================================================================

TObjArray* AliAnalysisTaskMuonHadronCorrelations::GetAcceptedTracksCentralBarrel(AliAODEvent *aodEvent) {

  // fills the array of central barrel tracks that pass the cuts

  TObjArray *tracks = new TObjArray;
  tracks->SetOwner(kTRUE);

  Int_t nTracks = aodEvent->GetNTracks();

  AliAODTrack *track = 0;

  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {
    track = aodEvent->GetTrack(iTrack);
    if (track->TestFilterBit(fFilterBitCentralBarrel) && TMath::Abs(track->Eta())<fMaxEtaCentralBarrel) {
      tracks->Add(new AliAODTrack(*track));
    }
  }

  return tracks;

}

//====================================================================================================================================================

TObjArray* AliAnalysisTaskMuonHadronCorrelations::GetAcceptedTracksMuonArm(AliAODEvent *aodEvent) {

  // fills the array of muon tracks that pass the cuts

  TObjArray *tracks = new TObjArray;
  tracks->SetOwner(kFALSE);

  Int_t nTracks = aodEvent->GetNTracks();

  AliAODTrack *track = 0;
  
  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {
    track = aodEvent->GetTrack(iTrack);
    if (track->IsMuonTrack() && track->GetMatchTrigger()>=fTriggerMatchLevelMuon) {
      tracks->Add(new AliAODTrack(*track));
    }
  }

  return tracks;

}

//====================================================================================================================================================

void AliAnalysisTaskMuonHadronCorrelations::SetCentBinning(Int_t nBins, Double_t *limits) {

  if (nBins>fNMaxBinsCentrality) {
    AliInfo(Form("WARNING : only %d centrality bins (out of the %d proposed) will be considered",fNMaxBinsCentrality,nBins));
    nBins = fNMaxBinsCentrality;
  }
  if (nBins<=0) {
    AliInfo("WARNING : at least one centrality bin must be considered");
    nBins = 1;
  }
  
  fNbinsCent = nBins;
  fCentAxis  = new TAxis(fNbinsCent, limits);

}

//====================================================================================================================================================

void AliAnalysisTaskMuonHadronCorrelations::SetPtBinning(Int_t nBins, Double_t *limits) {

  if (nBins>fNMaxBinsPt) {
    AliInfo(Form("WARNING : only %d pt bins (out of the %d proposed) will be considered",fNMaxBinsPt,nBins));
    nBins = fNMaxBinsPt;
  }
  if (nBins<=0) {
    AliInfo("WARNING : at least one pt bin must be considered");
    nBins = 1;
  }
  
  fNbinsPt = nBins;
  fPtAxis  = new TAxis(fNbinsPt, limits);

}

//====================================================================================================================================================

Int_t AliAnalysisTaskMuonHadronCorrelations::GetCentBin() {

  Double_t percentile = fAOD->GetCentrality()->GetCentralityPercentile(fCentMethod.Data());

  Int_t bin = fCentAxis->FindBin(percentile) - 1;
  if (bin >= fNbinsCent) bin = -1;
  return bin;
  
}

//====================================================================================================================================================

Double_t AliAnalysisTaskMuonHadronCorrelations::GetITSMultiplicity() {

  Double_t multiplicity = fAOD->GetHeader()->GetNumberOfITSClusters(1);

  return multiplicity;

}

//====================================================================================================================================================

void AliAnalysisTaskMuonHadronCorrelations::Terminate(Option_t *) {

  fOutputList = dynamic_cast<TList*> (GetOutputData(1));
  if (!fOutputList) {
    printf("ERROR: Output list not available\n");
    return;
  }  

}

//====================================================================================================================================================

