/*
  pshape.cc
  
  Toshiyuki Gogami, November 27, 2017
*/

void pshape_allch(int runnum=5035){
  // ====================================== //
  // ========= General conditions ========= //
  // ====================================== //
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  //int inte = 1;
  int run = runnum;
  //int ch  = channel;
  
  const int nch = 64;
  //const int n   = 74;
  const int n   = 400;
  //const int n   = 400;

  int evID;
  int nn[nch];
  double time[nch][n];
  double ph[nch][n];
    
  // ========================== //
  // ==== Open ROOT file ====== //
  // ========================== //
  char inputfilename[500];
  sprintf(inputfilename,"./rootfiles2/acsis_%d.root",run);
  TFile* f1 = new TFile(inputfilename);
  TTree* t1 = (TTree*)f1->Get("tree");
  //  t1->SetBranchAddress("evID",  &evID);
  //t1->SetBranchAddress("nent",  &nn);
  t1->SetBranchAddress("time",  &time);
  t1->SetBranchAddress("ph",    &ph);
  double ent = t1->GetEntries();
  //  ent = 30; // ------ The number of events analyzed ------


  int xbin = n;
  //double xmin = 0.0, xmax = 800.0;
  double xmin = 0.0, xmax = n*4.0; // 4 ns per ch (250 MHz)
  TH1F* h = new TH1F("h","",xbin,xmin,xmax);
  TH1F* haccum[nch];
  TH1F* htmp[nch];
  TF1* func1[nch];

  // ========================================== //
  // ======= Pedestal position evaluation ===== //
  // ========================================== //
  int nite = 100; // the number of events accumulated
  for(int i=0 ; i<nch ; i++){
    char hname[500];
    sprintf(hname,"haccum_%d",i);
    haccum[i] = new TH1F(hname,"",xbin,xmin,xmax);
    sprintf(hname,"htemp_%d",i);
    htmp[i] = (TH1F*)haccum[i]->Clone(hname);
    htmp[i]->GetYaxis()->SetRangeUser(-50.,0.);
    char fname[500];
    sprintf(fname,"func1_%d",i);
    func1[i]  = new TF1(fname,"[0]",0.0,xmax);
    func1[i]->SetParameter(0, 250.0);
  }
  //TCanvas* c0 = new TCanvas("c0","c0");
  //c0->Divide(8,4);
  //c0->cd();
  //htmp = (TH1F*)haccum[0]->Clone("htmp");
  for(int i=0 ; i<nite ; i++){
    t1->GetEntry(i);
    for(int j=0 ; j<nch ; j++){
      for(int k=0 ; k<n ; k++){
	htmp[j]->SetBinContent(k+1,ph[j][k]);
	//htmp[j]->SetBinError(k+1,0.0);
      }
      haccum[j]->Add(htmp[j]);
    }
  }
  for(int i=0 ; i<nch ; i++){
    haccum[i]->Scale(1./nite);
    char fname[500];
    //sprintf(hname,"haccum_%d",i);
    sprintf(fname,"func1_%d",i);
    haccum[i]->Fit(fname,"Nq","",0.0,xmax);
    //c0->cd(i+1);
    //haccum[i]->Draw();
    //func1[i]->Draw("same");
    cout << i << ": " << func1[i]->GetParameter(0) << " "
	 << func1[i]->GetParError(0) << endl;;
  }

  
  
  TCanvas* c1 = new TCanvas("c1","c1");
  //c1->Divide(6,6);
  TCanvas* c2 = new TCanvas("c2","c2");
  //c2->Divide(6,6);
  c1->Divide(6,6);
  c2->Divide(6,6);

  // add condition for updating canvas
  bool condition = false;
  
  // add counter of no of events
  Int_t no_of_events[nch] = {0};

;
 TH1* chan_hist = new TH1F("chan_hits","hits in channels",nch,0,nch);



  char pname[500];
  double ymax = 100.;
  double ymin = -20.;
  double offset;// = func1[ch]->GetParameter(0);
  for(int i=0 ; i<ent ; i++){
    t1->GetEntry(i);
    condition = false;
    c1->Divide(6,6);
    c2->Divide(6,6);


    for(int k=0 ; k<nch ; k++){
      offset = func1[k]->GetParameter(0);
      condition = false;
      for(int j=0 ; j<n ; j++){
	//h->SetBinContent(j+1,ph[ch][j]);
	//h->SetBinContent(j+1, ph[ch][j]-offset);
	htmp[k]->SetBinContent(j+1, ph[k][j]-offset);
	if( ph[k][j]-offset > 100){
	  condition = true;
	}
      }

      if(condition){
      chan_hist->Fill(k+1);
      }

      if(k<32){
	c1->cd(k+1);
	htmp[k]->Draw();
      }
      else if (k<64){
	c2->cd(k+1-32);
	htmp[k]->Draw();
      }
      ymin = -50.;
      ymax = 250.;
      htmp[k]->SetMinimum(ymin);
      htmp[k]->SetMaximum(ymax);
    }
    //h->SetMinimum(250.);
    //TLine l(0.0,ymin,xmax,ymax);
    //l.Draw("same");
    //cout << ch << ": "  << i+1<< endl;
    //h->Draw();

    if( condition){
      c1->Update();c2->Update();
      c2->WaitPrimitive();
    }

    c1->Clear();c2->Clear();

    //cin >> temp;
    
    sprintf(pname,"pshape_r%d-%d_00-31.png",run,i+1);
    //c1->Print(pname);
    sprintf(pname,"pshape_r%d-%d_32-63.png",run,i+1);
    //c2->Print(pname);
    
    cout << i << endl;
  }


  
}

