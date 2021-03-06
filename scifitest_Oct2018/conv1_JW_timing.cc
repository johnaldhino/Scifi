/*
  "conv1.cc"
  
  This macro converts fADC ROOT files 
  into another one with different variable leaves. 
  
  Toshiyuki Gogami, November 27, 2017
*/

#include <iostream>
#include <fstream>
#include <chrono>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TStyle.h>

using namespace std;
using namespace std::chrono;

void conv1_JW_timing(int runnum = 4600, int dataflag = 0){
  // =================================== //
  // ====== General conditions ========= //
  // =================================== //
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  
  high_resolution_clock::time_point ti_start = high_resolution_clock::now();
  // ================================================ //
  // ====== Parameters and variable definitions ===== //
  // ================================================ //
  int flag = dataflag;
  int run = runnum;
  //const int n = 74;     // The number of data samples (250 MHz sampling = 4 ns per ch)
  const int n = 24;     // The number of data samples (250 MHz sampling = 4 ns per ch)
  //const int n = 400;     // The number of data samples (250 MHz sampling = 4 ns per ch)
  //const int n = 25; 
  const int nch = 64;   // The number of channels (16ch * 2)
  int nn[nch];          // The number of data which are really stored  */
  //double time[nch][n];  // Time in ns
  double time[nch];      // Time at peak in ns
  double wtime[nch];     // Weighted time in ns
  double ph[nch][n];    // Pulse height
  int evID = 0;         // The number of event
  int nscifi[nch];      // Fiber number
  double intc[nch];     // Integrated charge
  double intc_raw[nch];     // Integrated charge
  
  
  // ============================= //
  // ====== Read Channel map ===== //
  // ============================= //
  char chmapdata[500];
  sprintf(chmapdata,"SciFi_chmap.dat");
  // ifstream* ifs = new ifstream("SciFi_chmap.dat");
  ifstream* ifs = new ifstream(chmapdata);
  cout << " _________________________________________" << endl;
  cout << " Channel map, " << chmapdata << " is used." << endl;
  cout << " _________________________________________" << endl;


  high_resolution_clock::time_point ti1 = high_resolution_clock::now();

  for(int i=0 ; i<nch ; i++){
    int ttemp, ttemp2;
    char ttempc[500];
    *ifs >> ttemp >> ttempc >> ttemp2 >> nscifi[i];
    nscifi[i] = nscifi[i] - 1; // Subtraction by 1 because # starts from 1
  }
  ifs->close();
  high_resolution_clock::time_point ti2 = high_resolution_clock::now();
  
  // =============================== //
  // ====== Read pedestal data ===== //
  // =============================== //
  char peddata[500];
  sprintf(chmapdata,"ped_integrals/pedestal_%d.dat",run);
  ifstream* dragon_ped = new ifstream(chmapdata);
  double pedch[nch];
  double RMSch[nch];
  double ttemp2;
  for(int i=0 ; i<nch ; i++){
    *dragon_ped >> ttemp2 >> pedch[i] >> RMSch[i];
    //cout << pedch[i] << endl;
    //pedch[i] = 0.0;
  }
  dragon_ped->close();
  //  std::cout << "I read the ped dat file!" << std::endl;
  
  
  // ================================== //
  // ====== Open input ROOT file  ===== //
  // ================================== //

  TH1F* hph[nch];

  char inputfilename[500];

  int xbin = n; 
  double xmin = 0.0, xmax = n*4.0;

  sprintf(inputfilename,"./rootfiles/fadcACSIS_%d.root",run);
  TFile* f1 = new TFile(inputfilename);
  TTree* t1 = (TTree*)f1->Get("T");

  for(int i=0 ; i<nch ; i++){ //JW if condition to get 00, 01, 02 etc (match with 1st root file
    char name[500];
    if( i <10){
    sprintf(name,"Ndata.sbs.hcal.a.raw0%d",i); // remove .2
    }
    else{
    sprintf(name,"Ndata.sbs.hcal.a.raw%d",i); // remove .2
    }
    t1->SetBranchAddress(name,&nn[i]);   

    if( i< 10){
      sprintf(name,"sbs.hcal.a.raw0%d",i);
    }
    else{
      sprintf(name,"sbs.hcal.a.raw%d",i);
    }
    t1->SetBranchAddress(name,&ph[i]);

    char hname[500];
    sprintf(hname,"hph%d",i);
    hph[i] = new TH1F(hname,"", xbin, xmin, xmax);

  }
  
  // std::cout << " I defined an input tree! " << std::endl;

  // ================================== //
  // ====== Create a new ROOT file  === //
  // ================================== //
  char outputfilename[500];
  char tempc[500];
  if(flag==0){
    // JW: added JW to start of root file name 
    sprintf(outputfilename,"./rootfiles2/acsis_%d.root",run);
  }
  else {
    // JW: added JW to start of root file name 
    sprintf(outputfilename,"./rootfiles2/charge_acsis_%d.root",run);
  }
  TFile* fnew = new TFile(outputfilename,"recreate");
  TTree* tnew = new TTree("tree","APEX SciFi test at EEL122 in 2018");
  tnew->Branch("runID",  &run,  "runID/I");
  tnew->Branch("evID",   &evID, "evID/I");
  sprintf(tempc,"nn[%d]/I", nch);         tnew->Branch("nent",  &nn,    tempc);
   std::cout << "I defined branches of a n output tree!" << std::endl;
  if(flag==0){
    //sprintf(tempc,"time[%d][%d]/D", nch, n);tnew->Branch("time",  &time,  tempc);
    sprintf(tempc,"ph[%d][%d]/D", nch, n);  tnew->Branch("ph",    &ph,    tempc);
  }
  sprintf(tempc,"time[%d]/D",   nch);  tnew->Branch("time",  &time,  tempc);
  sprintf(tempc,"wtime[%d]/D",  nch);  tnew->Branch("wtime", &wtime, tempc);
  sprintf(tempc,"intc[%d]/D",   nch);  tnew->Branch("intc",  &intc,  tempc);
  sprintf(tempc,"intc_raw[%d]/D",   nch);  tnew->Branch("intc_raw",  &intc_raw,  tempc);
  sprintf(tempc,"nscifi[%d]/I", nch);  tnew->Branch("nscifi",&nscifi,tempc);
  
  std::cout << "I fully defined output tree again" << std::endl;

  //  TH1F* hph[nch];
  // int xbin = n; 
  // double xmin = 0.0, xmax = n*4.0;
  double ph_max = 0.0;
  int ph_max_bin = 0;
  
  // ------------------------------- //
  // ---- Integration gate --------- //
  // ---- (intmax-intmin)*4.0 ns --- //
  // ------------------------------- //
  int intrange_min = 0;  // default integration range 
  int intrange_max = n; // default integration range
  int diffch1, diffch2;  // exception channel (for cable length test)
  int nbin_wtime = 10; // bin range for weighted time

  if (run==4664){
    diffch1 = 48;
    diffch2 = 49;
  }
  else if (run==4665 || run==4653|| run==4654 || 
	   run==4655 || run==4662 ){
    diffch1 = 0;
    diffch2 = 1;
  }
  else{
    diffch1 = -2;
    diffch2 = -2;
  }
  
  //  std::cout << "Just before getting entries!" << std::endl;
  double ent = t1->GetEntries();
  //  std::cout << "Just after getting entries " << std::endl;

  high_resolution_clock::time_point ti_calc1 = high_resolution_clock::now();

  high_resolution_clock::time_point ti_GE1;
  high_resolution_clock::time_point ti_GE2;

  high_resolution_clock::time_point ti_Weight1;
  high_resolution_clock::time_point ti_Weight2;

  high_resolution_clock::time_point ti_Int1;
  high_resolution_clock::time_point ti_Int2;

  high_resolution_clock::time_point ti_calc_ch_1;
  high_resolution_clock::time_point ti_calc_ch_2;

  high_resolution_clock::time_point ti_filling_1;     
  high_resolution_clock::time_point ti_filling_2;      


  for(int i=0 ; i<ent ; i++){
    evID++;
    // ----- Intializing variables ----- //
    for(int j=0 ; j<nch ; j++){

      nn[j]    = -2;
      intc[j]  = -2222.0;
      intc_raw[j]  = -2222.0;
      time[j]  = -2222.0;
      wtime[j] = -2222.0;
      for(int k=0 ; k<n   ; k++){
	//time[j][k] = -2222.0;
	ph[j][k]   = -2222.0;
      }
    }
    ph_max = 0.0;
    
    // -------------------------------- //
    // ------- Start filling data ----- //
    // -------------------------------- //
    
    //    std::cout << "just before getting individual entry " << std::endl;
    
    ti_GE1 = high_resolution_clock::now();
    t1->GetEntry(i);
    ti_GE2 = high_resolution_clock::now();

    //    std::cout << "just after getting indiv entry !!! "<< std::endl;

    for(int j=0 ; j<nch ; j++){

      ti_calc_ch_1 = high_resolution_clock::now();      
      
      // -------- Exception handling for cable length test ------ //
      ti_filling_1 = high_resolution_clock::now();      
      
      if(diffch1>-1 && diffch2>-1){
	if(j==diffch1 || j==diffch2){ 
	  intrange_min = 56;
	  intrange_max = 72;
	}
	else{ 
	  intrange_min = 5;
	  intrange_max = 21;
	}
      }
      
      // char hname[500];
      // sprintf(hname,"hph%d",j);
      // hph[j] = new TH1F(hname,"", xbin, xmin, xmax);
      ph_max = 0.0;
      ph_max_bin = 1;
      for(int k=0 ; k<n ; k++){
	//time[j][k] = k*4.0; // 4 ns per channel
	//hph[j]->SetBinContent(k+1,ph[j][k]); // before pedestal subtraction
	hph[j]->SetBinContent(k+1,ph[j][k]); // after pedestal subtraction
	
	// ------ Time at peak -------- ///
	if(intrange_min<k-1 && k<intrange_max+1){
	  if(ph_max<ph[j][k]){
	    ph_max = ph[j][k];
	    ph_max_bin = k;
	    time[j] = ph_max_bin*4.0; // time at peak (ns)
	  } 
	}
      }
      

      ti_filling_2 = high_resolution_clock::now();      

      // ---- Weighted time calculation  ----- //
      double temp100=0.0, temp200=0.0;
      int tempbin = 0;
      double tempn   = 0;

      ti_Weight1 = high_resolution_clock::now();

      for(int k=0 ; k<nbin_wtime ; k++){
	tempbin = k - nbin_wtime/2 + ph_max_bin;
	if(0<tempbin && tempbin<n){
	  temp100 = temp100 + (ph[j][tempbin])*tempbin;
	  //temp200 = temp200 + tempbin;
	  temp200 = temp200 + ph[j][tempbin];
	  tempn   = tempn + 1.0; // no use now
	}
      }
      wtime[j] = temp100/temp200*4.0; // weighted time (ns)
      
      ti_Weight2 = high_resolution_clock::now();
      // ----- Integrated charge ------- //

      ti_Int1 = high_resolution_clock::now();

      intc[j] = (hph[j]->Integral(intrange_min,intrange_max))-pedch[j];
      intc_raw[j] = (hph[j]->Integral(intrange_min,intrange_max));
      hph[j]->Clear(); // reset 
      ti_Int2 = high_resolution_clock::now();
      
      ti_calc_ch_2 = high_resolution_clock::now();      
    }
    //    std::cout <<"just before filling new tree!" << std::endl;
    tnew->Fill();

  }


  high_resolution_clock::time_point ti_calc2 = high_resolution_clock::now();

  // ---- Write data and close the new ROOT file ---- //

  std::cout << "checkpoint" << std::endl;
  tnew->Write();
  fnew->Close();

  auto duration = duration_cast<microseconds>( ti2 - ti1 ).count();

  auto dur_calc = duration_cast<microseconds>( ti_calc2 - ti_calc1 ).count();

  auto dur_calc_ch = duration_cast<microseconds>( ti_calc_ch_2 - ti_calc_ch_1 ).count();

  high_resolution_clock::time_point ti_end = high_resolution_clock::now();

  auto dur_all = duration_cast<microseconds>( ti_end - ti_start ).count();

  auto dur_GE = duration_cast<microseconds>( ti_GE2 - ti_GE1 ).count();

  auto dur_weight = duration_cast<microseconds>( ti_Weight2 - ti_Weight1 ).count();

  auto dur_integrals = duration_cast<microseconds>( ti_Int2 - ti_Int1 ).count();

  auto dur_filling = duration_cast<microseconds>( ti_filling_2 - ti_filling_1 ).count();




  cout << endl;
  cout << " .x conv1_JW.cc(" << run << "): " << endl;
  cout << " "
       << fnew->GetName()
       << " has been created from "
       << f1->GetName() << endl;
  cout << " duration was " << duration << endl;
  cout << "calc function was " << dur_calc << endl;

  cout << "Function for just one channel (and one entry) took " << dur_calc_ch << ", as a fraction of overall fucntion = " << Double_t(dur_calc_ch)/dur_calc << endl;

  cout << "GetEntry took " << dur_GE << " as a part of calc_ch = " << Double_t(dur_GE)/dur_calc_ch << endl;
  cout << "Weight took " << dur_weight << " as a part of calc_ch = " << Double_t(dur_weight)/dur_calc_ch << endl;
  cout << "filling_egrals took " << dur_integrals << " as a part of calc_ch = " << Double_t(dur_integrals)/dur_calc_ch << endl;
  cout << " first filling of vectors and histograms took " << dur_filling << " as a part of calc_ch = " << Double_t(dur_filling)/dur_calc_ch << endl;
 


  cout << " overall macro was " << dur_all << endl;
  cout << "Portion of time in calculating funciton: " << Double_t(dur_calc)/dur_all << endl;


  
}
