#include "PX_Quicksort.h"

static void swap(PX_QuickSortAtom array[], px_int left, px_int right)
{
	PX_QuickSortAtom temp;
	temp = array[left];
	array[left] = array[right];
	array[right] = temp;
}


static px_int partition_l(PX_QuickSortAtom array[], px_int left, px_int right, px_int pivot_index)
{
	px_float pivot_value = array[pivot_index].weight;
	px_int store_index = left;
	px_int i;

	swap(array, pivot_index, right);
	for (i = left; i < right; i++)
		if (array[i].weight < pivot_value) {
			swap(array, i, store_index);
			++store_index;
		}
		swap(array, store_index, right);
		return store_index;
}

static px_int partition_m(PX_QuickSortAtom array[], px_int left, px_int right, px_int pivot_index)
{
	px_float pivot_value = array[pivot_index].weight;
	px_int store_index = left;
	px_int i;

	swap(array, pivot_index, right);
	for (i = left; i < right; i++)
		if (array[i].weight > pivot_value) {
			swap(array, i, store_index);
			++store_index;
		}
		swap(array, store_index, right);
		return store_index;
}

void PX_Quicksort_MinToMax(PX_QuickSortAtom array[], px_int left, px_int right)
{
	px_int pivot_index = left;
	px_int pivot_new_index;


	while (right > left) 
	{
		pivot_new_index = partition_l(array, left, right, pivot_index);
		PX_Quicksort_MinToMax(array, left, pivot_new_index - 1);
		pivot_index = left = pivot_new_index + 1;
		
	}
}

void PX_Quicksort_MaxToMin(PX_QuickSortAtom array[], px_int left, px_int right)
{
	px_int pivot_index = left;
	px_int pivot_new_index;


	while (right > left) 
	{
		pivot_new_index = partition_m(array, left, right, pivot_index);
		PX_Quicksort_MaxToMin(array, left, pivot_new_index - 1);
		pivot_index = left = pivot_new_index + 1;

	}
}


