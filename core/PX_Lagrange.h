#ifndef __PX_LAGRANGE_H
#define __PX_LAGRANGE_H
#include "PX_Memory.h"
typedef struct  
{
	px_float lastdelay;
	px_float h[4];
	px_int ptL;
	px_float actdelay;
	PX_CircularBuffer circularbuffer;
}PX_Lagrange;

px_bool PX_LagrangeInitialize(PX_Lagrange* pLagrange, px_memorypool* mp, px_int size);
px_int PX_LagrangeCalculateCoeffs(PX_Lagrange* pLagrange, px_float delay);
px_float PX_LagrangeDelay(PX_Lagrange* pLagrange, px_float pos);
px_void PX_LagrangeSetDelay(PX_Lagrange* pLagrange, px_float pos);
px_float PX_LagrangeActivingDelay(PX_Lagrange* pLagrange);
px_void PX_LagrangeFree(PX_Lagrange* pLagrange);
#endif