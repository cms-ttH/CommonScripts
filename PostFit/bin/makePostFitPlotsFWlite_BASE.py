import FWCore.ParameterSet.Config as cms

process = cms.Process("makePostFitPlotsFWlite")

process.source = cms.Source("EmptySource")

process.fwliteInput = cms.PSet(
    path2Workspace  = cms.string("NONE"),
    path2Datacard   = cms.string("REPLACE_DATACARD"),
    path2FitResults = cms.string("REPLACE_MLFIT"),
    outputName      = cms.string("REPLACE_OUTPUT"),
    dirName         = cms.string("NONE"),

)

process.p = cms.Path()

