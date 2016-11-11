datacards=$1
#cate=(3j2t 3j3t ge4j2t ge4j3t_low ge4j3t_high ge4jge4t_low ge4jge4t_high merged)
cate=(dl_3j3t dl_ge4j3t_low dl_ge4j3t_high dl_ge4jge4t_low dl_ge4jge4t_high dl_merged)

let loop_cat=${#cate[@]}-1
for (( icat = 0; icat <= $loop_cat ; icat++ ))
do
   cd fits_${cate[icat]}
   
   cd unblind
   echo "  Plotting nuisances for fit to observed data"
   python ../../../../diffNuisances_Bfit.py -a mlfit_shapes_unblind.root -g diffNuisances_FitToData_Bfit_plots_${cate[icat]}.root
   python ../../../../diffNuisances.py -a mlfit_shapes_unblind.root -g diffNuisances_FitToData_plots_${cate[icat]}.root
   cd ..
   
   cd ..
done
