//--Author	D Glazier    XXth Jun 2007	
//--Rev         JRM Annand   20th Jul 2007  Adapt for system foam   
//--Rev         JRM Annand   25th Jun 2008  Remove brem-weight allusions   
//--Update      DI Glazier   12th Feb 2009  Add SaveEventAsParticles,rem fPSInt
//--Update      DI Glazier   30th Feb 2010  include GP4 to correct for boost rot.

//--Description
//                *** Acqu++ <-> Root ***
// Online/Offline Analysis of Sub-Atomic Physics Experimental Data 
//
// TMCPhotoPSGenerator
//
// Read E/theta dependence from data base file generated by SAID
// Calculate all 16 observables and use to generate lab 4-momenta

#ifndef __TMCPhotoPSGenerator_h__
#define __TMCPhotoPSGenerator_h__

#include "TMCFoamGenerator.h"
#include "TMCPhotoPSInt.h"

class TMCPhotoPSGenerator : public TMCFoamGenerator {
 protected:
  Int_t fNE;                              // # energy bins in database
  Int_t fNth;                             // # theta bins in database
  //TMCPhotoPSInt* fPSInt;                //removed dglazier fInt is used instead
 public:
  TMCPhotoPSGenerator( const Char_t*, Int_t = 999999 );
  virtual ~TMCPhotoPSGenerator();

  virtual void GP4( TMCParticle* );       // create 4-momenta for reaction
  virtual void SaveEventAsParticles(  );  //Overwrite to include writing polarisation
  ClassDef(TMCPhotoPSGenerator,1)
};

inline void TMCPhotoPSGenerator::GP4( TMCParticle* particle )
{
  // Phase-space distributions for arbitrary number of particles
  // Generate the kinematics for the N-Body decay and then in turn
  // call GP4 recursively to generate the decays for all daughters
  Int_t n = particle->GetNd();
  if( !n ) return;
  // generate N-body decay kinematics
  if( !particle->GenDecay() ){
    fIsError = kTRUE;
    return;
  }
  // generate decays for daughters (and any daughters they may have)
  TMCParticle* ptcl;
  TIter nextp( particle->GetDecayList() );
  Double_t phiR=0;//default do nothing
  if(particle==fReaction) phiR = fReaction->GetP4()->Phi();//unless reaction
  if(TString(particle->GetName())==TString("QP"))phiR=particle->GetP4()->Phi();
  for(Int_t i=0; i<n; i++){
    //phi angle from distribution was already in lab
    //BoostLab rotates assuming CM frame y=ZlabXZreaction
    //so particle rotated by beam phi angle, which is a rand #
    //"solution" rotate back to original phi
    //CAM 09/01/2017 Need to know why this is needed.
    if( !(ptcl = (TMCParticle*)nextp()) ) break;
   // std::cout <<"bfrot "<<ptcl->GetP4()->Phi() <<"         phiR = "<< phiR<<" "<<i<<" "<<ptcl->GetName()<<std::endl;//phiR is non zero and performing rotation
    ptcl->GetP4()->RotateZ(-phiR);
    //    if(TString(particle->GetName())==TString("Pi0"))ptcl->GetP4()->SetPhi(1.4);
    // ptcl->GetP4()->SetPhi(1.4);
    //std::cout <<"afrot "<<ptcl->GetP4()->Phi() <<std::endl;//phiR is non zero and performing rotation
    GP4( ptcl );
  }
}

#endif
