void LadderGen_DM(Char_t* calFile=NULL, Double_t eBeam=0.0, Int_t n=352,
		  Int_t qdcStart=800, Int_t tdcStart=1432, Int_t scalerStart=2865,/*400*/
		  Int_t opt=0)
{
// Generate Tagger ladder file. Input parameters and default values:
// qdcstart    start index of QDCs
// tdcstart    start index of TDCs
// scalerStart start index of Scalers
// n           number of ladder elements
// calFile     name of calibration file 
// eBeam       e- beam energy
// opt         ordering options main FPD or Microscope
// 
// ordering options
  enum { ELadd_FPD, ELadd_Micro };
  // Sequential ordering
  Int_t OrderSeq[] =
    {00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
  // Saw-tooth Sequential ordering
  Int_t OrderInvSeq[] =
    {15, 14, 13, 12, 11, 10, 09, 08, 07, 06, 05, 04, 03, 02, 01, 00,
     31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16 };
  // QDC order section AB
  Int_t ABqdc[] =
    {16, 00, 17, 01, 18, 02, 19, 03, 20, 04, 21, 05, 22, 06, 23, 07,
     24, 08, 25, 09, 26, 10, 27, 11, 28, 29, 30, 31, 12, 13, 14, 15};
  // TDC,scaler order section AB
  Int_t ABtdc[] =
    {12, 00, 13, 01, 14, 02, 15, 03, 16, 04, 17, 05, 18, 06, 19, 07,
     20, 08, 21, 09, 22, 10, 23, 11, 24, 25, 26, 27, 28, 29, 30, 31};
  // Sequential QDC order section C-V
  Int_t* CtoVqdc = OrderSeq;
  // TDC order section C-V
  Int_t CtoVtdc[] =
    {00, 16, 01, 17, 02, 18, 03, 19, 04, 20, 05, 21, 06, 22, 07, 23,
     08, 24, 09, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31};
  Int_t* MUqdc = OrderInvSeq;
  Int_t* MUtdc = OrderSeq;
  //
  // alphabetic sections
  Char_t* sect[] =
    {"AB","CD","EF","GH","IJ","KL","MN","OP","QR","ST","UV","WX","YZ"};


  //  Char_t* file_name = "DM_LadderGen.dat"; // output file name
  Char_t* file_name[100];
  //  sprintf(file_name,"Detector_PolObs_1_Tagger%i.ua.dat",eBeam);
  //  sprintf(file_name,"Detector_G_Proton_1_Tagger%i.ua.dat",eBeam);
  //sprintf(file_name,"Detector_Compton_Carbon_Tagger%i.ua.dat",eBeam);
  //sprintf(file_name,"Detector_Compton_Sigma2z_Tagger%i.ua.dat",eBeam);
  //sprintf(file_name,"Detector_G-E_HButanol_Tagger%i.ua.dat",eBeam);
  //  sprintf(file_name,"Detector_DoublePol_Deuteron_Tagger%i.ua.dat",eBeam);
  //sprintf(file_name,"Detector_G-E_Proton_09-15_Tagger%i.ua.dat",eBeam);
  sprintf(file_name,"Detector_Tagger%i.ua.dat",eBeam);
 
  Double_t eh = 2.5;                   // upper energy limit
  Double_t el = 0.20;                  // lower energy limit
  Double_t ec = 0.0005;                // QDC MeV/Channel
  Double_t ep = 0.0;                   // QDC pedestal
  Double_t th = 250.0;                 // time upper limit
  Double_t tl = -250.0;                  // time lower limit
  Double_t tc = 0.117;                 // TDC ns/channel
  Double_t tp = -3500.0;                 // TDC zero point channel;
  Double_t eCal = 0.0;
  Double_t goosey, eWidth;
  Int_t i,j,k,l,m;
  Int_t qdc,tdc,scaler;
  Char_t tdcname[16];
  FILE* lfile;
  FILE* cfile;
  Int_t* qdcOrder;
  Int_t* tdcOrder;
  //
  // Open output file name
  printf("Tagger ladder configuration saved to file %s\n",file_name);
  if( (lfile = fopen(file_name,"w")) == NULL ){
    printf("File open failed\n");
    return;
  }
  // Open energy calibration file if name supplied
  if( calFile ){
    if( (cfile = fopen(calFile,"r")) == NULL ){
      printf("File %s open failed\n", calFile);
    }
    else printf("Using electron-energy calibration file %s\n", calFile);
  }
  else cfile = NULL;
  //
  // Print some standard lines    
  fprintf(lfile,"##  Auto-Generated Tagger-Ladder Config File\n");
  fprintf(lfile,"##  ROOT Macro LadderGen_DM.C D.M. 05/10/2010 \n");
  fprintf(lfile,"##  Adapted from the file LadderGen.C by  J.R.M. Annand\n");
  fprintf(lfile,"##  Current version changed to suit DAQ changes. D.M. 06/02/2014 \n");
  fprintf(lfile,"##\n");
  fprintf(lfile,"##	#FP	Energy	Time	Pos\n");
  fprintf(lfile,"Size: 	%d 	0 	1 	1\n", n);
  fprintf(lfile,"##	Scalers	Calib	Overlap	Micro	#trig	Toff	Tres\n");
  fprintf(lfile,"Misc:	1	1	1	0	0	0.0	0.0\n");
  fprintf(lfile,"##\n");
  fprintf(lfile,"Window: -5. 5. 10. 20. \n");


  if( opt == ELadd_FPD ){
    qdcOrder = CtoVqdc;
    tdcOrder = CtoVtdc;
  }
  else{
    qdcOrder = MUqdc;
    tdcOrder = MUtdc;
  }

  for(i=0,m=0; i<n; i+=32,m++){
    fprintf(lfile,"##\n## G %d - %d  Section %s\n##\n", i, i+31, sect[m]);
    for (j=0; j<32; j++){
      if( (!i) && (opt == ELadd_FPD) ){
	  k = ABqdc[j];
	  l = ABtdc[j];
      }
      else k = l = j;
      qdc = qdcStart + qdcOrder[k] + i;
      tdc = tdcStart + tdcOrder[l] + i;
      scaler = scalerStart + qdcOrder[l] + i;
      if( cfile ){
	fscanf( cfile, "%lf%lf%lf", &goosey, &eCal, &eWidth );
	if( (Int_t)goosey != (i+j+1) ){
	  printf(" Data out of synch in calibration file %s %d %d\n",
		 calFile, goosey, i );
	  return;
	}
	eCal = eBeam - eCal;
      }
      fprintf(lfile,
     //	      "Element: %4d %.1f %.1f %.1f %.5f %4dM0 %.1f %.1f %.1f %.4f 0 0 0 %6.1f 0.0 %d\n",
     //	      qdc,el,eh,ep,ec,tdc,tl,th,tp,tc,eCal,scaler);
	      "Element: %4d %.1f %.1f %.1f %.5f %4dM0 %.1f %.1f %.1f %.4f 0 0 0 %6.2f %6.2f %d\n",
	      qdc,el,eh,ep,ec,tdc,tl,th,tp,tc,eCal,eWidth,scaler);
      if( (i+j) >= n ) break;
    }
  }
  //
  // Print some standard display lines
  fprintf(lfile,"Initialise: \n");
  fprintf(lfile,"Display: 1D Nhits       32 0 32\n");
  fprintf(lfile,"Display: 1D NhitsPrompt 32 0 32\n");
  fprintf(lfile,"Display: 1D NhitsRand   32 0 32\n");
  fprintf(lfile,"Display: 1D Hits        %d 0 %d\n", n,n);
  fprintf(lfile,"Display: 1D HitsPrompt  %d 0 %d\n", n,n);
  fprintf(lfile,"Display: 1D HitsRand    %d 0 %d\n", n,n);
  fprintf(lfile,"Display: 1D ScalerCurr %d 0 %d\n",n,n);
  fprintf(lfile,"Display: 1D ScalerAcc  %d 0 %d\n",n,n);
  fprintf(lfile,"Display: 1D Eelect   1000 0 %.1f\n", eBeam);
  fprintf(lfile,"Display: 1D NDoubles  	32 0 32\n");
  fprintf(lfile,"Display: 1D Doubles  	%d 0 %d\n",n,n);
  fprintf(lfile,"Display: 1D TimeOR     1000 %.1f %.1f\n", tl,th);
  fprintf(lfile,"Display: 2D TimeOR   200 %.1f %.1f -1 Hits %d 0 %d -1\n",tl,th,n,n);
  fprintf(lfile,"Display: 2D TimeOR   200 %.1f %.1f -1 Doubles %d 0 %d -1\n",tl,th,n,n);
  fprintf(lfile,"Display: 1D Time  1000 %.1f %.1f   0 %d\n", tl,th,n);
  fprintf(lfile,"#Display: 1D EnergyOR    200 0 %.1f\n", eh);
  fprintf(lfile,"#Display: 2D EnergyOR 100 0 %.1f -1 Hits %d 0 %d -1\n",eh,n,n);
  fprintf(lfile,"#Display: 2D EnergyOR 100 0 %.1f -1 Doubles %d 0 %d -1\n",eh,n,n);
  fprintf(lfile,"#Display: 1D Energy 100 0 %.1f   0 %d\n", eh,n);
  fclose(lfile);
  if(cfile)fclose(cfile);
  return;
}
//                                           
