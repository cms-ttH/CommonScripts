{   
    TFile *_file0 = TFile::Open("bkgonlyAsimov/mlfit_shapes_bkgonlyAsimov.root");
    // TString nuisances[40] = {"CMS_res_j","CMS_scale_j","CMS_ttH_CSVCErr1","CMS_ttH_CSVCErr2","CMS_ttH_CSVHF",
//     "CMS_ttH_CSVHFStats1","CMS_ttH_CSVHFStats2","CMS_ttH_CSVLF","CMS_ttH_CSVLFStats1","CMS_ttH_CSVLFStats2",
//     "CMS_ttH_PSscale_ttbarOther","CMS_ttH_PSscale_ttbarPlus2B","CMS_ttH_PSscale_ttbarPlusB",
//     "CMS_ttH_PSscale_ttbarPlusBBbar","CMS_ttH_PSscale_ttbarPlusCCbar","CMS_ttH_PU","CMS_ttH_Q2scale_ttbarOther",
//     "CMS_ttH_Q2scale_ttbarPlus2B","CMS_ttH_Q2scale_ttbarPlusB","CMS_ttH_Q2scale_ttbarPlusBBbar",
//     "CMS_ttH_Q2scale_ttbarPlusCCbar","CMS_ttH_QCDscale_ttbarPlus2B","CMS_ttH_QCDscale_ttbarPlusB",
//     "CMS_ttH_QCDscale_ttbarPlusBBbar","CMS_ttH_QCDscale_ttbarPlusCCbar","CMS_ttH_dl_Trig","CMS_ttH_eff_lepton",
//     "CMS_ttH_ljets_Trig","CMS_ttH_sl_eff_lepton","QCDscale_V","QCDscale_VV","QCDscale_singlet","QCDscale_ttH",
//     "QCDscale_ttbar","lumi_13TeV","pdf_gg","pdf_gg_ttH","pdf_qg","pdf_qqbar","r"};

    TString nuisances[32] = {"CMS_res_j","CMS_scale_j","CMS_ttH_CSVCErr1","CMS_ttH_CSVCErr2","CMS_ttH_CSVHF",
    "CMS_ttH_CSVHFStats1","CMS_ttH_CSVHFStats2","CMS_ttH_CSVLF","CMS_ttH_CSVLFStats1","CMS_ttH_CSVLFStats2","CMS_ttH_PU","CMS_ttH_Q2scale_ttbarOther",
    "CMS_ttH_Q2scale_ttbarPlus2B","CMS_ttH_Q2scale_ttbarPlusB","CMS_ttH_Q2scale_ttbarPlusBBbar",
    "CMS_ttH_Q2scale_ttbarPlusCCbar","CMS_ttH_QCDscale_ttbarPlus2B","CMS_ttH_QCDscale_ttbarPlusB",
    "CMS_ttH_QCDscale_ttbarPlusBBbar","CMS_ttH_QCDscale_ttbarPlusCCbar","CMS_ttH_dl_Trig",
    "QCDscale_V","QCDscale_VV","QCDscale_singlet","QCDscale_ttH",
    "QCDscale_ttbar","lumi_13TeV","pdf_gg","pdf_gg_ttH","pdf_qg","pdf_qqbar","r"};
    
     //TString nuisances[2] = {"CMS_res_j","r"};
     //cout<<fit_s->correlation("CMS_ttH_QCDscale_ttbarPlusB","CMS_ttH_QCDscale_ttbarPlusBBbar")<<endl;
     
     TCanvas* c1 = new TCanvas("c1","c1",1500,1200);
     c1->SetBottomMargin(5);
     c1->SetLeftMargin(5);
     
     TH2D* corrhisto = new TH2D("","",32,0,32,0,32);
     
     for(unsigned int i=0; i<32; i++)
     {
       for(unsigned int j=0; j<32; j++)
       {
          corrhisto->Fill(nuisances[i],nuisances[j],fit_s->correlation(nuisances[i],nuisances[j]));
       }
     }
     
     gStyle->SetOptStat(0);
     corrhisto->GetXaxis()->SetLabelSize(0.025);
     corrhisto->GetYaxis()->SetLabelSize(0.030);
     corrhisto->Draw("colz");

}
