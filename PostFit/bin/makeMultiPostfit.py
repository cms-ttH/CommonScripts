import os, sys
import subprocess

from EnvForCombine import PATH, LD_LIBRARY_PATH, PYTHONPATH, GENREFLEX, ROOTSYS, ROOT_INCLUDE_PATH

# SL

datacard_path = "/mnt/t3nfs01/data01/shome/gregor/datacardsV12_2/datacardsV12/ttHbb_13TeV_HIG-16-038"
config_base = "makePostFitPlotsFWlite_BASE.py"


to_plot = [

    # 44 
    #"BDT_common5_input_all_sum_pt_with_met",
    #"BDT_common5_input_aplanarity",
    #"BDT_common5_input_avg_btag_disc_btags",
    #"BDT_common5_input_closest_tagged_dijet_mass",
    #"BDT_common5_input_h3",
    #"BDT_common5_input_pt_all_jets_over_E_all_jets",

    # 54
    #"BDT_common5_input_HT",
    #"BDT_common5_input_M3",
    #"BDT_common5_input_all_sum_pt_with_met",
    #"BDT_common5_input_aplanarity",
    #"BDT_common5_input_avg_btag_disc_btags",
    #"BDT_common5_input_avg_dr_tagged_jets",
    #"BDT_common5_input_closest_tagged_dijet_mass",
    #"BDT_common5_input_tagged_dijet_mass_closest_to_125",
    #"Evt_Deta_JetsAverage",

    # 64    
    #"BDT_common5_input_all_sum_pt_with_met",
    #"BDT_common5_input_best_higgs_mass",
    #"BDT_common5_input_fifth_highest_CSV",
    #"BDT_common5_input_fourth_highest_btag",
    #"BDT_common5_input_invariant_mass_of_everything",
    #"BDT_common5_input_tagged_dijet_mass_closest_to_125",

    # 63
    #"BDT_common5_input_all_sum_pt_with_met",
    #"BDT_common5_input_aplanarity",
    #"BDT_common5_input_dEta_fn",
    #"BDT_common5_input_dev_from_avg_disc_btags",
    #"BDT_common5_input_min_dr_tagged_jets",
    #"BDT_common5_input_pt_all_jets_over_E_all_jets",
    #"blr",

    # DL 
    # dl_ge4j3t
    #"datacards_mass_tag_tag_min_deltaR_step7_cate3",
    #"datacards_twist_tag_tag_max_mass_step7_cate3",

    # dl_ge4jge4t
    #"datacards_mass_tag_tag_min_deltaR_step7_cate4",
    #"datacards_maxDeltaEta_tag_tag_step7_cate4",
 
    # dl_3j3t
    #"datacards_maxDeltaEta_jet_jet_step7_cate1",
    #"datacards_ptSum_jets_leptons_step7_cate1",
    
    "BDT"
    

]

# unblinded_postfit_inputvariables_datacards/datacards_twist_tag_tag_max_mass_step7_cate3/combined/ttH_hbb_13TeV_dl_ge4j3t.txt

# DL
#datacard_path = "/mnt/t3nfs01/data01/shome/gregor/postFitInputVariables"
#config_base = "makePostFitPlotsFWlite_BASE.py"
#initial_root = "/mnt/t3nfs01/data01/shome/gregor/postFitInputVariables/datacards_mvaEventP_maxDeltaEta_tag_tag_step7_cate0/combined/common/ttH_hbb_13TeV_dl.root"
#channel = "dl_j3_t2"
#
#to_plot = [
#    #"BDT",
#    #"datacards_mvaEventP_mass_jet_jet_min_deltaR_step7_cate2"
#    #"datacards_mvaEventP_ptSum_jets_leptons_step7_cate2"
#    #"datacards_mvaEventP_ptSum_jets_leptons_step7_cate0"
#    #"datacards_mvaEventP_maxDeltaEta_tag_tag_step7_cate0"
#
#]




for var in to_plot:
    for channel in ["dl_gej4_get4_high"]:

        print "Doing", var

        # SL
        # input
        #datacard_name = "inputs2DV12_datacard_{0}_{1}_hdecay.txt".format(channel, var)
        # final disc
        #datacard_name = "ttH_hbb_13TeV_{0}.txt".format(channel)

        # DL - input
        #datacard_name = "{0}/combined/ttH_hbb_13TeV_{1}.txt".format(var, channel)
        # DL - finaldiscr
        datacard_name = "ttH_hbb_13TeV_dl_ge4jge4t_high.txt"

        output_name = "postfit_{0}.root".format(var)
        config_name = "config_{0}.py".format(var)

        inf = open(config_base, "r")
        of = open(config_name, "w")

        for line in inf:        
            line = line.replace("REPLACE_DATACARD", os.path.join(datacard_path,datacard_name))    
            # Don't forget to update mlfit 
            line = line.replace("REPLACE_MLFIT", "/mnt/t3nfs01/data01/shome/gregor/datacardsV12_2/datacardsV12/ttHbb_13TeV_HIG-16-038/mlfit.root")            
            line = line.replace("REPLACE_OUTPUT", output_name)
            of.write(line)
        of.close()

        try:
            os.remove(output_name)
        except OSError:
            pass

        subprocess.Popen(["MakePostFitPlotsFWlite", config_name]).communicate()

        # DL - inputs
        #initial_root = "/mnt/t3nfs01/data01/shome/gregor/step7_desy/unblinded_postfit_inputvariables_datacards/{0}/combined/common/ttH_hbb_13TeV_dl.root".format(var)
        
        # DL - infaldisct
        initial_root = "/mnt/t3nfs01/data01/shome/gregor/datacardsV12_2/datacardsV12/ttHbb_13TeV_HIG-16-038/common/ttH_hbb_13TeV_dl.root"


        # SOME SL (FINALDISCR)
        #channel = channel.replace("sl","ljets")
        
        # DL
        #channel = channel.replace("dl_3j3t","dl_j3_t3")

        args = ["python", "plots.py", initial_root, output_name, channel, "BDT", var]

        process = subprocess.Popen(args,
                                   stdout=subprocess.PIPE,                               
                                   env=dict(os.environ, 
                                            PATH=PATH,
                                            LD_LIBRARY_PATH = LD_LIBRARY_PATH,
                                            PYTHONPATH=PYTHONPATH,
                                            ROOT_INCLUDE_PATH = ROOT_INCLUDE_PATH,
                                            ROOTSYS = ROOTSYS,
                                            GENREFLEX = GENREFLEX
                                        ))
        out =  process.communicate()
        print out[0]

        print out[1]

        

        
