from __future__ import print_function

import ROOT
ROOT.gROOT.SetBatch(True)

from collections import OrderedDict
import numpy as np
import math
import os

import rootpy
import rootpy.io

import matplotlib
matplotlib.use("agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.ticker import MultipleLocator, FormatStrFormatter
import rootpy.plotting.root2matplotlib as rplt

from sparse import save_hdict
from utils import bins_to_category, reduce_dict, make_hist, make_hist_bins
from representations import KIT_SL_v13_DatacardRepresentation, DESY_DL_v13_DatacardRepresentation, CombineRepresentation

from matplotlib import rc
rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
rc('text', usetex=True)
#matplotlib.rcParams['text.latex.preamble']=[r'\usepackage{mathastext}']

def svfg(fn, **kwargs):
    path = os.path.dirname(fn)
    if not os.path.exists(path):
        os.makedirs(path)
        time.sleep(2) #for NFS
    plt.savefig(fn, pad_inches=0.5, bbox_inches='tight', **kwargs)

if __name__ == "__main__":

    path = "~/tth/datacards/v13/"

    dcard_repr = CombineRepresentation()
    hists = dcard_repr.get_representation(path + "mlfitNamesShapes.root")

    dcard_repr_sl = KIT_SL_v13_DatacardRepresentation()
    nd_tot = OrderedDict()
    nd = dcard_repr_sl.get_representation(
        path + "/common/ttH_hbb_13TeV_sl.root"
    )
    nd_tot.update(nd)

    dcard_repr_dl = DESY_DL_v13_DatacardRepresentation()
    nd = dcard_repr_dl.get_representation(
        path + "/common/ttH_hbb_13TeV_dl.root"
    )
    nd_tot.update(nd)

    #map categories in mlfit to categories in datacards
    category_mapper = {
        'dl_3j3t': 'dl_j3_t3_BDT',
        'dl_ge4j3t_high': 'dl_gej4_t3_high_BDT',
        'dl_ge4j3t_low': 'dl_gej4_t3_low_BDT',
        'dl_ge4jge4t_high': 'dl_gej4_get4_high_BDT',
        'dl_ge4jge4t_low': 'dl_gej4_get4_low_BDT',
        'sl_j4_t4_high': 'finaldiscr_ljets_j4_t4_high',
        'sl_j4_t4_low': 'finaldiscr_ljets_j4_t4_low',
        'sl_j5_tge4_high': 'finaldiscr_ljets_j5_tge4_high',
        'sl_j5_tge4_low': 'finaldiscr_ljets_j5_tge4_low',
        'sl_jge6_t3_high': 'finaldiscr_ljets_jge6_t3_high',
        'sl_jge6_t3_low': 'finaldiscr_ljets_jge6_t3_low',
        'sl_jge6_tge4_high': 'finaldiscr_ljets_jge6_tge4_high',
        'sl_jge6_tge4_low': 'finaldiscr_ljets_jge6_tge4_low',
    }

    histo_binning = np.array([
        -2.4,
        -2.2,
        -2.0,
        -1.8,
        -1.6,
        -1.4,
        -1.2,
        -1.0,
        -0.8,
        -0.6,
        -0.4,
        -0.2
    ])

    for cat in hists.keys():
        hists[cat]["data_obs"] = OrderedDict()
        hists[cat]["data_obs"]["shapes_prefit"] = nd_tot[category_mapper[cat]]["data_obs"]["nominal"]
        for samp in hists[cat].keys():
            for syst in hists[cat][samp].keys():
                old_d = hists[cat][samp][syst]
                new_d = OrderedDict()
                for ibin in range(1, len(hists[cat]["data_obs"]["shapes_prefit"]) + 1):
                    label = "bin_{0}".format(ibin)
                    new_d[label] = old_d[label]
                hists[cat][samp][syst] = new_d

    bins = bins_to_category(hists)
    bins_sob = reduce_dict(
        bins,
        dcard_repr.calculate_signal_over_background
    )
    bins_sorted = sorted(bins_sob.keys(), key=lambda x: bins_sob[x], reverse=False)
    sob_data = [bins_sob[b] for b in bins_sorted]
    print("Best bins by SoB are")
    for bs in bins_sorted[-10:]:
        print(" {0} {1:.4f}".format(bs, bins_sob[bs]))

    nd3 = OrderedDict()
    for samp in ["total_signal", "total_background", "data_obs"]:
        nd3[samp] = OrderedDict()
        for syst in ["shapes_prefit", "shapes_fit_s", "shapes_fit_b"]:
            nd3[samp][syst] = OrderedDict()
            if not hists[cat][samp].has_key(syst):
                continue
            for cat, b in bins_sorted:
                nd3[samp][syst][(cat, b)] = hists[cat][samp][syst][b]

    #convert bins to histogram with name={sample}_sob_{systematic}
    hists = OrderedDict()
    for samp in nd3.keys():
        for syst in nd3[samp].keys():
            name = "_".join([samp, "sob", syst])
            if len(nd3[samp][syst]) > 0:
                name = name.replace("_nominal", "")
                hists[name] = make_hist(
                    name,
                    nd3[samp][syst],
                    sob_data,
                    histo_binning
                )
    # for samp in nd3.keys():
    #     for syst in nd3[samp].keys():
    #         name = "_".join([samp, "bins", syst])
    #         if len(nd3[samp][syst]) > 0:
    #             name = name.replace("_nominal", "")
    #             hists[name] = make_hist_bins(
    #                 name,
    #                 nd3[samp][syst],
    #                 sob_data,
    #             )

    save_hdict("sob.root", hists)

    #Now the plotting part
    tf = rootpy.io.File("sob.root")
    
    h_bkg_s = tf.Get("total_background_sob_shapes_fit_s")
    h_bkg_b = tf.Get("total_background_sob_shapes_fit_b")
    h_sig_s = tf.Get("total_signal_sob_shapes_fit_s")
    h_sig_p = tf.Get("total_signal_sob_shapes_prefit")
    
    h_data = tf.Get("data_obs_sob_shapes_prefit")
    
    rplt.hist(h_bkg_s)
    rplt.errorbar(h_data)

    color_signal = (40.0/255.0, 154.0/255.0, 249.0/255.0)

    plt.figure(figsize=(4, 4))
    
    a1 = plt.axes([0.0, 0.20, 1.0, 0.8])
    plot_data = rplt.errorbar(h_data, marker="o", color="black", label="data", capsize=0)
    
    #rplt.hist(hs2["bkg"] + hs2["sig"], fill=True, color="blue", lw=0);
    plot_bkg = rplt.hist(h_bkg_s, lw=2, linecolor="black", label="Background");
    h_bkg_b.linestyle = "dashed"
    #plot_bkg2 = rplt.hist(h_bkg_b, lw=2, linecolor="black", label="Bkgd. $(\mu = 0)$ fit");
    
    nb = h_bkg_s.GetNbinsX()
    xs = np.array([h_bkg_s.GetBinLowEdge(i) for i in range(1, nb+1)])
    ws = np.array([h_bkg_s.GetBinWidth(i) for i in range(1, nb+1)])
    ys_bottom = np.array([h_bkg_s.GetBinContent(i) for i in range(1, nb+1)])
    ys_sig = np.array([h_sig_s.GetBinContent(i) for i in range(1, nb + 1)])
    ys_sig_prefit = np.array([h_sig_p.GetBinContent(i) for i in range(1, nb + 1)])
    
    mu_upperlimit = 1.5
    mu_bestfit = -0.19
    plot_sig1 = plt.bar(xs, ys_sig_prefit, width=ws, bottom=ys_bottom, color="red", fill=True, lw=1, edgecolor="red",
                        label=r"$\mathrm{t\bar{t}H}\ (\mu_{\mathrm{SM}} = 1.0)$")
    plot_sig2 = plt.bar(xs, ys_sig, width=ws, bottom=ys_bottom, color=color_signal, fill=True, lw=1, edgecolor=color_signal,
                        label=r"$\mathrm{t\bar{t}H}\ (\mu = %.2f)$" % mu_bestfit)
    
    a1.set_yscale("log")
    a1.set_ylim(1, 1.2*10**6)
    plt.title(
        r"\textbf{CMS} \textit{Preliminary}",
        fontsize=16, x=0.05, ha="left", y=0.95, va="top", fontname="Helvetica"
    )
    plt.text(0.99, 1.00,
        "$11.4 - 12.9\ \mathrm{fb}^{-1}\ \mathrm{(13\ TeV)}$",
        fontsize=14, ha="right", va="bottom", transform=a1.transAxes, fontname="Helvetica"
    )
    plt.text(0.05, 0.85,
        "lepton+jets, dilepton\ncombined",
        fontname="Helvetica", fontsize=12, ha="left", va="top", transform=a1.transAxes
    )
    
    htot_usyst = h_bkg_s.Clone()
    htot_dsyst = h_bkg_s.Clone()
    for ibin in range(1, h_bkg_s.GetNbinsX() + 1):
        htot_usyst.SetBinContent(ibin, h_bkg_s.GetBinContent(ibin) + h_bkg_s.GetBinErrorUp(ibin))
        htot_dsyst.SetBinContent(ibin, h_bkg_s.GetBinContent(ibin) - h_bkg_s.GetBinErrorLow(ibin))
    
    rplt.fill_between(htot_usyst, htot_dsyst,
        hatch=r"////////",
        alpha=1.0, linewidth=0, facecolor="none", edgecolor="black", zorder=10, label=r"Bkgd. Unc"
    )
    plot_unc = mpatches.Patch(facecolor="none", edgecolor="black", label="Bkgd. Unc", hatch="////////")
    plt.legend(loc=1, handles = [plot_data, plot_sig1, plot_sig2, plot_bkg, plot_unc], numpoints = 1, frameon=False)
    
    a2 = plt.axes([0.0,0.0, 1.0, 0.20], sharex=a1)
    
    ratio_data = h_data.Clone()
    ratio_data.Divide(h_bkg_s)
    rplt.errorbar(ratio_data, marker="o", color="black", label="data", capsize=0)
    
    ratio_sig = h_sig_s.Clone()
    ratio_sig.Divide(h_bkg_s)
    
    ratio_sig2 = h_sig_p.Clone()
    ratio_sig2.Divide(h_bkg_s)
    
    ratio_syst_up = htot_usyst.Clone()
    ratio_syst_up.Divide(h_bkg_s)
    
    ratio_syst_down = htot_dsyst.Clone()
    ratio_syst_down.Divide(h_bkg_s)
    
    plt.bar(xs, np.array([ratio_sig2.GetBinContent(i) for i in range(1, nb + 1)]), width=ws, bottom=[1.0 for i in range(1, nb+1)], color="red", fill=True, lw=1, edgecolor="red")
    plt.bar(xs, np.array([ratio_sig.GetBinContent(i) for i in range(1, nb + 1)]), width=ws, bottom=[1.0 for i in range(1, nb+1)], color=color_signal, fill=True, lw=1, edgecolor=color_signal)
    
    rplt.fill_between(ratio_syst_down, ratio_syst_up,
        hatch=r"////////",
        alpha=1.0, linewidth=0, facecolor="none", edgecolor="black", zorder=10, label="Bkgd. Unc"
    )
    
    a1.set_ylim(1, 10**5)
    a2.set_ylim(0.5, 3.5)
    
    ticks = a1.get_xticks()
    #a1.get_xaxis().set_visible(False)
    a1.set_yticks(a1.get_yticks()[2:-2])
    a2.set_yticks(a2.get_yticks()[:-1])
    
    a1.set_xlim(left=-2.4, right=-0.4)
    a2.set_xlim(left=-2.4, right=-0.4)
    
    a2.set_ylabel("Ratio to Bkgd.", fontsize=10, fontname="Helvetica")
    a1.set_ylabel("Events / {0:.2f}".format(ws[0]), fontname="Helvetica", fontsize=14, va="top")
    a1.yaxis.set_label_coords(-0.12, 0.85)
    a2.yaxis.set_label_coords(-0.08, 0.5)
    
    a1.set_xticks(ticks[1:-2]);
    a2.set_xlabel("$\log_{10}{[S/B]}$ (prefit expected)", fontsize=14, fontname="Helvetica", ha="right", va="top")
    a2.xaxis.set_label_coords(1.00, -0.3)
    
    minorLocator = MultipleLocator(0.1)
    majorLocator = MultipleLocator(0.5)
    
    a2.xaxis.set_minor_locator(minorLocator)
    a2.xaxis.set_major_locator(majorLocator)
    
    a1.xaxis.set_minor_locator(minorLocator)
    a1.xaxis.set_major_locator(majorLocator)
    
    a2.axhline(1.0, color="black")
    svfg("./sob.pdf")
    svfg("./sob.eps")
