// Macro to read over TaggEff_analysis.root file,
// scan Set_X_*_Taggeff, Set_X_*_TaggeffEg, Set_X_*_PolEnh
// & Set_X_*_PolEngEg histograms, and get the bin centers
// and bin contents. 
//
// Output to a .dat text file for future reading and convenience.
//
// By Roddy G Macrae, February 2014
//
//
//----------------------------------------------------------------------------------------//



void Enhread(Char_t* fname)
{
  
  TFile* gfile = new TFile(fname);
  ofstream ofile;
  
  //Int_t SetN[];
 // Int_t SetN[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13};
  Int_t SetN[]={0,1,2,3,4};
 // Int_t Run[]={17297,17333,17366,17413,17452,17512,17578,17596,17639,17686,17761,17804,17860,17931};   //Run number of first TaggEff file in each set
  Int_t Run[]={14665,15116,15253,15421,15578};   //Run number of first TaggEff file in each set


  Char_t* Type[]={"_Para","_Perp","_Amor"};
  Char_t* HistoType[]={"_taggeffEg","_polenhEg"}; 

  TString Name;
  TString RunType;
  TString RunNumber;
  TString TaggFile;
  TString SetNumber;
  TString Histogram;

  
  for(Int_t j=0;j<3;j++){                  // For loop to loop over Para, Perp and Amor runs
    
    RunType.Form("%s",Type[j]);            //Assigns the para, perp, amor variables to the string
    cout << "" << endl;
    cout << "Run type is " << RunType << endl;
    
    for(Int_t n=0;n<5;n++){               // For loop to loop over the different tagging efficiency sets. CAM  Should This not be size SetN??
      
      SetNumber.Form("%d",SetN[n]);        // ''     ''  tagging efficiency set number
      RunNumber.Form("%d",Run[n]);
      
      cout << "Tagging efficiency set number = " << SetNumber << endl;
      
      for(Int_t x=0;x<2;x++){              // For loop to loop over the different kinds of histograms within each set
	
	Histogram.Form("%s",HistoType[x]); // ''     ''  tagging efficiency and pol histogram type 
	
	cout << "Histogram type is " << Histogram << endl;

	Name = "Set_"+SetNumber+RunType+Histogram;
	TaggFile = RunNumber;
	
	
	cout << "File name = " << Name << endl;
	
	// If statement to continue loop if the root histogram doesn't exist.
	TH1F* hist = (TH1F*)(gfile->Get(Name));
	if(!hist){
	  cout << "Could not find TaggEff histogram " << Name << endl;
	  continue;
	}
	
	
	TString title = hist->GetTitle();
	TString Output = "./Sets/Run_"+title+"_"+TaggFile+".dat";
	
	ofile.open(Output.Data());
	ofile << "# Bin" << "     " << "Bin Center" << "     " <<"Bin Content" << endl;

	ofile.precision(3);                                                                         // Sets the decimal place precision of the output files (so as to avoid integers and too many decimal places.
	ofile.setf(ios::fixed);                                                                     // Needs to be included to fix the precision and make the above statement work.
	ofile.setf(ios::showpoint);                                                                 // Included to make above statement work.
	
	for(Int_t i=1;i<353;i++){
	  
	  float bincenter  = hist->GetBinCenter(i);
	  float bincontent = hist->GetBinContent(i);
	  
	  ofile << "  " <<  i << "       " << bincenter  << "         " << bincontent  << endl;
	 
	}
	
	ofile.close();
	
      }                                // Closes 'x' for loop
    }                                  // Closes 'n' for loop
  }                                    // Closes 'j' for loop
  
  cout << "End of loop " << n << endl;
  
  return;
  
}
