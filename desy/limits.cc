////////////////////////////////////////////////
//                                            //
//   Written by: Christian Contreras-Campana  //
//   email: christian.contreras@desy.de       //
//   Date: 01.08.2015                         //
//                                            //
////////////////////////////////////////////////

// URL: https://ghm.web.cern.ch/ghm/plots

// Description: Sowftware produces a limit plot based on event category (in the form of pdf file)

// Use: 
// g++ -O -Wall `root-config --glibs --cflags` limits.cc -o limits
// limits ttH_hbb_13TeV_dl_*.lmt

#include <map>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include <TKey.h>
#include <TH1F.h>
#include <TRint.h>
#include <TROOT.h>
#include <TList.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TFrame.h>
#include <TString.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TObject.h>
#include <TSystem.h>
#include <TPRegexp.h>
#include <TIterator.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TObjString.h>
#include <TClonesArray.h>
#include <TApplication.h>
#include <TGraphAsymmErrors.h>

#include "tdrstyle.C"
#include "CMS_lumi.C"

 
int plotLimit(TApplication* rootapp, std::string mass = "125.0")
{
  std::map<std::string,std::ifstream*> m;

  // Convert internal labeling scheme to human readable event category labels
  std::map<std::string, std::string> cateLabelConvert;

  ////For HIG-16-038 (Higgs Couplings '16) we dropped the 2-tag categories. 
  ////Note that currently for the low and high BDT categories the input *.lmt limit file names must not have an underscore in the category name for the code to work.
  //cateLabelConvert["3j2t"]     = "3 jets, 2 b-tags";
  cateLabelConvert["3j3t"]     = "3 jets, 3 b-tags";
  //cateLabelConvert["ge4j2t"]   = "#geq 4 jets, 2 b-tags";
  //cateLabelConvert["ge4j3t_low"]   = "#geq 4 jets, 3 b-tags (low)";  
  //cateLabelConvert["ge4j3t_high"]   = "#geq 4 jets, 3 b-tags (high)";
  //cateLabelConvert["ge4jge4t_low"] = "#geq 4 jets, #geq 4 b-tags (low)";
  //cateLabelConvert["ge4jge4t_high"] = "#geq 4 jets, #geq 4 b-tags (high)";
  cateLabelConvert["ge4j3tlow"]   = "#geq 4 jets, 3 b-tags (low)";  
  cateLabelConvert["ge4j3thigh"]   = "#geq 4 jets, 3 b-tags (high)";
  cateLabelConvert["ge4jge4tlow"] = "#geq 4 jets, #geq 4 b-tags (low)";
  cateLabelConvert["ge4jge4thigh"] = "#geq 4 jets, #geq 4 b-tags (high)";
  cateLabelConvert["merged"]   = "Combined";

  Int_t cateIndex(0);

  Int_t n = cateLabelConvert.size();
  
  std::string line;

  Double_t obsLimit(0);
  Double_t expLimit(0);
  Double_t expLimitm1Sigma(0);
  Double_t expLimitp1Sigma(0);
  Double_t expLimitm2Sigma(0);
  Double_t expLimitp2Sigma(0);
  
  TString cate;

  std::vector<Double_t> binLabels;

  // Setup up index label scheme
  for (Int_t i = 0; i < n+1; ++i) {
    binLabels.push_back(i);
  }

  // Used for the observed limit  
  TH1F *h1 = new TH1F("h1", "h1 title", n, 0, n);
  h1->SetMarkerColor(kBlack);
  h1->SetBins(n, &binLabels.front());

  // Used for expected limit
  TH1F *h2 = new TH1F("h2", "h2 title", n, 0, n);
  h2->SetMarkerColor(kBlack);
  h2->SetBins(n, &binLabels.front());
  
  // Create graphs for limit plots
  TGraphAsymmErrors *gr_obs = new TGraphAsymmErrors(h1);;
  TGraphAsymmErrors *gr_exp = new TGraphAsymmErrors(h2);
  TGraphAsymmErrors *gr_exp_error = new TGraphAsymmErrors(h2);
  TGraphAsymmErrors *gr_exp_error2 = new TGraphAsymmErrors(h2);

  // Loop through the input files
  for(int i = 1; i < rootapp->Argc(); ++i) {

    std::string filename = std::string(rootapp->Argv(i));
    std::cout << "Filename: " << filename << std::endl;

    // Extract labels from file name string
    TObjArray* subjectArray = TString(filename).Tokenize("_");
    TString tmp = ((TObjString*)subjectArray->At(subjectArray->GetLast()))->GetString().Copy();

    cate = tmp.Contains(".S0") ? tmp.ReplaceAll(".S0.lmt","") : tmp.ReplaceAll(".lmt","");

    // Set up maping between event category and file handler 
    m.insert(std::pair<std::string, std::ifstream*>(cate.Data(),new std::ifstream(filename.c_str())));

    cateIndex = std::distance(cateLabelConvert.begin(), cateLabelConvert.find(cate.Data()));
    
    // Loop through file based on event category
    while(*m[cate.Data()]) {

      // Extract line from file
      std::getline(*m[cate.Data()], line);

      // Stop if not found
      if(!m[cate.Data()]) break;

      // Continue if line is a comment line
      if(line[0] == '#') continue;

      // Extract limit (obs, exp, 1 and 2 signma bands) values from the input file
      if(TPRegexp("Observed Limit: r < (\\w+.\\w+)").Match(TString(line))) {
	TObjArray *subStringArray = TPRegexp("Observed Limit: r < (\\w+.\\w+)").MatchS(TString(line));
	obsLimit = ((TObjString *)subStringArray->At(1))->GetString().Atof();
      }
      else if(TPRegexp("Expected 50.0%: r < (\\w+.\\w+)").Match(TString(line))) {
	TObjArray *subStringArray = TPRegexp("Expected 50.0%: r < (\\w+.\\w+)").MatchS(TString(line));
	expLimit = ((TObjString *)subStringArray->At(1))->GetString().Atof();
      }
      else if(TPRegexp("Expected 16.0%: r < (\\w+.\\w+)").Match(TString(line))) {
	TObjArray *subStringArray = TPRegexp("Expected 16.0%: r < (\\w+.\\w+)").MatchS(TString(line));
	expLimitm1Sigma = ((TObjString *)subStringArray->At(1))->GetString().Atof();
      }
      else if(TPRegexp("Expected 84.0%: r < (\\w+.\\w+)").Match(TString(line))) {
	TObjArray *subStringArray = TPRegexp("Expected 84.0%: r < (\\w+.\\w+)").MatchS(TString(line));
	expLimitp1Sigma = ((TObjString *)subStringArray->At(1))->GetString().Atof();
      }
      else if(TPRegexp("Expected  2.5%: r < (\\w+.\\w+)").Match(TString(line))) {
        TObjArray *subStringArray = TPRegexp("Expected  2.5%: r < (\\w+.\\w+)").MatchS(TString(line));
	expLimitm2Sigma = ((TObjString *)subStringArray->At(1))->GetString().Atof();
      }
      else if(TPRegexp("Expected 97.5%: r < (\\w+.\\w+)").Match(TString(line))) {
	TObjArray *subStringArray = TPRegexp("Expected 97.5%: r < (\\w+.\\w+)").MatchS(TString(line));
	expLimitp2Sigma = ((TObjString *)subStringArray->At(1))->GetString().Atof();
      }
    }
    
    // Print to terminal the values exracted for the limits
    std::cout << TString::Format("CLs: %f\t%f (%f, %f) : (%f, %f)\n", obsLimit, expLimit, expLimitm1Sigma, expLimitp1Sigma, expLimitm2Sigma, expLimitp2Sigma);
    std::cout << TString::Format("%s\t%.1f\t%.1f$^{+%.1f}_{-%.1f}$\n\n", cateLabelConvert[cate.Data()].c_str(), obsLimit, expLimit, expLimitp1Sigma-expLimit , expLimit-expLimitm1Sigma);
    
    // Set bin content for observed
    h1->SetBinContent(cateIndex+1, obsLimit);
    h2->SetBinContent(cateIndex+1, expLimit);

    // Set bin content for observed      
    gr_obs->SetPoint(cateIndex, cateIndex + 0.5, h1->GetBinContent(cateIndex+1));
    gr_obs->SetPointEYlow(cateIndex, 0.0); 
    gr_obs->SetPointEYhigh(cateIndex, 0.0);

    // Set bin content for expected
    gr_exp->SetPoint(cateIndex, cateIndex + 0.5, h2->GetBinContent(cateIndex+1));
    gr_exp->SetPointEYlow(cateIndex, 0.0); 
    gr_exp->SetPointEYhigh(cateIndex, 0.0);
    
    // Set bin content for 1 sigma band for expected
    gr_exp_error->SetPoint(cateIndex, cateIndex + 0.5, h2->GetBinContent(cateIndex+1));
    gr_exp_error->SetPointEYlow(cateIndex, std::fabs(expLimit-expLimitm1Sigma));
    gr_exp_error->SetPointEYhigh(cateIndex, std::fabs(expLimit-expLimitp1Sigma));

    // Set bin content for 2 sigma band for expected  
    gr_exp_error2->SetPoint(cateIndex, cateIndex + 0.5, h2->GetBinContent(cateIndex+1));
    gr_exp_error2->SetPointEYlow(cateIndex, std::fabs(expLimit-expLimitm2Sigma));
    gr_exp_error2->SetPointEYhigh(cateIndex, std::fabs(expLimit-expLimitp2Sigma));
  }

  // Set plot category index
  gr_exp_error2->GetHistogram()->SetBins(n, &binLabels.front());

  // Set plot category labels
  for (std::map<std::string, std::string>::iterator iter = cateLabelConvert.begin(); iter != cateLabelConvert.end(); ++iter) {

    Int_t i = std::distance(cateLabelConvert.begin(),iter);

    gr_exp_error2->GetXaxis()->SetBinLabel(i+1, cateLabelConvert.at(iter->first).c_str());
  }

  // Yellow band
  gr_exp_error2->Draw("Z2A*");
  gr_exp_error2->SetMarkerStyle(21);
  gr_exp_error2->SetMarkerSize(0.3);
  gr_exp_error2->SetMarkerColor(kYellow);
  gr_exp_error2->SetFillStyle(1005);
  gr_exp_error2->SetFillColor(kYellow);

  gr_exp_error2->GetYaxis()->SetTitle("95% CL limit #sigma/#sigma_{SM} at m_{H} = "+TString(mass)+" GeV");
  gr_exp_error2->GetYaxis()->SetTitleOffset(1.0);
  gr_exp_error2->GetYaxis()->SetTitleFont(42);
  gr_exp_error2->GetYaxis()->SetTitleSize(0.05); // 0.055
  gr_exp_error2->GetYaxis()->SetLabelOffset(0.005);
  gr_exp_error2->GetYaxis()->SetLabelFont(42);
  gr_exp_error2->GetYaxis()->SetLabelSize(0.04); // 0.05

  //gr_exp_error2->GetXaxis()->SetTitle("Included categories"); //doesn't fit on the canvas when labels are rotated...
  gr_exp_error2->GetXaxis()->SetTitle("");
  gr_exp_error2->GetXaxis()->SetTitleOffset(1.0); // 1.2
  gr_exp_error2->GetXaxis()->SetTitleFont(42);
  gr_exp_error2->GetXaxis()->SetTitleSize(0.05); // 0.055
  gr_exp_error2->GetXaxis()->SetLabelOffset(0.005); 
  gr_exp_error2->GetXaxis()->SetLabelFont(42);
  gr_exp_error2->GetXaxis()->SetLabelSize(0.04); // 0.055
  gr_exp_error2->GetXaxis()->LabelsOption("h");
  gr_exp_error2->GetXaxis()->SetRangeUser(0, n-0.05);
  
  gr_exp_error2->GetXaxis()->LabelsOption("d");

  gr_exp_error2->SetLineStyle(2);
  gr_exp_error2->SetLineWidth(2);

  // Green band
  gr_exp_error->Draw("Z2*same");
  gr_exp_error->SetMarkerStyle(21);
  gr_exp_error->SetMarkerSize(0.3);
  gr_exp_error->SetMarkerColor(kGreen);
  gr_exp_error->SetFillStyle(1005);
  gr_exp_error->SetFillColor(kGreen);
  
  gr_exp_error->SetLineStyle(2);
  gr_exp_error->SetLineWidth(2);

  // Expected limit
  gr_exp->Draw("Z*same");
  gr_exp->SetMarkerStyle(21);
  gr_exp->SetMarkerColor(kGreen);
  gr_exp->SetMarkerSize(0.3);
  
  gr_exp->SetLineStyle(2);
  gr_exp->SetLineWidth(2);

  // Observed limit
  gr_obs->Draw("Z*same");
  gr_obs->SetMarkerStyle(21);
  gr_obs->SetMarkerColor(kBlack);
  gr_obs->SetMarkerSize(1.1);

  // Setting maximum y-axis range
  Double_t yaxis_min = 0.05;
  Double_t yaxis_max = 1.0;

  // Set max and min values
  yaxis_min = TMath::Min(gr_obs->GetHistogram()->GetMinimum(), gr_exp_error2->GetHistogram()->GetMinimum());
  yaxis_max = TMath::Max(gr_obs->GetHistogram()->GetMaximum(), gr_exp_error2->GetHistogram()->GetMaximum());
  

  gr_exp_error2->SetMaximum(10.0*yaxis_max); // 1.5
//  gr_exp_error2->SetMinimum(0.5*yaxis_min);  // 1.0
  gr_exp_error2->SetMinimum(0.5);  //hardcoded

  // About to draw legend  
  TLegend *leg = new TLegend(0.65, 0.63, 0.94, 0.87, "", "brNDC");
  leg->SetTextFont(42);
  leg->SetFillColor(0);
  leg->SetShadowColor(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.045);

  leg->AddEntry(gr_obs, "Observed", "lep");
  leg->AddEntry(gr_exp_error, "Expected #pm 1 #sigma", "lf");
  leg->AddEntry(gr_exp_error2, "Expected #pm 2 #sigma", "lf");
  
  leg->Draw();
  
  return 0;
}

TCanvas* ttHPlot(TApplication* rootapp) {

  gROOT->LoadMacro("macros/tdrstyle.C");
  gROOT->LoadMacro("macros/CMS_lumi.C");

  int iPeriod = 0;          // 0=free form (uses lumi_sqrtS), 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV
  int iPos = 11;

  setTDRStyle();

  writeExtraText = false;   // false = remove Preliminary

  int W = 800;
  int H = 600;

  int H_ref = 600; 
  int W_ref = 800; 

  // references for T, B, L, R
  float T = 0.08*H_ref;
  float B = 0.12*H_ref; 
  float L = 0.12*W_ref;
  float R = 0.04*W_ref;

  //TString canvName = "ttH_hbb_13TeV_dl";
  //TString canvName = "ttH_hbb_13TeV_dl_BDTvsMEM_bdtMedianP023_LowAndHigh_nostat";
  //TString canvName = "ttH_hbb_13TeV_dl4t3t_BDTvsMEM_bdtMedianP023_LowAndHigh_nostat";
  //TString canvName = "ttH_hbb_13TeV_dl_2D_catremoval_ratePSscale_lepSF_unblind_InternalUnblinding";
  TString canvName = "ttH_hbb_13TeV_dl_unblinded_newMEM_newBDTinputs";

  if (writeExtraText) {
    canvName += "-prelim";
  }

  TCanvas *canv = new TCanvas(canvName, canvName, 50, 50, W, H);
  canv->SetFillColor(0);
  canv->SetBorderMode(0);
  canv->SetFrameFillStyle(0);
  canv->SetFrameBorderMode(0);
  canv->SetLeftMargin(L/W);
  canv->SetRightMargin(R/W);
  canv->SetTopMargin(T/H);
  canv->SetBottomMargin(B/H);
  canv->SetLogy(1);
  canv->cd();

  // Plotting ttH limits
  plotLimit(rootapp);

  // writing the lumi information and the CMS "logo"
  CMS_lumi(canv, iPeriod, iPos);

  canv->Update();
  canv->RedrawAxis();
  canv->GetFrame()->Draw();

  canv->Print(canvName+".pdf",".pdf");

  return canv;
}

int main(int argc, char* argv[]) {

  TApplication* rootapp = new TApplication("example", &argc, argv);

  ttHPlot(rootapp);

  //this crashes...
//  delete rootapp;
//  rootapp = NULL;

  return 0;
}
