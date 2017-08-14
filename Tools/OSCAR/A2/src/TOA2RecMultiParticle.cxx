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


#include "TOA2RecMultiParticle.h"
#include "TOA2RecParticle.h"

ClassImp(TOA2RecMultiParticle)

//______________________________________________________________________________
TOA2RecMultiParticle::TOA2RecMultiParticle(Int_t nPart)
{
    // Constructor using 'nPart' particles as the number of detected products.
 
    // init members
    fNRecParticles = nPart;
    fRecParticles = new TOA2RecParticle*[fNRecParticles];
    for (Int_t i = 0; i < fNRecParticles; i++) fRecParticles[i] = 0;
    fChiSquare = 0.;
}

//______________________________________________________________________________
TOA2RecMultiParticle::~TOA2RecMultiParticle()
{
    // Destructor.
    
    if (fRecParticles) 
    {
        for (Int_t i = 0; i < fNRecParticles; i++) 
            if (fRecParticles[i]) delete fRecParticles[i];
        delete [] fRecParticles;
    }
}

//______________________________________________________________________________
TOA2RecParticle* TOA2RecMultiParticle::GetReconstructedParticle(Int_t index) const
{
    // Return the reconstructed particle with index 'index'. Return 0 if there is no
    // particle with this index in the reconstructed particle list.

    // check index
    if (index < 0 || index >= fNRecParticles) 
    {
        Error("GetReconstructedParticle", "Index %d out of bounds (size: %d)", index, fNRecParticles);
        return 0;
    }

    // return particle
    return fRecParticles[index];
}

