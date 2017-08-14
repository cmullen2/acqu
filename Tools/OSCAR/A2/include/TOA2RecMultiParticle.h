/*************************************************************************
 * Author: Dominik Werthmueller, 2009-2014
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOA2RecMultiParticle                                                 //
//                                                                      //
// Base class for the reconstruction of multiple correlated particles   //
// in the A2 setup.                                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef OSCAR_TOA2RecMultiParticle
#define OSCAR_TOA2RecMultiParticle

#include "TObject.h"

class TOA2RecParticle;
class TOA2DetParticle;

class TOA2RecMultiParticle : public TObject
{

protected:
    Int_t fNRecParticles;                               // number of reconstructed particles
    TOA2RecParticle** fRecParticles;                    //[fNRecParticles] pointer to the reconstructed particle list
    Double_t fChiSquare;                                // chi square value of reconstruction

public:
    TOA2RecMultiParticle() : TObject(),
                             fNRecParticles(0), fRecParticles(0), fChiSquare(0.) { }
    TOA2RecMultiParticle(Int_t nPart);
    virtual ~TOA2RecMultiParticle();

    virtual Bool_t Reconstruct(Int_t nPart, TOA2DetParticle** partList) = 0;
    
    Int_t GetNReconstructedParticles() const { return fNRecParticles; }
    TOA2RecParticle** GetReconstructedParticles() const { return fRecParticles; }
    TOA2RecParticle* GetReconstructedParticle(Int_t index) const;
    Double_t GetChiSquare() const { return fChiSquare; }

    ClassDef(TOA2RecMultiParticle, 1)  // Reconstruction of multiple correlated particles in the A2 setup
};

#endif

