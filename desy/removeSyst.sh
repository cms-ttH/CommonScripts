## Makes datacards removing one by one a systematic
## Be careful with number of systematics (kmax), check in file how many are in there by default, otherwise replace statement won't work
## If there is a * in the datacard, the replace command is not needed and can be kept commented

## run as source removeSyst.sh in a datacard directory

systematics=("CMS_res_j" "CMS_scale_j" "CMS_ttH_CSVCErr1" "CMS_ttH_CSVCErr2" "CMS_ttH_CSVHF" "CMS_ttH_CSVHFStats1" 
"CMS_ttH_CSVHFStats2" "CMS_ttH_CSVLF" "CMS_ttH_CSVLFStats1" "CMS_ttH_CSVLFStats2" "CMS_ttH_QCDscale_ttbarPlus2B"
"CMS_ttH_QCDscale_ttbarPlusB" "CMS_ttH_QCDscale_ttbarPlusBBbar" "CMS_ttH_QCDscale_ttbarPlusCCbar" "QCDscale_ttH" "QCDscale_ttbar"
"lumi_13TeV" "pdf_gg" "pdf_gg_ttH" "pdf_qg" "pdf_qqbar" "CMS_ttH_PU" "CMS_ttH_dl_Trig" "CMS_ttH_PSscale_ttbarOther" 
"CMS_ttH_PSscale_ttbarPlus2B" "CMS_ttH_PSscale_ttbarPlusB" "CMS_ttH_PSscale_ttbarPlusBBbar" "CMS_ttH_PSscale_ttbarPlusCCbar" 
"CMS_ttH_Q2scale_ttbarOther" "CMS_ttH_Q2scale_ttbarPlus2B" "CMS_ttH_Q2scale_ttbarPlusB" 
"CMS_ttH_Q2scale_ttbarPlusBBbar" "CMS_ttH_Q2scale_ttbarPlusCCbar" "QCDscale_V" "QCDscale_VV" "QCDscale_singlet")


#cat mvaEventA_mvaWeight_combined_step7.dat | grep -v "BDTbin" > mvaEventA_mvaWeight_combined_step7_noMCstatUnc.dat
#sed -i 's/kmax 550/kmax 20/g#' mvaEventA_mvaWeight_combined_step7_noMCstatUnc.dat

let loop_systematics=${#systematics[@]}-1

for (( isyst = 0; isyst <= $loop_systematics ; isyst++ ))
do
   cat ttH_hbb_13TeV_dl_merged.txt | grep -v ${systematics[isyst]}" " > ttH_hbb_13TeV_dl_merged_no${systematics[isyst]}.txt
   #sed -i 's/kmax 36/kmax 35/g#' ttH_hbb_13TeV_dl_merged_no${systematics[isyst]}.txt
done
