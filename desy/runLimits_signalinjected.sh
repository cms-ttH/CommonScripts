##model.root originates from text2workspace.py ttH_hbb_13TeV_sl_dl_merged.txt -m 125 -b -o model.root

#first perform b+s fit of nuisances to real data and store it as workspace
combine model.root -M MultiDimFit --saveWorkspace -n _step1 --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin -10 --rMax 10 -m 125
signal=(0 1 2)
let loop_signal=${#signal[@]}-1
for (( isig = 0; isig <= $loop_signal ; isig++ ))
do
   ##'unblinded' way
   #then evaluate limit of Asimov data constructed using post-fit nuisances and with a signal of certain strength injected
   combine higgsCombine_step1.MultiDimFit.mH125.root -w w --snapshotName "MultiDimFit" -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -t -1 --expectSignal=${signal[isig]} -n _step2_expectSignal${signal[isig]}  | tee -a ttH_hbb_13TeV_unblind_expectSignal${signal[isig]}.lmt
   
   ##regular blinded way
   combine model.root -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -t -1 --expectSignal=${signal[isig]} | tee -a ttH_hbb_13TeV_blind_expectSignal${signal[isig]}.lmt
done

