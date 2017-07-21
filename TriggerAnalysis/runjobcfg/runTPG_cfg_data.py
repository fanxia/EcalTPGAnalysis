import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process("TPGANALYSIS",eras.Run2_2016)

process.load("SimCalorimetry.EcalTrigPrimProducers.ecalTriggerPrimitiveDigis_readDBOffline_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v14'
#process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v9'
process.load('Configuration.Geometry.GeometryExtended2016Reco_cff')

# ECAL Unpacker
process.load("EventFilter.EcalRawToDigi.EcalUnpackerMapping_cfi")
process.load("EventFilter.EcalRawToDigi.EcalUnpackerData_cfi")

# ECAL TPG Producer
process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")
process.load("Geometry.CaloEventSetup.CaloGeometry_cfi")
process.load("Geometry.CaloEventSetup.EcalTrigTowerConstituents_cfi")

process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.load('L1Trigger.Configuration.L1TReco_cff')

process.load('Configuration.EventContent.EventContent_cff')


process.raw2digi_step = cms.Path(process.RawToDigi)
process.endjob_step = cms.EndPath(process.endOfProcess)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step)

from L1Trigger.Configuration.customiseReEmul import L1TReEmulFromRAW
#,L1TEventSetupForHF1x1TPs  this last one is not in the release

#call to customisation function L1TReEmulFromRAW imported from L1Trigger.Configuration.customiseReEmul
process = L1TReEmulFromRAW(process)
from EventFilter.L1TRawToDigi.caloStage2Digis_cfi import caloStage2Digis
process.rawCaloStage2Digis = caloStage2Digis.clone()
process.rawCaloStage2Digis.InputLabel = cms.InputTag('rawDataCollector')


process.ecalTriggerPrimitiveDigis = cms.EDProducer("EcalTrigPrimProducer",
   InstanceEB = cms.string('ebDigis'),
   InstanceEE = cms.string('eeDigis'),
   Label = cms.string('ecalDigis'),
   BarrelOnly = cms.bool(False),
   Famos = cms.bool(False),
   TcpOutput = cms.bool(False),
   Debug = cms.bool(False),
   binOfMaximum = cms.int32(6), ## optional from release 200 on, from 1-10

)

#test HLT
process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
#process.hltHighLevel.HLTPaths = cms.vstring("HLT_ZeroBias_v*")



# ECAL rechits and co
process.load("Configuration/StandardSequences/Reconstruction_cff")
import RecoLocalCalo.EcalRecProducers.ecalGlobalUncalibRecHit_cfi
process.ecalUncalibHit = RecoLocalCalo.EcalRecProducers.ecalGlobalUncalibRecHit_cfi.ecalGlobalUncalibRecHit.clone()
process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")
process.load("RecoLocalCalo.EcalRecProducers.ecalDetIdToBeRecovered_cfi")
process.ecalRecHit.EBuncalibRecHitCollection = 'ecalUncalibHit:EcalUncalibRecHitsEB'
process.ecalRecHit.EEuncalibRecHitCollection = 'ecalUncalibHit:EcalUncalibRecHitsEE'

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
fileNames = (cms.untracked.vstring('root://xrootd-cms.infn.it//store/data/Run2016B/ZeroBias/RAW-RECO/LogError-PromptReco-v2/000/273/302/00000/04970E1E-861A-E611-827E-02163E014249.root'))
#fileNames = cms.untracked.vstring()
#fileNames = (cms.untracked.vstring('root://xrootd-cms.infn.it///store/data/Run2016H/DoubleEG/RAW-RECO/ZElectron-PromptReco-v2/000/281/613/00000/6C606673-CE84-E611-A7D4-02163E0126CD.root'))
#fileNames = (cms.untracked.vstring('root://xrootd-cms.infn.it//store/data/Run2016B/DoubleEG/RAW-RECO/ZElectron-PromptReco-v2/000/273/158/00000/0404DC6B-E519-E611-9CBA-02163E012950.root'))
)

process.tpAnalyzer = cms.EDAnalyzer("EcalTPGAnalyzer",
   VtxLabel = cms.InputTag("offlinePrimaryVertices"),
   TPCollection = cms.InputTag("ecalDigis","EcalTriggerPrimitives"),
   TPEmulatorCollection =  cms.InputTag("ecalTriggerPrimitiveDigis",""),
   DigiCollectionEB = cms.InputTag("ecalDigis","ebDigis"),
   DigiCollectionEE = cms.InputTag("ecalDigis","eeDigis"),
   GTRecordCollection = cms.string('gtDigis'),
   EcalRecHitCollectionEB = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
   EcalRecHitCollectionEE = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
   uncalibratedRecHitCollectionEB = cms.InputTag("ecalMaxSampleUncalibRecHit","EcalUncalibRecHitsEB"),
   uncalibratedRecHitCollectionEE = cms.InputTag("ecalMaxSampleUncalibRecHit","EcalUncalibRecHitsEE"),
   l1extraIsol=cms.InputTag("l1extraParticles","Isolated"), 
   l1extraNonIsol= cms.InputTag("l1extraParticles","NonIsolated"), 
   Print = cms.bool(False),
   L1Print = cms.bool(False),
   ReadTriggerPrimitives = cms.bool(True),
   UseEndCap = cms.bool(True),
   keepOnlyTowersAboveZero = cms.bool(True),
   skipWritingEndcapDigi = cms.bool(True)
)

process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.load("RecoLocalCalo.EcalRecProducers.ecalGlobalUncalibRecHit_cfi")
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")
process.load("RecoLocalCalo.EcalRecProducers.ecalDetIdToBeRecovered_cfi")
process.ecalRecHit.EBuncalibRecHitCollection = 'ecalUncalibHit:EcalUncalibRecHitsEB'
process.ecalRecHit.EEuncalibRecHitCollection = 'ecalUncalibHit:EcalUncalibRecHitsEE'

process.p = cms.Path(
   process.rawCaloStage2Digis*
   process.L1Reco*
   process.ecalTriggerPrimitiveDigis*
   process.ecalUncalibHit*
   process.ecalDetIdToBeRecovered*
   process.ecalRecHit*
 #  process.hltHighLevel*
   process.tpAnalyzer
)


process.schedule.append(process.p)

process.MessageLogger = cms.Service("MessageLogger"
   ,cout = cms.untracked.PSet(threshold = cms.untracked.string('ERROR')),
   destinations = cms.untracked.vstring('cout')
)
