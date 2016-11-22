import os
import sys
import time


import ROOT
ROOT.gROOT.SetBatch(True)

import pdb

import uuid

import matplotlib
from matplotlib import rc
if __name__== "__main__":
    matplotlib.use('PS')
import matplotlib.pyplot as plt

import numpy as np

import rootpy
import rootpy.io
from rootpy.plotting.root2matplotlib import errorbar, bar, hist, fill_between
from collections import OrderedDict

import pandas

import math

import matplotlib.patches as mpatches
import matplotlib.lines as mlines


matplotlib.rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})

matplotlib.rc("axes", labelsize=24)
matplotlib.rc("axes", titlesize=16)

#All the colors of the various processes
#extracted using the apple color picker tool
colors = {
    "ttbarOther": (251, 102, 102),
    "ttbarPlusCCbar": (204, 2, -0),
    "ttbarPlusB": (153, 51, 51),
    "ttbarPlusBBbar": (102, 0, 0),
    "ttbarPlus2B": (80, 0, 0),
    "ttH": (44, 62, 167),
    "ttH_hbb": (44, 62, 167),
    "ttH_nonhbb": (90, 115, 203),
    "diboson": (42, 100, 198),
    "wjets": (102, 201, 77),
    "zjets": (102, 201, 77),
    "singlet": (235, 73, 247),
    "ttv": (204, 204, 251),
    "qcd": (102, 201, 77),
    "qcd_ht300to500"   : (102, 201, 76),
    "qcd_ht300to500"   : (102, 201, 78),
    "qcd_ht500to700"   : (102, 201, 79),
    "qcd_ht700to1000"  : (102, 201, 80),
    "qcd_ht1000to1500" : (102, 201, 81),
    "qcd_ht1500to2000" : (102, 201, 82),
    "qcd_ht2000toinf"  : (102, 201, 83),
    "other": (251, 73, 255),

    "ttbarZ": (251, 177, 4),
    "ttbarW": (251, 177, 4),


}

components = {

}
#create floats of colors from 0..1
for cn, c in colors.items():
    colors[cn] = (c[0]/255.0, c[1]/255.0, c[2]/255.0)

#List of sample filenames -> short names suitable for latex
procs_names = [
    ("ttH_hbb", "tt+H(bb)"),
    ("ttH_nonhbb", "tt+H(nonbb)"),
    ("ttbarOther", "tt+l"),
    ("ttbarPlusBBbar", "tt+bb"),
    ("ttbarPlus2B", "tt+2b"),
    ("ttbarPlusB", "tt+b"),
    ("ttbarPlusCCbar", "tt+cc"),
    ("singlet", "single top"),
    ("diboson", "diboson"),
    ("wjets", "w+jets"),
    ("zjets", "z+jets"),
    ("ttbarZ", "tt+Z"),
    ("ttbarW", "tt+W"),
]




procs = [x[0] for x in procs_names]
signal_procs = ["ttH_hbb"]


#list of all variable names, suitable for latex
varnames = {
    "jetsByPt_0_pt": r"leading jet $p_T$ [GeV]",
    "jetsByPt_1_pt": r"subleading jet $p_T$ [GeV]",
    "jetsByPt_2_pt": r"third jet $p_T$ [GeV]",

    # "jet0_btagCSV": r"leading jet $b_{\mathrm{CSV}}$",
    # "jet1_btagCSV": r"subleading jet $b_{\mathrm{CSV}}$",

    # "jet0_btagBDT": r"leading jet $b_{\mathrm{cMVAv2}}$",
    # "jet1_btagBDT": r"subleading jet $b_{\mathrm{cMVAv2}}$",

    "jet0_eta": r"leading jet $\eta$",
    "jet1_eta": r"subleading jet $\eta$",
    "jet2_eta": r"third jet $\eta$",

    # "jet0_aeta": r"leading jet $|\eta|$",
    # "jet1_aeta": r"subleading jet $|\eta|$",

    "leps_0_pt": r"leading lepton $p_T$ [GeV]",
    # "lep1_pt": r"subleading jet $p_T$ [GeV]",

    # "lep0_eta": r"leading lepton $|\eta|$ [GeV]",
    # "lep1_eta": r"subleading jet $|\eta|$ [GeV]",

    "numJets": r"$N_{\mathrm{jets}}$",
    "numJets": r"$N_{\mathrm{CSVM}}$",

    "fatjetByPt_0_pt" : r"leading CA15 jet $p_T$ [GeV]", 
    "fatjetByPt_0_mass" : r"leading CA15 jet mass [GeV]", 

    "btag_LR_4b_2b_logit": r"$\log{[\mathcal{F} / (1 - \mathcal{F})]}$",
    "nfatjets": r"$N_{\mathcal{fatjets}}$",
    "topCandidate_pt": "top candidate $p_T$ [GeV]",
    "topCandidate_mass": "top candidate $M$ [GeV]",
    "topCandidate_masscal": "top candidate $M$ [GeV]",
    "topCandidate_fRec": r"top candidate $f_{\mathrm{rec}}$",
    "topCandidate_Ropt": r"top candidate $R_{\mathrm{opt}}$",
    "topCandidate_RoptCalc": r"top candidate $R_{\mathrm{opt}}, calc$",
    "topCandidate_n_subjettiness": r"top candidate n-subjettiness",

    "btag_LR_4b_2b_btagCSV_logit" : "BLR (CSV)",

    "multiclass_class" :  "Predicted class",
    "multiclass_proba_ttb" : "tt+b classifier output",
    "multiclass_proba_tt2b" : "tt+2b classifier output",
    "multiclass_proba_ttbb" : "tt+bb classifier output",
    "multiclass_proba_ttcc" : "tt+cc classifier output",
    "multiclass_proba_ttll" : "tt+light classifier output",

    "higgsCandidate_secondbtag_subjetfiltered" : "H candidate 2nd subjet b-tag",
    "higgsCandidate_bbtag" : "H candidate bb-tag",
    "higgsCandidate_tau1"  : r"H candidate $\tau_1$", 
    "higgsCandidate_tau2"  : r"H candidate $\tau_2$", 
    "higgsCandidate_mass" : "H candidate mass (ungroomed)", 
    "higgsCandidate_mass_softdropz2b1filt" : "H candidate mass (softdrop)",
    "higgsCandidate_sj12massb_subjetfiltered" : "H candidate mass (subjet-filtered)",
    "higgsCandidate_sj12masspt_subjetfiltered" : "H candidate mass (subjet-filtered)",         

    "nhiggsCandidate": "Number of higgs candidates",
    "higgsCandidate_pt": "H candidate $p_T$ [GeV]",
    "higgsCandidate_eta": "H candidate $\eta$",
    "higgsCandidate_mass": "H candidate $M$ [GeV]",
    "higgsCandidate_mass_pruned": "H candidate pruned $M$ [GeV]",
    "higgsCandidate_mass_softdrop": "H candidate softdrop $M$ [GeV]",
    "higgsCandidate_n_subjettiness": "H candidate n-subjettiness",
    "higgsCandidate_dr_top":  "$\\Delta R_{h,t}$",
    "numJets": "$N_{\\mathrm{jets}}$",
    "nBCSVM": "$N_{\\mathrm{CSVM}}$",
    "btag_LR_4b_2b_logit": "$\\log{\\mathcal{F} / (1 - \\mathcal{F})}$",
    "mem_SL_0w2h2t": "mem SL 0w2h2t",
    "mem_SL_2w2h2t": "mem SL 2w2h2t",
    "mem_SL_2w2h2t_sj": "mem SL 2w2h2t sj",
    "mem_DL_0w2h2t": "mem DL 0w2h2t",
    "mem_FH_4w2h2t": "mem FH 4w2h2t",
    "mem_FH_3w2h2t": "mem FH 3w2h2t",
    "mem_FH_4w2h1t": "mem FH 4w2h1t",
    "mem_FH_0w0w2h2t": "mem FH 0w0w2h2t",
    "mem_FH_0w0w2h1t": "mem FH 0w0w2h1t",
    "nPVs": "$N_{\\mathrm{PV}}$",
    "ntopCandidate": "$N_{\\mathrm{HTTv2}}$",
    "common_bdt": "BDT",
    "common_mem": "MEM",
    "Wmass": "$m_{qq}$"
}

#the units for variables
varunits = {
    "jetsByPt_0_pt": "GeV",
    "jetsByPt_1_pt": "GeV",
    "topCandidate_pt": "GeV",
    "topCandidate_mass": "GeV",
    "higgsCandidate_pt": "GeV",
    "higgsCandidate_mass": "GeV",
    "higgsCandidate_mass_pruned": "GeV",
    "higgsCandidate_mass_softdrop": "GeV",
}

def process_sample_hist(fnames, hname, func, bins, cut, **kwargs):
    """
    Takes a list of files and projects a 1D histogram with the specified cut.
    fnames (list of strings): list of filenames to be opened
    hname (string): name of the output histogram, must be unique
    func (string): the function (ROOT string) to be evaluated
    bins (3-tuple): the (nbins, low, high) of the histograms
    cut (string): the weight and cut string (ROOT format) to be evaluated.
    
    returns: TH1D in the gROOT directory
    """

    for ifn in range(len(fnames)):
        fn = fnames[ifn]
        if type(fn) is str:
            fnames[ifn] = (fnames[ifn], 1.0)
    
    hs = []
    for fn, xsw in fnames:
        tt = ROOT.TChain("tree")
        tt.Add(fn)
        ROOT.gROOT.cd()
        _hname = str(uuid.uuid4())
        ndims = len(bins)/3
        if ndims == 1:
            h = ROOT.TH1D(_hname, "", *bins)
        elif ndims == 2:
            h = ROOT.TH2D(_hname, "", *bins)
        h.Sumw2()
        h.SetDirectory(ROOT.gROOT)
        n = tt.Draw("{0} >> {1}".format(func, _hname), cut)
        h.Scale(xsw)
        h = rootpy.asrootpy(h)
        hs += [h]
    htot = sum(hs)
    htot.SetName(hname)
    return htot

def mc_stack(
    hlist,
    hs_syst,
    systematics,
    colors="auto"
    ):
    if colors=="auto":
        coloriter = iter(plt.cm.jet(np.linspace(0,1,len(hlist))))
        for h in hlist:
            h.color = next(coloriter)
    elif isinstance(colors, list) and len(colors) == len(hlist):
        for h, c in zip(hlist, colors):
            h.color = c

    for h in hlist:
        h.fillstyle = "solid"
    
    print hlist

    #FIXME: Temporary workaround for failed fill, only works when hatch is specified
    stack = hist(hlist, stacked=True, hatch=".", lw=2)
    htot = sum(hlist)
    htot.color="black"

    htot_u = rootpy.asrootpy(htot.Clone())
    htot_d = rootpy.asrootpy(htot.Clone())
    for i in range(1, htot.nbins()+1):
        htot_u.set_bin_content(i, htot.get_bin_content(i) + htot.get_bin_error(i))
        htot_d.set_bin_content(i, htot.get_bin_content(i) - htot.get_bin_error(i))

    htot_u.color="black"
    htot_d.color="black"

    #fill_between(htot_u, htot_d,
    #    color="black", hatch="////////",
    #    alpha=1.0, linewidth=0, facecolor="none", edgecolor="black", zorder=10,
    #)

    #add systematic uncertainties
    hstat = htot_u - htot_d
    errs = np.array([y for y in hstat.y()])
    errs = np.abs(errs)

    htot_usyst = htot.Clone()
    htot_dsyst = htot.Clone()
    for systUp, systDown in systematics:
        errs_syst_up = np.array([y for y in sum(hs_syst[systUp].values()).y()])
        errs_syst_down = np.array([y for y in sum(hs_syst[systDown].values()).y()])
        errs_syst = np.abs(errs_syst_up - errs_syst_down)
        errs = np.power(errs, 2) + np.power(errs_syst, 2)
        errs = np.sqrt(errs)
    for i in range(len(errs)):
        htot_usyst.SetBinContent(i+1, htot_usyst.GetBinContent(i+1) + errs[i]/2)
        htot_dsyst.SetBinContent(i+1, htot_dsyst.GetBinContent(i+1) - errs[i]/2)

    #fill_between(htot_usyst, htot_dsyst,
    #    color="gray", hatch=r"\\\\",
    #    alpha=1.0, linewidth=0, facecolor="none", edgecolor="gray", zorder=10,
    #)

    return {"hists":stack, "tot":htot, "tot_u":htot_u, "tot_d":htot_d, "tot_usyst":htot_usyst, "tot_dsyst":htot_dsyst}

def dice(h, nsigma=1.0):
    hret = h.clone()
    for i in range(1, h.nbins()+1):
        m, e = h.get_bin_content(i), h.get_bin_error(i)
        if e<=0:
            e = 1.0
        n = np.random.normal(m, nsigma*e)
        hret.set_bin_content(i, n)
    return hret

def make_uoflow(h):
    """
    Given a TH1 with bins [1...nbins], fill the underflow entries (bin 0) into the first bin and
    the overflow entries (nbins+1) into the last bin (nbins).
    """
    nb = h.GetNbinsX()
    #h.SetBinEntries(1, h.GetBinEntries(0) + h.GetBinEntries(1))
    #h.SetBinEntries(nb+1, h.GetBinEntries(nb) + h.GetBinEntries(nb + 1))
    h.SetBinContent(1, h.GetBinContent(0) + h.GetBinContent(1))
    h.SetBinContent(nb+1, h.GetBinContent(nb) + h.GetBinContent(nb + 1))
    h.SetBinError(1, math.sqrt(h.GetBinError(0)**2 + h.GetBinError(1)**2))
    h.SetBinError(nb+1, math.sqrt(h.GetBinError(nb)**2 + h.GetBinError(nb + 1)**2))

def zero_error(h):
    for i in range(1, h.GetNbinsX()+1):
        h.SetBinError(i, 0)

def fill_overflow(hist):
    """
    Puts the contents of the overflow bin in the last visible bin
    """
    nb = hist.GetNbinsX()
    o = hist.GetBinContent(nb + 1)
    oe = hist.GetBinError(nb + 1)
    hist.SetBinContent(nb, hist.GetBinContent(nb) + o)
    hist.SetBinError(nb, math.sqrt(hist.GetBinError(nb)**2 + oe**2))
    
    #fixme recalculate error
    hist.SetBinContent(nb+1, 0)
    hist.SetBinError(nb+1, 0)


def getHistogramsPrefit(tf, samples, hname, channel):

    hs = OrderedDict()

    for sample, sample_name in samples:

        try:

            # SL        
            # input
            #h = tf.get(sample + "_inputVar_" + channel + "_" + hname).Clone()
            # final disc
            #h = tf.get(sample + "_" + hname + "_" + channel).Clone()
            # DL
            h = tf.get(channel + "_" + hname+"/" + sample).Clone()

        except rootpy.io.file.DoesNotExist as e:
            continue
        hs[sample] = rootpy.asrootpy(h)

    print hs

    for sample, sample_name in samples:
        if not hs.has_key(sample):
            if len(hs)>0:
                hs[sample] = rootpy.asrootpy(0.0*hs.values()[0].Clone())
            else:
                return hs




    return hs



def getHistogramsPostfit(tf, samples, hname, channel, x_min, x_max):

    hs = OrderedDict()

    for sample, sample_name in samples:

        #print "NONE/shapes_fit_s/" + hname + "/" + sample

        print "NONE/shapes_fit_s/" + channel  + "_" + hname + "/" + sample

        try:
            # SL
            # input
            #h = tf.get( "NONE/shapes_fit_s/" + channel + "_" + hname + "/" + sample).Clone()
            # final disc
            #h = tf.get( "NONE/shapes_fit_s/"  + channel + "/" + sample).Clone()

            # DL 
            h = tf.get( "NONE/shapes_fit_s/"+channel+"/" + sample).Clone()
        except rootpy.io.file.DoesNotExist as e:
            continue

        h.GetXaxis().SetLimits(x_min,x_max)
        hs[sample] = rootpy.asrootpy(h)

    print "Post fit hs:",hs

    for sample, sample_name in samples:
        if not hs.has_key(sample):
            if len(hs)>0:
                hs[sample] = rootpy.asrootpy(0.0*hs.values()[0].Clone())
            else:
                return hs



    return hs


def escape_string(s):
    return s.replace("_", " ")

def draw_data_mc(tf_prefit,
                 tf_postfit,
                 channel,
                 hname, 
                 processes, 
                 signal_processes, 
                 **kwargs):

    # name (string) of the data process.
    # Example: "data" (real data), "data_obs" (fake data)
    #must exist in the input file
    dataname = kwargs.get("dataname", "data_obs")
    
    xlabel = kwargs.get("xlabel", escape_string(hname))
    xunit = kwargs.get("xunit", "")
    ylabel = kwargs.get("ylabel", "auto")
    rebin = kwargs.get("rebin", 1)
    title_extended = kwargs.get("title_extended", "")

    #legend properties
    do_legend = kwargs.get("do_legend", True)
    legend_loc = kwargs.get("legend_loc", (1.1,0.25))
    legend_fontsize = kwargs.get("legend_fontsize", 15)

    #Dictionary of sample (string) -> color (tuple of floats) to use as colors
    #or "auto" to generate a sequence of colors
    colors = kwargs.get("colors", "auto")

    #True if you want to put the contents of the overflow bin into the last
    #visible bin of the histogram, False otherwise
    show_overflow = kwargs.get("show_overflow", False)

    #function f: TH1D -> TH1D to apply on data to blind it.
    blindFunc = kwargs.get("blindFunc", None)

    #array of up-down pairs for systematic names to use for the systematic band,
    #e.g.[("_CMS_scale_jUp", "_CMS_scale_jDown")]
    systematics = kwargs.get("systematics", [])

    print "..."

    histograms_nominal_prefit  = getHistogramsPrefit(tf_prefit, processes, hname, channel)

    print histograms_nominal_prefit

    x_min = histograms_nominal_prefit["ttbarOther"].GetXaxis().GetXmin()
    x_max = histograms_nominal_prefit["ttbarOther"].GetXaxis().GetXmax()

    #"NONE/shapes_fit_s/s53_BDT_common5_input_all_sum_pt_with_met/ttbarOther
    histograms_nominal_postfit = getHistogramsPostfit(tf_postfit, processes, hname, channel, x_min, x_max)

    if len(histograms_nominal_prefit) == 0:
        raise KeyError("did not find any histograms for MC")

    histograms_systematic = OrderedDict()
    #get the systematically variated histograms
    for systUp, systDown in systematics:
        histograms_systematic[systUp] = getHistograms(tf, processes, hname+systUp)
        histograms_systematic[systDown] = getHistograms(tf, processes, hname+systDown)
        if len(histograms_systematic[systUp])==0 or len(histograms_systematic[systDown])==0:
            print "Could not read histograms for {0}".format(hname+systUp)

    processes_d = dict(processes)

    counts = {}
    

    for histo_dict in [histograms_nominal_prefit, histograms_nominal_postfit] + histograms_systematic.values():
        for (proc, h) in histo_dict.items():
            h.title = processes_d[proc] + " ({0:.1f})".format(h.Integral())
            counts[proc] = h.Integral()
            h.rebin(rebin)
            if show_overflow:
                fill_overflow(h)
            
    c = plt.figure(figsize=(6,10))

    #Create top panel
    a1 = plt.axes([0.0, 0.6, 1.0, 0.35])
        
    #c.suptitle(r"$\textbf{CMS}$ preliminary $\sqrt{s} = 13$ TeV"+title_extended,
    #    y=1.02, x=0.02,
    #    horizontalalignment="left", verticalalignment="bottom", fontsize=16
    #)
    stacked_hists = mc_stack(
        histograms_nominal_prefit.values(),
        histograms_systematic,
        systematics,
        colors = colors
    )
    

    histogram_total_mc = sum(histograms_nominal_prefit.values())
    histogram_total_mc_postfit = sum(histograms_nominal_postfit.values())

    #Create the normalized signal shape
    #histogram_signal = sum([histograms_nominal_prefit[sig] for sig in signal_processes])

    #hsig.Rebin(2)
    #if histogram_signal.Integral()>0:
    #    histogram_signal.Scale(0.2 * histogram_total_mc.Integral() / histogram_signal.Integral())
    #histogram_signal.title = processes[0][1] + " norm"
    #histogram_signal.linewidth=2
    #histogram_signal.fillstyle = None
    #draw the signal shape
    #hist([histogram_signal])
    
    histogram_total_mc.title = "pseudodata"
    histogram_total_mc.color = "black"

    histogram_total_bkg = sum([
        histograms_nominal_prefit[k] for k in histograms_nominal_prefit.keys()
        if k not in signal_processes]
    )
    
    #Get the data histogram
    data = None
    if dataname != "":


        # SL
        # input
        #data = tf_prefit.get(dataname + "_inputVar_" + channel + "_" + hname)
        # final disc
        #data = tf_prefit.get(dataname + "_" + hname + "_" + channel)
        # DL        
        data = tf_prefit.get(channel+"_"+hname+"/" + dataname)


        data.rebin(rebin)
        if blindFunc:
            data = blindFunc(data)
        if show_overflow:
            fill_overflow(data)
        data.title = "data ({0:.2f})".format(data.Integral())
        
        #set data error to 0 in case no data (FIXME) 
        for ibin in range(data.GetNbinsX()):
            if data.GetBinContent(ibin) == 0:
                data.SetBinError(ibin, 1)
        errorbar(data)

    if do_legend:
        #create nice filled legend patches for all the processes
        patches = []
        if data:
            dataline = mlines.Line2D([], [], color='black', marker='o', label=data.title)
            patches += [dataline]
        for (line1, line2), h in zip(stacked_hists["hists"], histograms_nominal_prefit.values()):
            patch = mpatches.Patch(color=line1.get_color(), label=h.title)
            patches += [patch]
        #patches += [mpatches.Patch(facecolor="none", edgecolor="black", label="stat", hatch="////////")]
        #patches += [mpatches.Patch(facecolor="none", edgecolor="gray", label="stat+syst", hatch=r"\\\\")]
        plt.legend(handles=patches, loc=legend_loc, numpoints=1, prop={'size':legend_fontsize}, ncol=2, frameon=False)
        
    #create an automatic bin width label on the y axis
    if ylabel == "auto":
        ylabel = "events" # / {0:.2f} {1}".format(histogram_signal.get_bin_width(1), xunit)
    plt.ylabel(ylabel)

    #hide x ticks on main panel
    ticks = a1.get_xticks()
    a1.get_xaxis().set_visible(False)
    
    a1.set_ylim(bottom=0, top=1.1*a1.get_ylim()[1])
    a1.grid(zorder=100000)

    plt.figtext(.73, .915, "Pre-Fit", fontsize=25)

    a3 = a1
    plt.figtext(.73, 0.515, "Post-Fit", fontsize=25)

    a3 = plt.axes([0.0,0.20, 1.0, 0.35], sharex=a1)
    stacked_hists = mc_stack(
        histograms_nominal_postfit.values(),
        histograms_systematic,
        systematics,
        colors = colors
    )


    #Get the data histogram
    data = None
    if dataname != "":
        # SL
        #input
        #data = tf_prefit.get(dataname + "_inputVar_" + channel + "_" + hname)
        # final disc
        #data = tf_prefit.get(dataname + "_" + hname + "_" + channel)
        # DL
        data = tf_prefit.get(channel+"_"+hname+"/" + dataname)

        data.rebin(rebin)
        if blindFunc:
            data = blindFunc(data)
        if show_overflow:
            fill_overflow(data)
        data.title = "data ({0:.2f})".format(data.Integral())
        
        #set data error to 0 in case no data (FIXME) 
        for ibin in range(data.GetNbinsX()):
            if data.GetBinContent(ibin) == 0:
                data.SetBinError(ibin, 1)
        errorbar(data)

    #create an automatic bin width label on the y axis
    if ylabel == "auto":
        ylabel = "events / {0:.2f} {1}".format(histogram_signal.get_bin_width(1), xunit)
    plt.ylabel(ylabel)

    if do_legend:
        #create nice filled legend patches for all the processes
        patches = []
        if data:
            dataline = mlines.Line2D([], [], color='black', marker='o', label=data.title)
            patches += [dataline]
        for (line1, line2), h in zip(stacked_hists["hists"], histograms_nominal_postfit.values()):
            patch = mpatches.Patch(color=line1.get_color(), label=h.title)
            patches += [patch]
        #patches += [mpatches.Patch(facecolor="none", edgecolor="black", label="stat", hatch="////////")]
        #patches += [mpatches.Patch(facecolor="none", edgecolor="gray", label="stat+syst", hatch=r"\\\\")]
        plt.legend(handles=patches, loc=legend_loc, numpoints=1, prop={'size':legend_fontsize}, ncol=2, frameon=False)



    ticks = a3.get_xticks()
    a3.get_xaxis().set_visible(False)
    a3.grid(zorder=100000)






    
    #do ratio panel
    if data:

        a2 = plt.axes([0.0,0.0, 1.0, 0.15], )

        a2.set_autoscale_on(False)
        a2.set_xlim(-0.7, 0.8)
        a2.set_xbound(-0.7, 0.8)

        plt.xlabel(xlabel)

        a2.grid()
        
        data_ratio = data.Clone()
        data_ratio.Divide(histogram_total_mc)
        
        data_ratio_postfit = data.Clone()
        data_ratio_postfit.Divide(histogram_total_mc_postfit)

        #In case MC was empty, set data/mc ratio to 0
        for ibin in range(data_ratio.GetNbinsX()):
            bc = histogram_total_mc.GetBinContent(ibin)
            if bc==0:
                data_ratio.SetBinContent(ibin, 0)

#        #create also the variated band
#        bg_unc_u = stacked_hists["tot_u"]
#        bg_unc_d = stacked_hists["tot_d"]
#
#        bg_unc_u.Divide(stacked_hists["tot"])
#        bg_unc_d.Divide(stacked_hists["tot"])
#
#        bg_unc_usyst = stacked_hists["tot_usyst"]
#        bg_unc_dsyst = stacked_hists["tot_dsyst"]
#
#        bg_unc_usyst.Divide(stacked_hists["tot"])
#        bg_unc_dsyst.Divide(stacked_hists["tot"])
        
        #blind the data also on the ratio
        if blindFunc:
            data_ratio = blindFunc(data_ratio)
        errorbar(data_ratio)
        errorbar(data_ratio_postfit, ecolor = 'red', c = 'red', markeredgecolor='red', color = 'red', markerfacecolor='red')


#        fill_between(
#            bg_unc_u, bg_unc_d,
#            color="black", hatch="////////",
#            alpha=1.0, linewidth=0, facecolor="none", edgecolor="black", zorder=10,
#        )
#
#        fill_between(
#            bg_unc_usyst, bg_unc_dsyst,
#            color="gray", hatch=r"\\\\",
#            alpha=1.0, linewidth=0, facecolor="none", edgecolor="gray", zorder=10,
#        )

#        plt.title("data={0:.1f} pre-fit={1:.1f} post-fit={1:.1f}".format(
#            data.Integral(),
#            histogram_total_mc.Integral(),
#            histogram_total_mc_postfit.Integral()
#            ), x=1.1, y=0.8, fontsize=14, horizontalalignment="left"
#        )

        plt.figtext(1.55, 0.095, "Data    = {0:.1f}".format(
            data.Integral(),
            ), fontsize=14, horizontalalignment="left"
        )

        plt.figtext(1.55, 0.07, "Pre-fit  = {0:.1f}".format(
            histogram_total_mc.Integral(),
            ), fontsize=14, horizontalalignment="left"
        )

        plt.figtext(1.55, 0.045, "Post-fit = {0:.1f}".format(
            histogram_total_mc_postfit.Integral()
            ), fontsize=14, horizontalalignment="left"
        )




        plt.ylabel(r"$\frac{\mathrm{data}}{\mathrm{pred.}}$", fontsize=32)
        plt.axhline(1.0, color="black")
        a2.set_ylim(0.5, 1.5)
        #hide last tick on ratio y axes
        a2.set_yticks(a2.get_yticks()[:-1])
        #a2.set_xticks(0)

        ticks = a2.get_xticks()
        #a2.get_xaxis().set_visible(False)

        if do_legend:
            #create nice filled legend patches for all the processes
            line_prefit = mlines.Line2D([], [], color='black', marker='o', label="data/pre-fit")
            line_postfit = mlines.Line2D([], [], color='red', marker='o', label="data/post-fit")
            patches = [line_prefit, line_postfit]

            plt.legend(handles=patches, loc=legend_loc, numpoints=1, prop={'size':legend_fontsize}, ncol=1, frameon=False)


  


def draw_mem_data_mc(*args, **kwargs):
    a1, a2, hs = draw_data_mc(*args, **kwargs)
    plt.sca(a1)
    h = hs["tth_13TeV_phys14"].Clone()
    h.fillstyle = "hollow"
    h.linewidth = 2
    h.title = h.title + " x10"
    h.Scale(10)
    hist(h)
    plt.legend(loc=(1.01,0.4))
    a1.set_ylim(bottom=0)
    return a1, a2, hs

def draw_shape(f, samples, hn, **kwargs):
    rebin = kwargs.get("rebin", 1)

    hs = []
    for s in samples:
        h = f.get(s[0] + hn).Clone()
        h.Scale(1.0 / h.Integral())
        h.rebin(rebin)
        h.title = s[1]
        hs += [h]

    coloriter = iter(plt.cm.jet(np.linspace(0,1,len(hs))))

    for h in hs:
        h.color = next(coloriter)
        errorbar(h)
    plt.legend()
    for h in hs:
        hist(h, lw=1, ls="-")

def svfg(fn):
    path = os.path.dirname(fn)
    if not os.path.exists(path):
        os.makedirs(path)
        time.sleep(2) #for NFS
    plt.savefig(fn, pad_inches=0.5, bbox_inches='tight')
    #plt.clf()





#temporarily disable true latex for fast testing
rc('text', usetex=False)
matplotlib.use('PS') #needed on T3

                    
# combine -M MaxLikelihoodFit -t -1 --expectSignal 0 --minimizerStrategy 0 --minimizerTolerance 0.1 slBDTonlyV11_datacard_ljets_j4_t3_hdecay.txt

#/mnt/t3nfs01/data01/shome/gregor/CMSSW_FOR_COMBINE/CMSSW_7_4_7/src/bianchini/TTHStudies/bin

inf_prefit  = rootpy.io.File(sys.argv[1])
inf_postfit = rootpy.io.File(sys.argv[2])
        
channel = sys.argv[3]
histname = sys.argv[4]
suffix = sys.argv[5]

outname = channel + histname + "_" + suffix

do_syst = False

#if kwargs.has_key("blindFunc"):
#    blind = kwargs.pop("blindFunc")
#    if blind_funcs.has_key(blind):
#        kwargs["blindFunc"] = blind_funcs[blind]

plt.clf()

fig = plt.figure(figsize=(6,6))
draw_data_mc(
    inf_prefit,
    inf_postfit,
    channel,
    histname,
    procs_names,
    signal_procs,
    colors = [colors.get(p) for p in procs],
)


print "saving {0}".format(outname)
svfg("output/" + outname + ".pdf")
svfg("output/" + outname + ".png")
plt.clf()

