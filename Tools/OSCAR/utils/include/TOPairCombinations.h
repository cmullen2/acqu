/*************************************************************************
 * Author: Dominik Werthmueller, 2008-2016
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOPairCombinations                                                   //
//                                                                      //
// This class calculates all pair combinations of a set containing      //
// N elements (N must be even). The elements are represented by integer //
// numbers from 0 to N-1.                                               //
//                                                                      //
// The total number of possible combinations are                        //
//                                                                      //
//             1        N!                                              //
// C_n(N) = ------- * ------                                            //
//          2^(N/2)   (N/2)!                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef OSCAR_TOPairCombinations
#define OSCAR_TOPairCombinations

#include "Rtypes.h"

class TOPairCombinations
{

protected:
    UInt_t fNElements;                          // Number of elements in the set
    UInt_t fNCombinations;                      // Number of combinations
    UInt_t fCurrCombination;                    // Current combination
    UInt_t** fCombinations;                     // Pointer to the 2-dim. array containing the combinations

    void Combine(UInt_t* listA, UInt_t sizeA, UInt_t* listB, UInt_t sizeB);

public:
    TOPairCombinations() : fNElements(0), fNCombinations(0), fCurrCombination(0), fCombinations(0) { }
    TOPairCombinations(UInt_t nElements, UInt_t* indices = 0);
    virtual ~TOPairCombinations();

    void PrintCombinations() const;
    UInt_t GetNElements() const { return fNElements; }
    UInt_t GetNCombinations() const { return fNCombinations; }
    UInt_t** GetCombinations() const { return fCombinations; }
    UInt_t* GetPair(UInt_t pr, UInt_t comb) const;

    static UInt_t CalculateNumberOfCombinations(UInt_t nElements);

    ClassDef(TOPairCombinations, 0)  // Calculates pair combinations
};

#endif

