datacards=$1
#cate=(dl_3j3t dl_ge4j3t_low dl_ge4j3t_high dl_ge4jge4t_low dl_ge4jge4t_high dl_merged)
#cate=(sl_merged dl_merged)
cate=(sl_dl_merged)

let loop_cat=${#cate[@]}-1
for (( icat = 0; icat <= $loop_cat ; icat++ ))
do
   mkdir fits_${cate[icat]}
   cd fits_${cate[icat]}
   
   echo "Building workspace for category "${cate[icat]}
   text2workspace.py ../$datacards/ttH_hbb_13TeV_${cate[icat]}.txt -m 125 -b -o model_${cate[icat]}.root
   
   mkdir unblind
   cd unblind
   echo "  Performing fits to observed data"
   #nohup combine ../model_${cate[icat]}.root -M MaxLikelihoodFit --minimizerStrategy 0 --minimizerTolerance 0.1 --rMin -10 --rMax 50 --saveShapes --saveWithUncertainties --plots -v 3 -m 125 -n _shapes_unblind > nohup_unblind.out 2>&1&    
   nohup combine ../model_${cate[icat]}.root -M MaxLikelihoodFit --minimizerStrategy 0 --minimizerTolerance 0.001 --minos all --rMin -10 --rMax 10 --saveShapes --saveWithUncertainties --plots -v 3 -m 125 -n _shapes_unblind_minosall > nohup_unblind_minosall.out 2>&1&    
   cd ..
   
   cd ..
done
