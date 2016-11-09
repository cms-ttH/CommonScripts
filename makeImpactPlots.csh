#!/bin/csh -f

### replace dl_ with sl_ or blank  # _noMCstats

set channel = "" ##  ("_dl" "_sl" "")

 text2workspace.py ttH_hbb_13TeV{$channel}_merged.txt -o ws_ttH_hbb_13TeV{$channel}.root
  
 combineTool.py -M Impacts -d ws_ttH_hbb_13TeV{$channel}.root -m 125 --doInitialFit --robustFit 1 --rMin -6 --rMax 6
 
 combineTool.py -M Impacts -d ws_ttH_hbb_13TeV{$channel}.root -m 125 --robustFit 1 --doFits --rMin -6 --rMax 6 --parallel 5
  
 combineTool.py -M Impacts -d ws_ttH_hbb_13TeV{$channel}.root -m 125 --rMin -6 --rMax 6 -o impacts_ws_ttH_hbb_13TeV{$channel}.json
 
 plotImpacts.py -i impacts_ws_ttH_hbb_13TeV{$channel}.json -o impacts_ttH_hbb_13TeV{$channel}    


echo "Finish making impact plot for $channel channel"
