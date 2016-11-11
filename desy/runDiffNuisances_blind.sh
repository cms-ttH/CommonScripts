datacards=$1
cate=(dl_3j3t dl_ge4j3t_low dl_ge4j3t_high dl_ge4jge4t_low dl_ge4jge4t_high dl_merged)

let loop_cat=${#cate[@]}-1
for (( icat = 0; icat <= $loop_cat ; icat++ ))
do
   cd fits_${cate[icat]}
   
   cd bkgonlyAsimov
   echo "  Plotting nuisances for background-only Asimov"
   python ../../../../diffNuisances.py -a mlfit_shapes_bkgonlyAsimov.root -g diffNuisances_FitToBkgAsimov_plots_${cate[icat]}.root
   cd ..
   
   cd bkgsigAsimov
   echo "  Plotting nuisances for background+signal Asimov"
   python ../../../../diffNuisances.py -a mlfit_shapes_bkgsigAsimov.root -g diffNuisances_FitToBkgsigAsimov_plots_${cate[icat]}.root
   cd ..
   
   cd ..
done
