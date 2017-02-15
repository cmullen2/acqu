//--Author	JRM Annand   24th May 2005	      
//--Rev         JRM Annand   18th Oct 2005  Debug hbook ntuple output    
//--Rev         JRM Annand   26th Apr 2006  Add options    
//--Rev         JRM Annand   11th May 2006  TH1D randoms, m runs of n events   
//--Rev 	JRM Annand...21st Jan 2007  4v0 update, fix beam
//--Rev 	JRM Annand...16st Apr 2008  Add TMCdS5MDMParticle
//--Rev 	JRM Annand... 3rd Jun 2008...const Char_t*...gcc 4.3
//--Rev 	JRM Annand... 3rd Jul 2008  Recoil particle for QF
//--Rev 	JRM Annand...16th Jul 2008  Move QF back to TMCParticle
//--Rev 	JRM Annand...25th Jan 2009  GetPDG_ID
//--Rev 	JRM Annand...27th Jan 2009  Give particles fBeam not fP4Beam
//--Rev 	DI Glazier...12th Feb 2009  Add InitTreeParticles SaveEventAsParticles
//--Rev 	JRM Annand...10th May 2009  QF without beam-energy compensation
//--Rev 	JRM Annand... 5th Jun 2009  Mods for beam-parent particles
//--Rev 	JRM Annand...20th Jul 2009  Beam primed...check energy
//--Rev 	JRM Annand... 1st Sep 2009  delete[]
//--Rev  	JRM Annand...25th Feb 2012  Out-of-plane angle
//--Rev  	JRM Annand...27th Feb 2012  Use-degrees option, and Theta-dist
//--Update 	JRM Annand...26th Apr 2013  Bug fix InitParticleDist()

//--Description
//                *** Acqu++ <-> Root ***
// Online/Offline Analysis of Sub-Atomic Physics Experimental Data 
//
// TMCGenerator
//
// Monte Carlo Kinematics Generator
// Beam + Target -> Reaction -> Products
// Store
// 1) Interaction vertex in target (x,y,z)
// 2) Beam 4-momentum
// 3) 4-momenta of reaction products
// Output as ROOT tree (decodable by macro MCReplay.C)
// OR as HBOOK ntuple, in identical format to mkin for input
// to CB/TAPS GEANT-3 model cbsim

#ifndef __TMCGenerator_h__
#define __TMCGenerator_h__

// Recognised configure commands
enum { EMCNThrow, EMCBeam, EMCTarget, EMCParticle, EMCUseDegrees,
       EMCResonance, EMCParticleDist,
       EMCTreeOut, EMCNtupleOut, EMCPartOut,
       EMCParticleCos2Phi, EMCParticleSinSqTheta,
       EMCParticleMDist, EMCParticlePDist, EMCParticleTDist,
       EMCParticleThetaDist, EMCParticleCosThDist, EMCParticlePhiDist,
       EMCParticleOOPDist
};
// Special particle decay modes
enum { EMCParticlePhaseSpace, EMCParticleQF, EMCParticleQQF,
       EMCParticleRadRes, EMCParticleResRad, EMCParticleQF_NBC
};

#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TLorentzVector.h"
#include "TList.h"
#include "TRandom.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TBranch.h"
#include "TA2System.h"
//#include "MCNtuple.h"
#include "TMCResonance.h"
#include "TMCFoamParticle.h"
#include "TMCdS5MDMParticle.h"

#include "TClonesArray.h" 

// Branch ID for Tree output
enum { EBranchPID, EBranchP4, ENBranch };

class TMCGenerator : public TA2System {
 protected:
  TRandom* fRand;                         // random number generator
  Int_t    fSeed;                         // initial random seed
  Int_t    fNrun;                         // # of runs
  TDatabasePDG* fPDG;                     // PDG data base
  Double_t fTargetMass;                   // Mass of target (GeV)
  Double_t fTargetQFRecoilMass;           // Mass of recoil fragment in QF
  Double_t fTargetRadius;                 // Target radius (cm)
  Double_t fTargetLength;                 // Target 1/2 length (cm)
  TVector3* fTargetCentre;                // centre-position of target
  TVector3* fVertex;                      // interaction vertex
  TVector3* fBeamCentre;                  // production point of beam
  TLorentzVector* fP4Target;              // target 4-momentum (usually at rest
  TLorentzVector* fP4Beam;                // 4-momentum of beam
  TLorentzVector* fP4R;                   // 4-momentum beam + target particle
  TMCParticle*    fBeam;                  // beam particle
  TMCParticle*    fTarget;                // quasi-free "particle" in target
  TMCParticle*    fReaction;              // reaction "particle"
  TClonesArray*   fClonePtcl;             // Array of particles for saving
  Int_t    fNThrow;                       // # of events
  Int_t    fNParticle;                    // # particles in final state
  TList*   fResonance;                    // list of resonances
  TList*   fReactionList;                 // list of foam-generating particles
  TList*   fParticle;                     // linked list of particles
  Char_t*  fTreeFileName;                 // name of output tree file
  Char_t*  fNtpFileName;                  // name of output tree file
  TFile*   fTreeFile;                     // output tree file
  TFile*   fNtpFile;                      // output tree file
  TTree*   fTree;                         // output tree
  TNtuple* fNtuple;                       // output ntuple
  TBranch** fBranch;                      // output branches
  void**   fEvent;                        // output data buffers
  Bool_t   fIsTreeOut;                    // root output ?
  Bool_t   fIsNtupleOut;                  // hbook output ?
  Bool_t   fIsPartOut;                    // TParticle output ?
  Bool_t   fIsError;                      // errors ?
  Bool_t   fUseDegrees;                   // input angles in degrees
 public:
  TMCGenerator( const Char_t*, Int_t = 999999 );
  virtual ~TMCGenerator();
  virtual void Flush( );                  // clear memory
  virtual void ParseBeam( Char_t* );      // beam parameter setup
  virtual void ParseTarget( Char_t* );    // target parameter setup
  virtual void ParseParticle( Char_t* );  // reaction particles setup
  virtual void InitParticleDist( Char_t*, TMCParticle*, Int_t );
  virtual void PostInit( );               // init after parameter input
  virtual void InitTree( Char_t*, Int_t = 0 ); // for ROOT TTree output
  virtual void InitTreeParticles( Char_t* , Int_t = 0 ); //for ROOT TParticle tree  
  virtual void SaveEvent();               // write event to TTree
  virtual void SaveEventAsParticles();    // write event to TTree as TParticles
  virtual void CloseEvent();              // close TTree and file
  virtual void SetConfig(Char_t*, Int_t); // parameter input from config file
  virtual void Generate();                // do the business
  virtual void GEvent();                  // generate single event
  virtual void GP4( TMCParticle* );       // create 4-momenta for reaction
  virtual TMCParticle* GetPDG_ID( Char_t*, Char_t*, Int_t=kFALSE,Int_t=kFALSE );

  // Tack-on functions for HBOOK ntuple output
  virtual void InitNtuple( Char_t* name, Int_t = 0 );
  virtual void SaveNtuple( );             // write event to HBOOK ntuple
  virtual void CloseNtuple( );            // close ntuple

  // Inlined getters...read-only protected variables
  TRandom* GetRand(){ return fRand; }
  Int_t GetSeed(){ return fSeed; }
  TDatabasePDG* GetPDG(){ return fPDG; }
  Double_t GetTargetMass(){ return fTargetMass; }
  Double_t GetTargetRadius(){ return fTargetRadius; }
  Double_t GetTargetLength(){ return fTargetLength; }
  TVector3* GetTargetCentre(){ return fTargetCentre; }
  TVector3* GetVertex(){ return fVertex; }
  TVector3* GetBeamCentre(){ return fBeamCentre; }
  TLorentzVector* GetP4Target(){ return fP4Target; }
  TLorentzVector* GetP4Beam(){ return fP4Beam; }
  TLorentzVector* GetP4R(){ return fP4R; }
  TMCParticle* GetBeam(){ return fBeam; }
  TMCParticle* GetReaction(){ return fReaction; }
  Int_t GetNthrow(){ return fNThrow; }
  Int_t GetNParticle(){ return fNParticle; }
  TList* GetResonance(){ return fResonance; }
  TList* GetReactionList(){ return fReactionList; }
  TList* GetParticle(){ return fParticle; }
  Char_t* GetTreeFileName(){ return fTreeFileName; }
  TFile*  GetTreeFile(){ return fTreeFile; }
  TTree*  GetTree(){ return fTree; }
  TBranch** GetBranch(){ return fBranch; }
  void**  GetEvent(){ return fEvent; }
  Bool_t  IsTreeOut(){ return fIsTreeOut; }
  Bool_t  IsNtupleOut(){ return fIsNtupleOut; }
  Bool_t  IsError(){ return fIsError; }

  TMCParticle* GetParticle(Char_t* name)
    { return (TMCParticle*)fParticle->FindObject( name ); }

  ClassDef(TMCGenerator,1)
};

//-----------------------------------------------------------------------------
inline void TMCGenerator::GEvent( )
{
  // Generate single reaction event
  // 1st Find Vertex and check its inside the target
  // then form beam+target reaction particle
  // and recursively generate all daughter-product distributions

  Double_t eBeam, mBeam, th, phi, costh, sinth;

  TVector3 beamTarget = *fTargetCentre - *fBeamCentre;
  Double_t tR;                    // target hit on radius tR
  Double_t tD;
  Double_t x,y,z;                 // Cartesian vertex
  Double_t tDmin = beamTarget.Mag() - 0.5 * fTargetLength;
  Double_t tDmax = tDmin + fTargetLength;
  TVector3 beamDir;
  TVector3 beamP;

  fIsError = kFALSE;                          // ensure error flag cleared
  tD = fRand->Uniform( tDmin, tDmax );        // distance along beam axis
  do{
    costh = fBeam->CosTheta();                // beam-particle polar
    sinth = TMath::Sqrt(1.0 - costh*costh);
    tR = tD * sinth/costh;                    // radius at interaction pt.
  }while( tR > fTargetRadius );               // try again if not inside
  phi = fBeam->Phi();                     // beam-particle asimuth
  x = sinth * TMath::Cos(phi);            // dir-cosines of beam
  y = sinth * TMath::Sin(phi);
  z = costh;
  beamDir.SetXYZ(x,y,z);                      // beam trajectory vector
  z = tD + fBeamCentre->Z();                  // Vertex coordinates
  x = x * tD/costh;
  y = y * tD/costh;
  fVertex->SetXYZ(x,y,z);                     // Cartesian vertex coords
  mBeam = fBeam->Mass();
  eBeam = fBeam->Energy() + mBeam;
  beamP = TMath::Sqrt( eBeam*eBeam - mBeam*mBeam ) * beamDir;
  
  // Save beam interaction vertex and energy. Add beam and target
  // (usually assumed at rest) 4-momenta to give "reaction particle"
  fP4Beam->SetVect( beamP );
  fP4Beam->SetE( eBeam );
  TLorentzVector p4r;
  // Does the beam "decay"
  // This is mainly a construct for (e,e')
  if( fBeam->IsDecay() ){
    TMCParticle* beamD = fBeam->GetFirstDecay();
    TLorentzVector* p4bD = beamD->GetP4();
    Double_t mD = p4bD->M();
    Double_t eD = eBeam;
    Double_t pD;
    while( eD >= eBeam ){
      pD = beamD->Momentum();
      eD = TMath::Sqrt(pD*pD + mD*mD);
    }
    // Decide whether to sample theta or cos(theta) distribution
    // for polar angle
    if( beamD->GetThetaLow() || beamD->GetThetaHigh() ){
      th = beamD->Theta();
      costh = TMath::Cos(th);
    }
    else{
      costh = beamD->CosTheta();
      th = TMath::ACos(costh);
    } 
    //
    // Now decide if sample a phi or out-of-plane distribution
    if( beamD->GetOOPLow() || beamD->GetOOPHigh() ){
      Double_t oop = beamD->OOP();
      TVector3 p3D(0,0,pD);
      p3D.RotateX(oop);
      p3D.RotateY(th);
      x = p3D.X();
      y = p3D.Y();
      z = p3D.Z();
      p4bD->SetVect(p3D);
      p4bD->SetE(eD);
    }
    // sample phi
    else{
      sinth = TMath::Sqrt(1.0 - costh*costh);
      phi = beamD->Phi();
      // Set components of 4-momenta in CM
      x = pD * sinth * TMath::Cos( phi );
      y = pD * sinth * TMath::Sin( phi );
      z = pD * costh;
      p4bD->SetXYZT(x,y,z,eD);
    }
    // Exchanged 4-momentum
    TLorentzVector p4exch = *fP4Beam - *p4bD;
    p4r = p4exch + *fP4Target; 
  }
  else p4r = *fP4Beam + *fP4Target;
  // Free target particle or nucleus....reaction particle = beam+target
  fReaction->SetP4( p4r );
  // Quasi-free moved to TMCParticle
  // Generate 4-momenta recursively for all steps of the reaction process
  // If error detected, e.g. if energy/momentum balance violated at
  // some stage, then do not store event
  //std::cout<<"Start "<<std::endl;
  GP4( fReaction );
  //std::cout<<"Stop "<<std::endl;
  // Write to Tree or Ntuple
  if( !fIsError ) SaveEvent();
}

//-----------------------------------------------------------------------------
inline void TMCGenerator::GP4( TMCParticle* particle )
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
  for(Int_t i=0; i<n; i++){
    if( !(ptcl = (TMCParticle*)nextp()) ) break;
    GP4( ptcl );
  }
}

#endif
