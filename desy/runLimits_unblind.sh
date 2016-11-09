datacards=$1
#nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_3j2t $datacards/ttH_hbb_13TeV_dl_3j2t.txt | tee -a ttH_hbb_13TeV_unblind_dl_3j2t.lmt > nohup_unblind_3j2t.out 2>&1&
nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_3j3t $datacards/ttH_hbb_13TeV_dl_3j3t.txt | tee -a ttH_hbb_13TeV_unblind_dl_3j3t.lmt > nohup_unblind_3j3t.out 2>&1&
#nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_ge4j2t $datacards/ttH_hbb_13TeV_dl_ge4j2t.txt   | tee -a ttH_hbb_13TeV_unblind_dl_ge4j2t.lmt > nohup_unblind_ge4j2t.out 2>&1&
##nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_ge4j3t $datacards/ttH_hbb_13TeV_dl_ge4j3t.txt   | tee -a ttH_hbb_13TeV_unblind_dl_ge4j3t.lmt > nohup_unblind_ge4j3t.out 2>&1&
##nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_ge4jge4t $datacards/ttH_hbb_13TeV_dl_ge4jge4t.txt | tee -a ttH_hbb_13TeV_unblind_dl_ge4jge4t.lmt > nohup_unblind_ge4jge4t.out 2>&1&

nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_ge4j3t_low  $datacards/ttH_hbb_13TeV_dl_ge4j3t_low.txt  | tee -a ttH_hbb_13TeV_unblind_dl_ge4j3tlow.lmt > nohup_unblind_ge4j3t_low.out 2>&1&
nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_ge4j3t_high $datacards/ttH_hbb_13TeV_dl_ge4j3t_high.txt | tee -a ttH_hbb_13TeV_unblind_dl_ge4j3thigh.lmt > nohup_unblind_ge4j3t_high.out 2>&1&
nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_ge4jge4t_low  $datacards/ttH_hbb_13TeV_dl_ge4jge4t_low.txt  | tee -a ttH_hbb_13TeV_unblind_dl_ge4jge4tlow.lmt > nohup_unblind_ge4jge4t_low.out 2>&1&
nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_ge4jge4t_high $datacards/ttH_hbb_13TeV_dl_ge4jge4t_high.txt | tee -a ttH_hbb_13TeV_unblind_dl_ge4jge4thigh.lmt > nohup_unblind_ge4jge4t_high.out 2>&1&

nohup combine -M Asymptotic --minimizerStrategy 0 --minimizerTolerance 0.1 -m 125 -n _unblind_merged $datacards/ttH_hbb_13TeV_dl_merged.txt | tee -a ttH_hbb_13TeV_unblind_dl_merged.lmt > nohup_unblind_merged.out 2>&1&
