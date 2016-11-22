# Introduction

Code responsible for preparing a comparison of pre-fit and post-fit
distributions. Includes `MakePostFitPlotsFWlite.cc` by Lorenzo
Bianchini for extracting the histograms from `combine` output and
additional scripts for pretty plotting.

# Installation

`CMSSW` environment is needed. Just clone `CommonScripts` into the `src` directory and `scram b`.

# Usage

Steps:
1. Create config file for `MakePostFitPlotsFWlite.cc`
2. Run `MakePostFitPlotsFWlite.cc`
3. Make plots (`plots.py`)

The script `makeMultiPostfit.py` takes care of all the steps in one go.
