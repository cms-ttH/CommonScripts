{

   TFile *_file0 = TFile::Open("higgsCombine_forZscore.MultiDimFit.mH125.root");
   TGraph* mygraph = (TGraph*) limit->Draw("2*deltaNLL:r","quantileExpected<1 && quantileExpected>-1","L");
   //mygraph->SetLineWidth(2);
   
   TLine *l = new TLine(1,0,1,7);
   l->SetLineColor(kRed);
   l->SetLineWidth(2);
   l->Draw("same");
   
   TLine *l_1sigma = new TLine(-2,1,2,1);
   l_1sigma->SetLineColor(kBlack);
   l_1sigma->SetLineWidth(1);
   l_1sigma->SetLineStyle(kDashed);
   l_1sigma->Draw("same");
   
   TLine *l_2sigma = new TLine(-2,4,2,4);
   l_2sigma->SetLineColor(kBlack);
   l_2sigma->SetLineWidth(1);
   l_2sigma->SetLineStyle(kDashed);
   l_2sigma->Draw("same");
   
   
   limit->Scan("sqrt(2*deltaNLL):r","r==1","L");
   //limit->Scan("sqrt(2*deltaNLL):r","r>0.95 && r>1.05","L");

}
