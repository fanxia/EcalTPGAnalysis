from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'DataTP2016Hv2'
config.General.workArea = 'DataTP2016Hv2'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runTPG_cfg_data.py'
#config.JobType.inputFiles = [ 'Summer15_25nsV5_DATA.db', 'Summer15_25nsV5_DATA_UncertaintySources_AK4PFchs.txt', 'Cert_246908-257599_13TeV_PromptReco_Collisions15_25ns_JSON.txt']
config.JobType.maxMemoryMB = 2000
config.JobType.outputFiles = ['ECALTPGtree.root']
config.Data.inputDataset = '/DoubleEG/Run2016H-ZElectron-PromptReco-v2/RAW-RECO'
config.Data.inputDBS = 'global'
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/Cert_271036-284044_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt'
#config.Data.runRange = '273158-274421'

#config.Data.splitting = 'LumiBased'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = 1800

#config.Data.splitting = 'EventBased'
#config.Data.unitsPerJob = 10 
#NJOBS = 1
#config.Data.totalUnits = config.Data.unitsPerJob * NJOBS 

config.Data.publication = False

config.Data.outLFNDirBase = '/store/user/fxia/TPG/RAWRECO_2016H' 
config.Data.ignoreLocality = True                                                                                                                                                                       
config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'  
