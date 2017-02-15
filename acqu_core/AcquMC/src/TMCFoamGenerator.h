//--Author	JRM Annand   30th Jun 2007  Move foam to generator	        //--Rev 	JRM Annand...16st Apr 2008  Add TMCdS5MDMParticle option
//--Rev 	JRM Annand...25th Jun 2008  General foam weighting option
//--Rev 	JRM Annand...26th Jun 2008  Different models into this class
//--Rev 	JRM Annand...17th Sep 2008  Weighting options extended
//--Update	JRM Annand...25th Jan 2009  Check not beam for fReaction

//--Description
//                *** Acqu++ <-> Root ***
// Online/Offline Analysis of Sub-Atomic Physics Experimental Data 
//
// TMCFoamGenerator
//
// Monte Carlo Kinematics Generator
// Use TFoam to sample N-dimensional kinematic distributions

#ifndef __TMCFoamGenerator_h__
#define __TMCFoamGenerator_h__

// Recognised configure commands
enum { EMCFoamOpt = 100, EMCFoamDist, EMCFoamLimits, EMCFoamParticle,
       EMCFoamPolB, EMCFoamPolT, EMCFoamPolR, EMCFoamWgt, EMCFoamModel };
enum { EMCFoamMaxDim = 16 };
// Specialist foam particles
enum { EMCFoamDefault, EMCFoamMDMdS5 };
// Specialist foam models
enum { EMCFoamDefaultM, EMCFoamSAIDM, EMCFoamMDMM, EMCFoamDgpM };

#include "TMCGenerator.h"
#include "TMCFoamInt.h"
#include "TMCPhotoPSInt.h"
#include "TMCdS5MDMInt.h"
#include "TMCDgpInt.h"

class TMCFoamGenerator : public TMCGenerator {
 protected:
  TList*   fFoamList;               // list of foam-generating particles
  TFoam* fFoamX;                    // Foam generator
  TMCFoamInt* fInt;                 // Generator n-dim function for TFoam
  Double_t* fX;                     // random generated vector
  Double_t* fXscale;                // scaled random generated vector
  Double_t* fScale;                 // Xi linear scale factors
  Double_t* fFoamInitOpt;           // options for initialisation stage
  Int_t fNDim;                      // Dimension of vector
  Int_t fNDataBase;                 // No. of vectors to generate
  Int_t fFnOpt;                     // Options for function choice
  Int_t fFoamModel;                 // Model options...SAID, MDM etc.
 public:
  TMCFoamGenerator( const Char_t*, Int_t = 999999 );
  virtual ~TMCFoamGenerator();
  virtual void Flush( );                  // clear memory
  virtual void PostInit( );               // init after parameter input
  virtual void SetConfig(Char_t*, Int_t); // parameter input from config file
  virtual void ReadDatabase( Char_t* );   // input kinematics distribution
  virtual void GEvent();                  // single event

  // Inlined getters...read-only protected variables
  TList* GetFoamList(){ return fFoamList; }
  virtual Double_t* GetFoamInitOpt(){ return fFoamInitOpt; }
  virtual Double_t* GetX(){ return fX; }
  virtual Double_t* GetXscale(){ return fXscale; }
  virtual Double_t* GetScale(){ return fScale; }
  virtual Int_t GetNDim(){ return fNDim; }
  virtual Int_t GetNDataBase(){ return fNDataBase; }
  virtual Int_t GetFnOpt(){ return fFnOpt; }

  ClassDef(TMCFoamGenerator,1)
};

//-----------------------------------------------------------------------------
inline void TMCFoamGenerator::ReadDatabase( Char_t* dbFile )
{
  // Read in gridded data from Physics model of reaction process
  // Create appropriate foam integrand function

  switch(fFoamModel){
  default:
  case EMCFoamDefaultM:
    fInt = new TMCFoamInt( fNDim, fScale, this, fFnOpt );
    break;
  case EMCFoamSAIDM:
    fInt = new TMCPhotoPSInt( fNDim, fScale, this, fFnOpt );
    break;
  case EMCFoamMDMM:
    fInt = new TMCdS5MDMInt( fNDim, fScale, this, fFnOpt );
    break;
  case EMCFoamDgpM:
    fInt = new TMCDgpInt( fNDim, fScale, this, fFnOpt );
    break;
  }
  fInt->ReadData( dbFile );
}
//-----------------------------------------------------------------------------
inline void TMCFoamGenerator::GEvent( )
{
  // Sample the N-Dim TFoam distribution
  // and then call standard event
  // std::cout <<"fXbf = "<< fX[0] << " fX1 = " << fX[1] << " fX2 " << fX[2] << std::endl;
  if( fFoamX ){
    fFoamX->MakeEvent();               // sample N-dimensional space
    fFoamX->GetMCvect( fX );           // get the sample
    fInt->Scale( fX, fXscale );        // scale vector
  }
 // std::cout <<"fXsc = "<< fXscale[0] << " fX1 = " << fXscale[1] << "  fX2 = " << fXscale[2] << " fX3 = " << fXscale[3] << " fX4 = " << fXscale[4] << std::endl;
  TMCGenerator::GEvent();
     

}


#endif
