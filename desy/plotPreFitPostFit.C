// Latest update: 31 October 2016
//
// Producing prefit and postfit distributions with uncertainty bands and ratio plots
//
// Run as: 
//    root -l
//    .L plotPreFitPostFit.C+
//    plotPreFitPostFit("mlfit_forshapes.root","outputdir")
// where mlfit_withshapes.root is an output file of for example
//    text2workspace.py datacard.dat -m 125 -b -o model.root
//    combine model.root -M MaxLikelihoodFit --minimizerStrategy 0 --minimizerTolerance 0.1 --rMin -10 --rMax 10 --saveShapes --saveWithUncertainties --plots -v 3 -m 125 -n _forshapes
//
// The output of this macro is written in outputdir/output.root, and png and eps files can be saved directly


// ROOT includes

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TAxis.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLine.h"
#include "TObjArray.h"
#include "TBranch.h"
#include "TGraph.h"
#include "TLatex.h"
#include "TF1.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TImage.h"

#include "TKey.h"
#include "TList.h"
#include "THStack.h"
#include "TPaveText.h"
#include "TColor.h"

// RooFit includes
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "RooArgList.h"
#include "RooWorkspace.h"
#include "RooAbsReal.h"
#include "RooFitResult.h"
#include "RooDataSet.h"
#include "RooStats/ModelConfig.h"
#include "RooHist.h"
#include "RooPlot.h"

// standard includes
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <vector>

#define arraysize(ar)  (sizeof(ar) / sizeof(ar[0]))


/////////////////////////////
//// Configuration block ////
/////////////////////////////
string processes_bkg[] = {"diboson","ttbarZ","ttbarW","wjets","zjets","singlet","ttbarPlusBBbar","ttbarPlus2B","ttbarPlusB","ttbarPlusCCbar","ttbarOther"};
// colors from https://git.cern.ch/web/TopAnalysis.git/blob/HEAD:/Configuration/analysis/ttH/src/SampleDefinitions.cc
int processes_bkg_color[] = {kBlue,kBlue-10,kBlue-10,kGreen-3,kGreen-3,kMagenta,kRed+3,kRed+2,kRed-2,kRed+1,kRed-7};  //from SampleDefinitions.cc
//string processes_bkg_legtitle[] = {"Diboson","t#bar{t}Z","t#bar{t}W","W+jets","Z+jets","Single Top","t#bar{t}+b#bar{b}","t#bar{t}+2b","t#bar{t}+b","t#bar{t}+c#bar{c}","t#bar{t}+lf"};
string processes_bkg_legtitle[] = {"Diboson","t#bar{t}V","t#bar{t}V","V+jets","V+jets","Single Top","t#bar{t}+b#bar{b}","t#bar{t}+2b","t#bar{t}+b","t#bar{t}+c#bar{c}","t#bar{t}+lf"}; //same title will result in merged processes in stack and legend

int processes_sig_color[] = {kazure+7};
string processes_sig_legtitle[] = {"t#bar{t}H"};
string data_legtitle = "Data";  //or e.g. "Pseudodata"
bool plotRooHistData = false; //true = asymmetric data uncertainties on main panel; only considered when not using original binning ('original' meaning the real binning of the templates, not the dummy bin numbers that the Higgs Combine tool creates afterwards)
string unc_legtitle = "Total uncertainty";
Style_t uncband_style = 3354; //or 3005? //3354 is usual style
Color_t uncband_color = 1; //or 30? //1 is usual color
bool dooriginalxaxis = true; //change x axis binning to 'original' binning, but for that you need to read the file below. ('original' means the real binning of the templates, not the dummy bin numbers that the Higgs Combine tool creates afterwards)
string originalxaxis_filename_dl = "ttHbb_13TeV_HIG-16-038/common/ttH_hbb_13TeV_dl.root"; //needed to read original binning for dilepton categories
string originalxaxis_filename_sl = "ttHbb_13TeV_HIG-16-038/common/ttH_hbb_13TeV_sl.root"; //needed to read original binning for single lepton categories
bool combination = true; //when running on different categories in the same mlfit root file
bool do_dl_3j3t_axisrangehack = true; //setting axis range from -1 to 0.6 instead of -1 to +1

//bit dirty coding: 
//the first element is the name of the category ('channel') in the mlfit input root file
//the second element in the map will contain 4 entries in this order: 1) x axis title, 2) file where the original x axis can be retrieved, 3) histogram name with original x axis and 4) the luminosity
std::map<TString,std::vector<string> > mapOfCategories;

string yaxistitle = "Number of Events";
string ratiotitle = "Data / MC";
double signalscale = 15; //factor to scale signal. This factor will be displayed in the legend, except when it is 1
bool stacksignal = false; //false = overlay signal with factor signalscale, true = stack signal with factor signalscale
string luminosity_dl = "11.4-12.9"; //inverse fb, to be displayed on dilepton plots
string luminosity_sl = "12.9"; //inverse fb, to be displayed on single lepton plots 
const int  prelim = 1; //0 = CMS, 1 = CMS Preliminary, 2 = CMS Private work, 3 = CMS Simulation 
string outfilename = "output.root";
bool writepng = true; //write plots in png format too
bool writeeps = true; //write plots in eps format too
int verbosity = 2; //0 = silent, 1 = little, 2 = loud (recommended)


void setStyle(TH1F* hist, int linecolor, int linewidth, int fillcolor)
{
//    hist->GetXaxis()->SetTitle(XAxis_);
//    hist->GetYaxis()->SetTitle(YAxis_);
    hist->GetXaxis()->SetLabelFont(42);
    hist->GetYaxis()->SetLabelFont(42);
    hist->GetXaxis()->SetTitleFont(42);
    hist->GetYaxis()->SetTitleFont(42);
    hist->GetYaxis()->SetTitleOffset(1.7);
    hist->GetXaxis()->SetTitleOffset(1.25);
    
    // Set styles for sample
    hist->SetFillColor(fillcolor);
    hist->SetLineColor(linecolor);
    hist->SetLineWidth(linewidth);
    if(string(hist->GetName()).find("data")==0){
        hist->SetMarkerStyle(20);
        hist->SetMarkerSize(1.);
        hist->SetLineWidth(2);
    }
}


//inspiration from analysis/common/src/plotterUtils.cc
void setHistoStyle(TH1* hist, Style_t line, Color_t lineColor, Size_t lineWidth, 
                           Style_t marker, Color_t markerColor, Size_t markerSize,
                           Style_t fill, Color_t fillColor)
{
    if(line != -1) hist->SetLineStyle(line);
    if(lineColor != -1) hist->SetLineColor(lineColor);
    if(lineWidth != -1) hist->SetLineWidth(lineWidth);
    
    if(fill != -1) hist->SetFillStyle(fill);
    if(fillColor != -1) hist->SetFillColor(fillColor);
    
    if(marker != -1) hist->SetMarkerStyle(marker);
    if(markerColor != -1) hist->SetMarkerColor(markerColor);
    if(markerSize != -1) hist->SetMarkerSize(markerSize);
}


//inspiration from analysis/common/src/plotterUtils.cc
TLegend* createLegend(const double& x1, const double& y1_min, const int nColumns, const int nEntries, const double& rowHeight) 
{
    // Defining the position of the upper right corner
    const double x2 = 1.0 - gStyle->GetPadRightMargin() - gStyle->GetTickLength();
    const double y2 = 1.0 - gStyle->GetPadTopMargin()  - gStyle->GetTickLength();
    
    // Raising the bottom border if the number of entries is small enough
    double y1(y1_min);
    if(nEntries > 0) {
      const int nRows = nEntries%nColumns > 0 ? nEntries/nColumns + 1 : nEntries/nColumns;
      y1 = std::max(y2 - nRows*rowHeight, y1_min);
    }
    
    TLegend* legend = new TLegend(x1,y1,x2,y2);
    legend->SetNColumns(nColumns);
    legend->SetColumnSeparation(0.1/double(nColumns));
    legend->SetFillStyle(0);
    legend->SetBorderSize(0);
    legend->Clear();
    
    return legend;
}


//inspiration from analysis/common/src/plotterUtils.cc, added 'simulation', but luminosity is now string instead of const double
void drawCmsLabels(const int cmsprelim, const double& energy, TString& luminosityInInverseFb, const double& textSize)
{
    TString text = luminosityInInverseFb + " fb^{-1} (13 TeV)";
    TPaveText *label = new TPaveText();
    label->SetX1NDC(gStyle->GetPadLeftMargin());
//    label->SetY1NDC(1.0-gStyle->GetPadTopMargin());
    label->SetY1NDC(1.0-gStyle->GetPadTopMargin()-0.05);
    label->SetX2NDC(1.0-gStyle->GetPadRightMargin());
    label->SetY2NDC(1.0);
    label->SetTextFont(42);
    //label->AddText(Form(text, luminosity, energy));
    label->AddText(text);
    label->SetFillStyle(0);
    label->SetBorderSize(0);
    if (textSize!=0) label->SetTextSize(textSize);
    label->SetTextAlign(32);
    label->Draw("same");
    if(cmsprelim > -1) {
        TPaveText *cms = new TPaveText();
        cms->AddText("CMS");

        cms->SetX1NDC(      gStyle->GetPadLeftMargin() + gStyle->GetTickLength()        );
        cms->SetY1NDC(1.0 - gStyle->GetPadTopMargin()  - gStyle->GetTickLength() - 0.05 );
        cms->SetX2NDC(      gStyle->GetPadLeftMargin() + gStyle->GetTickLength() + 0.15 );
        cms->SetY2NDC(1.0 - gStyle->GetPadTopMargin()  - gStyle->GetTickLength()        );

        cms->SetFillStyle(0);
        cms->SetBorderSize(0);
        if (textSize!=0) cms->SetTextSize(textSize*1.1);
        cms->SetTextAlign(12);
        cms->SetTextFont(61);
        cms->Draw("same");
    }

    if(cmsprelim > 0) {
      TPaveText *extra = new TPaveText();
      if(cmsprelim == 2) { extra->AddText("Private Work"); }
      else if(cmsprelim == 3) { extra->AddText("simulation"); }
      else { extra->AddText("Preliminary"); }

      extra->SetX1NDC(      gStyle->GetPadLeftMargin() + gStyle->GetTickLength()        );
      extra->SetY1NDC(1.0 - gStyle->GetPadTopMargin()  - gStyle->GetTickLength() - 0.10 );
      extra->SetX2NDC(      gStyle->GetPadLeftMargin() + gStyle->GetTickLength() + 0.15 );
      extra->SetY2NDC(1.0 - gStyle->GetPadTopMargin()  - gStyle->GetTickLength() - 0.05 );

      extra->SetFillStyle(0);
      extra->SetBorderSize(0);
      if (textSize!=0) extra->SetTextSize(textSize);
      extra->SetTextAlign(12);
      extra->SetTextFont(52);
      extra->Draw("same");
    }
}


void drawFitLabel(TString fitlabel, const double& textSize)
{
    TPaveText *label = new TPaveText();
    label->SetX1NDC(gStyle->GetPadLeftMargin() + gStyle->GetTickLength());
    label->SetY1NDC(1.0-gStyle->GetPadTopMargin());
    label->SetX2NDC(gStyle->GetPadLeftMargin() + gStyle->GetTickLength() + 0.15);
    label->SetY2NDC(0.95);
    label->SetTextFont(42);
    if (textSize!=0) label->SetTextSize(textSize);
    label->AddText(fitlabel);
    label->SetFillStyle(0);
    label->SetBorderSize(0);
    label->Draw("same");
}


TH1F* histo_convertbinning(TH1F* histogram_abstractbinning, int nbins_original, const Double_t* originalbinning)
{
   TH1F* histo_originalbinning = new TH1F(TString(histogram_abstractbinning->GetName())+"_originalbinning",TString(histogram_abstractbinning->GetTitle())+"_originalbinning",nbins_original,originalbinning);
   histo_originalbinning->Sumw2();
   for(int bini=0; bini<nbins_original+1; bini++)
   {
	histo_originalbinning->SetBinContent(bini,histogram_abstractbinning->GetBinContent(bini));
	histo_originalbinning->SetBinError(bini,histogram_abstractbinning->GetBinError(bini));
   }   

   return histo_originalbinning;
}


void drawPlots(TFile* input, TString shapes, string outputPath, TFile* output)
{
        TString shapelable = "";
	if(shapes == "shapes_prefit") shapelable = "pre-fit";
	else if(shapes == "shapes_fit_b") shapelable = "post-fit background";
	else if(shapes == "shapes_fit_s") shapelable = "post-fit background+signal";

	if(verbosity>0)
	{
	   std::cout << "Drawing " << shapelable << " histograms" << std::endl; 
	   std::cout << "---------------------------------------" << std::endl;
	}	
	   
	TDirectoryFile* shapes_prefit = (TDirectoryFile*) input->Get(shapes);	

	TIter next(shapes_prefit->GetListOfKeys());
        TKey *key;
	int cat=0; //category counter
	
	if(!combination)
	{
	  //when running on separate-category fits
	  //only for dilepton, single lepton not yet supported in this running configuration (i.e. when runnin on a mlfit output root file with the fit result of only 1 category -- the names change due to the merging of datacards)
	  mapOfCategories["dl_j3_t3"].push_back("BDT discriminant");        	
	  mapOfCategories["dl_j3_t3"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_j3_t3"].push_back("dl_j3_t3_BDT/data_obs");
	  mapOfCategories["dl_gej4_t3_low"].push_back("MEM discriminant (BDT low)");        	
	  mapOfCategories["dl_gej4_t3_low"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_gej4_t3_low"].push_back("dl_gej4_t3_low_BDT/data_obs");
          mapOfCategories["dl_gej4_t3_high"].push_back("MEM discriminant (BDT high)");
          mapOfCategories["dl_gej4_t3_high"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_gej4_t3_high"].push_back("dl_gej4_t3_high_BDT/data_obs");
          mapOfCategories["dl_gej4_get4_low"].push_back("MEM discriminant (BDT low)");
          mapOfCategories["dl_gej4_get4_low"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_gej4_get4_low"].push_back("dl_gej4_get4_low_BDT/data_obs");
          mapOfCategories["dl_gej4_get4_high"].push_back("MEM discriminant (BDT high)");
          mapOfCategories["dl_gej4_get4_high"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_gej4_get4_high"].push_back("dl_gej4_get4_high_BDT/data_obs");
	}
	else
	{
	  //when running on separate categories merged.
	  //dilepton
	  mapOfCategories["dl_dl_3j3t"].push_back("BDT discriminant");        	
	  mapOfCategories["dl_dl_3j3t"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_dl_3j3t"].push_back("dl_j3_t3_BDT/data_obs");
	  mapOfCategories["dl_dl_3j3t"].push_back(luminosity_dl);
	  mapOfCategories["dl_dl_ge4j3t_low"].push_back("MEM discriminant");        	
	  mapOfCategories["dl_dl_ge4j3t_low"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_dl_ge4j3t_low"].push_back("dl_gej4_t3_low_BDT/data_obs");
	  mapOfCategories["dl_dl_ge4j3t_low"].push_back(luminosity_dl);
          mapOfCategories["dl_dl_ge4j3t_high"].push_back("MEM discriminant");
          mapOfCategories["dl_dl_ge4j3t_high"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_dl_ge4j3t_high"].push_back("dl_gej4_t3_high_BDT/data_obs");
	  mapOfCategories["dl_dl_ge4j3t_high"].push_back(luminosity_dl);
          mapOfCategories["dl_dl_ge4jge4t_low"].push_back("MEM discriminant");
          mapOfCategories["dl_dl_ge4jge4t_low"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_dl_ge4jge4t_low"].push_back("dl_gej4_get4_low_BDT/data_obs");
	  mapOfCategories["dl_dl_ge4jge4t_low"].push_back(luminosity_dl);
          mapOfCategories["dl_dl_ge4jge4t_high"].push_back("MEM discriminant");
          mapOfCategories["dl_dl_ge4jge4t_high"].push_back(originalxaxis_filename_dl);
          mapOfCategories["dl_dl_ge4jge4t_high"].push_back("dl_gej4_get4_high_BDT/data_obs");
	  mapOfCategories["dl_dl_ge4jge4t_high"].push_back(luminosity_dl);
	  
	  //single lepton
	  mapOfCategories["sl_ch1"].push_back("MEM discriminant");        	
	  mapOfCategories["sl_ch1"].push_back(originalxaxis_filename_sl);
          mapOfCategories["sl_ch1"].push_back("data_obs_finaldiscr_ljets_j4_t4_high");
	  mapOfCategories["sl_ch1"].push_back(luminosity_sl);
	  mapOfCategories["sl_ch2"].push_back("MEM discriminant");        	
	  mapOfCategories["sl_ch2"].push_back(originalxaxis_filename_sl);
          mapOfCategories["sl_ch2"].push_back("data_obs_finaldiscr_ljets_j4_t4_low");
	  mapOfCategories["sl_ch2"].push_back(luminosity_sl);
	  mapOfCategories["sl_ch3"].push_back("MEM discriminant");        	
	  mapOfCategories["sl_ch3"].push_back(originalxaxis_filename_sl);
          mapOfCategories["sl_ch3"].push_back("data_obs_finaldiscr_ljets_j5_tge4_high");
	  mapOfCategories["sl_ch3"].push_back(luminosity_sl);
	  mapOfCategories["sl_ch4"].push_back("MEM discriminant");        	
	  mapOfCategories["sl_ch4"].push_back(originalxaxis_filename_sl);
          mapOfCategories["sl_ch4"].push_back("data_obs_finaldiscr_ljets_j5_tge4_low");
	  mapOfCategories["sl_ch4"].push_back(luminosity_sl);
	  mapOfCategories["sl_ch5"].push_back("MEM discriminant");        	
	  mapOfCategories["sl_ch5"].push_back(originalxaxis_filename_sl);
          mapOfCategories["sl_ch5"].push_back("data_obs_finaldiscr_ljets_jge6_t3_high");
	  mapOfCategories["sl_ch5"].push_back(luminosity_sl);
	  mapOfCategories["sl_ch6"].push_back("MEM discriminant");        	
	  mapOfCategories["sl_ch6"].push_back(originalxaxis_filename_sl);
          mapOfCategories["sl_ch6"].push_back("data_obs_finaldiscr_ljets_jge6_t3_low");
	  mapOfCategories["sl_ch6"].push_back(luminosity_sl);
	  mapOfCategories["sl_ch7"].push_back("MEM discriminant");        	
	  mapOfCategories["sl_ch7"].push_back(originalxaxis_filename_sl);
          mapOfCategories["sl_ch7"].push_back("data_obs_finaldiscr_ljets_jge6_tge4_high");
	  mapOfCategories["sl_ch7"].push_back(luminosity_sl);
	  mapOfCategories["sl_ch8"].push_back("MEM discriminant");        	
	  mapOfCategories["sl_ch8"].push_back(originalxaxis_filename_sl);
          mapOfCategories["sl_ch8"].push_back("data_obs_finaldiscr_ljets_jge6_tge4_low");
	  mapOfCategories["sl_ch8"].push_back(luminosity_sl);
	}
	
	//loop over the channels (categories) present in the subdirectories of the mlfit root file
	while((key = (TKey*) next())) 
	{   
	   input->cd();
	   TDirectoryFile* channel = (TDirectoryFile*) shapes_prefit->Get(key->GetName());
	   if(verbosity>1)
	   {
	      std::cout << " channel: " << channel->GetName() << std::endl;
	   }
	   
	   //TEMP TESTING!!
	   //if(TString(channel->GetName()) != "dl_dl_3j3t") continue;
	   	   
	   std::vector<Double_t> mybinlowedges;
	   int nbins_original = 0;
	   if(dooriginalxaxis)
	   { 
	     if(verbosity>1) std::cout << "     Reading histogram " << mapOfCategories[TString(channel->GetName())].at(2) << " from external file " << mapOfCategories[TString(channel->GetName())].at(1) <<" to retrieve original x axis" << std::endl;
	     TFile* originalxaxis_file = TFile::Open(mapOfCategories[TString(channel->GetName())].at(1).c_str());
	     TH1F* horiginalxaxis = (TH1F*) originalxaxis_file->Get(mapOfCategories[TString(channel->GetName())].at(2).c_str());
	     TAxis* originalaxis = horiginalxaxis->GetXaxis();
	     nbins_original = originalaxis->GetNbins();
	     cout << "     Original number of bins: " << nbins_original << endl;
	     for(int edgei=0; edgei<nbins_original+1;edgei++)
	     {
	       mybinlowedges.push_back(originalaxis->GetBinLowEdge(edgei+1));
	       cout << "      bin edge " << edgei+1 << ": " << originalaxis->GetBinLowEdge(edgei+1) << endl;
	     }
	     if(plotRooHistData)
	     {
	       cout << "RooHist data format currently not compatible with retransformation to original binning: will not plot asymmetric errors on data" << endl;
	       plotRooHistData = false;
	     }
	   }
	   
	   Double_t binlowedges[mybinlowedges.size()];
	   for(unsigned int edgei=0; edgei<mybinlowedges.size();edgei++)
	   {
	       binlowedges[edgei] = mybinlowedges.at(edgei);
	   }
	  	 	  
	
	   vector<TH1F*> histos_bkg;
	   vector<TH1F*> histos_sig;
	   //TH1F* total = 0;
	   TH1F* total_bkg = 0;
	   TH1F* total_sig = 0;
	   RooHist* hdata = 0;  //data or pseudodata
	   TH1F* hdata_hist = 0;  //data or pseudodata; sort of copy (of the RooHist above) intended for the ratio plot. Data errors in this ratio plot will be the square root of the bin content... (not correct for very small bin contents!)
	   TH1F* total_bkg_uncband = 0;
	
	   input->cd();
	   if(verbosity>1)
	   {
	      std::cout << "  reading histograms from file" << std::endl;
	   }
	   channel->cd();
	   //channel->ls();
	   
	   
	   //merge the histograms with the same legend title
	   //the resulting histos_bkg_merged vector will have the same size as the original processes_bkg array, but there will be empty pointers for the empty histograms 
	   //as well as for the histograms which are added to another histogram. By checking later on for empty pointers one can keep track of what not to plot in the stack or the legend.
	   std::vector<TH1F*> histos_bkg_merged;
	   unsigned int bkgindex_toskip = 9999;
	   for(unsigned int bkgi=0; bkgi<arraysize(processes_bkg); bkgi++)
	   {
	      if(verbosity>1) std::cout << "     " << processes_bkg[bkgi].c_str() << std::endl;
	      if(bkgi == bkgindex_toskip)
	      {
		 TH1F* hdummy = 0;
		 histos_bkg_merged.push_back(hdummy);
	         continue;  //means this histo is already merged with a previous one
	      }
	      TH1F* htemp_i = (TH1F*) channel->Get(processes_bkg[bkgi].c_str());
	      for(unsigned int bkgj=bkgi+1; bkgj<arraysize(processes_bkg); bkgj++)
	      {
		  if(processes_bkg_legtitle[bkgi] == processes_bkg_legtitle[bkgj])
		  {
		       //only consider adding something if the histo to add to is not an empty pointer
		       if(htemp_i)
		       {
		         bkgindex_toskip = bkgj;
		         TH1F* htemp_j = (TH1F*) channel->Get(processes_bkg[bkgj].c_str());
		         if(htemp_j)
			 {
			    std::cout << "         ... adding " << processes_bkg[bkgj].c_str() << " to "<<processes_bkg[bkgi].c_str()<< std::endl;
			    htemp_i->Add(htemp_j); //only add it to another histogram if it's not an empty pointer
			 }
		       }
		  } 
	      }
	      histos_bkg_merged.push_back(htemp_i); //may be an empty pointer that is pushed back	      
	   }
	   
	   	   	   
	   //if needed, convert the binning of the histgrams to the original binning
	   for(unsigned int bkgi=0; bkgi<histos_bkg_merged.size(); bkgi++)
	   {
	      TH1F* htemp = histos_bkg_merged.at(bkgi);
	      if(dooriginalxaxis)
	      {
		 if(htemp) histos_bkg.push_back(histo_convertbinning(htemp,nbins_original,binlowedges));
		 else histos_bkg.push_back(htemp); //push back empty pointer
	      }
	      else histos_bkg.push_back((TH1F*) htemp->Clone());
	   }
	   
	   
	   	
	   //total = (TH1F*) channel->Get("total");
	   TH1F* total_bkg_tmp = (TH1F*) channel->Get("total_background");
	   if(dooriginalxaxis) total_bkg = histo_convertbinning(total_bkg_tmp,nbins_original,binlowedges);
	   else total_bkg = (TH1F*) total_bkg_tmp->Clone();
	   TH1F* total_sig_tmp = (TH1F*) channel->Get("total_signal");
	   if(dooriginalxaxis) total_sig = histo_convertbinning(total_sig_tmp,nbins_original,binlowedges);
	   else total_sig = (TH1F*) total_sig_tmp->Clone();
	   if(dooriginalxaxis) total_bkg_uncband = histo_convertbinning(total_bkg,nbins_original,binlowedges);
	   else total_bkg_uncband = (TH1F*) total_bkg->Clone();
   
	   float maximum = total_bkg->GetBinContent(total_bkg->GetMaximumBin());
	   
	   TLegend* leg = 0;
	   leg = createLegend(0.5,0.7,2,6,0.3);
	   
	   if(verbosity>1) std::cout << "    Retrieving data..." << std::endl;

           RooPlot* rooplotdata = (RooPlot*) input->Get(TString(key->GetName())+"_prefit");	   
	   TString hdataname = "h_"+TString(key->GetName());
	   hdata = rooplotdata->getHist(hdataname);

	   //manually copying RooHist data object to a TH1F object
	   hdata_hist = (TH1F*) total_bkg->Clone(); //will be overwritten just below
	   
	   if(do_dl_3j3t_axisrangehack)
	   {
	      if(TString(channel->GetName()).Contains("dl_j3t3") || TString(channel->GetName()).Contains("dl_3j3t"))
	      {
	         if(verbosity>1) std::cout<<"       WARNING: x axis range of category "<<TString(channel->GetName())<<" is being changed w.r.t default from datacard"<<std::endl;
	         hdata_hist->GetXaxis()->SetRangeUser(-1,0.6);
	      }
	   }	   
	   
	   for(int bini=0; bini<hdata->GetN()+1; bini++)
	   {
	        Double_t pointx = 0, pointy = 0;
		hdata->GetPoint(bini,pointx,pointy);
		//std::cout << "     x = " << pointx << ", y = " << pointy << std::endl;
	        hdata_hist->SetBinContent(bini+1,pointy);
		hdata_hist->SetBinError(bini+1,sqrt(pointy));
	   }
	   if(verbosity>1) std::cout << "    Data retrieved." << std::endl;
	   
           maximum = std::max(total_bkg->GetBinContent(total_bkg->GetMaximumBin()), hdata->GetMaximum());
	   hdata->SetTitle("");
	   hdata_hist->SetNameTitle("data","");
	   leg->AddEntry(hdata,TString(data_legtitle),"LEP");
	     
	   if(signalscale != 1)
	   {
	      ostringstream ss;
              ss << signalscale;
	      string signalscalestr(ss.str());
	      leg->AddEntry(total_sig,TString(processes_sig_legtitle[0])+" x"+TString(signalscalestr),"F");
	   }
	   else leg->AddEntry(total_sig,TString(processes_sig_legtitle[0]),"F"); 
	   
	   
	   //stack histograms
	   THStack* stack(0);
           if(histos_bkg.size()) stack = new THStack("stack", "");
           for(unsigned int iHist_bkg = 0; iHist_bkg < histos_bkg.size(); iHist_bkg++)
	   { 	      
	      if(!histos_bkg[iHist_bkg]) continue;
	      setStyle(histos_bkg[iHist_bkg],1,1,processes_bkg_color[iHist_bkg]);
	      histos_bkg[iHist_bkg]->GetYaxis()->SetRangeUser(0,1.7*maximum);
              stack->Add((TH1F*) histos_bkg[iHist_bkg]->Clone());
	      //leg->AddEntry(histos_bkg[iHist_bkg],TString(processes_bkg_legtitle[iHist_bkg]),"F");
           }
	   
	   
	   //add legend of non-empty pointer histograms in opposite direction
	   for(int iHist_bkg = histos_bkg.size()-1; iHist_bkg >=0; iHist_bkg--)
	   { 
	      if(!histos_bkg[iHist_bkg]) continue;
	      leg->AddEntry(histos_bkg[iHist_bkg],TString(processes_bkg_legtitle[iHist_bkg]),"F");	      
	   }
	   
	   	   
	   if(stacksignal)
	   {
	     setStyle(total_sig,1,1,processes_sig_color[0]);
	     if(signalscale != 1) total_sig->Scale(signalscale);
	     stack->Add((TH1F*) total_sig->Clone());
	   }	
	   
	   setHistoStyle(total_bkg_uncband, 1,1,1, -1,-1,-1, uncband_style, uncband_color);
		   	
			   
           if(verbosity>1) std::cout << "  creating canvas" << std::endl;

	   TCanvas* c1 = new TCanvas(TString(shapes)+TString(channel->GetName()),TString(shapes)+TString(channel->GetName()),1000,800);	   
	   c1->cd(); 
	   
	   Int_t logx = c1->GetLogx();
           Double_t left = c1->GetLeftMargin();
//	   Double_t left = c1->GetLeftMargin()*1.3;
           Double_t right = c1->GetRightMargin();
	   // y:x size ratio for canvas
           double canvAsym = 4./3.;
           // ratio size of pad with plot and pad with ratio
           double ratioSize = 0.36;
           // change old pad
           c1->SetBottomMargin(ratioSize);
           c1->SetRightMargin(right);
           c1->SetLeftMargin(left);
           c1->SetBorderMode(0);
           c1->SetBorderSize(0);
           c1->SetFillColor(10);

           
	   if(verbosity>1) std::cout << "  drawing plot" << std::endl;
	   setStyle(hdata_hist,1,1,0);

//	   setStyle(hdata->GetHistogram(),1,1,0);
           if(plotRooHistData)
	   {
	     hdata->GetYaxis()->SetTitle(TString(yaxistitle));
	     hdata->GetYaxis()->SetTitleOffset(1.5);
	     hdata->GetYaxis()->SetTitleSize(0.03);
	     hdata->GetYaxis()->SetLabelSize(0.03);
	     hdata->GetYaxis()->SetRangeUser(0,1.7*maximum);
	     hdata->GetXaxis()->SetRangeUser(0,total_bkg->GetNbinsX()); //the RooHist object has some extra empty bins by default for some reason...
//	     hdata->SetStats(0); //may be obsolete and/or doesn't work
	     hdata->GetHistogram()->SetStats(0);
	     hdata->Draw("AP"); //I draw the data first because I don't manage to change the y-axis range of the stack
	   }
	   else
	   {
	     hdata_hist->GetYaxis()->SetTitle(TString(yaxistitle));
	     hdata_hist->GetYaxis()->SetTitleOffset(1.5);
	     hdata_hist->GetYaxis()->SetTitleSize(0.03);
	     hdata_hist->GetYaxis()->SetLabelSize(0.03);
	     hdata_hist->GetYaxis()->SetRangeUser(0,1.7*maximum); //doesn't work
             hdata_hist->GetXaxis()->SetLabelSize(0); //removing x axis labels..
	     hdata_hist->Draw("P");
	   }
			   
////           gStyle->SetHistTopMargin(0.);
           stack->Draw("HIST SAME");	   
	   stack->GetHistogram()->GetYaxis()->SetTitle(TString(yaxistitle));
	   stack->GetHistogram()->GetYaxis()->SetTitleOffset(1.35);
	   
	   total_bkg_uncband->Draw("SAME E2");	   

	   //setStyle(hdata,1,1,0);
	   if(plotRooHistData) hdata->Draw("SAME P"); //to get it back in front of the stack...
	   else hdata_hist->Draw("SAME P");

	   if(!stacksignal)
	   {
	         if(signalscale != 1) total_sig->Scale(signalscale);
	         setStyle(total_sig,processes_sig_color[0],2,0);
		 total_sig->Draw("SAME HIST");
	   } 


           leg->AddEntry(total_bkg_uncband,TString(unc_legtitle),"F");
           leg->Draw("SAME");
	   TString luminosity = TString(mapOfCategories[TString(channel->GetName())].at(3));
	   drawCmsLabels(prelim, 13, luminosity, 0.035);
           drawFitLabel(shapelable, 0.035);
	   
	   
	   if(verbosity>1) std::cout << "  drawing ratio plot" << std::endl;

           //ratio plot; inspiration from TopAnalysis/Configuration/analysis/common/src/plotterUtils.cc
	   //but: for example when using a RooHist for the data (asymmetric errors...) we still use a simple histogram deduced from the RooHist as a RooHist inheriting form TGraph doesn't have a divide function...
	   hdata_hist->GetXaxis()->SetLabelSize(0); //adding back x axis labels..
	   TH1F* ratio1 = (TH1F*) hdata_hist->Clone();
//	   ratio1->Sumw2(); //(z)
           ratio1->SetLineColor(hdata_hist->GetLineColor());
           ratio1->SetLineStyle(hdata_hist->GetLineStyle());
           ratio1->SetLineWidth(hdata_hist->GetLineWidth());
	   //setting errors of background artificially to 0 for the calculation of the data errors in the ratio, otherwise the data errors are too big
	   TH1F* total_bkg_zeroerrors = (TH1F*) total_bkg->Clone();
	   for(int bini=0; bini<total_bkg_zeroerrors->GetNbinsX()+1; bini++)
	   {
	      total_bkg_zeroerrors->SetBinError(bini,0);	   
	   }
           ratio1->Divide(total_bkg_zeroerrors);
	   
	   TH1F* ratio_uncband = (TH1F*) total_bkg_uncband->Clone();
           ratio_uncband->SetLineColor(total_bkg_uncband->GetLineColor());
           ratio_uncband->SetLineStyle(total_bkg_uncband->GetLineStyle());
           ratio_uncband->SetLineWidth(total_bkg_uncband->GetLineWidth());
           ratio_uncband->Divide(total_bkg_zeroerrors);

           /// create new pad for ratio plot
           TPad *rPad = new TPad("rPad","",0,0,1,ratioSize+0.001); 
	   rPad->SetFillStyle(0);
           rPad->SetFillColor(0);
           rPad->SetBorderSize(0);
           rPad->SetBorderMode(0);
//	   rPad->SetTopMargin(1-ratioSize);
           rPad->Draw();
           rPad->cd();
           rPad->SetLogy(0);
           rPad->SetLogx(logx);
           rPad->SetTicky(1);
      
           /// configure ratio plot
           double scaleFactor = 1./(canvAsym*ratioSize);
           ratio1->SetStats(kFALSE);
           ratio1->SetTitle("");
//           ratio1->SetMaximum(0.5);
//           ratio1->SetMinimum(2);
           ratio1->GetYaxis()->SetRangeUser(0,2);
	   	
    
           /// configure axis of ratio plot
	   ratio1->GetXaxis()->SetTitleSize(0.12);
           ratio1->GetXaxis()->SetTitleOffset(hdata_hist->GetXaxis()->GetTitleOffset()*0.9);
	   ratio1->GetXaxis()->SetLabelSize(0.1);
           ratio1->GetXaxis()->SetTitle(hdata_hist->GetXaxis()->GetTitle());

           ratio1->GetYaxis()->CenterTitle();
           ratio1->GetYaxis()->SetTitle(TString(ratiotitle));	   
           ratio1->GetYaxis()->SetTitleSize(0.08);
           ratio1->GetYaxis()->SetTitleOffset(0.55);
           ratio1->GetYaxis()->SetLabelSize(hdata->GetYaxis()->GetLabelSize()*scaleFactor);
           ratio1->GetYaxis()->SetLabelOffset(hdata->GetYaxis()->GetLabelOffset()*3.3);
           ratio1->GetYaxis()->SetTickLength(0.03);
           ratio1->GetYaxis()->SetNdivisions(505);
           ratio1->GetXaxis()->SetRange(hdata_hist->GetXaxis()->GetFirst(), hdata_hist->GetXaxis()->GetLast());
           ratio1->GetXaxis()->SetNoExponent(kTRUE);
           ratio1->GetXaxis()->SetTitle(TString(mapOfCategories[TString(channel->GetName())].at(0)));
    
           /// delete axis of initial plot
           hdata->GetXaxis()->SetLabelSize(0);
           hdata->GetXaxis()->SetTitleSize(0);	   
           hdata->GetXaxis()->SetNoExponent(kFALSE);

	   stack->GetXaxis()->SetLabelSize(0);
	   stack->GetXaxis()->SetTitleSize(0);	   
           stack->GetXaxis()->SetNoExponent(kFALSE);

           /// draw ratio plot
           ratio1->DrawClone("SAME");
	   ratio_uncband->DrawClone("SAME E2");
           rPad->Update();
           rPad->Modified();
	   rPad->SetTopMargin(0.05);
           rPad->SetBottomMargin(0.15*scaleFactor);
           rPad->SetRightMargin(right);
           gPad->SetLeftMargin(left);
           gPad->RedrawAxis();
    
           /// draw grid
           rPad->SetGrid(0,1);


	   if(verbosity>1) std::cout << "  writing plot" << std::endl;
	   
	   output->cd();
	   TString plotname = TString(shapes)+"_"+TString(channel->GetName());	   
	   c1->Write(plotname);
	   if(writepng) c1->Print(TString(outputPath)+plotname+".png","png");
	   if(writeeps) c1->Print(TString(outputPath)+plotname+".eps","eps");
	   if(writepng) c1->Print(TString(outputPath)+plotname+".png","png"); //for some reason I need to write it twice to resolve a strange plotting issue
	   
	   cat++; //category counter
	}
}


void plotPreFitPostFit(std::string inputFile, std::string outputPath="PreFitPostFitplots/"){

	// Some Global preferences
	gSystem->Load("libHiggsAnalysisCombinedLimit");
	gROOT->SetBatch(true);
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0);
//	gStyle->SetPalette(1,0);
        		
        gSystem->mkdir(outputPath.c_str(),0777);
	TFile *outfile = new TFile(TString(outputPath)+TString(outfilename),"RECREATE");
	
	TFile *infile = TFile::Open(inputFile.c_str());
	infile->cd();
 
        drawPlots(infile,"shapes_prefit",outputPath,outfile);
        drawPlots(infile,"shapes_fit_b",outputPath,outfile);
        drawPlots(infile,"shapes_fit_s",outputPath,outfile);
		   
	if(verbosity>0) std::cout << std::endl << "Stacked plots written to " << outputPath+outfilename << std::endl;
	outfile->Close();
	infile->Close();	
	
}

