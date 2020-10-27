#ifndef PX_QUICKSORT_H
#define PX_QUICKSORT_H

#include "PX_Typedef.h"

typedef struct
{
	px_float weight;
	px_void *pData;
}PX_QuickSortAtom;
void PX_Quicksort_MinToMax(PX_QuickSortAtom array[], px_int left, px_int right);
void PX_Quicksort_MaxToMin(PX_QuickSortAtom array[], px_int left, px_int right);
#endif
