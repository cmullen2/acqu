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
// Begin_Latex C_{n}(N) = #frac{1}{2^{N/2}} #upoint #frac{N!}{(N/2)!}End_Latex
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TError.h"

#include "TOPairCombinations.h"

ClassImp(TOPairCombinations)

//______________________________________________________________________________
TOPairCombinations::TOPairCombinations(UInt_t nElements, UInt_t* indices)
{
    // Constructor for 'nElement' elements to combine.
    // If 'indices' is non-zero, the indices of the elements will be read
    // from this array. Otherwise an indexing from 0 to 'nElements'-1 will be used.

    fNElements       = 0;
    fNCombinations   = 0;
    fCurrCombination = 0;
    fCombinations    = 0;

    // check if nElements is even
    if (nElements % 2 == 1)
    {
        Error("TOPairCombinations()", "Number of elements (%d) has to be even!", nElements);
        return;
    }

    // Set number of elements and combinations
    fNElements = nElements;
    fNCombinations = CalculateNumberOfCombinations(fNElements);
    fCurrCombination = 0;

    // Create combinations array
    fCombinations = new UInt_t*[fNCombinations];
    for (UInt_t i = 0; i < fNCombinations; i++) fCombinations[i] = new UInt_t[fNElements];

    // Create combinations
    UInt_t listA[fNElements];
    UInt_t listB[fNElements];

    // initialize the second list
    for (UInt_t i = 0; i < fNElements; i++)
    {
        if (indices) listB[i] = indices[i];
        else listB[i] = i;
    }

    // create combinations
    Combine(listA, 0, listB, fNElements);
}

//______________________________________________________________________________
TOPairCombinations::~TOPairCombinations()
{
    // Destructor.

    // delete combinations array
    if (fCombinations)
    {
        for (UInt_t i = 0; i < fNCombinations; i++) delete [] fCombinations[i];
        delete [] fCombinations;
    }
}

//______________________________________________________________________________
void TOPairCombinations::PrintCombinations() const
{
    // Prints all combinations to stdout.

    // Iterate over all combinations
    for (UInt_t i = 0; i < fNCombinations; i++)
    {
        printf("{");

        // Iterate over all elements
        for (UInt_t j = 0; j < fNElements; j++)
        {
            if (j % 2 == 0) printf("[");
            printf("%d ", fCombinations[i][j]);
            if (j % 2 == 1) printf("\b], ");
        }

        printf("\b\b}\n");
    }
}

//______________________________________________________________________________
void TOPairCombinations::Combine(UInt_t* listA, UInt_t sizeA, UInt_t* listB, UInt_t sizeB)
{
    // Recursive function used for the calculation of the
    // combinations.
    // parameters: listA: empty at the first call
    //             sizeA: size of list A
    //             listB: filled with all elements at the first call
    //             sizeB: size of list B
    // This function fills different combinations of elements into
    // list A while deleting them in list B. If list B contains only
    // 2 elements the recursion is stopped and by adding list A and B
    // a new combination is found.

    // End of the recursion
    if (sizeB == 2)
    {
        UInt_t currElement = 0;

        // read out A
        for (UInt_t i = 0; i < sizeA; i++) fCombinations[fCurrCombination][currElement++] = listA[i];

        // read the two elements in B
        fCombinations[fCurrCombination][currElement++] = listB[0];
        fCombinations[fCurrCombination][currElement++] = listB[1];

        // increment combination counter
        fCurrCombination++;
    }
    else
    {
        // index of a second element
        UInt_t second = 1;

        // modify lists A and B
        while (second != sizeB)
        {
            UInt_t listA_new[6];
            UInt_t sizeA_new = sizeA;
            UInt_t listB_new[6];
            UInt_t sizeB_new = 0;

            // copy A
            for (UInt_t i = 0; i < sizeA; i++) listA_new[i] = listA[i];

            // put the first element of B into A
            listA_new[sizeA_new++] = listB[0];

            // put a second element into A
            listA_new[sizeA_new++] = listB[second];

            // put the rest into the new B
            for (UInt_t i = 0; i < sizeB; i++)
            {
                if (i != 0 && i != second) listB_new[sizeB_new++] = listB[i];
            }

            // call recursion
            Combine(listA_new, sizeA_new, listB_new, sizeB_new);

            // get to the next second element
            second++;
        }
    }
}

//______________________________________________________________________________
UInt_t* TOPairCombinations::GetPair(UInt_t pr, UInt_t comb) const
{
    // Returns a pointer to the 2 elements of the pair pr of the
    // combination comb.

    // Check if pair is in range
    if (pr >= fNElements / 2)
    {
        Error("GetPair()", "Number of pair (%d) has to be smaller than %d!", pr, fNElements / 2);
        return 0;
    }

    // Check if combination is in range
    if (comb >= fNCombinations)
    {
        Error("GetPair()", "Number of combination (%d) has to be smaller than %d!", comb, fNCombinations);
        return 0;
    }

    // Return pair
    return &fCombinations[comb][2 * pr];
}

//______________________________________________________________________________
UInt_t TOPairCombinations::CalculateNumberOfCombinations(UInt_t nElements)
{
    // Returns the number of possible pair combinations of a set
    // containing nElements elements.

    // check if nElements is even
    if (nElements % 2 == 1)
    {
        Error("CalculateNumberOfCombinations()",
              "Number of elements (%d) has to be even!", nElements);
        return 0;
    }

    UInt_t a = 1 << (nElements / 2);
    UInt_t b = 1;

    for (UInt_t i = nElements; i > nElements / 2; i--) b *= i;

    return b / a;
}

