// -*- C++ -*-
//
// Class:      EcalTPGAnalyzer
//
//
// Original Author:  Pascal Paganini
//
//


// system include files
#include <memory>
#include <utility>

#include "DataFormats/Math/interface/deltaR.h"
// user include files
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EcalTriggerPrimitiveDigi.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgoRcd.h"

#include "DataFormats/DetId/interface/DetId.h"

#include "Geometry/CaloTopology/interface/EcalTrigTowerConstituentsMap.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMask.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskAlgoTrigRcd.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskTechTrigRcd.h"

#include "CondFormats/DataRecord/interface/EcalTPGTowerStatusRcd.h"
#include "CondFormats/EcalObjects/interface/EcalTPGTowerStatus.h"


#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"
//L1 Trigger
//#include "L1Trigger/L1ExtraFromDigis/interface/L1ExtraParticleMapProd.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"

//RCT
#include "CondFormats/L1TObjects/interface/L1RCTChannelMask.h"
#include "CondFormats/DataRecord/interface/L1RCTChannelMaskRcd.h"

#include "EcalTPGAnalyzer.h"


//#include "DQM/EcalCommon/interface/Numbers.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/CaloTowers/interface/CaloTowerDetId.h"

// Trip Masking info
#include "CondFormats/EcalObjects/interface/EcalTPGStripStatus.h"
#include "CondFormats/DataRecord/interface/EcalTPGStripStatusRcd.h"

#include "FWCore/Framework/interface/Run.h"

#include <TMath.h>
#include <sstream>

#define DEBUG_ 0

using namespace edm;
using namespace std;
class CaloSubdetectorGeometry;

EcalTPGAnalyzer::EcalTPGAnalyzer(const edm::ParameterSet&  iConfig)
{
  
#ifdef DEBUG
  cout<<"in constructor"<<endl;
#endif
  
  noL1Info = false;
  
  tpCollection_ = iConfig.getParameter<edm::InputTag>("TPCollection") ;
  tpEmulatorCollection_ = iConfig.getParameter<edm::InputTag>("TPEmulatorCollection") ;
  digiCollectionEB_ = iConfig.getParameter<edm::InputTag>("DigiCollectionEB") ;
  digiCollectionEE_ = iConfig.getParameter<edm::InputTag>("DigiCollectionEE") ;
  gtRecordCollectionTag_ = iConfig.getParameter<std::string>("GTRecordCollection") ;
  EcalRecHitCollectionEB_ = iConfig.getParameter<edm::InputTag>("EcalRecHitCollectionEB") ;
  EcalRecHitCollectionEE_ = iConfig.getParameter<edm::InputTag>("EcalRecHitCollectionEE") ;
  // l1extraIsol_ =iConfig.getParameter<edm::InputTag> ("l1extraIsol");
  // l1extraNonIsol_ =iConfig.getParameter<edm::InputTag> ("l1extraNonIsol");
  egTag_ = edm::InputTag("caloStage2Digis", "EGamma");
  m_egToken_             = consumes<l1t::EGammaBxCollection>(egTag_);
  mpegTag_ = edm::InputTag("caloStage2Digis", "MP" );
  mp_egToken_             = consumes<l1t::EGammaBxCollection>(mpegTag_);
  rctSourceEm_token_=  consumes<L1CaloEmCollection>(edm::InputTag("gctDigis"));
  tpCollection1_ = consumes<EcalTrigPrimDigiCollection>( iConfig.getParameter<edm::InputTag>("TPCollection") );
  tpEmulatorCollection1_ = consumes<EcalTrigPrimDigiCollection>( iConfig.getParameter<edm::InputTag>("TPEmulatorCollection") );
  digiCollectionEB1_ = consumes<EBDigiCollection>( iConfig.getParameter<edm::InputTag>("DigiCollectionEB") );
  digiCollectionEE1_ = consumes<EEDigiCollection>( iConfig.getParameter<edm::InputTag>("DigiCollectionEE") );
  gtRecordCollectionTag1_ = consumes<L1GlobalTriggerReadoutRecord>( iConfig.getParameter<std::string>("GTRecordCollection") );
  EcalRecHitCollectionEB1_ = consumes<EcalRecHitCollection>( iConfig.getParameter<edm::InputTag>("EcalRecHitCollectionEB") );
  EcalRecHitCollectionEE1_ = consumes<EcalRecHitCollection>( iConfig.getParameter<edm::InputTag>("EcalRecHitCollectionEE") );
  // l1extraIsolT_  = consumes<l1extra::L1EmParticleCollection>( iConfig.getParameter<edm::InputTag>("l1extraIsol") );
  // l1extraNonIsolT_  = consumes<l1extra::L1EmParticleCollection>( iConfig.getParameter<edm::InputTag>("l1extraNonIsol") );
  vtxLabel_  = consumes<vector<reco::Vertex>>(iConfig.getParameter<InputTag>("VtxLabel"));


  
  allowTP_ = iConfig.getParameter<bool>("ReadTriggerPrimitives");
  useEE_ = iConfig.getParameter<bool>("UseEndCap");
  skipWritingEndcapDigis_ = iConfig.getParameter<bool>("skipWritingEndcapDigi");
  keepOnlyTowersAboveZero_ = iConfig.getParameter<bool>("keepOnlyTowersAboveZero") ;
  print_ = iConfig.getParameter<bool>("Print");
  L1print_ = iConfig.getParameter<bool>("L1Print");
  
    // file
  file_ = new TFile("ECALTPGtree.root","RECREATE");
  file_->cd() ;
  
#ifdef DEBUG
  cout<<"past the cd() call to file_"<<endl;
#endif
  
    // trees
  tree_ = new TTree( "EcalTPGAnalysis","EcalTPGAnalysis" );
  tree_->Branch("runNb",&treeVariables_.runNb,"runNb/i"); //
  tree_->Branch("evtNb",&treeVariables_.evtNb,"evtNb/l"); //
  tree_->Branch("bxNb",&treeVariables_.bxNb,"bxNb/i"); //
  tree_->Branch("bxGT",&treeVariables_.bxGT,"bxGT/i"); //
  tree_->Branch("orbitNb",&treeVariables_.orbitNb,"orbitNb/l"); //
  tree_->Branch("lumiBlock",&treeVariables_.lumiBlock,"lumiBlock/i"); //
  tree_->Branch("timeStamp",&treeVariables_.timeStamp,"timeStamp/d"); //
  tree_->Branch("nVtx",&treeVariables_.nVtx,"nVtx/i");//nvtx
  tree_->Branch("nbOfActiveTriggers",&treeVariables_.nbOfActiveTriggers,"nbOfActiveTriggers/i"); //
  tree_->Branch("activeTriggers",treeVariables_.activeTriggers,"activeTriggers[nbOfActiveTriggers]/I"); //
  tree_->Branch("nbOfActiveTechTriggers",&treeVariables_.nbOfActiveTechTriggers,"nbOfActiveTechTriggers/i"); //
  tree_->Branch("activeTechTriggers",treeVariables_.activeTechTriggers,"activeTechTriggers[nbOfActiveTechTriggers]/I");
  tree_->Branch("activeTriggersBX",treeVariables_.activeTriggersBX,"activeTriggersBX[160]/I"); //  
  
  tree_->Branch("nbOfTowers",&treeVariables_.nbOfTowers,"nbOfTowers/i"); //
  
  tree_->Branch("ieta", treeVariables_.ieta,"ieta[nbOfTowers]/I");//
  tree_->Branch("iphi", treeVariables_.iphi,"iphi[nbOfTowers]/I");//
  tree_->Branch("nbOfXtals", treeVariables_.nbOfXtals,"nbOfXtals[nbOfTowers]/I");//
  tree_->Branch("rawTPData", treeVariables_.rawTPData,"rawTPData[nbOfTowers]/I");//
  tree_->Branch("rawTPEmul1", treeVariables_.rawTPEmul1,"rawTPEmul1[nbOfTowers]/I");//
  tree_->Branch("rawTPEmul2", treeVariables_.rawTPEmul2,"rawTPEmul2[nbOfTowers]/I");//
  tree_->Branch("rawTPEmul3", treeVariables_.rawTPEmul3,"rawTPEmul3[nbOfTowers]/I");//
  tree_->Branch("rawTPEmul4", treeVariables_.rawTPEmul4,"rawTPEmul4[nbOfTowers]/I");//
  tree_->Branch("rawTPEmul5", treeVariables_.rawTPEmul5,"rawTPEmul5[nbOfTowers]/I");//
  tree_->Branch("crystNb", treeVariables_.crystNb,"crystNb[nbOfTowers]/I");//
  tree_->Branch("eRec", treeVariables_.eRec,"eRec[nbOfTowers]/F");//
  tree_->Branch("ttFlag", treeVariables_.ttFlag,"ttFlag[nbOfTowers]/I");//
  tree_->Branch("sevlv", treeVariables_.sevlv,"sevlv[nbOfTowers]/I");//
  tree_->Branch("spike", treeVariables_.spike,"spike[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulsFGVB1", treeVariables_.rawTPEmulsFGVB1,"rawTPEmulsFGVB1[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulsFGVB2", treeVariables_.rawTPEmulsFGVB2,"rawTPEmulsFGVB2[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulsFGVB3", treeVariables_.rawTPEmulsFGVB3,"rawTPEmulsFGVB3[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulsFGVB4", treeVariables_.rawTPEmulsFGVB4,"rawTPEmulsFGVB4[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulsFGVB5", treeVariables_.rawTPEmulsFGVB5,"rawTPEmulsFGVB5[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulttFlag1", treeVariables_.rawTPEmulttFlag1,"rawTPEmulttFlag1[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulttFlag2", treeVariables_.rawTPEmulttFlag2,"rawTPEmulttFlag2[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulttFlag3", treeVariables_.rawTPEmulttFlag3,"rawTPEmulttFlag3[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulttFlag4", treeVariables_.rawTPEmulttFlag4,"rawTPEmulttFlag4[nbOfTowers]/I");//
  tree_->Branch("rawTPEmulttFlag5", treeVariables_.rawTPEmulttFlag5,"rawTPEmulttFlag5[nbOfTowers]/I");//

  // tree_->Branch("TCCid", &treeVariables_.TCCid,"TCCid/i");//
  // tree_->Branch("TowerInTCC", &treeVariables_.TowerInTCC,"TowerInTCC/i");//
  // tree_->Branch("strip", &treeVariables_.strip,"strip/i");//
  
  treeShape_ = new TTree( "EcalShape","EcalShape" );
  treeShape_->Branch("evtNb",&treeVariablesShape_.evtNb,"evtNb/l"); //
  treeShape_->Branch("ieta", &treeVariablesShape_.ieta,"ieta/i");//
  treeShape_->Branch("iphi", &treeVariablesShape_.iphi,"iphi/i");//
  treeShape_->Branch("ixXtal", &treeVariablesShape_.ixXtal,"ixXtal/i");//
  treeShape_->Branch("iyXtal", &treeVariablesShape_.iyXtal,"iyXtal/i");//
  treeShape_->Branch("TCCid", &treeVariablesShape_.TCCid,"TCCid/i");//
  treeShape_->Branch("TowerInTCC", &treeVariablesShape_.TowerInTCC,"TowerInTCC/i");//
  treeShape_->Branch("strip", &treeVariablesShape_.strip,"strip/i");//
  treeShape_->Branch("nbOfSamples", &treeVariablesShape_.nbOfSamples,"nbOfSamples/i");//
  treeShape_->Branch("samp", treeVariablesShape_.samp,"samp[nbOfSamples]/I");//
  
  treeAux = new TTree("treeAux","treeAux");
  treeAux->Branch("runNb",&ecalAux_.runNb,"runNb/i"); //
  treeAux->Branch("nMaskedCh",&ecalAux_.nMaskedCh,"nMaskedCh/i"); //
  treeAux->Branch("iMaskedTTeta",ecalAux_.iMaskedTTeta,"iMaskedTTeta[nMaskedCh]/I"); //
  treeAux->Branch("iMaskedTTphi",ecalAux_.iMaskedTTphi,"iMaskedTTeta[nMaskedCh]/I"); //

  treeAux->Branch("nMaskedChannelsFromStrips",&ecalAux_.nMaskedChannelsFromStrips,"nMaskedChannelsFromStrips/i"); //
  treeAux->Branch("iMaskedChannelsFromStripsX",ecalAux_.iMaskedChannelsFromStripsX,"iMaskedChannelsFromStripsX[nMaskedChannelsFromStrips]/I"); //
  treeAux->Branch("iMaskedChannelsFromStripsY",ecalAux_.iMaskedChannelsFromStripsY,"iMaskedChannelsFromStripsY[nMaskedChannelsFromStrips]/I"); //
  treeAux->Branch("iMaskedChannelsFromStripsZ",ecalAux_.iMaskedChannelsFromStripsZ,"iMaskedChannelsFromStripsZ[nMaskedChannelsFromStrips]/I"); //
  
    //L1 tree branches
  tree_->Branch("nbOfL1IsoCands",&treeVariables_.nbOfL1IsoCands,"nbOfL1IsoCands/i"); //
  tree_->Branch("L1IsoIeta", treeVariables_.L1IsoIeta,"L1IsoIeta[nbOfL1IsoCands]/I");//
  tree_->Branch("L1IsoIphi", treeVariables_.L1IsoIphi,"L1IsoIphi[nbOfL1IsoCands]/I");//
  tree_->Branch("L1IsoRank", treeVariables_.L1IsoRank,"L1IsoRank[nbOfL1IsoCands]/I");//
  
  tree_->Branch("nbOfL1NonisoCands",&treeVariables_.nbOfL1NonisoCands,"nbOfL1NonisoCands/i"); //
  tree_->Branch("L1NonisoIeta", treeVariables_.L1NonisoIeta,"L1NonisoIeta[nbOfL1NonisoCands]/I");//
  tree_->Branch("L1NonisoIphi", treeVariables_.L1NonisoIphi,"L1NonisoIphi[nbOfL1NonisoCands]/I");//
  tree_->Branch("L1NonisoRank", treeVariables_.L1NonisoRank,"L1NonisoRank[nbOfL1NonisoCands]/I");//

  tree_->Branch("nMaskedRCT",&treeVariables_.nMaskedRCT,"nMaskedRCT/i"); //
  tree_->Branch("iMaskedRCTeta",treeVariables_.iMaskedRCTeta,"iMaskedRCTeta[nMaskedRCT]/I"); //
  tree_->Branch("iMaskedRCTcrate",treeVariables_.iMaskedRCTcrate,"iMaskedRCTcrate[nMaskedRCT]/I"); //
  tree_->Branch("iMaskedRCTphi",treeVariables_.iMaskedRCTphi,"iMaskedRCTphi[nMaskedRCT]/I"); //
  
  // L1 pre-firing tree branches
  
  tree_->Branch("nbOfL1preIsoCands",&treeVariables_.nbOfL1preIsoCands,"nbOfL1preIsoCands/i"); //
  tree_->Branch("L1preIsoIeta", treeVariables_.L1preIsoIeta,"L1preIsoIeta[nbOfL1preIsoCands]/I");//
  tree_->Branch("L1preIsoIphi", treeVariables_.L1preIsoIphi,"L1preIsoIphi[nbOfL1preIsoCands]/I");//
  tree_->Branch("L1preIsoRank", treeVariables_.L1preIsoRank,"L1preIsoRank[nbOfL1preIsoCands]/I");//
  
  tree_->Branch("nbOfL1preNonisoCands",&treeVariables_.nbOfL1preNonisoCands,"nbOfL1preNonisoCands/i"); //
  tree_->Branch("L1preNonisoIeta", treeVariables_.L1preNonisoIeta,"L1preNonisoIeta[nbOfL1preNonisoCands]/I");//
  tree_->Branch("L1preNonisoIphi", treeVariables_.L1preNonisoIphi,"L1preNonisoIphi[nbOfL1preNonisoCands]/I");//
  tree_->Branch("L1preNonisoRank", treeVariables_.L1preNonisoRank,"L1preNonisoRank[nbOfL1preNonisoCands]/I");//
  

  // ontime firing tree branches
  /*
    tree_->Branch("nbOfL1ontimeIsoCands",&treeVariables_.nbOfL1ontimeIsoCands,"nbOfL1ontimeIsoCands/i"); //
    tree_->Branch("L1ontimeIsoIeta", treeVariables_.L1ontimeIsoIeta,"L1ontimeIsoIeta[nbOfL1ontimeIsoCands]/I");//
    tree_->Branch("L1ontimeIsoIphi", treeVariables_.L1ontimeIsoIphi,"L1ontimeIsoIphi[nbOfL1ontimeIsoCands]/I");//
    tree_->Branch("L1ontimeIsoRank", treeVariables_.L1ontimeIsoRank,"L1ontimeIsoRank[nbOfL1ontimeIsoCands]/I");//
    
    tree_->Branch("nbOfL1ontimeNonisoCands",&treeVariables_.nbOfL1ontimeNonisoCands,"nbOfL1ontimeNonisoCands/i"); //
    tree_->Branch("L1ontimeNonisoIeta", treeVariables_.L1ontimeNonisoIeta,"L1ontimeNonisoIeta[nbOfL1ontimeNonisoCands]/I");//
    tree_->Branch("L1ontimeNonisoIphi", treeVariables_.L1ontimeNonisoIphi,"L1ontimeNonisoIphi[nbOfL1ontimeNonisoCands]/I");//
    tree_->Branch("L1ontimeNonisoRank", treeVariables_.L1ontimeNonisoRank,"L1ontimeNonisoRank[nbOfL1ontimeNonisoCands]/I");//
  */

  // L1 post-firing tree branches
  tree_->Branch("nbOfL1postIsoCands",&treeVariables_.nbOfL1postIsoCands,"nbOfL1postIsoCands/i"); //
  tree_->Branch("L1postIsoIeta", treeVariables_.L1postIsoIeta,"L1postIsoIeta[nbOfL1postIsoCands]/I");//
  tree_->Branch("L1postIsoIphi", treeVariables_.L1postIsoIphi,"L1postIsoIphi[nbOfL1postIsoCands]/I");//
  tree_->Branch("L1postIsoRank", treeVariables_.L1postIsoRank,"L1postIsoRank[nbOfL1postIsoCands]/I");//
  
  tree_->Branch("nbOfL1postNonisoCands",&treeVariables_.nbOfL1postNonisoCands,"nbOfL1postNonisoCands/i"); //
  tree_->Branch("L1postNonisoIeta", treeVariables_.L1postNonisoIeta,"L1postNonisoIeta[nbOfL1postNonisoCands]/I");//
  tree_->Branch("L1postNonisoIphi", treeVariables_.L1postNonisoIphi,"L1postNonisoIphi[nbOfL1postNonisoCands]/I");//
  tree_->Branch("L1postNonisoRank", treeVariables_.L1postNonisoRank,"L1postNonisoRank[nbOfL1postNonisoCands]/I");//
  //
  
  
    //   ADD the branches from here http://azabi.web.cern.ch/azabi/TRIGGER/CODE/SimpleNtpleCustom.cc
  tree_->Branch("trig_tower_adc",  &treeVariables_.twrADC,  "twrADC[nbOfTowers]/I");
  tree_->Branch("trig_tower_sFGVB",  &treeVariables_.sFGVB,  "sFGVB[nbOfTowers]/I");

#ifdef DEBUG
  cout<<"declared tree branches"<<endl;
#endif
  
}


EcalTPGAnalyzer::~EcalTPGAnalyzer()
{
  file_->cd();
  tree_->Write();
  treeShape_->Write();
  treeAux->Write();
  file_->Close();
}

// void EcalTPGAnalyzer::beginJob()
// {
//     myevt = 0;
//     std::cout << " beginJob myevt = " << myevt << std::endl;



// }

void EcalTPGAnalyzer::beginRun( const edm::EventSetup & iSetup,const edm::Run & iRun)
{
  myevt = 0;
  startTime_ =(iRun.beginTime().value() >> 32) + 1e-6*(iRun.beginTime().value() & 0xffffffff);
  std::cout << __LINE__ <<  " beginRun myevt = " << myevt << ", startTime " << startTime_ << std::endl;
  
}


void EcalTPGAnalyzer::analyze(const edm::Event& iEvent, const  edm::EventSetup & iSetup)
{
  
  
  using namespace edm;
  using namespace std;
  //const double pi = 3.14159265358979323846;
  myevt++;
#ifdef DEBUG    
  cout << " in analyze method \t analyze myevt = " << myevt << endl;
#endif
  if (DEBUG_) cout << __LINE__ << endl; 
    
  ESHandle< EcalElectronicsMapping > ecalmapping;
  iSetup.get< EcalMappingRcd >().get(ecalmapping);
  theMapping_ = ecalmapping.product();
  if ( myevt == 1){  
    // electronics mapping
    // ESHandle< EcalElectronicsMapping > ecalmapping;
    // iSetup.get< EcalMappingRcd >().get(ecalmapping);
    // theMapping_ = ecalmapping.product();
    
    // Reading strip statuses
    try{
      edm::ESHandle<EcalTPGStripStatus> theEcalTPGStripStatus_handle;
      iSetup.get<EcalTPGStripStatusRcd>().get(theEcalTPGStripStatus_handle);
      const EcalTPGStripStatus * ecaltpgStripStatus = theEcalTPGStripStatus_handle.product();
      const EcalTPGStripStatusMap &stripMap = ecaltpgStripStatus->getMap();
      EcalTPGStripStatusMapIterator itSt;
      ecalAux_.nMaskedChannelsFromStrips = 0;
      for (itSt = stripMap.begin();itSt != stripMap.end();++itSt) {
	if(itSt->second > 0) {
	  // lets decode the ID
	  int strip = itSt->first/8;
	  int pseudostrip = strip & 0x7;
	  strip /= 8;
	  int tt = strip & 0x7F;
	  strip /= 128;
	  int tccid = strip & 0x7F;
	  std::cout << "Bad strip ID = " << itSt->first
		    << " TCC " << tccid << " TT " << tt << " ST " << pseudostrip
		    << ", status = " << itSt->second << std::endl;
	  std::vector<DetId> xtalId =
	    theMapping_->pseudoStripConstituents(tccid, tt, pseudostrip);
	  std::vector<DetId>::iterator it;
	  for(it = xtalId.begin(); it != xtalId.end(); it++) {
	    EEDetId eeid = *it;
	    std::cout << eeid << " ";
	    // int iz = eeid.zside();
	    // int ix = eeid.ix();
	    // int iy = eeid.iy();
	    ecalAux_.iMaskedChannelsFromStripsX[ecalAux_.nMaskedChannelsFromStrips]=eeid.ix();
	    ecalAux_.iMaskedChannelsFromStripsY[ecalAux_.nMaskedChannelsFromStrips]=eeid.iy();
	    ecalAux_.iMaskedChannelsFromStripsZ[ecalAux_.nMaskedChannelsFromStrips]=eeid.zside();
	    ecalAux_.nMaskedChannelsFromStrips++;
	  }
	  std::cout << std::endl;
	}
      }
    }
    catch (cms::Exception& exception){
      std::cout <<  "   ===> EcalTPGStripStatusRcd not found => skip strip masking info" << std::endl;
    }
    
    // Reading TPG tower statuses for first event (masked/not masked information)        
    edm::ESHandle<EcalTPGTowerStatus> theEcalTPGTowerStatus_handle;
    iSetup.get<EcalTPGTowerStatusRcd>().get(theEcalTPGTowerStatus_handle);
    const EcalTPGTowerStatus * ecaltpgTowerStatus=theEcalTPGTowerStatus_handle.product();
    
    const EcalTPGTowerStatusMap &towerMap=ecaltpgTowerStatus->getMap();
    EcalTPGTowerStatusMapIterator  it;
    
    uint nMaskedChannels = 0;
    for (it=towerMap.begin();it!=towerMap.end();++it) {
      if ((*it).second > 0) // if status not equals 0 then channle was massked 
	{
	  EcalTrigTowerDetId  ttId((*it).first);
	  ecalAux_.iMaskedTTeta[nMaskedChannels] = ttId.ieta();
	  ecalAux_.iMaskedTTphi[nMaskedChannels] = ttId.iphi();
	  nMaskedChannels++;
	}
    }
    
    ecalAux_.nMaskedCh = nMaskedChannels;
  if (DEBUG_) cout << __LINE__ << endl; 
    
    // Storing eta and phi of masked TT to 'treeAux' root tree
    treeAux->Fill() ;
    // geometry
    ESHandle<CaloGeometry> theGeometry;
    ESHandle<CaloSubdetectorGeometry> theEndcapGeometry_handle, theBarrelGeometry_handle;
    
    iSetup.get<CaloGeometryRecord>().get( theGeometry );
    iSetup.get<EcalEndcapGeometryRecord>().get("EcalEndcap",theEndcapGeometry_handle);
    iSetup.get<EcalBarrelGeometryRecord>().get("EcalBarrel",theBarrelGeometry_handle);
    
    iSetup.get<IdealGeometryRecord>().get(eTTmap_);
    theEndcapGeometry_ = &(*theEndcapGeometry_handle);
    theBarrelGeometry_ = &(*theBarrelGeometry_handle);
    
  
    //Adding RCT mask
    // list of RCT channels to mask
    vector<int> RCTMaskedEta;
    vector<int> RCTMaskedPhi;
    edm::ESHandle<L1RCTChannelMask> channelMask;
    iSetup.get<L1RCTChannelMaskRcd>().get(channelMask);
    const L1RCTChannelMask* cEs = channelMask.product();
    uint nMaskedRCT = 0;
    for(int i = 0; i< 18; i++)
      for(int j =0; j< 2; j++)
	for(int k =0; k<28; k++)
	  if(cEs->ecalMask[i][j][k]){
	    //cout << "ECAL masked channel: RCT crate " << i << " iphi " << j <<" ieta " <<k <<endl; 
	    treeVariables_.iMaskedRCTeta[nMaskedRCT]=k;
	    treeVariables_.iMaskedRCTphi[nMaskedRCT]=j;
	    treeVariables_.iMaskedRCTcrate[nMaskedRCT]=i;
	    nMaskedRCT++;
	  }
    treeVariables_.nMaskedRCT = nMaskedRCT;
    
  }

  map<EcalTrigTowerDetId, towerEner> mapTower ;
  map<EcalTrigTowerDetId, towerEner>::iterator itTT ;
  map<EcalTrigTowerDetId, EBDataFrame> mapShapeEB ;
  map<EcalTrigTowerDetId, EBDataFrame>::iterator itEB ;
  map<EcalTrigTowerDetId, EEDataFrame> mapShapeEE ;
  map<EcalTrigTowerDetId, EEDataFrame> ::iterator itEE ; 
  
  if (DEBUG_) cout << __LINE__ << endl; 

  ///////////////////////////
  // get Evts info
  ///////////////////////////
  
  treeVariables_.runNb = iEvent.id().run() ;
  treeVariables_.evtNb = iEvent.id().event() ;
  treeVariables_.bxNb = iEvent.bunchCrossing() ;
  treeVariables_.orbitNb = iEvent.orbitNumber() ;
  treeVariables_.lumiBlock = iEvent.luminosityBlock();
  
  treeVariablesShape_.evtNb = iEvent.id().event() ;



  
#ifdef DEBUG
  cout<<"obtained basic event vars like run and event number"<<endl;
#endif

  // Add vtx info//
  edm::Handle<vector<reco::Vertex>> vtxHandle;
  iEvent.getByToken(vtxLabel_, vtxHandle);
  
  treeVariables_.nVtx = -1;
  if (vtxHandle.isValid()) {
    treeVariables_.nVtx= 0;
    for (vector<reco::Vertex>::const_iterator v = vtxHandle->begin(); v != vtxHandle->end(); ++v) {
      // if (v->ndof() > 4. && fabs(v->z()) <= 24. && fabs(v->position().rho()) <= 2.) can be used for good vtx
      treeVariables_.nVtx++;
    }
  }

  
  
  //===================TIMESTAMP INFORMTION=================================
  // Code added to get the time in seconds
  //double startTime_= 0.0;
  //startTime_ +=  1215107133.;//(iRun.beginTime()).unixTime();// 2016/1/1 was 2016/7/3 1215107133.;
  //if (myevt==1) 
  //  startTime_ += 33470900.; //start time of run (empirically)
  startTime_ = 1461466500; // 
  unsigned int  timeStampLow = ( 0xFFFFFFFF & iEvent.time().value() );
  unsigned int  timeStampHigh = ( iEvent.time().value() >> 32 );
  
  double rawEventTime = ( double)(timeStampHigh)+((double )(timeStampLow)/1000000.);
  double eventTime = rawEventTime - startTime_; //Notice the subtraction of the "starttime"
  cout << __LINE__ << " " << eventTime << " "<< rawEventTime << " " << startTime_<< endl; 
  treeVariables_.timeStamp = eventTime;
  //cout << __LINE__ << " " << iEvent.time().value() << " " << timeStampLow << " " << timeStampHigh << " " << rawEventTime << " " << eventTime << endl; 
  if (DEBUG_) cout << __LINE__ << endl; 

  // print_ = true;
  if (print_)
    std::cout<<"==========="<<iEvent.id()<<"   bxNb:"<<iEvent.bunchCrossing()<<" orbit:"<<iEvent.orbitNumber()<<std::endl ;

  
  //cout << __LINE__ << endl;  
  
  ///////////////////////////
  // get L1 Trigger info
  ///////////////////////////
  edm::Handle< L1GlobalTriggerReadoutRecord > gtRecord;
  iEvent.getByLabel( edm::InputTag(gtRecordCollectionTag_), gtRecord);
  DecisionWord dWord = gtRecord->decisionWord();   // this will get the decision word *before* masking disabled bits
  edm::ESHandle< L1GtTriggerMask > l1GtTmAlgo;
  iSetup.get< L1GtTriggerMaskAlgoTrigRcd >().get( l1GtTmAlgo );        
  std::vector<unsigned int> triggerMaskAlgoTrig = l1GtTmAlgo.product()->gtTriggerMask();
  
  L1GtfeWord gtfeWord = gtRecord->gtfeWord();
  treeVariables_.bxGT = gtfeWord.bxNr();
  //cout << __LINE__ << endl;
      
#ifdef DEBUG
  cout<<"obtained handles"<<endl;
#endif
  
  
  int hashedTr[160];
  for ( int i = 0; i < 160; ++i ){
    hashedTr[i]=0;
    treeVariables_.activeTriggersBX[i]=0; 
    treeVariables_.activeTriggers[i]= 0;
  }
  for(int iebx=0; iebx<=4; iebx++) {
    DecisionWord gtDecisionWord = gtRecord->decisionWord(iebx-2);
    //	std::cout<<"iebx -2 "<<iebx-2<<" size "<<gtDecisionWord.size()<<std::endl;
    for ( int i = 0; i < 160; ++i ) {
      if ( gtDecisionWord[i] ) {
	//	      std::cout<<i<<"  "<<gtDecisionWord[i]<<std::endl;
	int coef = 0;
	if (iebx == 0 ) coef = 1;
	if (iebx == 1 ) coef = 10;
	if (iebx == 2 ) coef = 100;
	if (iebx == 3 ) coef = 1000;
	if (iebx == 4 ) coef = 10000;;
	hashedTr[i]+= coef;
	treeVariables_.activeTriggersBX[i]= hashedTr[i];
      }
      //cout << __LINE__ << endl;
    }
  }
  if (DEBUG_) cout << __LINE__ << endl; 
    

  // apply masks on algo    
  int iDaq = 0;
  int iBit = -1;
  treeVariables_.nbOfActiveTriggers = 0 ;
  for ( int itBit = 0; itBit < 160; ++itBit ) {
    //   cout << __LINE__ << endl;
    iBit++;
    int fired=dWord[iBit];
    int maskBit = triggerMaskAlgoTrig[iBit] & (1 << iDaq);
    if (maskBit) fired  = 0; //else std::cout<<"not********** asked********"<<std::endl;
    if (fired) {
      treeVariables_.activeTriggers[treeVariables_.nbOfActiveTriggers] = iBit ;
      treeVariables_.nbOfActiveTriggers++ ;      
    }
    //cout << __LINE__ << endl;

  }
  if (DEBUG_) cout << __LINE__ << endl; 
  
  treeVariables_.nbOfActiveTechTriggers = 0;
  TechnicalTriggerWord tw = gtRecord->technicalTriggerWord();
  if ( ! tw.empty() ) {
    for ( int itechbit = 0; itechbit < 64; ++itechbit ) {
      treeVariables_.activeTechTriggers[treeVariables_.nbOfActiveTechTriggers] = 0; // ADD THIS 
      if ( tw[itechbit] ){
	treeVariables_.activeTechTriggers[treeVariables_.nbOfActiveTechTriggers] = itechbit;
	treeVariables_.nbOfActiveTechTriggers++ ;
      }
      // cout << __LINE__ << endl;

    }
  }
  if (DEBUG_) cout << __LINE__ << endl; 


#ifdef DEBUG
cout<<"obtained active trigger info"<<endl;
#endif

 if (DEBUG_) cout << __LINE__ << endl; 



    /////////////////////////// 
    // Get TP data  
    ///////////////////////////

 edm::Handle<EcalTrigPrimDigiCollection> tp;
    iEvent.getByLabel(tpCollection_,tp);
    if (print_) std::cout<<"TP collection size="<<tp.product()->size()<<std::endl ;
    
    for (unsigned int i=0;i<tp.product()->size();i++) {
        EcalTriggerPrimitiveDigi d = (*(tp.product()))[i];
        const EcalTrigTowerDetId TPtowid= d.id();
        towerEner tE ;
	//tE.TCCid_= theMapping_->TCCid(TPtowid);
	//tE.TowerInTCC_ = theMapping_->iTT(TPtowid);
	//	const EcalTriggerElectronicsId elId = theMapping_->getTriggerElectronicsId(id) ;
	//tE.strip_ = 0;//elId.pseudoStripId() ;
	
        tE.iphi_ = TPtowid.iphi() ;
        tE.ieta_ = TPtowid.ieta() ;
	tE.ttFlag_ = d[0].ttFlag();
        tE.tpgADC_ = (d[0].raw()&0xfff) ;
	tE.twrADC = (d[0].raw()&0xff) ;
	tE.sFGVB = (d[0].sFGVB());
	mapTower[TPtowid] = tE ;
    }
    if (DEBUG_) cout << __LINE__ << endl; 



  if (!noL1Info) {
    ///////////////////////////
    // get L1 candidate
    ///////////////////////////
    m_cellPos.clear();
    edm::ESHandle<CaloGeometry> geoHandle;
    iSetup.get<CaloGeometryRecord>().get(geoHandle);
    //Gets the geometry of the endcap
    const CaloGeometry& geometry = *geoHandle;
    //const CaloSubdetectorGeometry *barrelGeometry =   geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
    const std::vector< DetId > m_barrelCells = geometry.getValidDetIds(DetId::Ecal, EcalBarrel);
    const std::vector< DetId > m_endcapCells = geometry.getValidDetIds(DetId::Ecal, EcalEndcap);
    //const CaloSubdetectorGeometry *endcapGeometry = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);

    

  //   for (std::vector<DetId>::const_iterator endcapIt = m_endcapCells.begin();
  // 	 endcapIt!=m_endcapCells.end();
  // 	 ++endcapIt) {
  //     // const CaloCellGeometry *cellGeometry = endcapGeometry->getGeometry(*endcapIt);
  //     std::pair < double, double> pos, ipos; 
     
  //     //cout << __LINE__ << " " << cellGeometry->getPosition().phi() << " " << cellGeometry->getPosition().eta()<< " " << endcapIt->rawId()<< endl; 
  //    EBDetId id(*endcapIt) ;
  //    const EcalTrigTowerDetId towid= id.tower();
  //    pos=make_pair(l1t::CaloTools::towerPhi(towid.ieta(), towid.iphi()),  l1t::CaloTools::towerEta(towid.ieta()));
  //    ipos=make_pair(towid.iphi(), towid.ieta()); 

  //    m_cellPos.insert(std::pair < pair < double, double>, pair <double, double> > (pos, ipos)); 
  //    //cout << __LINE__ << " " << pos.first << " "<< pos.second << " " << ipos.first << " " << ipos.second<< endl;
     
  //   }
  //   for (std::vector<DetId>::const_iterator barrelIt = m_barrelCells.begin();
  // 	 barrelIt!=m_barrelCells.end();
  // 	 ++barrelIt) {
  //     std::pair < double, double> pos, ipos; 
  //     //const CaloCellGeometry *cellGeometry = barrelGeometry->getGeometry(*barrelIt);

  //    EBDetId id(*barrelIt) ;
  //    const EcalTrigTowerDetId towid= id.tower();
  //    pos=make_pair(l1t::CaloTools::towerPhi(towid.ieta(), towid.iphi()),  l1t::CaloTools::towerEta(towid.ieta()));
  //    ipos=make_pair(towid.iphi(), towid.ieta()); 
  //    m_cellPos.insert(std::pair < pair < double, double>, pair <double, double> > (pos, ipos)); 

      
  // }    
    Handle< BXVector<l1t::EGamma> > egs;
    iEvent.getByToken(m_egToken_,egs);
    
    Handle< BXVector<l1t::EGamma> > mp_egs;
    iEvent.getByToken(mp_egToken_,mp_egs);

    std::auto_ptr<l1t::EGammaBxCollection> matching_eg (new l1t::EGammaBxCollection);
    matching_eg->setBXRange(-2,2); 
    
    for (int ibx =egs->getFirstBX() ; ibx<=egs->getLastBX() ; ibx++){
      l1t::EGamma * myEG=0; 
      for (auto itp = egs->begin(ibx); itp != egs->end(ibx) ; itp++){
	myEG=(l1t::EGamma*) &*itp;
	if (myEG->hwPt() <= 0) continue;
	// cout << __LINE__  << " " << myevt << " " << iEvent.id().event() << " " << ibx << " tp pt " <<  myEG->hwPt()<< " " << myEG->hwEta() << " " << myEG->hwPhi()<< " " <<myEG->eta() << " "<< myEG->phi()<< "  " << int(28./63.*myEG->hwEta()) << " "<< int(myEG->hwPhi()/2.)<< " " <<  myEG->hwIso() << " " << endl;
	// pair <double, double> myPhiEta; 
	// myPhiEta= make_pair(myEG->phi(),  myEG->eta()); 
	
	// double dRmin=99; 
	// int ieta=0; 
	// int iphi=0;
	
	// for (map<pair<double, double>, pair <double, double>  >::const_iterator it= m_cellPos.begin() ; it!= m_cellPos.end() ; it++){
	//   double dR=reco::deltaR(it->first.second, it->first.first, myEG->eta(), myEG->phi());
	//   if (dR < dRmin){
	//     dRmin=dR; 
	   
	//     ieta=it->second.second;
	//     iphi=it->second.first; 
	//   }
	// }
	// cout << __LINE__ << " "<< dRmin <<  " " << ieta << " " << iphi<< endl ; 

	// myEG->setHwPhi(iphi); 
	// myEG->setHwEta(ieta); 
	myEG->setHwPhi(int(myEG->hwPhi()/2.)); 
	myEG->setHwEta(int(28./63.*myEG->hwEta())); 

	
	//	cout << __LINE__  << " " << myevt << " " << iEvent.id().event() << " " << ibx << " tp pt " <<  myEG->hwPt()<< " " << myEG->hwEta() << " " << myEG->hwPhi()<< " " << myEG->eta() << " "<< myEG->phi()<< " " <<  myEG->hwIso() << " " << myEG->towerIEta() << " " << myEG->towerIPhi() << endl;
	matching_eg->push_back(ibx, *myEG); 	
      }//itp
      
    }//ibx
       
    // edm::Handle < L1CaloEmCollection > em;
    // iEvent.getByToken(rctSourceEm_token_,em);
    // cout << __LINE__  << " L1CaloEm " << em->size() << endl; 
    // for (auto itp = em->begin(); itp != em->end() ; itp++){
    //   cout << __LINE__  << " " << myevt << " " << iEvent.id().event() << " " << itp->bx() << " " << itp->rank() << " " << itp->regionId().ieta() << " " << itp->regionId().iphi() << endl;
    // }

    int isocounter=0;
    int nonisocounter=0; 
    for (int ibx =-2; ibx<2 ; ibx++){
      for ( auto itp =  matching_eg->begin(ibx); itp !=  matching_eg->end(ibx) ; itp++){
	//cout << __LINE__ << endl;
	if (itp->hwPt() <=0) continue; 
	if (itp->hwIso()==0) nonisocounter++; 
	if (itp->hwIso()==1) isocounter++; 
      }//itp
    }//ibx
    
    if (L1print_)  std::cout << "iso size " << isocounter << std::endl;
    treeVariables_.nbOfL1IsoCands = isocounter;

    if (L1print_) std::cout << "noniso size " << nonisocounter << std::endl;
    treeVariables_.nbOfL1NonisoCands = nonisocounter;
    if (DEBUG_) cout << __LINE__ << endl; 

    isocounter = 0;
    nonisocounter =0;
    //    for( l1extra::L1EmParticleCollection::const_iterator emItr = emIsolColl->begin(); emItr != emIsolColl->end() ;++emItr){
    for (int ibx =-2; ibx<2 ; ibx++){
      for ( auto itp =  matching_eg->begin(ibx); itp !=  matching_eg->end(ibx) ; itp++){
	if (itp->hwIso()==1){
	  treeVariables_.L1IsoIphi[isocounter] = itp->hwPhi();
	  treeVariables_.L1IsoIeta[isocounter] = itp->hwEta();
	  treeVariables_.L1IsoRank[isocounter] = itp->hwPt(); 
	  isocounter++;
	}//Iso
	if (itp->hwIso()==0){
	  treeVariables_.L1NonisoIphi[nonisocounter] = itp->hwPhi();
	  treeVariables_.L1NonisoIeta[nonisocounter] = itp->hwEta();
	  treeVariables_.L1NonisoRank[nonisocounter] = itp->hwPt(); 
	  nonisocounter++;
	}//nonIso
      }//itp
    }//ibx

    //PRE-FIRING INFO
  


    int isocounter1=0;
    int nonisocounter1=0;
    for (int ibx =-2; ibx<0 ; ibx++){
      //cout << __LINE__ << endl;
      for ( auto itp =  matching_eg->begin(ibx); itp !=  matching_eg->end(ibx) ; itp++){
    	cout << __LINE__ << endl;
    	if (itp->hwPt() <=0) continue; 
    	if (itp->hwIso()==0){
	  treeVariables_.L1preNonisoIeta[nonisocounter1] = itp->hwEta();
	  treeVariables_.L1preNonisoIphi[nonisocounter1] = itp->hwPhi();
	  treeVariables_.L1preNonisoRank[nonisocounter1] = itp->hwPt(); 
	  nonisocounter1++;
	}

    	if (itp->hwIso()==1){
	  treeVariables_.L1preIsoIeta[isocounter1] = itp->hwEta();
	  treeVariables_.L1preIsoIphi[isocounter1] = itp->hwPhi();
	  treeVariables_.L1preIsoRank[isocounter1] = itp->hwPt(); //Do we have the rank now?
	  isocounter1++;
	}
      //cout << __LINE__ << endl;
      }
    }
    treeVariables_.nbOfL1preNonisoCands = nonisocounter1; 
    if (DEBUG_) cout << __LINE__ << endl; 
    treeVariables_.nbOfL1preIsoCands = isocounter1; 
    if (DEBUG_) cout << __LINE__ << endl; 
    
    //ontime fire INFO
    isocounter1=0;
    nonisocounter1=0;
    for ( auto itp =  matching_eg->begin(0); itp !=  matching_eg->end(0) ; itp++){
      //cout << __LINE__ << endl;
      if ( itp->hwPt() <=0 ) continue; 
      if (itp->hwIso()==0){ 
	treeVariables_.L1NonisoIeta[nonisocounter1] = itp->hwEta();
	treeVariables_.L1NonisoIphi[nonisocounter1] = itp->hwPhi();
	treeVariables_.L1NonisoRank[nonisocounter1] = itp->hwPt(); //Do we have the rank now?
	nonisocounter1++;
      }
      //cout << __LINE__ << endl;
      if (itp->hwIso()==1) {
	treeVariables_.L1IsoIeta[isocounter1] = itp->hwEta();
	treeVariables_.L1IsoIphi[isocounter1] = itp->hwPhi();
	treeVariables_.L1IsoRank[isocounter1] = itp->hwPt(); //Do we have the rank now?
	isocounter1++;
      }
    }
  
    treeVariables_.nbOfL1NonisoCands = nonisocounter1; 
    //cout << __LINE__ << endl;
    if (DEBUG_) cout << __LINE__ << endl; 

    treeVariables_.nbOfL1IsoCands = isocounter1; 
    if (DEBUG_) cout << __LINE__ << endl; 
    
    //POST-FIRING INFO
    isocounter1=0;
    nonisocounter1=0;
    for (int ibx =1; ibx<=+2 ; ibx++){
      //cout << __LINE__ << endl;
      for ( auto itp =  matching_eg->begin(ibx); itp !=  matching_eg->end(ibx) ; itp++){
    	cout << __LINE__ << endl;
    	if ( itp->hwPt() <=0 ) continue; 
    	if (itp->hwIso()==0){
	  treeVariables_.L1postNonisoIeta[nonisocounter1] = itp->hwEta();
	  treeVariables_.L1postNonisoIphi[nonisocounter1] = itp->hwPhi();
	  treeVariables_.L1postNonisoRank[nonisocounter1] = itp->hwPt(); //Do we have the rank now?
	  nonisocounter1++;
	}
    	if (itp->hwIso()==1) {
	  treeVariables_.L1postIsoIeta[isocounter1] = itp->hwEta();
	  treeVariables_.L1postIsoIphi[isocounter1] = itp->hwPhi();
	  treeVariables_.L1postIsoRank[isocounter1] = itp->hwPt(); //Do we have the rank now?
	  isocounter1++;
	}
      }
      //cout << __LINE__ << endl;
    }
    treeVariables_.nbOfL1postNonisoCands = nonisocounter1; 
    treeVariables_.nbOfL1postIsoCands = isocounter1; 
    //cout << __LINE__ << endl;

  }//if!noL1Info
#ifdef DEBUG
  cout<<"obtained L1 event infor"<<endl;
#endif

    ///////////////////////////
    // Get Emulators TP
    ///////////////////////////

    edm::Handle<EcalTrigPrimDigiCollection> tpEmul ;
    iEvent.getByLabel(tpEmulatorCollection_, tpEmul);
    if (print_) std::cout<<"TPEmulator collection size="<<tpEmul.product()->size()<<std::endl ;
    
    for (unsigned int i=0;i<tpEmul.product()->size();i++) {
        EcalTriggerPrimitiveDigi d = (*(tpEmul.product()))[i];
        const EcalTrigTowerDetId TPtowid= d.id();
        itTT = mapTower.find(TPtowid) ;
        if (itTT != mapTower.end())
	  for (int j=0 ; j<5 ; j++) {
	    (itTT->second).tpgEmul_[j] = (d[j].raw()&0xfff) ;
	    (itTT->second).tpgEmulFlag_[j] = d[j].ttFlag();
	    (itTT->second).tpgEmulsFGVB_[j] = d[j].sFGVB();
	  }
    }
    if (DEBUG_) cout << __LINE__ << endl; 

    ///////////////////////////
    // Get nb of crystals read out
    ///////////////////////////

    // Get EB xtal digi inputs
    edm::Handle<EBDigiCollection> digiEB;
    //    iEvent.getByLabel(digiCollectionEB_, digiEB);
    iEvent.getByToken(digiCollectionEB1_,digiEB);
    EBDataFrame dfMaxEB ;
    if(print_) cout << " EB digi collection size " << digiEB.product()->size()  << endl ;
    if (DEBUG_) cout << __LINE__ << endl; 

    for (unsigned int i=0;i<digiEB.product()->size();i++) {
        const EBDataFrame & df = (*(digiEB.product()))[i];    
        const EBDetId & id = df.id();
        const EcalTrigTowerDetId towid = id.tower();
        itTT = mapTower.find(towid) ;
        if (itTT != mapTower.end()) {
            (itTT->second).nbXtal_++ ;
            bool fill(false) ;
            if (((itTT->second).tpgADC_ & 0xff)>0) fill = true ;   
	    for (int j=0 ; j<5 ; j++) if (((itTT->second).tpgEmul_[j] & 0xff)>8) fill = true ;			
            if (fill) {
	      if(print_) cout<<"TP="<<((itTT->second).tpgADC_ & 0xff)<<" eta="<<towid.ieta()<<" phi="<<towid.iphi()<<endl ;
	      if (print_) for (int j=0 ; j<5 ; j++) if (((itTT->second).tpgEmul_[j] & 0xff)>8) cout << "tp emul "<<  j << " " << ((itTT->second).tpgEmul_[j] & 0xff)<< endl;
                treeVariablesShape_.ieta = towid.ieta() ;
                treeVariablesShape_.iphi = towid.iphi() ;
                treeVariablesShape_.ixXtal = id.iphi() ;
                treeVariablesShape_.iyXtal = id.ieta() ;
                treeVariablesShape_.TCCid = theMapping_->TCCid(towid);
                treeVariablesShape_.TowerInTCC = theMapping_->iTT(towid);
                const EcalTriggerElectronicsId elId = theMapping_->getTriggerElectronicsId(id) ;
                treeVariablesShape_.strip = elId.pseudoStripId() ;
                treeVariablesShape_.nbOfSamples = df.size() ;
                for (int s=0 ; s<df.size() ; s++) treeVariablesShape_.samp[s] = df[s].adc() ; 
                treeShape_->Fill() ;
            }
        }
    }

    ///////////////////////////
    // Get rechits and spikes
    ///////////////////////////

    edm::ESHandle<EcalSeverityLevelAlgo> sevlv;
    iSetup.get<EcalSeverityLevelAlgoRcd>().get(sevlv);


    
    // channel status
    edm::ESHandle<EcalChannelStatus> pChannelStatus;
    iSetup.get<EcalChannelStatusRcd>().get(pChannelStatus);
  

    //  const EcalChannelStatus *chStatus = pChannelStatus.product();
    //const EcalRecHit * rh; 
   
    // Get EB rechits
    edm::Handle<EcalRecHitCollection> rechitsEB; 
    //    iEvent.getByLabel(EcalRecHitCollectionEB_, rechitsEB) ;
    iEvent.getByToken(EcalRecHitCollectionEB1_,rechitsEB); 
    //std::cout << " rechitsEB size " << rechitsEB.product()->size() << std::endl;
    float maxRecHitEnergy = 0. ;
    if (rechitsEB.product()->size()!=0) {
      for ( EcalRecHitCollection::const_iterator rechitItr = rechitsEB->begin(); rechitItr != rechitsEB->end(); ++rechitItr ) {   
	EBDetId id = rechitItr->id(); 
	const EcalTrigTowerDetId towid = id.tower();
	
	itTT = mapTower.find(towid) ;

	if (itTT != mapTower.end()) {
	       
	  double theta = theBarrelGeometry_->getGeometry(id)->getPosition().theta() ;
	  (itTT->second).eRec_ += rechitItr->energy()*sin(theta) ;
	  if (maxRecHitEnergy < rechitItr->energy()*sin(theta) && rechitItr->energy()*sin(theta) > 1. ){
	    (itTT->second).sevlv_ = sevlv->severityLevel(id, *rechitsEB); 

	  }
	  (itTT->second).crystNb_++;
	}
      }
    }
   if (DEBUG_) cout << __LINE__ << endl; 

  
    if (0) {
        // Get EE xtal digi inputs
        edm::Handle<EEDigiCollection> digiEE;
	//        iEvent.getByLabel(digiCollectionEE_, digiEE);
	iEvent.getByToken(digiCollectionEE1_,digiEE);
	EEDataFrame dfMaxEE ;
	if(print_) cout << " EE digi collection size " << digiEE.product()->size()  << endl ;
        for (unsigned int i=0;i<digiEE.product()->size();i++) {
            const EEDataFrame & df = (*(digiEE.product()))[i];
            const EEDetId & id = df.id();
            const EcalTrigTowerDetId towid = (*eTTmap_).towerOf(id);
            itTT = mapTower.find(towid) ;
            if (itTT != mapTower.end()) {
                (itTT->second).nbXtal_++ ;
                if (!skipWritingEndcapDigis_)
                {
                    bool fill(false) ;
                    if (((itTT->second).tpgADC_ & 0xff)>0) fill = true ;
                    for (int j=0 ; j<5 ; j++) if (((itTT->second).tpgEmul_[j] & 0xff)>0) fill = true ;
                    for (int s=1 ; s<df.size() ; s++) if (df[s].adc()-df[0].adc()>15) fill = true ;
                    if (fill) {
		      if(print_) cout<<"TP="<<((itTT->second).tpgADC_ & 0xff)<<" eta="<<towid.ieta()<<" phi="<<towid.iphi()<<endl ;
                        treeVariablesShape_.ieta = towid.ieta() ;
                        treeVariablesShape_.iphi = towid.iphi() ;
                        treeVariablesShape_.ixXtal = id.ix() ;
                        treeVariablesShape_.iyXtal = id.iy() ;
			const EcalTriggerElectronicsId elId = theMapping_->getTriggerElectronicsId(id) ;
                        treeVariablesShape_.TCCid = theMapping_->TCCid(towid);
                        treeVariablesShape_.TowerInTCC = theMapping_->iTT(towid);
                        treeVariablesShape_.strip = elId.pseudoStripId() ;
                        treeVariablesShape_.nbOfSamples = df.size() ;
                        for (int s=0 ; s<df.size() ; s++) treeVariablesShape_.samp[s] = df[s].adc() ; 
                        treeShape_->Fill() ;
                    }
                }
            }
        }  // loop over EE digi
    } // UseEE_

    ///////////////////////////  
    // fill tree
    ///////////////////////////  

    int towerNb = 0 ;
    for (itTT = mapTower.begin() ; itTT != mapTower.end() ; ++itTT) {

        // select only non zero towers
        bool fill(true) ;
        bool nonZeroEmul(false) ;
        for (int i=0 ; i<5 ; i++) if (((itTT->second).tpgEmul_[i]&0xff) > 0) nonZeroEmul = true ;
        if ( keepOnlyTowersAboveZero_ && ((itTT->second).tpgADC_&0xff) <= 0 && (!nonZeroEmul) ) fill = false ;
        if (print_ && fill) {
            std::cout<<"ieta="<<(itTT->second).ieta_<<" "<<(itTT->second).iphi_<<" tp="<<((itTT->second).tpgADC_&0xff)<<" tpEmul=" ;
            for (int i=0 ; i<5 ; i++) std::cout<<((itTT->second).tpgEmul_[i]&0xff)<<" " ;
            std::cout<<" nbXtal="<<(itTT->second).nbXtal_ ;
            std::cout<<std::endl ;      
        }
	//	fill=true;
        if (fill) {
            treeVariables_.ieta[towerNb] = (itTT->second).ieta_ ;
            treeVariables_.iphi[towerNb] = (itTT->second).iphi_ ;
            treeVariables_.nbOfXtals[towerNb] = (itTT->second).nbXtal_ ;
            treeVariables_.rawTPData[towerNb] = (itTT->second).tpgADC_ ;
            treeVariables_.rawTPEmul1[towerNb] = (itTT->second).tpgEmul_[0] ;
            treeVariables_.rawTPEmul2[towerNb] = (itTT->second).tpgEmul_[1] ;
            treeVariables_.rawTPEmul3[towerNb] = (itTT->second).tpgEmul_[2] ;
            treeVariables_.rawTPEmul4[towerNb] = (itTT->second).tpgEmul_[3] ;
            treeVariables_.rawTPEmul5[towerNb] = (itTT->second).tpgEmul_[4] ;
            treeVariables_.rawTPEmulttFlag1[towerNb] = (itTT->second).tpgEmulFlag_[0] ;
            treeVariables_.rawTPEmulttFlag2[towerNb] = (itTT->second).tpgEmulFlag_[1] ;
            treeVariables_.rawTPEmulttFlag3[towerNb] = (itTT->second).tpgEmulFlag_[2] ;
            treeVariables_.rawTPEmulttFlag4[towerNb] = (itTT->second).tpgEmulFlag_[3] ;
            treeVariables_.rawTPEmulttFlag5[towerNb] = (itTT->second).tpgEmulFlag_[4] ;
            treeVariables_.rawTPEmulsFGVB1[towerNb] = (itTT->second).tpgEmulsFGVB_[0] ;
            treeVariables_.rawTPEmulsFGVB2[towerNb] = (itTT->second).tpgEmulsFGVB_[1] ;
            treeVariables_.rawTPEmulsFGVB3[towerNb] = (itTT->second).tpgEmulsFGVB_[2] ;
            treeVariables_.rawTPEmulsFGVB4[towerNb] = (itTT->second).tpgEmulsFGVB_[3] ;
            treeVariables_.rawTPEmulsFGVB5[towerNb] = (itTT->second).tpgEmulsFGVB_[4] ;
            treeVariables_.crystNb[towerNb] = (itTT->second).crystNb_ ;
            treeVariables_.eRec[towerNb] = (itTT->second).eRec_ ;
            treeVariables_.sevlv[towerNb] = (itTT->second).sevlv_ ;
            treeVariables_.ttFlag[towerNb] = (itTT->second).ttFlag_ ;
	    //treeVariables_.TCCid[towerNb] = (itTT->second).TCCid_;
	    //treeVariables_.TowerInTCC[towerNb] = (itTT->second).TowerInTCC_;
	    //treeVariables_.strip[towerNb] = (itTT->second).strip_;
	    
            treeVariables_.spike[towerNb] = (itTT->second).spike_ ;
	    treeVariables_.twrADC[towerNb] =  (itTT->second).twrADC;
	    treeVariables_.sFGVB[towerNb] =  (itTT->second).sFGVB;
	    
            if (abs(treeVariables_.ieta[towerNb])>17) {
                unsigned int maxEmul = 0 ;
                for (int i=0 ; i<5 ; i++) if (((itTT->second).tpgEmul_[i]&0xff) > maxEmul) maxEmul = ((itTT->second).tpgEmul_[i]&0xff) ;
            }
            towerNb++ ;
        }

    }
    
    treeVariables_.nbOfTowers = towerNb ;
    tree_->Fill() ;

}

DEFINE_FWK_MODULE(EcalTPGAnalyzer);
