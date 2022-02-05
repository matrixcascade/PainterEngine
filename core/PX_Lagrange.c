#include "PX_Lagrange.h"

px_bool PX_LagrangeInitialize(PX_Lagrange* pLagrange, px_memorypool* mp, px_int size)
{
	PX_memset(pLagrange, 0, sizeof(PX_Lagrange));
	if (!PX_CircularBufferInitialize(mp, &pLagrange->circularbuffer, size))
		return PX_FALSE;
	PX_LagrangeCalculateCoeffs(pLagrange, 0);
	return PX_TRUE;
}

px_int PX_LagrangeCalculateCoeffs(PX_Lagrange* pLagrange, px_float delay)
{
	px_int intd = (px_int)delay;
	px_float Dm1 = delay - (px_float)intd;
	px_float D = Dm1 + 1;
	px_float Dm2 = Dm1 - 1;
	px_float Dm3 = Dm1 - 2;
	px_float DxDm1 = D * Dm1;
	px_float Dm2xDm3 = Dm2 * Dm3;
	pLagrange->h[0] = (-1 / 6.f) * Dm1 * Dm2xDm3;
	pLagrange->h[1] = 0.5f * D * Dm2xDm3;
	pLagrange->h[2] = -0.5f * DxDm1 * Dm3;
	pLagrange->h[3] = (1 / 6.f) * DxDm1 * Dm2;

	return intd-1;
}
px_float PX_LagrangeDelay(PX_Lagrange* pLagrange, px_float pos)
{
	px_int i;
	px_float sum = 0;
	if (pos != pLagrange->lastdelay) {
		pLagrange->ptL = PX_LagrangeCalculateCoeffs(pLagrange,pos);
		pLagrange->lastdelay = pos;
	}

	for (i = 0; i < 4; i++) {
		sum += pLagrange->circularbuffer.buffer[(pLagrange->circularbuffer.pointer + pLagrange->ptL + i) % pLagrange->circularbuffer.size] * pLagrange->h[i];
	}
	return sum;
}

px_void PX_LagrangeSetDelay(PX_Lagrange* pLagrange, px_float pos)
{
	pLagrange->actdelay = pos;
}

px_float PX_LagrangeActivingDelay(PX_Lagrange* pLagrange)
{
	return PX_LagrangeDelay(pLagrange, pLagrange->actdelay);
}

px_void PX_LagrangeFree(PX_Lagrange* pLagrange)
{
	PX_CircularBufferFree(&pLagrange->circularbuffer);
}
