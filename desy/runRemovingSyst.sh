## Runs the datacards with a removed systematic (output of removeSyst.sh)
## The procedure may need to be reconsidered though; for the unblinded case one could also check the effect of freezing nuisances but at the post-fit value, this can be different than just removing the nuisance from the datacards

## run as source runRemovingSyst.sh <datacard_dir>

datacards=$1
systematics=("CMS_res_j" "CMS_scale_j" "CMS_ttH_CSVCErr1" "CMS_ttH_CSVCErr2" "CMS_ttH_CSVHF" "CMS_ttH_CSVHFStats1" "CMS_ttH_CSVHFStats2" "CMS_ttH_CSVLF" "CMS_ttH_CSVLFStats1" "CMS_ttH_CSVLFStats2" "CMS_ttH_QCDscale_ttbarPlus2B"
"CMS_ttH_QCDscale_ttbarPlusB" "CMS_ttH_QCDscale_ttbarPlusBBbar" "CMS_ttH_QCDscale_ttbarPlusCCbar" "QCDscale_ttH" "QCDscale_ttbar"
"lumi_13TeV" "pdf_gg" "pdf_gg_ttH" "pdf_qg" "pdf_qqbar" "CMS_ttH_PU" "CMS_ttH_dl_Trig" "CMS_ttH_PSscale_ttbarOther" 
"CMS_ttH_PSscale_ttbarPlus2B" "CMS_ttH_PSscale_ttbarPlusB" "CMS_ttH_PSscale_ttbarPlusBBbar" "CMS_ttH_PSscale_ttbarPlusCCbar" 
"CMS_ttH_Q2scale_ttbarOther" "CMS_ttH_Q2scale_ttbarPlus2B" "CMS_ttH_Q2scale_ttbarPlusB" 
"CMS_ttH_Q2scale_ttbarPlusBBbar" "CMS_ttH_Q2scale_ttbarPlusCCbar" "QCDscale_V" "QCDscale_VV" "QCDscale_singlet")


combine -M Asymptotic --run="blind" --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n "_systeffect_blind_nominal" $datacards/ttH_hbb_13TeV_dl_merged.txt | tee -a ttH_hbb_13TeV_systeffect_blind_dl_merged.lmt
#combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1  -m 125 -n "_systeffect_unblind_nominal" $datacards/ttH_hbb_13TeV_dl_merged.txt | tee -a ttH_hbb_13TeV_systeffect_unblind_dl_merged.lmt


let loop_systematics=${#systematics[@]}-1

for (( isyst = 0; isyst <= $loop_systematics ; isyst++ ))
do
   combine -M Asymptotic --run="blind" --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n "_systeffect_blind_no"${systematics[isyst]} $datacards/ttH_hbb_13TeV_dl_merged_no${systematics[isyst]}.txt | tee -a ttH_hbb_13TeV_systeffect_blind_dl_merged_no${systematics[isyst]}.lmt
   #combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1  -m 125 -n "_systeffect_no"${systematics[isyst]} ttH_hbb_13TeV_dl_merged_no${systematics[isyst]}.txt | tee -a ttH_hbb_13TeV_systeffect_unblind_dl_merged_no${systematics[isyst]}.lmt
done
