/*************************************************************************
 * Author: Dominik Werthmueller, Markus Oberle, 2009-2014
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOA2RecPi0Pi0                                                        //
//                                                                      //
// Class for the reconstruction of two pi0 mesons in the A2 setup using //
// their 2 gamma decays.                                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TLorentzVector.h"

#include "TOA2RecPi0Pi0.h"
#include "TOA2DetParticle.h"
#include "TOA2RecParticle.h"
#include "TOPairCombinations.h"
#include "TOGlobals.h"

ClassImp(TOA2RecPi0Pi0)

//______________________________________________________________________________
TOA2RecPi0Pi0::TOA2RecPi0Pi0(Int_t nPart)
    : TOA2RecMultiParticle(2)
{
    // Constructor.

    // init members
    fNeutronCand = 0;
}

//______________________________________________________________________________
TOA2RecPi0Pi0::~TOA2RecPi0Pi0()
{
    // Destructor.

    if (fNeutronCand) delete fNeutronCand;
}

//______________________________________________________________________________
Bool_t TOA2RecPi0Pi0::Reconstruct(Int_t nPart, TOA2DetParticle** partList)
{
    // Reconstruction method.

    // check the number of particles
    if (nPart != 4 && nPart != 5)
    {
        if (!nPart) Error("Reconstruct", "Number of particles cannot be zero!");
        else Error("Reconstruct", "Number of particles must be 4 or 5!");
        return kFALSE;
    }
    else
    {
        Int_t id_1_1, id_1_2, id_2_1, id_2_2;
        TOA2DetParticle* bestGammas[4];

        // check number of particles
        if (nPart == 4)
        {
            // find the best two pi0
            fChiSquare = FindBestPi0Pi0(partList, &id_1_1, &id_1_2, &id_2_1, &id_2_2);

            // leave if no solution was selected
            if (fChiSquare == 1e30) return kFALSE;

            // save the gammas
            bestGammas[0] = partList[id_1_1];
            bestGammas[1] = partList[id_1_2];
            bestGammas[2] = partList[id_2_1];
            bestGammas[3] = partList[id_2_2];
        }
        else if (nPart == 5)
        {
            // find the best combination to form two pi0
            Double_t bestChi = 1e30;
        
            // subset of 4 particles
            TOA2DetParticle* subset[4];

            // loop over all 5 subsets of 4 particles
            for (Int_t i = 0; i < 5; i++)
            {
                // fill 4 elements into array
                Int_t k = 0;
                for (Int_t j = 0; j < 5; j++) 
                {
                    if (j != i) subset[k++] = partList[j];
                }

                // find the best combinations
                Double_t chi = FindBestPi0Pi0(subset, &id_1_1, &id_1_2, &id_2_1, &id_2_2);

                // continue when no solution was selected
                if (chi == 1e30) continue;

                // save the best solution
                if (chi < bestChi)
                {
                    // save currently best combination
                    bestGammas[0] = subset[id_1_1];
                    bestGammas[1] = subset[id_1_2];
                    bestGammas[2] = subset[id_2_1];
                    bestGammas[3] = subset[id_2_2];
                    bestChi = chi;
                }
            }

            // save best chi square value
            fChiSquare = bestChi;

            // leave if no solution was selected
            if (fChiSquare == 1e30) return kFALSE;

            //
            // find neutron by comparing pointers of the particle input
            // and the detected gammas list
            //
            // loop over all particles
            for (Int_t i = 0; i < 5; i++)
            {
                Bool_t inGammaList = kFALSE;

                // loop over the gammas
                for (Int_t j = 0; j < 4; j++)
                {
                    // check if particle element is in gamma list
                    if (partList[i] == bestGammas[j]) 
                    {
                        inGammaList = kTRUE;
                        break;
                    }
                }

                // check if missing element in gamma list was found
                if (!inGammaList)
                {
                    fNeutronCand = new TOA2DetParticle(*partList[i]);   
                    break;
                }
            }
        
        } // if: 5 particles found
 
        // create first pi0
        fRecParticles[0] = new TOA2RecParticle(2);
        fRecParticles[0]->Reconstruct(2, bestGammas);

        // create second pi0
        fRecParticles[1] = new TOA2RecParticle(2);
        fRecParticles[1]->Reconstruct(2, bestGammas + 2);
    
        return kTRUE;

    } // if: 4 or 5 particles found
}

//______________________________________________________________________________
Double_t TOA2RecPi0Pi0::FindBestPi0Pi0(TOA2DetParticle** list,
                                       Int_t* id_1_1, Int_t* id_1_2, Int_t* id_2_1, Int_t* id_2_2)
{
    // Find the best two pi0 by applying a chi square search for the best
    // 2g combinations in the particle list 'list' containing 'n' particles.
    // The indices of the two particles belonging to the first pi0 will be stored
    // in id_1_1 and id_1_2, the indices of the two particles belonging to the 
    // second pi0 will be stored in id_2_1 and id_2_2.
    
    Double_t bestChi = 1e30;
    Double_t chi;
    TLorentzVector p4Gamma[4];
    TLorentzVector p4Pi0[2];
    Double_t IM[2];
    Double_t IM_err[3];
    TOPairCombinations combi(4);
    UInt_t** c = combi.GetCombinations();
    
    // calculate photon 4-vectors
    for (Int_t i = 0; i < 4; i++) list[i]->Calculate4Vector(p4Gamma+i, 0.);
 
    // loop over all combinations
    for (Int_t i = 0; i < 3; i++)
    {
        // constitute the pions
        p4Pi0[0] = p4Gamma[c[i][0]] + p4Gamma[c[i][1]];
        p4Pi0[1] = p4Gamma[c[i][2]] + p4Gamma[c[i][3]];
        
        // calculate invariant mass
        IM[0] = p4Pi0[0].M();
        IM[1] = p4Pi0[1].M();
        
        // calculate invariant mass error
        IM_err[0] = IM[0] * list[c[i][0]]->CalcChi2IMError(list[c[i][1]]);
        IM_err[1] = IM[1] * list[c[i][2]]->CalcChi2IMError(list[c[i][3]]);

        // calculate chi
        chi = ((TOGlobals::kPi0Mass - IM[0]) / IM_err[0]) * ((TOGlobals::kPi0Mass - IM[0]) / IM_err[0]) +
              ((TOGlobals::kPi0Mass - IM[1]) / IM_err[1]) * ((TOGlobals::kPi0Mass - IM[1]) / IM_err[1]);
 
        // if better solution found
        if (chi < bestChi)
        {
            *id_1_1 = c[i][0];
            *id_1_2 = c[i][1];
            *id_2_1 = c[i][2];
            *id_2_2 = c[i][3];
            bestChi = chi;
        }
    }

    return bestChi;
}

