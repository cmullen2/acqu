//variables for run file
const Int_t nruns =50;            //maximum no. of tagging efficiency runs
const Int_t maxfiles =5;               //maximum no. of files per run, assume 5 (linear polarisation)
const Int_t nchan =352;           //maximum no. of tagger channels

Int_t nsets=1;                    //Actual number of runs.
Int_t nfiles[nruns];              //Actual number of files per run.
Char_t filename[nruns][maxfiles][50];  //File names
Int_t filetype[nruns][maxfiles];       //File Types, BG, amorphous, parallel or perpendicular
Int_t fileno[nruns][maxfiles];         //Order files for analysis(0-4)

//Variables for tagger energy calibration file
Bool_t Egfile=0;
Char_t energyfile[100];
Float_t chanEg[nchan]=0;
Float_t chanEb[nchan]=0;
Float_t Egmin;
Float_t Egmax;

//Filenames
Char_t outfilename[100] ="TaggEff_analysis";
Char_t temp[100];
Bool_t uselog =NULL;

//Histograms arrays that will be used.
TH1F* time[nruns][maxfiles];
TH1F* hits[nruns][maxfiles];
TH1F* randoms[nruns][maxfiles];
TH1F* scalers[nruns][maxfiles];
TH1F* taggeff[nruns][maxfiles];
TH1F* taggeffBG[nruns][maxfiles];
TH1F* polenh[nruns][maxfiles];
TH1F* hallBG[nruns];
TH1F* hallBGi[nruns][maxfiles];

TH1F* taggeffEg[nruns][maxfiles];
TH1F* polenhEg[nruns][maxfiles];

void TaggEff(Char_t* runlist=NULL)
{

  if (!runlist){
    cout<<endl;
    cout<<"The macro needs some inputs!"<<endl;
    cout<<endl;
    cout<<"Usage: TaggEff(runlist,uselog,energyfile,writeout)"<<endl;
    cout<<"A filelist and the input files are necessary, the rest are optional."<<endl;
    cout<<"Filelist format: filename  status(data/junk)  type(BG/amor/para/perp)  set no. date(01.01.2014)"<<endl;
    cout<<endl;
    cout<<"Options: by default these are off."<<endl;
    cout<<"        uselog: Plot output histograms with log axes."<<endl;
    cout<<"        energyfile: Define a channel-energy calibration file. Format is the second output from ugcal."<<endl;
    cout<<"        writeout: Output text files with values and errors"<<endl;
    cout<<endl;
    cout<<"By default a root file and a PDF file with the histograms are output."<<endl;
    cout<<endl;
    break;
  }

  //Read in the files to be analysed
  scan_files(runlist);

  //Check to see if there is an energy input file and if so read in the energies.
  if(Egfile)
    scan_energies(energyfile);

  //Create the root outputfile to be used.
  sprintf(temp,"%s.root",outfilename);
  TFile *rootout = new TFile(temp, "RECREATE");  

  //Define the histograms to be filled
  for (Int_t i=0;i<nsets;i++)
    define_histos(i,Egfile);

  //Determine the tagging efficiency etc.
  for (Int_t i=0;i<nsets;i++)
    {
      taggeff_ana(i,Egfile);
    }

  //Create a nice pdf file of all the plots
  plot_hists(uselog,Egfile);

  rootout->Write();
  delete rootout;


  return;

}
/*-------------------------------------------------------------------------*/
void scan_files(Char_t* runlist)
{

  Char_t line[30];
  Char_t datadir[100];

  Char_t fname[50];
  Char_t fstatus[20];
  Char_t ftype[20];
  Int_t BGno[nruns] = 0;
  Int_t fset;
  Int_t ftno =0;
  Int_t ft=0;

  ifstream list;
  list.open (runlist);

  if(list.is_open()){

    //scan the file list for things
    while(!list.eof())
      {
	list>>line;

	if(!strcmp(line,"File-Directory:"))
	  {
	    list>>datadir;
	  }

	if(!strcmp(line,"Energy-File:"))
	  {
	    list>>energyfile;
	    Egfile=1;
	  }

	if(!strcmp(line,"Output-File:"))
	  {
	    list>>outfilename;
	  }

	if(!strcmp(line,"Plot-Log:"))
	  {
	    list>>uselog;
	  }

	if(!strcmp(line,"File:"))
	  {
	    list>>fname>>fstatus>>ftype>>fset;

	    ftno=0;
	    ft=0;

	    if(!strcmp(fstatus, "data"))
	      {
		if(fset==nsets){
		  nsets=fset+1;
		  BGno[fset]=0;
		}
		if(!strcmp(ftype, "BG"))
		  { 
		    ftno=BGno[fset];
		    BGno[fset]+=1;
		    ft=1;
		  }
		if(!strcmp(ftype, "amor"))
		  {
		    ftno=2;
		    nfiles[fset]+=3;
		    ft=2;
		  }
		if(!strcmp(ftype, "para"))
		  {
		    ftno=3;
		    nfiles[fset]+=1;
		    ft=3;
		  }
		if(!strcmp(ftype, "perp"))
		  {
		    ftno=4;
		    nfiles[fset]+=1;
		    ft=4;
		  }

		if(ft>0){
		  filetype[fset][ftno]=ft;
		  sprintf(filename[fset][ftno],"%s%s",datadir,fname);


		  fileno[fset][ftno]=getfno(fname);

		}

	      }//'data' file
	  }//scan list
      }//opne list

    //fix no files for each
    for(Int_t j=0;j<nsets;j++)
      {
     	for(Int_t i=0;i<nfiles[j];i++)
     	  {

     	    cout<<j<<" "<<i<<" "<<" "<<filetype[j][i]<<" "<<filename[j][i]<<" "<<fileno[j][i]<<" "<<BGno[fset]<<" ";

     	    if(filetype[j][i]){
     	      cout<<"ok"<<" ";
     	    }

     	    cout<<endl;
     	  }
      }
    //     cout<<energyfile<<" "<<outfilename<<endl;
    list.close();
    return;  

  }//close list.open check
  else
    {
      cout<<runlist<<" could not be opened!"<<endl;
      break;
    }

}

/*-------------------------------------------------------------------------*/
Int_t getfno(Char_t* infile)
{

  Int_t i =0, j=0;
  Char_t tmp[256];
  Int_t num=0;
  Int_t ndigits=0;

  for(i=0;infile[i];i++)
    {
      j=0;
      while(infile[i]>='0' && infile[i]<='9')
	{
	  tmp[j]=infile[i];
	  i++;
	  j++;
	  ndigits++;
	}
    }
  j=0;

  for(i=ndigits;i>0;i--)
    {
      num+=(tmp[i-1]-48)*pow(10,j);
      j++;
    }
  return num;
}

/*-------------------------------------------------------------------------*/

void scan_energies(Char_t* energyfile)
{

  Float_t channo;
  Float_t Eg;
  Float_t Eb;
  Int_t i=nchan;

  ifstream energies;
  energies.open (energyfile);

  if(energies.is_open()){
    while(energies>>channo>>Eg>>Eb){
      chanEg[i]=Eg+(Eb/2);
      chanEb[i]=Eb;
      i--;
    }

    Egmin=chanEg[1];
    Egmax=chanEg[nchan];

    energies.close();
  }
  else
    {
      cout<<energyfile<<" could not be opened!"<<endl;
      cout<<"Continuing without photon energy output!"<<endl;
      Egfile=0;
    }
  return;
}

/*-------------------------------------------------------------------------*/
//Define the histograms to be filled
void define_histos(Int_t setno, Int_t useEg)
{

  Char_t hname[50];
  Char_t htitle[50];
  Char_t sname[50];
  Char_t lname[50];

  Int_t files = nfiles[setno];

  sprintf(hname,"Set_%i_hallBG_sum",setno);
  //  sprintf(htitle,"Set %i Hall Background",setno);
  sprintf(htitle,"Hall Background Sum");
  hallBG[setno]=new TH1F(hname,htitle,nchan,0,nchan);
  hallBG[setno]->SetXTitle("Tagger Channel [#]");
  hallBG[setno]->SetYTitle("Counts");
  hallBG[setno]->SetFillColor(3);
  hallBG[setno]->SetLineColor(1);

  for(Int_t i=0;i<2;i++)
    {
      if(filetype[setno][i]){

	sprintf(hname,"Set_%i_hallBG_%i",setno,i);
	//     sprintf(htitle,"Set %i Hall Background",setno);
	sprintf(htitle,"Hall Background %i",i);
	hallBGi[setno][i]=new TH1F(hname,htitle,nchan,0,nchan);
	hallBGi[setno][i]->SetXTitle("Tagger Channel [#]");
	hallBGi[setno][i]->SetYTitle("Counts");
	hallBGi[setno][i]->SetFillColor(3);
	hallBGi[setno][i]->SetLineColor(1);
	hallBGi[setno][i]->SetStats(kFALSE);
      }
    }

  for(Int_t i=2;i<files;i++)
    {
      if(i==2){
	sprintf(sname,"Amor");
	sprintf(lname,"Amorphous");
      }
      if(i==3){
	sprintf(sname,"Para");
	sprintf(lname,"Parallel");
      }
      if(i==4){
	sprintf(sname,"Perp");
	sprintf(lname,"Perpendicular");
      }

      sprintf(hname,"Set_%i_%s_time",setno,sname);
      //sprintf(htitle,"Set %i TaggEff_%i Time: %s",setno,fileno[setno][i],lname);
      sprintf(htitle,"Coincidence Time: %s",lname);
      time[setno][i]=new TH1F(hname,htitle,1000,-200,200);
      time[setno][i]->SetXTitle("Coincidence Time [ns]");
      time[setno][i]->SetYTitle("Counts");
      time[setno][i]->SetFillColor(5);
      time[setno][i]->SetLineColor(1);
      time[setno][i]->SetStats(kFALSE);

      sprintf(hname,"Set_%i_%s_hits",setno,sname);
      //sprintf(htitle,"Set %i TaggEff_%i Hits: %s",setno,fileno[setno][i],lname);
      sprintf(htitle,"Hits: %s",lname);
      hits[setno][i]=new TH1F(hname,htitle,nchan,0,nchan);
      hits[setno][i]->SetXTitle("Tagger Channel [#]");
      hits[setno][i]->SetYTitle("Counts");
      hits[setno][i]->SetFillColor(2);
      hits[setno][i]->SetLineColor(1);
      hits[setno][i]->SetStats(kFALSE);

      sprintf(hname,"Set_%i_%s_randoms",setno,sname);
      //sprintf(htitle,"Set %i TaggEff_%i Randoms: %s",setno,fileno[setno][i],lname);
      sprintf(htitle,"Randoms: %s",lname);
      randoms[setno][i]=new TH1F(hname,htitle,nchan,0,nchan);
      randoms[setno][i]->SetXTitle("Tagger Channel [#]");
      randoms[setno][i]->SetYTitle("Counts");
      randoms[setno][i]->SetFillColor(4);
      randoms[setno][i]->SetLineColor(1);
      randoms[setno][i]->SetStats(kFALSE);

      sprintf(hname,"Set_%i_%s_scalers",setno,sname);
      //      sprintf(htitle,"Set %i TaggEff_%i Scalers: %s",setno,fileno[setno][i],lname);
      sprintf(htitle,"Scalers: %s",lname);
      scalers[setno][i]=new TH1F(hname,htitle,nchan,0,nchan);
      scalers[setno][i]->SetXTitle("Tagger Channel [#]");
      scalers[setno][i]->SetYTitle("Counts");
      scalers[setno][i]->SetFillColor(5);
      scalers[setno][i]->SetLineColor(1);
      scalers[setno][i]->SetStats(kFALSE);

      sprintf(hname,"Set_%i_%s_taggeff",setno,sname);
      //sprintf(htitle,"Set %i TaggEff_%i Tagging Efficiency: %s",setno,fileno[setno][i],lname);
      sprintf(htitle,"Tagging Efficiency: %s",lname);
      taggeff[setno][i]=new TH1F(hname,htitle,nchan,0,nchan);
      taggeff[setno][i]->SetXTitle("Tagger Channel [#]");
      taggeff[setno][i]->SetYTitle("Efficiency_{Tagg}");
      taggeff[setno][i]->SetLineColor(1);
      taggeff[setno][i]->SetStats(kFALSE);

      if(useEg){
	sprintf(hname,"Set_%i_%s_taggeffEg",setno,sname);
	// sprintf(htitle,"Set %i TaggEff_%i Tagging Efficiency: %s",setno,fileno[setno][i],lname);
	sprintf(htitle,"Tagging Efficiency: %s",lname);
	taggeffEg[setno][i]=new TH1F(hname,htitle,nchan,chanEg);
	taggeffEg[setno][i]->SetXTitle("E_{#gamma} [MeV]");
	taggeffEg[setno][i]->SetYTitle("Efficiency_{Tagg}");
	taggeffEg[setno][i]->SetLineColor(1);
	taggeffEg[setno][i]->SetStats(kFALSE);
      }

      if(i>2){
	sprintf(hname,"Set_%i_%s_polenh",setno,sname);
	sprintf(htitle,"Polarisation Enhancement: %s",lname);
	polenh[setno][i]=new TH1F(hname,htitle,nchan,0,nchan);
	polenh[setno][i]->SetXTitle("Tagger Channel [#]");
	polenh[setno][i]->SetYTitle("Enhancement");
	polenh[setno][i]->SetLineColor(1);
	polenh[setno][i]->SetStats(kFALSE);

	if(useEg){
	  sprintf(hname,"Set_%i_%s_polenhEg",setno,sname);
	  sprintf(htitle,"TaggEff_%i Polarisation Enhancement: %s",fileno[setno][i],lname);
	  polenhEg[setno][i]=new TH1F(hname,htitle,nchan,chanEg);
	  polenhEg[setno][i]->SetXTitle("E_{#gamma} [MeV]");
	  polenhEg[setno][i]->SetYTitle("Enhancement");
	  polenhEg[setno][i]->SetLineColor(1);
	  polenhEg[setno][i]->SetStats(kFALSE);
	}

      }

    }

  return;
}

/*-------------------------------------------------------------------------*/
//Determine the tagging efficiency etc.
void taggeff_ana(Int_t setno, Int_t useEg)
{
  TH1F *fscalers;
  TH1F *prompts;
  TH1F *accidentals;
  TH1F *clocks;
  TH1F *coinctime;

  Double_t t_hallbg = 0;
  Double_t bgfactor = 0;

  Int_t files = nfiles[setno];
  Int_t k=nchan;
  Int_t noBG=0;


  for(Int_t i=0;i<files;i++)
    {
      if(filetype[setno][i]==1)
	noBG=1;

      bgfactor = 0;

      //Open the histogram file to be read in.
      TFile *file = new TFile(filename[setno][i],"READ");

      //Read in the appropriate histograms
      coinctime =(TH1F*)file->Get("FPD_TimeOR");
      clocks =(TH1F*)file->Get("SumScalers190to191");
      fscalers= (TH1F*)file->Get("FPD_ScalerAcc");
      prompts = (TH1F*)file->Get("FPD_HitsPrompt");
      accidentals = (TH1F*)file->Get("FPD_HitsRand");

      fscalers->Sumw2();
      prompts->Sumw2();
      accidentals->Sumw2();

      /*Hall BG section*/
      if(i<2){
	hallBG[setno]->Add(fscalers,1);
	hallBGi[setno][i]->Add(fscalers,1);
	t_hallbg+=clocks->GetBinContent(1);
      }

      /* Radiator file section*/
      if(i>1){

	time[setno][i]->Add(coinctime);
	hits[setno][i]->Add(prompts,1);
	randoms[setno][i]->Add(accidentals,1);
	scalers[setno][i]->Add(fscalers,1);

	//Make sure there is clock information for Hall BG runs.
	if(t_hallbg==0){
	  bgfactor=1;
	cout<<"Hall background clock information not found for set "<<setno<<"!"<<endl;
	}
	else
	  bgfactor = clocks->GetBinContent(1)/t_hallbg;

	if(bgfactor<=0){
	  cout<<"Run clock information not found! No hall background correction made."<<endl;
	}

	prompts->Add(accidentals,-1);
	fscalers->Add(hallBG[setno],-1*bgfactor);
	taggeff[setno][i]->Divide(prompts,fscalers,1,1);

	//Plot Tagg Eff as a function of Eg if energies present
	if(useEg){
	  k=nchan;
	  for(Int_t j=1;j<nchan+1;j++){
	    taggeffEg[setno][i]->SetBinContent(j,taggeff[setno][i]->GetBinContent(k));
	    taggeffEg[setno][i]->SetBinError(j,taggeff[setno][i]->GetBinError(k));
	    taggeffEg[setno][i]->SetAxisRange(Egmin, Egmax);
	    k--;
	  }
	}

	//If diamond data there determine polarisation enhancement
	if(i>2){
	  polenh[setno][i]->Divide(hits[setno][i],hits[setno][2]);

	  //Plot as a function of Eg if energies present
	  if(useEg){
	    k=nchan;
	    for(Int_t j=1;j<nchan+1;j++){
	      polenhEg[setno][i]->SetBinContent(j,polenh[setno][i]->GetBinContent(k));
	      polenhEg[setno][i]->SetBinError(j,polenh[setno][i]->GetBinError(k));
	      polenhEg[setno][i]->SetAxisRange(Egmin, Egmax);
	      k--;
	    }
	  }

	}//polarisation enhancement loop
      }//tagg eff loop

      file->Close();

    }//file loop


  if(noBG==0)
    cout<<"No hall background files defined for set "<<setno<<"! Corrections not made."<<endl;

  return;
}
/*-------------------------------------------------------------------------*/

void plot_hists(Int_t uselog, Int_t useEg)
{

  Int_t files =0;

  Char_t settex[50];
  Char_t filetex[50];
  Char_t pagetitle[100];
  Char_t rad[3][50] = {"Amorphous", "Diamond Parallel", "Diamond Perpendicular"};

  TCanvas *canvas = new TCanvas("canvas","Canvas",700,1000);

  sprintf(temp,"%s.pdf",outfilename);
  TPDF *pdfout = new TPDF(temp,111);

  TPad *timepad = new TPad("timepad","",0.01,0.61,0.32,0.89,10);
  TPad *bgpad1 = new TPad("bgpad1","",0.34,0.61,0.66,0.89,10);
  TPad *bgpad2 = new TPad("bgpad2","",0.67,0.61,0.99,0.89,10);
  TPad *hitspad = new TPad("hitspad","",0.01,0.31,0.49,0.59,10);
  TPad *scalerspad = new TPad("scalerspad","",0.51,0.31,0.99,0.59,10);
  TPad *effpad = new TPad("effpad","",0.01,0.01,0.99,0.29,10);
  TPad *enhpad = new TPad("enhpad","",0.01,0.01,0.99,0.29,10);

  for (Int_t i=0;i<nsets;i++){
    files = nfiles[i];
 
    sprintf(settex,"Tagging Efficiency run %i",i);

    for(Int_t j=2;j<files;j++){
      canvas->SetFillColor(18);
      canvas->SetBorderSize(2);
      canvas->SetFrameFillColor(10);

      sprintf(filetex,"File number %i; Radiator: %s ",fileno[i][j],rad[j-2]);

      TPaveText *titlepad = new TPaveText(0.01,0.91,0.99,0.99);
      titlepad->SetFillColor(10);
      titlepad->AddText(settex);
      titlepad->AddText(filetex);
      titlepad->Draw();

      if(j==2){
	canvas->cd();
	timepad->SetPad(0.01,0.61,0.32,0.89);
	timepad->Draw();
	timepad->cd();
	time[i][j]->SetAxisRange(-10,50);
	time[i][j]->Draw("h,e");

	canvas->cd();
	bgpad1->SetPad(0.34,0.61,0.66,0.89);
	bgpad1->Draw();
	bgpad1->cd();
	bgpad1->SetLogy();
	hallBGi[i][0]->Draw("h,e");

	canvas->cd();
	bgpad2->SetPad(0.67,0.61,0.99,0.89);
	bgpad2->Draw();
	bgpad2->cd();
	bgpad2->SetLogy();
	hallBGi[i][1]->Draw("h,e");

	canvas->cd();
	hitspad->SetPad(0.01,0.31,0.49,0.59);
 	hitspad->Draw();
 	hitspad->cd();
  	hits[i][j]->Draw("h,e");
	randoms[i][j]->Draw("h,e,same");

 	canvas->cd();
	scalerspad->SetPad(0.51,0.31,0.99,0.59);
	scalerspad->Draw();
 	scalerspad->cd();
 	scalers[i][j]->Draw("h,e");
 	hallBG[i]->Draw("h,e,same");

 	canvas->cd();
	effpad->SetPad(0.01,0.01,0.99,0.29);
       	effpad->Draw();
 	effpad->cd();
	taggeff[i][j]->SetMinimum(-0.1);
	taggeff[i][j]->SetMaximum(1);
	taggeff[i][j]->Draw("e");
      }
      if(j>2){
	canvas->cd();
	timepad->SetPad(0.01,0.61,0.32,0.89);
	timepad->Draw();
	timepad->cd();
	time[i][j]->SetAxisRange(-10,50);
	time[i][j]->Draw("h,e");

	canvas->cd();
	hitspad->SetPad(0.34,0.61,0.66,0.89);
 	hitspad->Draw();
 	hitspad->cd();
  	hits[i][j]->Draw("h,e");
	randoms[i][j]->Draw("h,e,same");

 	canvas->cd();
	scalerspad->SetPad(0.67,0.61,0.99,0.89);
	scalerspad->Draw();
 	scalerspad->cd();
 	scalers[i][j]->Draw("h,e");
 	hallBG[i]->Draw("h,e,same");

 	canvas->cd();
	effpad->SetPad(0.01,0.31,0.99,0.59);
       	effpad->Draw();
 	effpad->cd();
	taggeff[i][j]->SetMinimum(-0.1);
	taggeff[i][j]->SetMaximum(1);
	taggeff[i][j]->Draw("e");

 	canvas->cd();
	enhpad->SetPad(0.01,0.01,0.99,0.29);
       	enhpad->Draw();
 	enhpad->cd();
	polenh[i][j]->SetMinimum(0);
	polenh[i][j]->SetMaximum(5);
	polenh[i][j]->Draw("e");
      }

      canvas->cd();
      canvas->Update();

      sprintf(pagetitle,"Run %i; File %i; %s ",i,fileno[i][j],rad[j-2]);
 
      if((j==4)&&useEg){
	pdfout->SetTitle(pagetitle);    
	pdfout->NewPage();
	titlepad->DeleteText();


	TPaveText *titlepad = new TPaveText(0.01,0.91,0.99,0.99);
	titlepad->SetFillColor(10);
	titlepad->AddText(settex);
	titlepad->AddText("Polarisation Enhancements");
	titlepad->Draw();

	canvas->SetFillColor(18);
	canvas->SetBorderSize(2);
	canvas->SetFrameFillColor(10);

	canvas->cd();
	bgpad1->SetPad(0.01,0.46,0.99,0.89);
	bgpad1->Draw();
	bgpad1->cd();
	bgpad1->SetLogy(0);
	polenhEg[i][3]->Draw("e");

	canvas->cd();
	bgpad2->SetPad(0.01,0.01,0.99,0.44);
	bgpad2->Draw();
	bgpad2->cd();
	bgpad2->SetLogy(0);
	polenhEg[i][j]->Draw("e");

	canvas->cd();
	canvas->Update();

	sprintf(pagetitle,"Run %i; Polarisation Enhancements",i);
      }

      pdfout->SetTitle(pagetitle); 
      pdfout->NewPage();
      titlepad->DeleteText();

    }//files
  }//sets

  //titlepad->Close();

  pdfout->Close();
  canvas->Close();

  //titlepad->Close();

  //   delete titlepad;
  //   delete timepad;
  //   delete bgpad1;
  //   delete bgpad2;
  //   delete hitspad;
  //   delete scalerspad;
  //   delete effpad;
  //     delete enhpad;

  delete canvas;
  delete pdfout;

  return;
}

/*-------------------------------------------------------------------------*/
