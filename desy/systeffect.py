## uses the limits files which are output of runRemovingSyst.sh and produces a ranked table of effects on the limit when a systematic is removed

## run as python systeffect.py


# for command line options
from optparse import OptionParser

# regular expressions
import re

# working with numbers
import math
from math import sqrt

# import system to terminate process
import sys

# import pathname module
import os,os.path

# miscellaneous
from array import array

###################
## OPTION PARSER ##
###################

# Parse the options
optParser = OptionParser()

#optParser.add_option("-d", "--bayesianlimdir", dest="bayesianlimdir",default="/user/gvonsem/VectorLikeQuarkSearch/LimitSetting/Theta/theta/utils2/examples/VLQ/scan/Bayesian_PBS/Results/",
#                  help="Specify the directory with the CLs log files", metavar="")
#optParser.add_option("-t", "--tagname", dest="tagname",
#                  help="Specify the tagname you used in the submitOnPBS.py script to produce the CLs limits", metavar="")
#optParser.add_option("-w", "--writedir", dest="writedir",
#                  help="Specify the directory in which you want to store the limits", metavar="")									
#		
#										 								 														
#(options, args) = optParser.parse_args()
#
#if options.tagname == None:
#		optParser.error("Please specify a tagname you used in the submitOnPBS.py script to produce the CLs limits\n")



##################
## MAIN ROUTINE ##
##################

printsortedlist = True;
printlatex = True;
systematics = ["CMS_res_j","CMS_scale_j","CMS_ttH_CSVCErr1","CMS_ttH_CSVCErr2","CMS_ttH_CSVHF","CMS_ttH_CSVHFStats1","CMS_ttH_CSVHFStats2","CMS_ttH_CSVLF","CMS_ttH_CSVLFStats1","CMS_ttH_CSVLFStats2","CMS_ttH_QCDscale_ttbarPlus2B","CMS_ttH_QCDscale_ttbarPlusB","CMS_ttH_QCDscale_ttbarPlusBBbar","CMS_ttH_QCDscale_ttbarPlusCCbar","QCDscale_ttH","QCDscale_ttbar",
"lumi_13TeV","pdf_gg","pdf_gg_ttH","pdf_qg","pdf_qqbar","CMS_ttH_PU","CMS_ttH_dl_Trig","CMS_ttH_PSscale_ttbarOther","CMS_ttH_PSscale_ttbarPlus2B","CMS_ttH_PSscale_ttbarPlusB", 
"CMS_ttH_PSscale_ttbarPlusBBbar","CMS_ttH_PSscale_ttbarPlusCCbar","CMS_ttH_Q2scale_ttbarOther","CMS_ttH_Q2scale_ttbarPlus2B","CMS_ttH_Q2scale_ttbarPlusB", 
"CMS_ttH_Q2scale_ttbarPlusBBbar","CMS_ttH_Q2scale_ttbarPlusCCbar","QCDscale_V","QCDscale_VV","QCDscale_singlet"]
#systematics_latex = ["CMS\_res\_j","CMS\_scale\_j","CMS\_ttH\_CSVCErr1","CMS\_ttH\_CSVCErr2","CMS\_ttH\_CSVHF","CMS\_ttH\_CSVHFStats1","CMS\_ttH\_CSVHFStats2","CMS\_ttH\_CSVLF","CMS\_ttH\_CSVLFStats1","CMS\_ttH\_CSVLFStats2",
#"CMS\_ttH\_QCDscale\_ttbarPlus2B","CMS\_ttH\_QCDscale\_ttbarPlusB","CMS\_ttH\_QCDscale\_ttbarPlusBBbar","CMS\_ttH\_QCDscale\_ttbarPlusCCbar","QCDscale\_ttH","QCDscale\_ttbar",
#"lumi\_13TeV","pdf\_gg","pdf\_gg\_ttH","pdf\_qg","pdf\_qqbar","CMS\_ttH\_PU","CMS\_ttH\_dl\_Trig","CMS\_ttH\_Q2scale\_ttbarOther","CMS\_ttH\_Q2scale\_ttbarPlus2B","CMS\_ttH\_Q2scale\_ttbarPlusB", 
#"CMS\_ttH\_Q2scale\_ttbarPlusBBbar","CMS\_ttH\_Q2scale\_ttbarPlusCCbar","QCDscale\_V","QCDscale\_VV","QCDscale\_singlet"]
systematics_latex = ["Jet energy resolution","Jet energy scale","$b$-Tag Charm Uncertainty (linear)","$b$-Tag Charm Uncertainty (quadratic)","$b$-Tag HF contamination","$b$-Tag HF stats (linear)","$b$-Tag HF stats (quadratic)",
"$b$-Tag LF contamination","$b$-Tag LF stats (linear)","$b$-Tag HF stats (quadratic)","QCDscale ($t\\bar{t}+2b$)","QCDscale ($t\\bar{t}+b$)","QCDscale ($t\\bar{t}+b\\bar{b}$)","QCDscale ($t\\bar{t}+c\\bar{c}$)","QCDscale ttH","QCDscale $t\\bar{t}$",
"Integrated luminosity","pdf ($gg$)","pdf ttH ($gg$)","pdf ($qg$)","pdf ($q\\bar{q}$)","Pileup","Dilepton trigger",
"PS Scale ($t\\bar{t}$+lf)","PS Scale ($t\\bar{t}+2b$)","PS Scale ($t\\bar{t}+b$)", 
"PS Scale ($t\\bar{t}+b\\bar{b}$)","PS Scale ($t\\bar{t}+c\\bar{c}$)",
"Q2 Scale ($t\\bar{t}$+lf)","Q2 Scale ($t\\bar{t}+2b$)","Q2 Scale ($t\\bar{t}+b$)", 
"Q2 Scale ($t\\bar{t}+b\\bar{b}$)","Q2 Scale ($t\\bar{t}+c\\bar{c}$)","QCDscale V","QCDscale VV","QCDscale single top"]


limitfilename_base = "ttH_hbb_13TeV_systeffect_blind_dl_merged"
#limitfilename_base = "ttH_hbb_13TeV_systeffect_unblind_dl_merged"
limitfilename_allsyst= limitfilename_base+".lmt"

expected = True
observed = False

searchstring = "Expected 50.0%"
if expected:
   searchstring = "Expected 50.0%"
if observed:
   searchstring = "Observed Limit"


limitfilenames_systremoved = []
counter = 0
for systi in systematics:
   limitfilenames_systremoved.append(limitfilename_base+"_no"+systi+".lmt")
   #print limitfilenames_systremoved[counter]
   counter = counter + 1


limitfile_allsyst = open(limitfilename_allsyst,'r')
limit_allsyst = -999
print "With all systematics"
for line in limitfile_allsyst:
   if not line.rfind(searchstring) == -1:
      linesplitted = line.split()
      limit_allsyst = linesplitted[4]
      print "   "+searchstring+": "+limit_allsyst


limittuples = []
effecttuples = []

counter = 0
for ifile in limitfilenames_systremoved:
   #print "removing "+systematics[counter]
   for line in open(ifile,'r'):
      if not line.rfind(searchstring) == -1:
         linesplitted = line.split()
	 limit_systremoved = linesplitted[4]
         #print "   Expected 50.0%: "+limit_systremoved
	 limittuple = (systematics_latex[counter],float(limit_systremoved))
	 limittuples.append(limittuple)
	 effect_systremoved = round(100 * (float(limit_allsyst) - float(limit_systremoved)) / float(limit_allsyst), 2)
	 effecttuple = (systematics_latex[counter],effect_systremoved)
	 #print effecttuple
	 effecttuples.append(effecttuple)
   counter = counter + 1


effecttuples.sort(key=lambda effect: effect[1], reverse=True) 

if printsortedlist:
   print ""
   print "SORTED LIST (systematic, effect in %)"
   for mytuple in effecttuples:
      print mytuple


if printlatex:
   print ""
   print "LATEX TABLE"
   #from AN-15-216

   print "\\begin{table}[hbtp] \\small"
   if expected:
      #print "\\caption{The improvement in the median expected limit when removing exactly one systematic uncertainty for the analysis with all dilepton categories combined. There are "+str(len(effecttuples))+" systematic uncertainties in total. Each systematic uncertainty listed is independent from the others.}"
      print "\\caption{The improvement in the median expected limit when removing exactly one systematic uncertainty for the analysis with all dilepton categories combined. Listed are all uncertainties with a non-zero influence. Each systematic uncertainty listed is independent from the others.}"
      print "\\label{tab:systRemoveOne_dilep_exp}"
   if observed:
      print "\\caption{The change in the observed limit when removing exactly one systematic uncertainty for the analysis with all dilepton categories combined. Listed are all uncertainties with a non-zero influence. Each systematic uncertainty listed is independent from the others.}"
      print "\\label{tab:systRemoveOne_dilep_obs}"   
   print "\\centering"
   print "\\begin{tabular}{lc}"
   print "\\hline"
   print "\\hline"
   if expected:
      print "Removed uncertainty   & Improvement in the expected limit (\\%) \\\\"
   if observed:  
      print "Removed uncertainty   & Change in the limit (\\%) \\\\"
   for mytuple in effecttuples:
      print mytuple[0] + " & " + str(mytuple[1]) + " \\\\"
   print "\\hline\\hline"
   print "\\end{tabular}"  
   print "\\end{table}"




