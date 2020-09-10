#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TString.h>
#include <TMath.h>
#include <TF1.h>
#include <TPaveText.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "aggregate.h"

using namespace std;

TString exptName(Int_t snailNum){
  if(snailNum < 100 || snailNum == 500) return "prex";
  else return "crex";
}

void initBranches(TTree* cyc){
  cyc->SetBranchAddress("snailNum", &snailNum);
  cyc->SetBranchAddress("runNum", &runNum);
  cyc->SetBranchAddress("cycleNum", &cycleNum);
  cyc->SetBranchAddress("firstOffStartMPS", &fOffMPS1);
  cyc->SetBranchAddress("firstOffEndMPS", &fOffMPS2);
  cyc->SetBranchAddress("onStartMPS", &onMPS1);
  cyc->SetBranchAddress("onEndMPS", &onMPS2);
  cyc->SetBranchAddress("lastOffStartMPS", &lOffMPS1);
  cyc->SetBranchAddress("lastOffEndMPS", &lOffMPS2);
  cyc->SetBranchAddress("cycleTime", &cycleTime);
  cyc->SetBranchAddress("AnalyzingPower", &anPow);

  for(Int_t i = 0; i < nPols; i++){PolVar pol; pols0.push_back(pol);}
  for(Int_t i = 0; i < nAccVars; i++){DataVar data; vars0.push_back(data);}
  for(Int_t i = 0; i < nPols; i++){PolVar pol; pols4.push_back(pol);}
  for(Int_t i = 0; i < nAccVars; i++){DataVar data; vars4.push_back(data);}
  for(Int_t i = 0; i < nVars; i++){DataVar data; vars.push_back(data);}

  for(Int_t i = 0; i < nPols; i++){cyc->SetBranchAddress(polNames0[i].Data(), &pols0[i]);}
  for(Int_t i = 0; i < nAccVars; i++){cyc->SetBranchAddress(varNames0[i].Data(), &vars0[i]);}
  for(Int_t i = 0; i < nPols; i++){cyc->SetBranchAddress(polNames4[i].Data(), &pols4[i]);}
  for(Int_t i = 0; i < nAccVars; i++){cyc->SetBranchAddress(varNames4[i].Data(), &vars4[i]);}
  for(Int_t i = 0; i < nVars; i++){cyc->SetBranchAddress(varNames[i].Data(), &vars[i]);}
}

void setPol0Strs(Int_t i){
  Float_t par = fits0[i]->GetParameter(0);
  Float_t parErr = fits0[i]->GetParError(0);
  Float_t chi2 = fits0[i]->GetChisquare();
  Int_t ndf = fits0[i]->GetNDF();

  pol0Texts[i]->AddText(Form("--------Fit Results--------"));
  pol0Texts[i]->AddText(Form("Mean +/- Err: %.3f +/- %.3f", par, parErr));
  if(pol0Pcts[i])
    pol0Texts[i]->AddText(Form("Rel. Error: %.3f%%", 100.0*parErr/par));
  pol0Texts[i]->AddText(Form("Chi^2 / ndf: %f / %d", chi2, ndf));
  pol0Texts[i]->SetBorderSize(1); pol0Texts[i]->SetFillColor(0);
}

void setPol4Strs(Int_t i){
  Float_t par = fits4[i]->GetParameter(0);
  Float_t parErr = fits4[i]->GetParError(0);
  Float_t chi2 = fits4[i]->GetChisquare();
  Int_t ndf = fits4[i]->GetNDF();

  pol4Texts[i]->AddText(Form("--------Fit Results--------"));
  pol4Texts[i]->AddText(Form("Mean +/- Err: %.3f +/- %.3f", par, parErr));
  if(pol4Pcts[i])
    pol4Texts[i]->AddText(Form("Rel. Error: %.3f%%", 100.0*parErr/par));
  pol4Texts[i]->AddText(Form("Chi^2 / ndf: %f / %d", chi2, ndf));
  pol4Texts[i]->SetBorderSize(1); pol4Texts[i]->SetFillColor(0);
}

void makePol0Plots(Int_t snlNum, Int_t *msmt){
  TString snailName = Form("snail%i", snlNum);
  TString outputDir(Form("%s/snails/%s", getenv("COMPMON_WEB"), snailName.Data()));
  for(Int_t i = 0; i < nPols; i++){
    TCanvas *c = new TCanvas(Form("cPol_%s", polNames0[i].Data()), "Pol Canvas", 1200, 600);
    TPad *pPol1 = new TPad(Form("pPol1_%s", polNames0[i].Data()), "Pol Avg", 0.0, 0.3, 0.7, 1.0);
    pPol1->SetGridx(1); pPol1->SetGridy(1);
    TPad *pPol2 = new TPad(Form("pPol2_%s", polNames0[i].Data()), "Pol Pull Plot", 0.7, 0.0, 1.0, 1.0);
    TPad *pPol3 = new TPad(Form("pPol3_%s", polNames0[i].Data()), "Pol Pull Graph", 0.0, 0.0, 0.7, 0.3);
    pPol3->SetGridx(1); pPol3->SetGridy(1);
    pPol1->Draw(); pPol2->Draw(); pPol3->Draw();

    pPol1->cd();
    pol0Hists[i]->Fit(fits0[i], "Q", "", 0, nCycles);
    Float_t par = fits0[i]->GetParameter(0);
    for(Int_t j = 0; j < nCycles; j++){
      Float_t nDiffs = (pol0Hists[i]->GetBinContent(j + 1) - par)/pol0Hists[i]->GetBinError(j + 1);
      pol0Pull2Ds[i]->SetBinContent(j + 1, nDiffs); pol0Pulls[i]->Fill(nDiffs);
    }
    
    setPol0Strs(i);
    pol0Hists[i]->SetStats(0);
    pol0Hists[i]->Draw("P");
    pol0Texts[i]->Draw("same");

    pPol2->cd();
    pol0Pulls[i]->SetStats(220);
    pol0Pulls[i]->Draw();
    
    pPol3->cd();
    pol0Pull2Ds[i]->SetStats(0);
    pol0Pull2Ds[i]->Draw();

    c->Print(Form("%s/agg_plots_%04i.pdf", outputDir.Data(), (*msmt)++), "pdf");
  }
  //return msmt;
}

void makePol4Plots(Int_t snlNum, Int_t *msmt){
  TString snailName = Form("snail%i", snlNum);
  TString outputDir(Form("%s/snails/%s", getenv("COMPMON_WEB"), snailName.Data()));
  for(Int_t i = 0; i < nPols; i++){
    TCanvas *c = new TCanvas(Form("cPol_%s", polNames4[i].Data()), "Pol Canvas", 1200, 600);
    TPad *pPol1 = new TPad(Form("pPol1_%s", polNames4[i].Data()), "Pol Avg", 0.0, 0.3, 0.7, 1.0);
    pPol1->SetGridx(1); pPol1->SetGridy(1);
    TPad *pPol2 = new TPad(Form("pPol2_%s", polNames4[i].Data()), "Pol Pull Plot", 0.7, 0.0, 1.0, 1.0);
    TPad *pPol3 = new TPad(Form("pPol3_%s", polNames4[i].Data()), "Pol Pull Graph", 0.0, 0.0, 0.7, 0.3);
    pPol3->SetGridx(1); pPol3->SetGridy(1);
    pPol1->Draw(); pPol2->Draw(); pPol3->Draw();

    pPol1->cd();
    pol4Hists[i]->Fit(fits4[i], "Q", "", 0, nCycles);
    Float_t par = fits4[i]->GetParameter(0);
    for(Int_t j = 0; j < nCycles; j++){
      Float_t nDiffs = (pol4Hists[i]->GetBinContent(j + 1) - par)/pol4Hists[i]->GetBinError(j + 1);
      pol4Pull2Ds[i]->SetBinContent(j + 1, nDiffs); pol4Pulls[i]->Fill(nDiffs);
    }
    
    setPol4Strs(i);
    pol4Hists[i]->SetStats(0);
    pol4Hists[i]->Draw("P");
    pol4Texts[i]->Draw("same");

    pPol2->cd();
    pol4Pulls[i]->SetStats(220);
    pol4Pulls[i]->Draw();
    
    pPol3->cd();
    pol4Pull2Ds[i]->SetStats(0);
    pol4Pull2Ds[i]->Draw();

    c->Print(Form("%s/agg_plots_%04i.pdf", outputDir.Data(), (*msmt)++), "pdf");
  }
  //return msmt;
}

void makeVar0Plots(Int_t snlNum, Int_t *msmt){
  TString snailName = Form("snail%i", snlNum);
  TString outputDir(Form("%s/snails/%s", getenv("COMPMON_WEB"), snailName.Data()));
  for(Int_t i = 0; i < 2*nAccVars; i++){
    TString canName = Form("cAccVar_%s", varNames0[(Int_t)i/2].Data());
    if(i % 2 == 1)
      canName = Form("cAccVar_%s_RMS", varNames0[(Int_t)i/2].Data());
    printf("MSMT #%i: %s\n", *msmt, canName.Data());
    TCanvas *c = new TCanvas(canName.Data(), "Acc Var Canvas", 1200, 400);
    c->SetGridx(1); c->SetGridy(1);
    var0Hists[i]->SetStats(0);
    var0Hists[i]->Draw("P");
    c->Print(Form("%s/agg_plots_%04i.pdf", outputDir.Data(), (*msmt)++), "pdf");
  }
}

void makeVar4Plots(Int_t snlNum, Int_t *msmt){
  TString snailName = Form("snail%i", snlNum);
  TString outputDir(Form("%s/snails/%s", getenv("COMPMON_WEB"), snailName.Data()));
  for(Int_t i = 0; i < 2*nAccVars; i++){
    TString canName = Form("cAccVar_%s", varNames4[(Int_t)i/2].Data());
    if(i % 2 == 1)
      canName = Form("cAccVar_%s_RMS", varNames4[(Int_t)i/2].Data());
    printf("MSMT #%i: %s\n", *msmt, canName.Data());
    TCanvas *c = new TCanvas(canName.Data(), "Acc Var Canvas", 1200, 400);
    c->SetGridx(1); c->SetGridy(1);
    var4Hists[i]->SetStats(0);
    var4Hists[i]->Draw("P");
    c->Print(Form("%s/agg_plots_%04i.pdf", outputDir.Data(), (*msmt)++), "pdf");
  }
}

void makeVarPlots(Int_t snlNum, Int_t *msmt){
  TString snailName = Form("snail%i", snlNum);
  TString outputDir(Form("%s/snails/%s", getenv("COMPMON_WEB"), snailName.Data()));
  for(Int_t i = 0; i < 2*nAccVars; i++){
    TString canName = Form("cAccVar_%s", varNames[(Int_t)i/2].Data());
    if(i % 2 == 1)
      canName = Form("cAccVar_%s_RMS", varNames[(Int_t)i/2].Data());
    printf("MSMT #%i: %s\n", *msmt, canName.Data());
    TCanvas *c = new TCanvas(canName.Data(), "Var Canvas", 1200, 400);
    c->SetGridx(1); c->SetGridy(1);
    varHists[i]->SetStats(0);
    varHists[i]->Draw("P");
    c->Print(Form("%s/agg_plots_%04i.pdf", outputDir.Data(), (*msmt)++), "pdf");
  }
}

void aggregateGrand(Int_t snlNum){
  TFile *grand = new TFile(Form("%s/%sGrandCompton.root", getenv("COMPMON_GRAND"), exptName(snlNum).Data()), "READ");
  TTree *cyc = (TTree *)grand->Get("cyc");

  nCycles = (Int_t)cyc->GetEntries(Form("snailNum==%i", snlNum));
  printf("Number of cycles: %i\n", nCycles);
  initHists(snlNum);
  initFits();
  initBranches(cyc);
  
  Int_t nBin = 1;
  for(Int_t i = 0; i < cyc->GetEntries(); i++){
    cyc->GetEntry(i);
    if(snailNum != snlNum) continue;
    for(Int_t j = 0; j < nPols; j++){
      pol0Hists[j]->SetBinContent(nBin, pols0[j].mean*pol0Mults[j]);
      pol0Hists[j]->SetBinError(nBin, pols0[j].meanErr*pol0Mults[j]);
      pol4Hists[j]->SetBinContent(nBin, pols4[j].mean*pol4Mults[j]);
      pol4Hists[j]->SetBinError(nBin, pols4[j].meanErr*pol4Mults[j]);
      if(nBin % 3 == 1){
        pol0Hists[j]->GetXaxis()->SetBinLabel(nBin, Form("%i.%i", runNum, cycleNum));
        pol4Hists[j]->GetXaxis()->SetBinLabel(nBin, Form("%i.%i", runNum, cycleNum));
      }
    }
    for(Int_t j = 0; j < 2*nAccVars; j+=2){
      var0Hists[j]->SetBinContent(nBin, vars0[(Int_t)j/2].mean);
      var0Hists[j]->SetBinError(nBin, vars0[(Int_t)j/2].meanErr);
      var4Hists[j]->SetBinContent(nBin, vars4[(Int_t)j/2].mean);
      var4Hists[j]->SetBinError(nBin, vars4[(Int_t)j/2].meanErr);
      var0Hists[j+1]->SetBinContent(nBin, vars0[(Int_t)j/2].rms);
      var0Hists[j+1]->SetBinError(nBin, vars0[(Int_t)j/2].rmsErr);
      var4Hists[j+1]->SetBinContent(nBin, vars4[(Int_t)j/2].rms);
      var4Hists[j+1]->SetBinError(nBin, vars4[(Int_t)j/2].rmsErr);
      if(nBin % 3 == 1){
        var0Hists[j]->GetXaxis()->SetBinLabel(nBin, Form("%i.%i", runNum, cycleNum));
        var4Hists[j]->GetXaxis()->SetBinLabel(nBin, Form("%i.%i", runNum, cycleNum));
        var0Hists[j+1]->GetXaxis()->SetBinLabel(nBin, Form("%i.%i", runNum, cycleNum));
        var4Hists[j+1]->GetXaxis()->SetBinLabel(nBin, Form("%i.%i", runNum, cycleNum));
      }
    }
    for(Int_t j = 0; j < 2*nVars; j+=2){
      varHists[j]->SetBinContent(nBin, vars[(Int_t)j/2].mean);
      varHists[j]->SetBinError(nBin, vars[(Int_t)j/2].meanErr);
      varHists[j+1]->SetBinContent(nBin, vars[(Int_t)j/2].rms);
      varHists[j+1]->SetBinError(nBin, vars[(Int_t)j/2].rmsErr);
      if(nBin % 3 == 1){
        varHists[j]->GetXaxis()->SetBinLabel(nBin, Form("%i.%i", runNum, cycleNum));
      }
    }
    nBin++;
  }

  Int_t msmt = 0;
  //gStyle->SetOptStat(0);
  makePol0Plots(snlNum, &msmt);
  makeVar0Plots(snlNum, &msmt);
  //makePol4Plots(snlNum, &msmt);
  //makeVar4Plots(snlNum, &msmt);
  makeVarPlots(snlNum, &msmt);

  TString outputDir = Form("%s/snails/snail%i", getenv("COMPMON_WEB"), snlNum);
  gSystem->Exec(Form("pdfunite %s/agg_plots_*.pdf %s/agg_plots.pdf", outputDir.Data(), outputDir.Data()));
  gSystem->Exec(Form("rm -f %s/agg_plots_*.pdf", outputDir.Data()));
}
