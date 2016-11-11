datacards=$1
cate=(dl_3j3t dl_ge4j3t_low dl_ge4j3t_high dl_ge4jge4t_low dl_ge4jge4t_high dl_merged)

let loop_cat=${#cate[@]}-1
for (( icat = 0; icat <= $loop_cat ; icat++ ))
do
   mkdir fits_${cate[icat]}
   cd fits_${cate[icat]}
   
   echo "Building workspace for category "${cate[icat]}
   text2workspace.py ../$datacards/ttH_hbb_13TeV_${cate[icat]}.txt -m 125 -b -o model_${cate[icat]}.root
   
   mkdir bkgonlyAsimov
   cd bkgonlyAsimov
   echo "  Performing fits to background-only Asimov"
   nohup combine ../model_${cate[icat]}.root -M MaxLikelihoodFit --minos=all --saveShapes --saveWithUncertainties --plots -v 3 -m 125 -t -1 --expectSignal 0 -n _shapes_bkgonlyAsimov > nohup_bkgonlyAsimov.out 2>&1&    

   cd ..
   mkdir bkgsigAsimov
   cd bkgsigAsimov
   echo "  Performing fits to background+signal Asimov"
   nohup combine ../model_${cate[icat]}.root -M MaxLikelihoodFit --minos=all --saveShapes --saveWithUncertainties --plots -v 3 -m 125 -t -1 --expectSignal 1 -n _shapes_bkgsigAsimov > nohup_bkgsigonlyAsimov.out 2>&1&
   
   cd ..
   
   cd ..
done
