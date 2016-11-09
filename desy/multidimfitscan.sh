#model.root originates from text2workspace.py ttH_hbb_13TeV_sl_dl_merged.txt -m 125 -b -o model.root

combine model.root -M MultiDimFit --algo=grid -m 125 --floatOtherPOI=1 -P r --setPhysicsModelParameterRanges r=-2,2 --saveInactivePOI=1 --minimizerTolerance=0.001 -n _forZscore
