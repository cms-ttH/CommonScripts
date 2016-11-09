#####

full unblinding procedure and corresponding combine commands

#Step 1: Nuisance pulls in b-only fit per category and categories combined per channel
      1.1 per category
      combine -M MaxLikelihoodFit -d card --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin -5 --rMax 5 --minos all
      1.2 SL or DL combined
      combine -M MaxLikelihoodFit -d card --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin -10 --rMax 10 --minos all

      script to make Nuisance Parameter correlation plots?
      script to make b-only fit pull plots? diffNuisancesOnlyB.py?

#Step 2: Nuisance pulls in s+b fit per category and categories combined per channel
      2.1 per category
      combine -M MaxLikelihoodFit -d card --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin -10 --rMax 10 --minos all
      2.2 SL or DL combined
      combine -M MaxLikelihoodFit -d card --minimizerStrategy 0 --minimizerTolerance 0.001 --rMin -10 --rMax 10 --minos all

#Step 3: Pre-fit and post-fit (s+b) discriminant plots (combined fit per channel)
      script to make these plots based on the mlfit.root?      

#Step 4: Limits per category and categories combined per channel
      Shell script to run the limits per category and per channel?

      scripts to make limit table/plot?

#Step 5: Background-only fit (DL+SL combined fit)
      script to make shape comparison plots for any uncertainty?
      same commands as in step 1

#Step 6: Signal+background fit (DL+SL combined fit)
      same commands as in step 2

#Step 7: Post-fit plots for relevant input variables (DL+SL combined fit)
      script from Gregor

#Step 8: Pre-fit and post-fit discriminator plots (DL+SL combined s+b fit)
      same commands as in step 3
      
#Step 9: Limits (DL+SL combined fit)
      same commands as in step 4

#Step10: Best-fit value of signal strength mu (DL+SL combined fit)
	 command?
	 script to make the table and plot for best-fit mu	 	 
