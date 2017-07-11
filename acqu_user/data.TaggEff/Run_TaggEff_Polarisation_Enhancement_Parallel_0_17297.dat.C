{
//=========Macro generated from canvas: genCanvas/genCanvas
//=========  (Wed Jun  7 11:53:57 2017) by ROOT version5.34/36
   TCanvas *genCanvas = new TCanvas("genCanvas", "genCanvas",52,73,800,1000);
   gStyle->SetOptStat(0);
   genCanvas->Range(0,0,1,1);
   genCanvas->SetFillColor(0);
   genCanvas->SetBorderMode(0);
   genCanvas->SetBorderSize(2);
   genCanvas->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: genCanvas_1
   TPad *genCanvas_1 = new TPad("genCanvas_1", "genCanvas_1",0.01,0.51,0.99,0.99);
   genCanvas_1->Draw();
   genCanvas_1->cd();
   genCanvas_1->Range(0,0,1,1);
   genCanvas_1->SetFillColor(0);
   genCanvas_1->SetBorderMode(0);
   genCanvas_1->SetBorderSize(2);
   genCanvas_1->SetGridx();
   genCanvas_1->SetGridy();
   genCanvas_1->SetFrameBorderMode(0);
   genCanvas_1->Modified();
   genCanvas->cd();
  
// ------------>Primitives in pad: genCanvas_2
   genCanvas_2 = new TPad("genCanvas_2", "genCanvas_2",0.01,0.01,0.99,0.49);
   genCanvas_2->Draw();
   genCanvas_2->cd();
   genCanvas_2->Range(0,0,1,1);
   genCanvas_2->SetFillColor(0);
   genCanvas_2->SetBorderMode(0);
   genCanvas_2->SetBorderSize(2);
   genCanvas_2->SetGridx();
   genCanvas_2->SetGridy();
   genCanvas_2->SetFrameBorderMode(0);
   genCanvas_2->Modified();
   genCanvas->cd();
   genCanvas->Modified();
   genCanvas->cd();
   genCanvas->SetSelected(genCanvas);
}
