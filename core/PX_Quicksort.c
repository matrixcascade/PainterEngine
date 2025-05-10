#include "PX_Quicksort.h"

static void PX_Quicksort_swap(px_byte *array,px_int size, px_int left, px_int right)
{
	
	px_dword* pdw1, * pdw2;
	px_byte* pbyte1, * pbyte2;
	px_int i;
	pdw1 = (px_dword*)(array + left * size);
	pdw2 = (px_dword*)(array + right * size);
	pbyte1 = array + left*size+(size>>2)*4;
	pbyte2 = array + right*size+(size&3);
	
	for (i = 0; i < (size>>2); i++)
	{
		px_dword dwtemp = pdw1[i];
		pdw1[i] = pdw2[i];
		pdw2[i] = dwtemp;
	}
	for (i = 0; i < (size & 3); i++)
	{
		px_byte  btemp;
		btemp = pbyte1[i];
		pbyte1[i] = pbyte2[i];
		pbyte2[i] = btemp;
	}
}

static px_double PX_Quicksort_GetWeight(px_byte* array, px_int size, px_int offset, PX_QUICKSORT_REORDER_TYPE type, px_int index)
{
	
	switch (type)
	{
	case PX_QUICKSORT_REORDER_TYPE_U8:
		return (px_double)(*(px_byte*)(array + index * size+ offset));
	case PX_QUICKSORT_REORDER_TYPE_U16:
		return (px_double)(*(px_uint16*)(array + index * size + offset));
	case PX_QUICKSORT_REORDER_TYPE_U32:
		return (px_double)(*(px_uint32*)(array + index * size + offset));
	case PX_QUICKSORT_REORDER_TYPE_I8:
		return (px_double)(*(px_char*)(array + index * size + offset));
	case PX_QUICKSORT_REORDER_TYPE_I16:
		return (px_double)(*(px_int16*)(array + index * size + offset));
	case PX_QUICKSORT_REORDER_TYPE_I32:
		return (px_double)(*(px_int32*)(array + index * size + offset));
	case PX_QUICKSORT_REORDER_TYPE_F32:
		return (px_double)(*(px_float*)(array + index * size + offset));
	case PX_QUICKSORT_REORDER_TYPE_F64:
		return (px_double)(*(px_double*)(array + index * size + offset));
	default:
		return 0;
	}
}


static px_int PX_Quicksort_partition_l(px_byte *array, px_int size,px_int offset, PX_QUICKSORT_REORDER_TYPE type, px_int left, px_int right, px_int pivot_index)
{
	px_double pivot_value = PX_Quicksort_GetWeight(array, size, offset, type, pivot_index);
	px_int store_index = left;
	px_int i;

	PX_Quicksort_swap(array, size, pivot_index, right);
	for (i = left; i < right; i++)
	{
		px_double cweight = PX_Quicksort_GetWeight(array, size, offset, type, i);
		if (cweight < pivot_value)
		{
			PX_Quicksort_swap(array,size, i, store_index);
			++store_index;
		}
	}
	PX_Quicksort_swap(array, size, store_index, right);
	return store_index;
}

static px_int PX_Quicksort_partition_m(px_byte *array, px_int size, px_int offset, PX_QUICKSORT_REORDER_TYPE type, px_int left, px_int right, px_int pivot_index)
{
	px_double pivot_value = PX_Quicksort_GetWeight(array, size, offset, type, pivot_index);
	px_int store_index = left;
	px_int i;

	PX_Quicksort_swap(array, size, pivot_index, right);
	for (i = left; i < right; i++)
	{
		px_double cweight = PX_Quicksort_GetWeight(array, size, offset, type, i);
		if (cweight > pivot_value)
		{
			PX_Quicksort_swap(array, size, i, store_index);
			++store_index;
		}
	}
	PX_Quicksort_swap(array, size, store_index, right);
	return store_index;
}

px_void PX_Quicksort_MinToMax(px_void *array,px_int size,px_int offset, PX_QUICKSORT_REORDER_TYPE type, px_int left, px_int right)
{
	px_int pivot_index = left;
	px_int pivot_new_index;

	while (right > left)
	{
		pivot_new_index = PX_Quicksort_partition_l((px_byte *)array, size, offset, type, left, right, pivot_index);
		PX_Quicksort_MinToMax(array,size,offset,type, left, pivot_new_index - 1);
		pivot_index = left = pivot_new_index + 1;

	}
}

px_void PX_Quicksort_ArrayMinToMax(PX_QuickSortAtom array[], px_int left, px_int right)
{
	PX_Quicksort_MinToMax(array, sizeof(PX_QuickSortAtom), PX_STRUCT_OFFSET(PX_QuickSortAtom, weight),PX_QUICKSORT_REORDER_TYPE_F32, left, right);
}

px_void PX_Quicksort_MaxToMin(px_void* array, px_int size, px_int offset, PX_QUICKSORT_REORDER_TYPE type, px_int left, px_int right)
{
	px_int pivot_index = left;
	px_int pivot_new_index;
	while (right > left)
	{
		pivot_new_index = PX_Quicksort_partition_m((px_byte *)array, size, offset, type, left, right, pivot_index);
		PX_Quicksort_MaxToMin(array,size,offset,type,left, pivot_new_index - 1);
		pivot_index = left = pivot_new_index + 1;
	}
}

px_void PX_Quicksort_ArrayMaxToMin(PX_QuickSortAtom array[], px_int left, px_int right)
{
	PX_Quicksort_MaxToMin(array, sizeof(PX_QuickSortAtom), PX_STRUCT_OFFSET(PX_QuickSortAtom, weight), PX_QUICKSORT_REORDER_TYPE_F32, left, right);
}


