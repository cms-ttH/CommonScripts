datacards=$1
cate=(3j3t ge4j3t_low ge4j3t_high ge4jge4t_low ge4jge4t_high merged)

let loop_cat=${#cate[@]}-1
for (( icat = 0; icat <= $loop_cat ; icat++ ))
do
   mkdir fits_${cate[icat]}
   cd fits_${cate[icat]}
   
   echo "Building workspace for category "${cate[icat]}
   text2workspace.py ../$datacards/ttH_hbb_13TeV_dl_${cate[icat]}.txt -m 125 -b -o model_dl_${cate[icat]}.root
   
   mkdir bkgonlyAsimov
   cd bkgonlyAsimov
   echo "  Performing fits to background-only Asimov"
   nohup combine ../model_dl_${cate[icat]}.root -M MaxLikelihoodFit --minos=all --saveShapes --saveWithUncertainties --plots -v 3 -m 125 -t -1 --expectSignal 0 -n _shapes_bkgonlyAsimov > nohup_bkgonlyAsimov.out 2>&1&    

   cd ..
   mkdir bkgsigAsimov
   cd bkgsigAsimov
   echo "  Performing fits to background+signal Asimov"
   nohup combine ../model_dl_${cate[icat]}.root -M MaxLikelihoodFit --minos=all --saveShapes --saveWithUncertainties --plots -v 3 -m 125 -t -1 --expectSignal 1 -n _shapes_bkgsigAsimov > nohup_bkgsigonlyAsimov.out 2>&1&
   
   cd ..
   
   cd ..
done
