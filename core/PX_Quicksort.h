#ifndef PX_QUICKSORT_H
#define PX_QUICKSORT_H

#include "PX_Typedef.h"
typedef enum
{
	PX_QUICKSORT_REORDER_TYPE_U8,
	PX_QUICKSORT_REORDER_TYPE_U16,
	PX_QUICKSORT_REORDER_TYPE_U32,
	PX_QUICKSORT_REORDER_TYPE_I8,
	PX_QUICKSORT_REORDER_TYPE_I16,
	PX_QUICKSORT_REORDER_TYPE_I32,
	PX_QUICKSORT_REORDER_TYPE_F32,
	PX_QUICKSORT_REORDER_TYPE_F64,
}PX_QUICKSORT_REORDER_TYPE;
typedef struct
{
	px_float weight;
	px_void *pData;
}PX_QuickSortAtom;
px_void PX_Quicksort_MinToMax(px_void* array, px_int size, px_int offset, PX_QUICKSORT_REORDER_TYPE type, px_int left, px_int right);
px_void PX_Quicksort_MaxToMin(px_void* array, px_int size, px_int offset, PX_QUICKSORT_REORDER_TYPE type, px_int left, px_int right);
px_void PX_Quicksort_ArrayMinToMax(PX_QuickSortAtom array[], px_int left, px_int right);
px_void PX_Quicksort_ArrayMaxToMin(PX_QuickSortAtom array[], px_int left, px_int right);
#endif
