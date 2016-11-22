// Final pre-fit and post-fit discriminants and yields plots for HIG-16-038
// 
// Set inputs and define categories and processes to plot at bottom
// in plotFitResults() method.
//
// Execute with root -l -b -q plotFitResults.C+

#include <cmath>
#include <exception>
#include <iostream>
#include <vector>

#include "TCanvas.h"
#include "TColor.h"
#include "TDirectoryFile.h"
#include "TError.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "THStack.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TString.h"
#include "TStyle.h"
#include "TUUID.h"


// A physics process such as tt+bb or data
// Stores process-specific information such as labels and colours
class Process {
public:
  enum Type { BKG, SIG, DATA };

  Process()
    : type_(BKG), hasScale_(false), scale_(1.) {}
  Process(const TString& theLabel, const Type theType=BKG)
    : type_(theType), hasScale_(false), scale_(1.), label_(theLabel) {}

  TString label() const { return label_; }

  unsigned int nTemplates() const { return names_.size(); }
  TString name(const unsigned int i) const { return names_.at(i); }
  Type type() const { return type_; }

  bool hasScale() const { return hasScale_; }
  double scale() const { return scale_; }
  int color() const { return color_; }

  void addTemplate(const TString& theName) {
    names_.push_back(theName);
  }
  void setScale(const double theScale) {
    hasScale_ = true;
    scale_ = theScale;
  }
  void setLabel(const TString& theLabel) {
    label_ = theLabel;
  }
  void setType(const Type theType) {
    type_ = theType;
  }
  void setColor(const int theColor) {
    color_ = theColor;
  }


private:
  std::vector<TString> names_;
  Type type_;
  bool hasScale_;
  double scale_;
  TString label_;
  int color_;
};
typedef std::vector<Process> Processes;


class Category {
public:
  enum Channel { DL, SL };

  Category()
    : xMin_(1), xMax_(-1) {}
  Category(const Channel theChannel, const TString& theLabel1, const TString& theLabel2)
    : channel_(theChannel), label1_(theLabel1), label2_(theLabel2), xMin_(1), xMax_(-1) {}

  TString name() const { return name_; }
  Channel channel() const { return channel_; }
  TString label1() const { return label1_; } // #jets, #b-tags
  TString label2() const { return label2_; } // BDT cut (for 2D)
  TString discriminantLabel() const { return discriminantLabel_; }

  bool adjustXRange() const { return xMax_ > xMin_; }
  double xMin() const { return xMin_; }
  double xMax() const { return xMax_; }

  void setName(const TString& theName) {
    name_ = theName;
  }
  void setChannel(const Channel theChannel) {
    channel_ = theChannel;
  }
  void setLabel1(const TString& theLabel) {
    label1_ = theLabel;
  }
  void setLabel2(const TString& theLabel) {
    label2_ = theLabel;
  }
  void setDiscriminantLabel(const TString& theDiscriminantLabel) {
    discriminantLabel_ = theDiscriminantLabel;
  }
  // Sets histogram range from
  // bin-min: bin including min
  // bim-max: bin including max
  void setXRange(const double min, const double max) {
    xMin_ = min;
    xMax_ = max;
  }


private:
  Channel channel_;
  TString name_;
  TString label1_;
  TString label2_;
  TString discriminantLabel_;
  double xMin_;
  double xMax_;
};
typedef std::vector<Category> Categories;


class Fit {
public:
  virtual ~Fit() {};
  virtual TString name() const = 0;
  virtual TString label() const = 0;
  virtual bool isSignalFit() const { return false; }
};
typedef std::vector<Fit*> Fits;

class PreFit : public Fit {
  TString name() const { return "shapes_prefit"; }
  TString label() const { return "pre-fit expectation"; }
};

class PostFit : public Fit {
  TString name() const { return "shapes_fit_s"; }
  TString label() const { return ""; }
  bool isSignalFit() const { return true; }
};


class LabelFactory {
 public:
  LabelFactory() {}

  TLegend* createLegend() const;
  TPaveText* createLumiLabel(const Category& category) const;
  TPaveText* createCMSLabel() const;
  TPaveText* createCategoryLabel(const Category& category) const;
  TPaveText* createFitLabel(const Fit& fit) const;
};
  
TLegend* LabelFactory::createLegend() const {
  // hardcode size of legend
  const double x1 = gStyle->GetPadLeftMargin()+0.5*(1.-gStyle->GetPadLeftMargin()-gStyle->GetPadRightMargin());
  const double dy = 0.24;

  // Defining the position of the upper right corner
  const double x2 = 1.0 - gStyle->GetPadRightMargin() - gStyle->GetTickLength() - 0.01;
  const double y2 = 1.0 - gStyle->GetPadTopMargin()  - gStyle->GetTickLength();
    
  TLegend* leg = new TLegend(x1,y2-dy,x2,y2);
  leg->SetNColumns(2);
  leg->SetColumnSeparation(0.1/2.);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextFont(42);
    
  return leg;
}

// HIG-16-038: different lumi per channel
TPaveText* LabelFactory::createLumiLabel(const Category& category) const {
  const double x1 = gStyle->GetPadLeftMargin();
  const double x2 = 1.0 - gStyle->GetPadRightMargin() + 0.045;
  const double y1 = 1.0 - gStyle->GetPadTopMargin();
  const double y2 = 1.0;
  TString lumiinfo = "2.7 fb^{-1} (13 TeV)";
  TPaveText *label = new TPaveText(x1,y1,x2,y2,"NDC");
  label->SetTextAlign(32);
  label->SetTextFont(42);
  label->SetBorderSize(0);
  label->SetFillColor(0);
  label->SetTextSize(0.04);
  if( category.channel() == Category::DL ) {
    label->AddText("11.4 - 12.9 fb^{-1} (13 TeV)");
  } else {
    label->AddText("12.9 fb^{-1} (13 TeV)");
  }
  
  return label;
}

TPaveText* LabelFactory::createCMSLabel() const {
  const double x1 = gStyle->GetPadLeftMargin()+gStyle->GetTickLength();
  const double y2 = 1.0 - gStyle->GetPadTopMargin()-gStyle->GetTickLength();
  
  TPaveText* label = new TPaveText(x1,y2-0.05,x1+0.3,y2,"NDC");
  label->SetTextAlign(12);
  label->SetTextFont(42);
  label->SetBorderSize(0);
  label->SetFillColor(0);
  label->SetTextSize(0.045);
  // Following CMS style conventions
  // https://ghm.web.cern.ch/ghm/plots/
  label->AddText("#scale[1.2]{#bf{CMS}} #it{Preliminary}");  

  return label;
}

TPaveText* LabelFactory::createCategoryLabel(const Category& category) const {
  const double x1 = gStyle->GetPadLeftMargin()+gStyle->GetTickLength();
  const double x2 = gStyle->GetPadLeftMargin()+0.5*(1.-gStyle->GetPadLeftMargin()-gStyle->GetPadRightMargin());
  const double y2 = 1.0 - gStyle->GetPadTopMargin()-gStyle->GetTickLength()-0.05;
  const double dy = 0.13;

  TPaveText* label = new TPaveText(x1,y2-dy,x2,y2,"NDC");
  label->SetTextAlign(12);
  label->SetTextFont(42);
  label->SetBorderSize(0);
  label->SetFillColor(0);
  label->SetTextSize(0.036);
  const TString line1 = category.channel()==Category::DL ? "dilepton" : "lepton+jets";
  const TString line2 = category.label1();
  const TString line3 = category.label2();
  label->AddText( TString("#splitline{"+line1+"}{#splitline{"+line2+"}{"+line3+"}}") ); 
    
  return label;
}

TPaveText* LabelFactory::createFitLabel(const Fit& fit) const {
  const double x1 = gStyle->GetPadLeftMargin()+gStyle->GetTickLength()+0.005;
  const double y2 = 1.0 - gStyle->GetPadTopMargin()-gStyle->GetTickLength()-0.16;
  
  TPaveText* label = new TPaveText(x1,y2-0.05,x1+0.3,y2,"NDC");
  label->SetTextAlign(12);
  label->SetTextFont(42);
  label->SetBorderSize(0);
  label->SetFillColor(0);
  label->SetFillStyle(0);
  label->SetTextSize(0.036);
  label->AddText(fit.label());  

  return label;
}


// Get templates from file
//
// This method knows from which file to take the templates:
// - data from DL or SL datacard input files
// - MC from combine output
// Expects category labels as in combine output; automatically
// converted to input file syntax.
//
// The MC templates are converted to the original discriminant
// binning as in the data. Data histograms are set with
// Poisson error bars.
//
// To speed up reading, create reader object once and pass _reference_.
// (Keeps files open internally and I did not write proper copy constrcutors etc.)
class TemplateReader {
public:
  TemplateReader()
    : datacardInputsDL_(0), datacardInputsSL_(0), mlfitOutput_(0) {}
  ~TemplateReader();

  void openDatacardInputsDL(const TString& name); 
  void openDatacardInputsSL(const TString& name); 
  void openMlfitOutput(const TString& name);

  TH1* getTemplate(const Fit& fit, const Category& category, const Process& process) const;
  TH1* getYields(const Fit& fit, const Categories& categories, const Process& process) const;

  
private:
  TFile* datacardInputsDL_;
  TFile* datacardInputsSL_;
  TFile* mlfitOutput_;

  TFile* open(const TString& name) const;
  TH1* getTemplateData(const Category& category) const;
  TH1* getTemplateMC(const Fit& fit, const Category& category, const Process& process) const;
  TH1* getTemplate(TFile* file, const TString& histName) const;

  void convertBinning(TH1* &h, const TH1* hOrigBinning) const;
  void replaceWithCopyWithPoissonErrors(TH1* &hOrig) const;
  void setStyle(const Process& process, TH1* &h) const;
  void adjustXRange(const Category& category, TH1* &h) const;
};


TemplateReader::~TemplateReader() {
  if( datacardInputsDL_ ) {
    datacardInputsDL_->Close();
    delete datacardInputsDL_;
  }
  if( datacardInputsSL_ ) {
    datacardInputsSL_->Close();
    delete datacardInputsSL_;
  }
  if( mlfitOutput_ ) {
    mlfitOutput_->Close();
    delete mlfitOutput_;
  }
}


void TemplateReader::openDatacardInputsDL(const TString& name) {
    datacardInputsDL_ = open(name);
  }
void TemplateReader::openDatacardInputsSL(const TString& name) { 
    datacardInputsSL_ = open(name);
  }
void TemplateReader::openMlfitOutput(const TString& name) { 
    mlfitOutput_ = open(name);
  }
TFile* TemplateReader::open(const TString& name) const {
  TFile* file = new TFile(name,"READ");
  if( !file->IsOpen() ) {
    std::cerr << "\n\nERROR: input file '" << name << "' cannot be opened\n" << std::endl;
    throw std::exception();
  }
  return file;
}


// Get TH1 of name 'histName' from file.
//
// Adds a unique string to the histogram's name, such that the
// method can be called repeatedly without having to worry about
// creating objects of same name.
//
// Throws an exception '1' if the histogram does not exist.
TH1* TemplateReader::getTemplate(TFile* file, const TString& histName) const {
  TH1* h = 0;
  file->GetObject(histName,h);
  if( h == 0 ) {
    throw 1;
  } else {
    h->SetDirectory(0);
    TString name = h->GetName();
    TUUID id;
    h->SetName(name+id.AsString());
    h->UseCurrentStyle();
  }

  return h;
}


TH1* TemplateReader::getTemplateData(const Category& category) const {
  // In case of data, take from datacards input

  // Histogram name and directory depends on channel
  TString name = "";
  TFile* file = 0;
  if( category.channel() == Category::DL ) {
    // DL channel
    name = category.name();
    name.ReplaceAll("3j","j3");
    name.ReplaceAll("ge4j","gej4");
    name.ReplaceAll("3t","_t3");
    name.ReplaceAll("ge4t","_get4");
    name += "_BDT/data_obs";
    file = datacardInputsDL_;
  } else {
    // SL channel
    TString cat = category.name(); // e.g. sl_j4_t4_high
    cat.ReplaceAll("sl_","");	   // e.g. --> j4_t4_high
    name = "data_obs_finaldiscr_ljets_"+cat;
    file = datacardInputsSL_;
  }

  try {
    return getTemplate(file,name);
  } catch(int e) {
    std::cerr << "\n\nERROR: Data template '" << name << "' not found" << std::endl;
    throw std::exception();
  }
  return 0;
}


TH1* TemplateReader::getTemplateMC(const Fit& fit, const Category& category, const Process& process) const {
  // In case of MC, take templates from mlfit.root output

  const TString path = fit.name()+"/"+category.name()+"/";
  TH1* hTemp = 0;
  for(unsigned int i = 0; i < process.nTemplates(); ++i) {
    TH1* h = 0;
    try {
      h = getTemplate(mlfitOutput_,path+process.name(i));
    } catch(int e) {
      // Some minor backgrounds do not have any events in some
      // categories and then the template does not exist. In
      // this case, replace by empty dummy histogram.
      std::cout << "Template '" << path+process.name(i) << "' not found: using empty histogram instead" << std::endl;
      h = getTemplate(mlfitOutput_,path+"total_background");
      h->Reset();      
    }
    if( hTemp == 0 ) {
      hTemp = h;
    } else {
      hTemp->Add(h);
      delete h;
    }
  }

  return hTemp;
}


void TemplateReader::adjustXRange(const Category& category, TH1* &h) const {
  if( category.adjustXRange() ) {
    const int binMin = h->FindBin(category.xMin());
    const int binMax = h->FindBin(category.xMax());
    h->GetXaxis()->SetRange(binMin,binMax);
  }
}


void TemplateReader::setStyle(const Process& process, TH1* &h) const {
  if( process.type() == Process::DATA ) {
    h->SetMarkerStyle(20);
    h->SetMarkerColor(kBlack);
    h->SetLineColor(kBlack);
    h->SetLineWidth(2);
  } else if( process.type() == Process::SIG ) {
    h->SetLineColor(process.color());
    h->SetLineWidth(3);
  } else {
    h->SetFillColor(process.color());
    h->SetLineColor(kBlack);
    h->SetLineWidth(1);
  }
  h->SetTitle("");
  // adjust for usage on top part of canves
  // with ratio pad below
  h->GetXaxis()->SetTitle("");  // will be set on ratio
  h->GetXaxis()->SetLabelSize(0);
  h->GetYaxis()->SetTickLength(gStyle->GetTickLength("Y")/0.8);
}


// Set Poisson errors for histogram
// works only for histograms with INTEGER bin content and no weights
// (this implies that TH1::Sumw2() is/has *not* been called!)
void TemplateReader::replaceWithCopyWithPoissonErrors(TH1* &hOrig) const {
  // need to make a copy (not clone!) of the histogram without
  // the sum-of-weights array (sumw2)
  // https://root.cern.ch/doc/v606/TH1_8cxx_source.html#l08442
  const TString nameOrig = hOrig->GetName();
  hOrig->SetName("tmp");
  TH1* hNew = new TH1D(nameOrig,"",
		       hOrig->GetNbinsX(),
		       hOrig->GetXaxis()->GetBinLowEdge(1),
		       hOrig->GetXaxis()->GetBinUpEdge(hOrig->GetNbinsX()));
  hNew->SetDirectory(0);
  hNew->SetMarkerStyle(hOrig->GetMarkerStyle());
  hNew->SetMarkerSize(hOrig->GetMarkerSize());
  hNew->SetMarkerColor(hOrig->GetMarkerColor());
  hNew->SetLineStyle(hOrig->GetLineStyle());
  hNew->SetLineWidth(hOrig->GetLineWidth());
  hNew->SetLineColor(hOrig->GetLineColor());
  for(int bin = 1; bin <= hNew->GetNbinsX(); ++bin) {
    const double x = hOrig->GetBinCenter(bin);
    const int n = hOrig->GetBinContent(bin);
    for(int i = 0; i < n; ++i) {
      hNew->Fill(x);
    }
  }
  delete hOrig;

  // Add support for Poisson errors in data hist:
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/PoissonErrorBars
  hNew->SetBinErrorOption(TH1::kPoisson);

  hOrig = hNew;
}


// Convert binning of histograms in mlfit.root to the actual
// discriminant's binning
// 
// In mlfit.root, templates 'h' are stored using a generic binning
// with bin width 1, starting from 0. This method rebins 'h'
// using the binning in 'hOrigBinning' from the datacard input.
void TemplateReader::convertBinning(TH1* &h, const TH1* hOrigBinning) const {
  const TString name = h->GetName();
  h->SetName("tmp");
  TH1* hNew = static_cast<TH1*>(hOrigBinning->Clone(name));
  hNew->Sumw2();
  for(int bin = 1; bin <= hNew->GetNbinsX(); ++bin) {
    hNew->SetBinContent(bin,h->GetBinContent(bin));
    hNew->SetBinError(bin,h->GetBinError(bin));
  }
  delete h;
  h = hNew;
}

TH1* TemplateReader::getTemplate(const Fit& fit, const Category& category, const Process& process) const {
  TH1* hTemp = 0;
  if( process.type() == Process::DATA ) {
    // in case of data, replace error bars with Poisson errors
    hTemp = getTemplateData(category);
    replaceWithCopyWithPoissonErrors(hTemp);

  } else {
    // in case of MC
    // - apply original discriminant's binning
    // - apply scale factors where appropriate, e.g. for signal
    hTemp = getTemplateMC(fit,category,process);

    // rebin with original binning, which can be taken from data hist
    TH1* hData = getTemplateData(category);
    convertBinning(hTemp,hData);
    delete hData;

    if( process.hasScale() ) hTemp->Scale(process.scale());

    if( process.type() == Process::SIG ) {
      for(int bin = 1; bin <= hTemp->GetNbinsX(); ++bin) {
	hTemp->SetBinError(bin,0);
      }
    }
  }
  adjustXRange(category,hTemp);
  setStyle(process,hTemp);

  return hTemp;
}


TH1* TemplateReader::getYields(const Fit& fit, const Categories& categories, const Process& process) const {
  TUUID id;
  TH1* hYields = new TH1D(TString("Yields")+id.AsString(),";;N(events)",categories.size(),0,categories.size());
  int bin = 1;
  for(auto& category: categories) {
    TH1* h = getTemplate(fit,category,process); // this is the actual template
    double yield = 0.;
    double error = 0.;
    double error2 = 0.;
    for(int tbin = 1; tbin <= h->GetNbinsX(); ++tbin) {
      yield += h->GetBinContent(tbin);
      error += h->GetBinError(tbin);
      error2 += h->GetBinError(tbin)*h->GetBinError(tbin);
    }
    // WARNING: treating errors for MC as fully correlated
    // between the template bins. THIS IS AN APPROXIMATION 
    // THAT IS MAYBE NOT OK. A correct treatment requires
    // toy sampling of the actual model
    hYields->SetBinContent(bin,yield);
    hYields->SetBinError(bin,error);
    ++bin;
  }
  if( process.type() == Process::DATA ) {
    replaceWithCopyWithPoissonErrors(hYields);
  }
  setStyle(process,hYields);

  return hYields;
}




class YieldTableRow {
public:
  void newRow(const TString& processName) {
    char txt[200];
    sprintf(txt,"%-18s",processName.Data());
    process_ = TString(txt);
    values_.clear();
  }

  void addYield(const double value, const double error) {
    char txt[50];
    sprintf(txt,"%7.1f +/- %7.1f",value,error);
    values_.push_back(TString(txt));
  }

  void addYields(const TH1* h) {
    for(int bin = 1; bin <= h->GetNbinsX(); ++bin) {
      addYield(h->GetBinContent(bin),h->GetBinError(bin));
    }
  }

  void print() const {
    std::cout << " | " << process_ << std::flush;
    for(auto& val: values_) {
      std::cout << " | " << val << std::flush;
    }
    std::cout << " |" << std::endl;
  }

private:
  TString process_;
  std::vector<TString> values_;
};



// Get TGraphErrors with same y and errors as 'h'
TGraphErrors* getErrorBand(const TH1* h) {
  std::vector<double> posX;
  std::vector<double> posY;
  std::vector<double> errX;
  std::vector<double> errY;
  for(int bin = 1; bin <= h->GetNbinsX(); ++bin) {
    const double binMin = h->GetXaxis()->GetBinLowEdge(bin);
    const double binMax = h->GetXaxis()->GetBinUpEdge(bin);
    posX.push_back( 0.5*(binMax+binMin) );
    posY.push_back( h->GetBinContent(bin) );
    errX.push_back( std::abs(0.5*(binMax-binMin)) );
    errY.push_back( h->GetBinError(bin) ); 
  }

  TGraphErrors* g = new TGraphErrors( posX.size(),
				      &(posX.front()),
				      &(posY.front()),
				      &(errX.front()),
				      &(errY.front())  );
  g->SetMarkerStyle(0);
  g->SetFillStyle(3354);
  g->SetFillColor(kBlack);
  g->SetLineColor(kBlack);

  return g;
}


double getYMax(const TH1* h) {
  double ymax = 0.;
  for(int bin = 1; bin <= h->GetNbinsX(); ++bin) {
    double y = h->GetBinContent(bin) + h->GetBinError(bin);
    if( y > ymax ) ymax = y;
  }
  
  return ymax;
}


TPad* createRatioPad() {
  TPad* pad = new TPad("bpad","",0,0,1,1);
  pad->SetTopMargin(0.8 - 0.8*pad->GetBottomMargin()+0.2*pad->GetTopMargin());
  pad->SetFillStyle(0);
  pad->SetFrameFillColor(10);
  pad->SetFrameBorderMode(0);

  return pad;
}


TGraphAsymmErrors* createRatio(const TH1* hData, const TH1* hBkg) {
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> xe;
  std::vector<double> yeup;
  std::vector<double> yedn;
  for(int bin = 1; bin <= hData->GetNbinsX(); ++bin) {
    if( hData->GetBinContent(bin)>0. && hBkg->GetBinContent(bin)>0. ) {
      x.push_back(hData->GetBinCenter(bin));
      xe.push_back(0.);
      const double yData = hData->GetBinContent(bin);
      const double yBkg = hBkg->GetBinContent(bin);
      y.push_back(yData/yBkg);
      yeup.push_back(hData->GetBinErrorUp(bin)/yBkg);
      yedn.push_back(hData->GetBinErrorLow(bin)/yBkg);
    }
  }
  TGraphAsymmErrors* g = new TGraphAsymmErrors(x.size(),
                                               &(x.front()),&(y.front()),
                                               &(xe.front()),&(xe.front()),
                                               &(yedn.front()),&(yeup.front())  );
  g->SetMarkerStyle(hData->GetMarkerStyle());
  g->SetMarkerSize(hData->GetMarkerSize());

  return g;
}


TGraphAsymmErrors* createRatioErrorBand(TH1* hBkg) {
  TGraphAsymmErrors* g = createRatio(hBkg,hBkg);
  for(int i = 0; i < g->GetN(); ++i) {
    const int bin = hBkg->FindBin(g->GetY()[i]);
    g->GetEXlow()[i] = 0.5*hBkg->GetBinWidth(bin);
    g->GetEXhigh()[i] = 0.5*hBkg->GetBinWidth(bin);
  }
  g->SetMarkerStyle(0);
  g->SetFillStyle(3354);
  g->SetFillColor(kBlack);
  g->SetLineColor(kBlack);  

  return g;
}


TH1* createSignalRatio(const TH1* hSig, const TH1* hBkg, const Process& signal) {
  TH1* hRS = static_cast<TH1*>(hSig->Clone("hRatioSignal"));
  if( signal.hasScale() ) {
    hRS->Scale(1./signal.scale());
  }
  hRS->Add(hBkg);
  hRS->Divide(hBkg);

  return hRS;
}


TH1* createRatioFrame(const TH1* hMain, const double range) {
  TH1* hRatioFrame = static_cast<TH1*>( hMain->Clone("RatioFrame") );
  for(int xBin = 1; xBin <= hRatioFrame->GetNbinsX(); ++xBin) {
    hRatioFrame->SetBinContent(xBin,1.);
  }
  hRatioFrame->SetLineStyle(2);
  hRatioFrame->SetLineWidth(1);
  hRatioFrame->SetFillColor(0);
  hRatioFrame->GetYaxis()->SetNdivisions(205);
  hRatioFrame->GetYaxis()->SetTickLength(gStyle->GetTickLength("Y")/0.2);
  hRatioFrame->GetXaxis()->SetLabelSize(gStyle->GetLabelSize("X"));
  hRatioFrame->GetYaxis()->CenterTitle();
  hRatioFrame->GetYaxis()->SetRangeUser(1.-range,1.+range);

  return hRatioFrame;
}


// Draw the discriminant in a given category
void drawCategory(const TemplateReader& reader,
		  const Fit& fit,
		  const Category& category,
		  const Processes& bkgs,
		  const Process& signal) {
  const LabelFactory lFactory;

  // create legend
  TLegend* leg = lFactory.createLegend();

  // get data
  TH1* hData = reader.getTemplate(fit,category,Process("data",Process::DATA));
  hData->GetYaxis()->SetTitle("Events");
  leg->AddEntry(hData,"data","PE");

  // get signal
  TH1* hSig = reader.getTemplate(fit,category,signal);
  TString sigEntry = "";
  if( fit.isSignalFit() ) {
    sigEntry = signal.label() + "(#mu=-0.19)";
  } else {
    if( signal.hasScale() ) {
      sigEntry += signal.scale();
      sigEntry += " #times ";
    }
    sigEntry += signal.label();
  }
  leg->AddEntry(hSig,sigEntry,"L");

  // stack backgrounds
  double totBkgYield = 0.;
  std::vector<TH1*> hBkgs;
  for(auto& background: bkgs) {
    TH1* h = reader.getTemplate(fit,category,background);
    totBkgYield += h->Integral();
    leg->AddEntry(h,background.label(),"F");
    hBkgs.push_back(h);
  }
  THStack* sBkgs = new THStack("sBkgs","");
  for(std::vector<TH1*>::const_reverse_iterator it = hBkgs.rbegin();
      it != hBkgs.rend(); ++it) {
    sBkgs->Add(*it);
  }
  // get total background (for errors and ratio)
  Process pBkgTot("total background");
  pBkgTot.addTemplate("total_background");
  TH1* hBkgTot = reader.getTemplate(fit,category,pBkgTot);
  if( std::abs( hBkgTot->Integral() - totBkgYield )/hBkgTot->Integral() > 1E-4 ) {
    std::cerr << "\n\nERROR: Total Bkg != Stack: " << fit.name()+"/"+category.name() << std::endl;
    std::cerr << "  total_background = " << hBkgTot->Integral() << std::endl;
    std::cerr << "             stack = " << totBkgYield << std::endl;
    throw std::exception();
  }
  TGraphErrors* gErrorBand = getErrorBand(hBkgTot);
  //  hBkgTot->SetMarkerStyle(21);

  // adjust y-axis range
  const double ymaxSig = getYMax(hSig);
  const double ymaxBkg = getYMax(hBkgTot);
  double ymax = 1.1*getYMax(hData); // add extra 10% to account for assymmetric errors (Poisson)
  if( ymaxSig > ymax ) ymax = ymaxSig;
  if( ymaxBkg > ymax ) ymax = ymaxBkg;
  hData->GetYaxis()->SetRangeUser(2E-3,1.7*ymax);

  // create ratios
  TGraphAsymmErrors* gRatioDataToBkg = createRatio(hData,hBkgTot);
  TGraphAsymmErrors* gRatioErrorBand = createRatioErrorBand(hBkgTot);
  TH1* hRatioSignal = createSignalRatio(hSig,hBkgTot,signal);

  // labels
  TPaveText* labelLumi = lFactory.createLumiLabel(category);
  TPaveText* labelCMS = lFactory.createCMSLabel();
  TPaveText* labelCategory = lFactory.createCategoryLabel(category);
  TPaveText* labelFit = lFactory.createFitLabel(fit);

  TCanvas* can = new TCanvas("can","",600,750);
  can->SetBottomMargin(0.2 + 0.8*can->GetBottomMargin()-0.2*can->GetTopMargin());
  can->cd();
  hData->Draw("PE1");
  sBkgs->Draw("HISTsame");
  gErrorBand->Draw("E2same");
  //hBkgTot->Draw("PE1same");
  hData->Draw("PE1same");
  hSig->Draw("HISTsame");
  leg->Draw("same");
  labelLumi->Draw("same");
  labelCMS->Draw("same");
  labelCategory->Draw("same");
  labelFit->Draw("same");

  gPad->RedrawAxis();

  // draw ratio plot relative to BG-only or S+B fit result
  TPad* ratioPad = createRatioPad();
  ratioPad->Draw();
  ratioPad->cd();
  
  TH1* hRatioFrame = createRatioFrame(hData,0.72);
  hRatioFrame->Draw("HIST");
  hRatioFrame->GetXaxis()->SetTitle(category.discriminantLabel()+" discriminant");
  hRatioFrame->GetYaxis()->SetTitle("Data/Bkg.");
  gRatioErrorBand->Draw("E2same");
  gRatioDataToBkg->Draw("PE0same");
  gPad->RedrawAxis();

  TString outName = "HIG-16-038_FinalDiscriminant_";
  if( fit.name() == "shapes_prefit" ) outName += "prefit_";
  else outName += "postfit-s_";
  outName += category.name();
  can->SaveAs("plots/"+outName+".pdf");

  delete sBkgs;
  delete hBkgTot;
  delete hSig;
  delete hData;
  delete hRatioFrame;
  delete gRatioErrorBand;
  delete gRatioDataToBkg;
  delete hRatioSignal;
  delete leg;
  delete labelCMS;
  delete labelLumi;
  delete labelCategory;
  delete labelFit;
  delete can;
}



// Draw and print the yields per category
void drawYields(TemplateReader& reader,
		const Fit& fit,
		const Categories& categories,
		const Processes& bkgs,
		const Process& signal,
		const TString& outID) {

  if( categories.size() > 0 ) {
    const LabelFactory lFactory;

    // create legend
    TLegend* leg = lFactory.createLegend();
    
    // get data
    TH1* hData = reader.getYields(fit,categories,Process("data",Process::DATA));
    hData->GetYaxis()->SetTitle("Events");
    leg->AddEntry(hData,"data","PE");
    
    // get signal
    TH1* hSig = reader.getYields(fit,categories,signal);
    TString sigEntry = "";
    if( fit.isSignalFit() ) {
      sigEntry = signal.label() + "(#mu=-0.19)";
    } else {
      if( signal.hasScale() ) {
	sigEntry += signal.scale();
	sigEntry += " #times ";
      }
      sigEntry += signal.label();
    }
    leg->AddEntry(hSig,sigEntry,"L");
    
    // stack backgrounds
    std::vector<TH1*> hBkgs;
    TH1* hBkgTot = 0;
    for(auto& background: bkgs) {
      TH1* h = reader.getYields(fit,categories,background);
      leg->AddEntry(h,background.label(),"F");
      hBkgs.push_back(h);
      if( hBkgTot == 0 ) {
	hBkgTot = static_cast<TH1*>(h->Clone("hBkgTotYields"));
      } else {
	for(int bin = 1; bin <= hBkgTot->GetNbinsX(); ++bin) {
	  const double y1 = hBkgTot->GetBinContent(bin);
	  const double y2 = h->GetBinContent(bin);
	  const double e1 = hBkgTot->GetBinError(bin);
	  const double e2 = h->GetBinError(bin);
	  hBkgTot->SetBinContent(bin,y1+y2);
	  // WARNING: treating errors for MC as fully uncorrelated
	  // between the processes because the major systematcis
	  // (50% xs, Q2scale) are. THIS IS AN APPROXIMATION 
	  // THAT IS MAYBE NOT OK. A correct treatment requires
	  // toy sampling of the actual model.
	  hBkgTot->SetBinError(bin, sqrt( e1*e1 + e2*e2 ) );
	}
      }
    }
    hBkgTot->SetMarkerStyle(24);
    THStack* sBkgs = new THStack("sBkgsYields","");
    for(std::vector<TH1*>::const_reverse_iterator it = hBkgs.rbegin();
	it != hBkgs.rend(); ++it) {
      sBkgs->Add(*it);
    }
    TGraphErrors* gErrorBand = getErrorBand(hBkgTot);
    //  hBkgTot->SetMarkerStyle(21);
    
    // adjust y-axis range (hardcoded for the time being)
    hData->GetYaxis()->SetRangeUser(8E-1,1E6);
    
    // create ratios
    TGraphAsymmErrors* gRatioDataToBkg = createRatio(hData,hBkgTot);
    TGraphAsymmErrors* gRatioErrorBand = createRatioErrorBand(hBkgTot);
    
    // labels
    TPaveText* labelLumi = lFactory.createLumiLabel(categories.front());
    TPaveText* labelCMS = lFactory.createCMSLabel();

    const double x1 = gStyle->GetPadLeftMargin()+gStyle->GetTickLength();
    const double x2 = gStyle->GetPadLeftMargin()+0.5*(1.-gStyle->GetPadLeftMargin()-gStyle->GetPadRightMargin());
    const double y2 = 1.0 - gStyle->GetPadTopMargin()-gStyle->GetTickLength()-0.08;
    const double dy = 0.04;
    TPaveText* labelChannel = new TPaveText(x1,y2-dy,x2,y2,"NDC");
    labelChannel->SetTextAlign(12);
    labelChannel->SetTextFont(42);
    labelChannel->SetBorderSize(0);
    labelChannel->SetFillColor(0);
    labelChannel->SetTextSize(0.036);
    const TString line = categories.front().channel()==Category::DL ? "dilepton" : "lepton+jets";
    labelChannel->AddText(line);

    TPaveText* labelFit = lFactory.createFitLabel(fit);
    labelFit->SetY1(labelFit->GetY1()+0.05);
    labelFit->SetY2(labelFit->GetY2()+0.05);
  
    gStyle->SetPadBottomMargin(0.15);
    TCanvas* can = new TCanvas("canYields","",600,750);
    can->SetBottomMargin(0.32);

    can->cd();
    hData->Draw("PE1");
    sBkgs->Draw("HISTsame");
    gErrorBand->Draw("E2same");
    //    hBkgTot->Draw("PE1same");
    hData->Draw("PE1same");
    hSig->Draw("HISTsame");
    leg->Draw("same");
    labelLumi->Draw("same");
    labelCMS->Draw("same");
    labelFit->Draw("same");
    labelChannel->Draw("same");
    
    gPad->RedrawAxis();
    gPad->SetLogy();
    
    // draw ratio plot relative to BG-only or S+B fit result
    TPad* pad = new TPad("rpad","",0,0,1,1);
    pad->SetTopMargin(0.68);
    pad->SetFillStyle(0);
    pad->SetFrameFillColor(10);
    pad->SetFrameBorderMode(0);
    pad->Draw();
    pad->cd();
    
    TH1* hRatioFrame = createRatioFrame(hData,0.72);
    hRatioFrame->Draw("HIST");
    int bin = 1;
    hRatioFrame->GetXaxis()->SetTitle("");
    for(auto& category: categories) {
      const TString label1 = category.label1(); // these are the jet-tag labels
      const TString line1 = label1( 0, label1.First(",") ); // jet
      const TString line2 = label1( label1.First(",")+2,99 );  // tags
      const TString line3 = category.label2()+" ";
      hRatioFrame->GetXaxis()->SetBinLabel(bin,"#splitline{#splitline{"+line1+"}{"+line2+" }}{"+line3+"}");
      ++bin;
    }
    hRatioFrame->GetXaxis()->LabelsOption("v");
    hRatioFrame->GetXaxis()->SetLabelSize(0.04);
    hRatioFrame->GetYaxis()->SetTitle("Data/Bkg.");
    gRatioErrorBand->Draw("E2same");
    gRatioDataToBkg->Draw("PE0same");
    gPad->RedrawAxis();
    
    TString outName = "HIG-16-038_Yields_";
    outName += outID+"_";
    if( fit.name() == "shapes_prefit" ) outName += "prefit";
    else outName += "postfit-s";
    can->SaveAs("plots/"+outName+".pdf");

    // print yield table
    // | process | cat1 | cat2 | ... |
    std::cout << "\n\n\nYIELDS "+outID+" "+fit.label() << std::endl;
    YieldTableRow row;
    for(size_t iproc = 0; iproc < bkgs.size(); ++iproc) {
      row.newRow(bkgs.at(iproc).label());
      row.addYields(hBkgs.at(iproc));
      row.print();
    }
    row.newRow("Total bkg");
    row.addYields(hBkgTot);
    row.print();
    row.newRow("ttH");
    row.addYields(hSig);
    row.print();
    row.newRow("Data");
    row.addYields(hData);
    row.print();

    
    delete sBkgs;
    delete hBkgTot;
    delete hSig;
    delete hData;
    delete hRatioFrame;
    delete gRatioErrorBand;
    delete gRatioDataToBkg;
    delete leg;
    delete labelCMS;
    delete labelLumi;
    delete labelFit;
    delete can;

    gStyle->SetPadBottomMargin(0.12);
  }
}


// will draw the yields separately for DL and SL channels
void drawYieldsPerChannel(TemplateReader& reader,
			  const Fit& fit,
			  const Categories& categories,
			  const Processes& bkgs,
			  const Process& signal) {
  Categories categoriesDL;
  Categories categoriesSL;
  for(auto& category: categories) {
    if( category.channel() == Category::DL ) categoriesDL.push_back(category);
    else                                     categoriesSL.push_back(category);
  }
  drawYields(reader,fit,categoriesDL,bkgs,signal,"DL");
  drawYields(reader,fit,categoriesSL,bkgs,signal,"SL");
}


void setStyle() {
  // Suppress message when canvas has been saved
  gErrorIgnoreLevel = 1001;

  // Zero horizontal error bars
  gStyle->SetErrorX(0);

  //  For the canvas
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetCanvasDefH(800); //Height of canvas
  gStyle->SetCanvasDefW(800); //Width of canvas
  gStyle->SetCanvasDefX(0);   //Position on screen
  gStyle->SetCanvasDefY(0);
  
  //  For the frame
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(10);
  gStyle->SetFrameFillColor(kBlack);
  gStyle->SetFrameFillStyle(0);
  gStyle->SetFrameLineColor(kBlack);
  gStyle->SetFrameLineStyle(0);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetLineWidth(2);
    
  //  For the Pad
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(kWhite);
  gStyle->SetPadGridX(false);
  gStyle->SetPadGridY(false);
  gStyle->SetGridColor(0);
  gStyle->SetGridStyle(3);
  gStyle->SetGridWidth(1);
  
  //  Margins
  gStyle->SetPadTopMargin(0.06);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadLeftMargin(0.18);
  gStyle->SetPadRightMargin(0.05);

  //  For the histo:
  gStyle->SetHistLineColor(kBlack);
  gStyle->SetHistLineStyle(0);
  gStyle->SetHistLineWidth(2);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetEndErrorSize(4);
  gStyle->SetHatchesLineWidth(1);

  //  For the statistics box:
  gStyle->SetOptStat(0);
  
  //  For the axis
  gStyle->SetAxisColor(1,"XYZ");
  gStyle->SetTickLength(0.024,"X");
  gStyle->SetTickLength(0.03,"Y");
  gStyle->SetNdivisions(510,"XYZ");
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetStripDecimals(kFALSE);
  
  //  For the axis labels and titles
  gStyle->SetTitleColor(1,"XYZ");
  gStyle->SetLabelColor(1,"XYZ");
  gStyle->SetLabelFont(42,"XYZ");
  gStyle->SetLabelOffset(0.007,"XYZ");
  gStyle->SetLabelSize(0.04,"XYZ");
  gStyle->SetTitleFont(42,"XYZ");
  gStyle->SetTitleSize(0.047,"XYZ");
  gStyle->SetTitleXOffset(1.2);
  gStyle->SetTitleYOffset(1.9);

  //  For the legend
  gStyle->SetLegendBorderSize(0);
}



void plotFitResults() {
  setStyle();

  // DEFINE INPUT FILES /////////////////////////////////////
  TemplateReader reader;
  reader.openDatacardInputsDL("inputs/ttH_hbb_13TeV_dl.root");
  reader.openDatacardInputsSL("inputs/ttH_hbb_13TeV_sl.root");
  reader.openMlfitOutput("inputs/mlfitNamesShapes.root");



  // DEFINE PROCESSES ///////////////////////////////////////

  // backgrounds
  Process ttbarOther("t#bar{t}+LF");
  ttbarOther.addTemplate("ttbarOther");
  ttbarOther.setColor(kRed-7);

  Process ttbarPlusCCbar("t#bar{t}+c#bar{c}");
  ttbarPlusCCbar.addTemplate("ttbarPlusCCbar");
  ttbarPlusCCbar.setColor(kRed+1);

  Process ttbarPlusB("t#bar{t}+b");
  ttbarPlusB.addTemplate("ttbarPlusB");
  ttbarPlusB.setColor(kRed-2);

  Process ttbarPlus2B("t#bar{t}+2b");
  ttbarPlus2B.addTemplate("ttbarPlus2B");
  ttbarPlus2B.setColor(kRed+2);

  Process ttbarPlusBBbar("t#bar{t}+b#bar{b}");
  ttbarPlusBBbar.addTemplate("ttbarPlusBBbar");
  ttbarPlusBBbar.setColor(kRed+3);

  Process singlet("single-t");
  singlet.addTemplate("singlet");
  singlet.setColor(kMagenta);

  Process ttbarV("t#bar{t}+V");
  ttbarV.addTemplate("ttbarW");
  ttbarV.addTemplate("ttbarZ");
  ttbarV.setColor(kBlue-10);

  Process vjets("V+jets");
  vjets.addTemplate("zjets");
  vjets.addTemplate("wjets");
  vjets.setColor(kGreen-3);

  Process diboson("diboson");
  diboson.addTemplate("diboson");
  diboson.setColor(kBlue);

  // add processes to plotted backgrounds
  Processes bkgs = {
    ttbarOther,
    ttbarPlusCCbar,
    ttbarPlusB,
    ttbarPlus2B,
    ttbarPlusBBbar,
    singlet,
    ttbarV,
    vjets,
    diboson
  };

  // signal
  Process signal("t#bar{t}H",Process::SIG);
  signal.addTemplate("total_signal");
  signal.setScale(15.);
  signal.setColor(kAzure+7);

  // data is retrieved automatically and does not need to
  // be specified



  // DEFINE CATEGORIES //////////////////////////////////////
  
  // DL categories
  Category dl_3j3t(Category::DL,"3 jets, 3 b-tags","");
  dl_3j3t.setName("dl_3j3t");
  dl_3j3t.setDiscriminantLabel("BDT");
  dl_3j3t.setXRange(-1.,0.4);

  Category dl_ge4j3t_high(Category::DL,"#geq4 jets, 3 b-tags","BDT>0.23");
  dl_ge4j3t_high.setName("dl_ge4j3t_high");
  dl_ge4j3t_high.setDiscriminantLabel("MEM");

  Category dl_ge4j3t_low(Category::DL,"#geq4 jets, 3 b-tags","BDT<0.23");
  dl_ge4j3t_low.setName("dl_ge4j3t_low");
  dl_ge4j3t_low.setDiscriminantLabel("MEM");

  Category dl_ge4jge4t_high(Category::DL,"#geq4 jets, #geq4 b-tags","BDT>0.23");
  dl_ge4jge4t_high.setName("dl_ge4jge4t_high");
  dl_ge4jge4t_high.setDiscriminantLabel("MEM");

  Category dl_ge4jge4t_low(Category::DL,"#geq4 jets, #geq4 b-tags","BDT<0.23");
  dl_ge4jge4t_low.setName("dl_ge4jge4t_low");
  dl_ge4jge4t_low.setDiscriminantLabel("MEM");


  // SL categories
  Category sl_j4_t4_high(Category::SL,"4 jets, 4 b-tags","BDT>0.26");
  sl_j4_t4_high.setName("sl_j4_t4_high");
  sl_j4_t4_high.setDiscriminantLabel("MEM");

  Category sl_j4_t4_low(Category::SL,"4 jets, 4 b-tags","BDT<0.26");
  sl_j4_t4_low.setName("sl_j4_t4_low");
  sl_j4_t4_low.setDiscriminantLabel("MEM");

  Category sl_j5_tge4_high(Category::SL,"5 jets, #geq4 b-tags","BDT>0.26");
  sl_j5_tge4_high.setName("sl_j5_tge4_high");
  sl_j5_tge4_high.setDiscriminantLabel("MEM");

  Category sl_j5_tge4_low(Category::SL,"5 jets, #geq4 b-tags","BDT<0.26");
  sl_j5_tge4_low.setName("sl_j5_tge4_low");
  sl_j5_tge4_low.setDiscriminantLabel("MEM");

  Category sl_jge6_t3_high(Category::SL,"#geq6 jets, 3 b-tags","BDT>0.12");
  sl_jge6_t3_high.setName("sl_jge6_t3_high");
  sl_jge6_t3_high.setDiscriminantLabel("MEM");

  Category sl_jge6_t3_low(Category::SL,"#geq6 jets, 3 b-tags","BDT<0.12");
  sl_jge6_t3_low.setName("sl_jge6_t3_low");
  sl_jge6_t3_low.setDiscriminantLabel("MEM");

  Category sl_jge6_tge4_high(Category::SL,"#geq6 jets, #geq4 b-tags","BDT>0.13");
  sl_jge6_tge4_high.setName("sl_jge6_tge4_high");
  sl_jge6_tge4_high.setDiscriminantLabel("MEM");

  Category sl_jge6_tge4_low(Category::SL,"#geq6 jets, #geq4 b-tags","BDT<0.13");
  sl_jge6_tge4_low.setName("sl_jge6_tge4_low");
  sl_jge6_tge4_low.setDiscriminantLabel("MEM");


  // add all considered categories
  Categories categories = {
    dl_3j3t,
    dl_ge4j3t_low,
    dl_ge4j3t_high,
    dl_ge4jge4t_low,
    dl_ge4jge4t_high,
    sl_jge6_t3_low,
    sl_jge6_t3_high,
    sl_j4_t4_low,
    sl_j4_t4_high,
    sl_j5_tge4_low,
    sl_j5_tge4_high,
    sl_jge6_tge4_low,
    sl_jge6_tge4_high
  };



  // DEFINE FITS ////////////////////////////////////////////

  // considered fits
  Fits fits = { new PreFit(), new PostFit() };


  // do plots
  for(auto& fit: fits) { // loop over fits

    //discriminant per category
    for(auto& category: categories) {
      drawCategory(reader,*fit,category,bkgs,signal);
    }

    // yields
    drawYieldsPerChannel(reader,*fit,categories,bkgs,signal);
  }

}
