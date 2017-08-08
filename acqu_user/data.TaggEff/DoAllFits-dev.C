#include <iostream>
#include<string>
#include <fstream>
#include <sstream>
using namespace std;

void DoAllFits()
{
   cout << " Started Do All Fits " << endl; 
  ifstream perp("/home/chris/acqu/acqu_user/data.TaggEff/SetsList_Perp.txt");
  string s;
 
  ifstream para("/home/chris/acqu/acqu_user/data.TaggEff/SetsList_Para.txt");
  string t;
  
  gROOT->LoadMacro("cbremFit_R.C++");
  //gROOT->LoadMacro("cbremFit_R-BACKUP.C++");
  cout << "Finished Loading macro " << endl;
  //Make lists of para and perp files using : ls Sets/Run_TaggEff_Polarisation_Enhancement_Parallel_* > SetsList_Para.txt

 
  // PERP LOOP //
  
 while(getline(perp,s) && getline(para,t)){
   cout << " Inside Loop " << endl;
   // Comment perp input list stream in first and produce perpendicular table files/fits.
   // Once that is completed, comment out the perpendicular input stream and put in the 
   // parallel input stream. The parallel output tables get appended to the end of the 
   // perpendicular ones so need to be done in order (perp fits followed by para fits).
   
   istringstream(s); // Perp input list stream
   fitData(s.data());
   
   TString names(s);
   names+=".table";
   cout << "Saved to " << names.Data() << endl;
   
 //  istringstream(t); // Para input list stream
 //  fitData(t.data(),names.Data());
   
 }
  

} // end function
