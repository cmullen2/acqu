//variables for run file
const Int_t nruns =500;            //maximum no. of tagging efficiency runs
const Int_t maxfiles =5;               //maximum no. of files per run, assume 5 (linear polarisation)
const Int_t nchan =352;           //maximum no. of tagger channels

Int_t nsets=1;                    //Actual number of runs.
Int_t nfiles[nruns];              //Actual number of files per run.
Char_t filename[nruns][maxfiles][100];  //File names
Int_t *filetype[nruns][maxfiles];       //File Types, BG, amorphous, parallel or perpendicular
Int_t fileno[nruns][maxfiles];         //Order files for analysis(0-4)

//Variables for tagger energy calibration file
Bool_t Egfile=0;
Char_t energyfile[100];
Float_t chanEg[nchan]=0;
Float_t chanEb[nchan]=0;
Float_t Egmin; 
Float_t Egmax;

//Filenames
//Char_t outfilename[100] ="TaggEff_Nov-2013_R";
//Char_t outfilename[100] ="TaggEff_Feb-2014_R";
//Char_t outfilename[100] ="TaggEff_Apr-2014_R";
//Char_t outfilename[100] ="TaggEff_May-2014_R";
//Char_t outfilename[100] ="TaggEff_Mar-2015_R";
//Char_t outfilename[100] ="TaggEff_May-2015_R";
Char_t outfilename[100] ="TaggEff_Aug-2016_R";

Char_t temp[150];
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
 cout << "CAM 1st "<< endl;

  //Check to see if there is an energy input file and if so read in the energies.
  if(Egfile)
    scan_energies(energyfile);

  //Create the root outputfile to be used.
  sprintf(temp,"%s.root",outfilename);
  TFile *rootout = new TFile(temp, "RECREATE");  

  //Define the histograms to be filled
  for (Int_t i=0;i<nsets;i++)
    define_histos(i,Egfile);

  //  return;
  cout << "Doing taggeffanan " << nsets << " " << energyfile << endl;
  //Determine the tagging efficiency etc.
  for (Int_t i=0;i<nsets;i++)
    {
      taggeff_ana(i,Egfile);
    }
cout << "CAM 2st "<< endl;
  //Create a nice pdf file of all the plots
  plot_hists(uselog,Egfile);

  rootout->Write();
  delete rootout;

  return;

}
/*-------------------------------------------------------------------------*/
void scan_files(Char_t* runlist)
{

  Char_t line[300];
  Char_t datadir[150];
  
  Char_t fname[150];
  Char_t fstatus[20];
  Char_t ftype[20];
  //  Int_t BGno[nruns] = 0;
  Int_t BGno[nruns] = {}; //CAM edit
  Int_t fset = 0;
  Int_t ftno = 0;
  Int_t ft=0;
  ifstream list;
  list.open (runlist);


  if(list.is_open()){

    //Test end of file here
    //scan the file list for things
    while(!list.eof())			//Incorrect coding as will always go one line too far!
      {
	list>>line;
	if (!list.eof())
	  {
			
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
		cout << "fset =" << fset << "  nsets=" << nsets<< endl;
		cout  << line << " fstatus =" << fstatus << "  ftype=" <<ftype << "" <<  endl;

		ftno=0;
		ft=0;


		if(!strcmp(fstatus, "data"))
		  {
		    if(fset==nsets){
		      nsets=fset+1;
		      BGno[fset]=0;
		      cout << "After array assignation " <<BGno[fset]<<endl;
		
	
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
		      //fprintf(stdout,"%s%s",datadir,fname);
		    }
		
		
		
		  }
	      }

	  }

      }
    
    //fix no files for each
    //          for(Int_t j=0;j<nsets;j++)
    //            {
    //      	for(Int_t i=0;i<nfiles[j];i++)
    //      	  {

    //      	    cout<<j<<" "<<i<<" "<<" "<<filetype[j][i]<<" "<<filename[j][i]<<" "<<fileno[j][i]<<" "<<BGno[fset]<<" ";

    //      	    if(filetype[j][i]){
    //      	      cout<<"ok"<<" ";
    //      	    }

    //      	    cout<<endl;
    //      	  }
    //            }
    //     cout<<energyfile<<" "<<outfilename<<endl;
    list.close();
    return;  

  }//close list.open check
  else
    {
      cout<< runlist <<" could not be opened!" << endl;
      break;
    }
  
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

  Char_t hname[100];
  Char_t htitle[150];
  Char_t sname[150];
  Char_t lname[150];
  Int_t Run;
  Int_t BGRun0,BGRun1,AmorRun2,ParaRun3,PerpRun4;

  Int_t files = nfiles[setno];

  sprintf(hname,"Set_%i_hallBG_sum",setno);
  //  sprintf(htitle,"Set %i Hall Background",setno);
  //  sprintf(htitle,"Hall Background Sum");
  sscanf(filename[setno][0],"data/TaggEff_%d%*s",&BGRun0);
  sscanf(filename[setno][1],"data/TaggEff_%d%*s",&BGRun1);
  

  sprintf(htitle,"Hall_Background_Sum_Runs_%d_%d", BGRun0,BGRun1);

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
	sscanf(filename[setno][2],"data/TaggEff_%d%*s",&AmorRun2);

	
      }
      if(i==3){
	sprintf(sname,"Para");
	sprintf(lname,"Parallel");
	sscanf(filename[setno][3],"data/TaggEff_%d%*s",&ParaRun3);


      }
      if(i==4){
	sprintf(sname,"Perp");
	sprintf(lname,"Perpendicular");
	sscanf(filename[setno][4],"data/TaggEff_%d%*s",&PerpRun4);


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


      sprintf(hname,"Set_%i_%s_taggeff",setno,sname);                                           // RODDY: Tagging efficiency
      //sprintf(htitle,"Set %i TaggEff_%i Tagging Efficiency: %s",setno,fileno[setno][i],lname);

      if(i==2){
	sprintf(htitle,"Tagging_Efficiency_%s_%i",lname,setno);
      }
      if(i==3){
	sprintf(htitle,"Tagging_Efficiency_%s_%i",lname,setno);
      }
      if(i==4){
	sprintf(htitle,"Tagging_Efficiency_%s_%i",lname,setno);
      }
      taggeff[setno][i]=new TH1F(hname,htitle,nchan,0,nchan);
      taggeff[setno][i]->SetXTitle("Tagger Channel [#]");
      taggeff[setno][i]->SetYTitle("Efficiency_{Tagg}");
      taggeff[setno][i]->SetLineColor(1);
      taggeff[setno][i]->SetStats(kFALSE);
      

     
      if(useEg){
	sprintf(hname,"Set_%i_%s_taggeffEg",setno,sname);                                        // RODDY: Tagging efficiency Eg
	if(i==2){
	  sprintf(htitle,"Tagging_Efficiency_%s_%i",lname,setno);
	}
	if(i==3){
	  sprintf(htitle,"Tagging_Efficiency_%s_%i",lname,setno);
	}
	if(i==4){
	  sprintf(htitle,"Tagging_Efficiency_%s_%i",lname,setno);
	}
	// sprintf(htitle,"Set %i TaggEff_%i Tagging Efficiency: %s",setno,fileno[setno][i],lname);
	taggeffEg[setno][i]=new TH1F(hname,htitle,nchan,chanEg);
	taggeffEg[setno][i]->SetXTitle("E_{#gamma} [MeV]");
	taggeffEg[setno][i]->SetYTitle("Efficiency_{Tagg}");
	taggeffEg[setno][i]->SetLineColor(1);
	taggeffEg[setno][i]->SetStats(kFALSE);
      }

      if(i>2){
	sprintf(hname,"Set_%i_%s_polenh",setno,sname);                                           // RODDY: Polenh
	if(i==3){
	  //	  sprintf(htitle,"Polarisation_Enhancement_%s_%d_%d",lname,ParaRun3,AmorRun2);
	  sprintf(htitle,"Polarisation_Enhancement_%s_%i",lname,setno);
	}
	if(i==4){
	  //	  sprintf(htitle,"Polarisation_Enhancement_%s_%d_%d",lname,PerpRun4,AmorRun2);
	  sprintf(htitle,"Polarisation_Enhancement_%s_%i",lname,setno);
	}
	polenh[setno][i]=new TH1F(hname,htitle,nchan,0,nchan);
	polenh[setno][i]->SetXTitle("Tagger Channel [#]");
	polenh[setno][i]->SetYTitle("Enhancement");
	polenh[setno][i]->SetLineColor(1);
	polenh[setno][i]->SetStats(kFALSE);
	
	
	
	if(useEg){
	  sprintf(hname,"Set_%i_%s_polenhEg",setno,sname);                                       // RODDY: Polenh Eg
	  if(i==3){
	    //	    sprintf(htitle,"TaggEff_Polarisation_Enhancement_%s_%d_%d",lname,ParaRun3,AmorRun2);
	    sprintf(htitle,"TaggEff_Polarisation_Enhancement_%s_%i",lname,setno);
	  }
	  if(i==4){
	    //	    sprintf(htitle,"TaggEff_Polarisation_Enhancement_%s_%d_%d",lname,PerpRun4,AmorRun2);
	    sprintf(htitle,"TaggEff_Polarisation_Enhancement_%s_%i",lname,setno);
	  }
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
  Double_t livefactor = 0;
  
  Int_t files = nfiles[setno];
  Int_t k=nchan;
  Int_t jkl=nchan;
  Int_t noBG=0;

  cout<< "In taggeffanan " << setno << "  " << files << endl;

  for(Int_t i=0;i<files;i++)
    {
      if(filetype[setno][i]==1)
	noBG=1;				//Is this the closing of the above if statement?!!
      
      bgfactor = 0;
      
      // Open the histogram file to be read in.//

      TFile *file = new TFile(filename[setno][i],"READ");
      //file->ls();
      cout << "On file " << filename[setno][i] << endl;

      // Read in the appropriate histograms //

      coinctime =(TH1F*)file->Get("FPD_TimeOR");                // Gets the timing alignment
   //   fscalers = (TH1F*)file->Get("BadScR_SumFPDScalers"); //Roddys Bad scalers not present in my files yet, use old method. 
     fscalers = (TH1F*)file->Get("BadScR_SumFPDScalersLiveCorr"); // CAM added 
      prompts = (TH1F*)file->Get("CaLib_Tagg_Eff_Ladder_Hits");             // CAM Added due to Dominiks comments
//      fscalers = (TH1F*)file->Get("FPD_ScalerAcc");
//      prompts = (TH1F*)file->Get("FPD_HitsPrompt");             // Gets the prompt hits
      accidentals = (TH1F*)file->Get("FPD_HitsRand");           // Gets the random hits
      clocks =(TH1F*)file->Get("SumScalers0to191");
    
      fscalers->Sumw2();
      //cout << "breaks on the prompts line" <<endl; 	//Breaks when using Calib prompts!! Histo empty maybe? Calib prompt histo seems to be empty for a lot of BG files why is this?
      prompts->Sumw2();
      accidentals->Sumw2();
      
      /*Hall BG section*/
      if(i<2){
	hallBG[setno]->Add(fscalers,1);
	hallBGi[setno][i]->Add(fscalers,1);
	
	jkl=nchan;
	for(Int_t j=1;j<nchan+1;j++){
	  //	cout << "  scalersBG  " << hallBGi[setno][i]->GetBinContent(jkl)  <<endl;
	
	}


	if(setno<46){
	  t_hallbg+=clocks->GetBinContent(192); // BEFORE 2015 BEAMTIMES USE SCALER 192
	}
	
	if(setno>45){
	  t_hallbg+=clocks->GetBinContent(191); // 2015 BEAMTIMES USE SCALER 191
	}
	//	cout << "Hall background = " << t_hallbg << endl;
	//	cout <<"Clocks bin 1 = " << t_hallbg << endl;
	//	cout <<"Clocks bin 2 = " << clocks->GetBinContent(1) << endl;
      }
      
      /* Radiator file section */
      if(i>1){
	
	//	cout << "Second Clocks bin 1 = " << clocks->GetBinContent(1) << endl;
	//	cout << "Second Clocks bin 2 = " << clocks->GetBinContent(2) << endl;
	
	//livefactor = clocks->GetBinContent(1)/clocks->GetBinContent(2);
	//	cout << "LiveFactor = " << livefactor << endl;
	
	//fscalers->Scale(livefactor);               // Livetime correction
       	time[setno][i]->Add(coinctime);
	hits[setno][i]->Add(prompts,1);
	randoms[setno][i]->Add(accidentals,1);
	scalers[setno][i]->Add(fscalers,1);
	
	//Make sure there is clock information for Hall BG runs.
	if(t_hallbg==0){
	  bgfactor=1;
	  cout<<"Hall background clock information not found for set "<<setno<<"!"<<endl;
	}
	if(setno<46){
	    bgfactor = clocks->GetBinContent(192)/t_hallbg;
	  }
	if(setno>45){
	  bgfactor = clocks->GetBinContent(191)/t_hallbg; // 2015 BEAMTIMES 
	}
	
	if(bgfactor<=0){
	  cout<<"Run clock information not found! No hall background correction made."<<endl;
	}
	
	prompts->Add(accidentals,-1);
	fscalers->Add(hallBG[setno],-1*bgfactor);
	
	cout << "Set: "<< setno << "  i: " <<  i << ", bgfactor = " << bgfactor << endl;
	//	cout << " Clocks 1 = " << clocks->GetBinContent(192) << ", t_hallbg = " << t_hallbg << endl;
	//	cout << "prompts!!!!!    " << hits[setno][i] << " randos  " << randoms[setno][i]<< "  scalers " << scalers[setno][i]  <<endl;
	
	taggeff[setno][i]->Divide(prompts,fscalers,1,1);
	
	//Plot Tagg Eff as a function of Eg if energies present
	if(useEg){
	  k=nchan;
	  for(Int_t j=1;j<nchan+1;j++){
	    //	  cout << "prompts!!!!!    " << hits[setno][i]->GetBinContent(k) << " randos  " << randoms[setno][i]->GetBinContent(k)<< "  scalers " << scalers[setno][i]->GetBinContent(k)  <<endl;
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
  Int_t fileno[nruns][maxfiles]   =  {{339,340,341},{413,419,417},{534,535,536},
				      {585,583,584},{639,638,637},{689,688,687},
				      {740,739,738},{793,792,790},{1002,1000,1001},
				      {1064,1062,1063},{1113,1112,1111},{1160,1158,1159},
				      {1229,1227,1226},{1278,1276,1277},{1317,1315,1316},
				      {1996,1994,1995},{2032,2030,2031}{2065,2063,2064},
				      {2102},{2160,2161,2162},{2189,2188,2187},{2223,2224,2225},
				      {2246,2244,2245},{2314,2315,2316},{2350,2348,2349},
				      {2381,2380,2379},{2418,2420,2419},{2485,2483,2484},
				      {2513,2515,2516},{2599,2597,2596},{2623,2624,2626},
				      {2703},{2754},{2815},{2850},{2980},{3211,3209,3210},
				      {3249,3248,3247},{3292,3291,3290},{3298,3296,3297},
				      {3332,3333},{3378,3377},{4283,4284,4285},
				      {4331,4333,4332},{4382,4381,4380},{4484,4485,4486},
				      {8096},{8116,8114,8115},{8213},{8312},{8364},{8403},
				      {8456},{8508},{8581},{8634,8635,8636},
				      {8673},{8733,8732,8731},{8775,8778,8776},{8822},
				      {8864,8866,8865},{8920,8922,8921},{8959,8958,8957},
				      {9138,9137,9136},{9170,9171,9172},{11187,11186,11185},
                                      {11237,11235,11236},{11288,11286,11287},{11332},
				      {11438},{11480,11479,11478},{11534,11533,11532},
				      {11587,11585,11586},{11642,11638,11639},{11697,11705,11706},
				      {11779,11777,11776},{11840,11839,11838},{11928,11926,11927},
				      {11979,11977,11978}};
				

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

      sprintf(filetex,"File number %i; Radiator: %s ",fileno[i][j-2],rad[j-2]);

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
	time[i][j]->SetAxisRange(-10,10);
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
	scalerspad->SetLogy();
	scalerspad->Draw();
 	scalerspad->cd();
 	scalers[i][j]->Draw("h,e");
 	//hallBG[i]->Draw("h,e,same");

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
	hitspad->SetPad(0.01,0.61,0.49,0.89);
 	hitspad->Draw();
 	hitspad->cd();
  	hits[i][j]->Draw("h,e");
	randoms[i][j]->Draw("h,e,same");

 	canvas->cd();
	scalerspad->SetPad(0.51,0.61,0.99,0.89);
	scalerspad->Draw();
	scalerspad->SetLogy();
 	scalerspad->cd();
 	scalers[i][j]->Draw("h,e");
	//hallBG[i]->Draw("h,e,same");

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
