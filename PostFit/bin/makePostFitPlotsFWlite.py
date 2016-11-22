import FWCore.ParameterSet.Config as cms

process = cms.Process("makePostFitPlotsFWlite")

process.source = cms.Source("EmptySource")


process.fwliteInput = cms.PSet(



    path2Workspace  = cms.string("NONE"),
    path2Datacard   = cms.string("/mnt/t3nfs01/data01/shome/gregor/inputs2DV12RAW/inputs2DV12RAW_datacard_BDT_common5_input_all_sum_pt_with_met_hdecay.txt"),
    path2FitResults = cms.string("/mnt/t3nfs01/data01/shome/gregor/inputs2DV12RAW/mlfit.root"),
    outputName      = cms.string("test.root"),
    dirName         = cms.string("NONE"),

)

process.p = cms.Path()

