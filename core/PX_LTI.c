#include "PX_LTI.h"

px_bool PX_LTIInitialize(PX_LTI* pLTI, px_memorypool* mp, px_int sizeB, px_int sizeA)
{
	PX_memset(pLTI, 0, sizeof(PX_LTI));

	if (sizeA > PX_COUNTOF(pLTI->a)) goto _ERROR;
	if (sizeB > PX_COUNTOF(pLTI->b)) goto _ERROR;

	if (!PX_CircularBufferInitialize(mp, &pLTI->cbuf, sizeB))return PX_FALSE;
	if (!PX_CircularBufferInitialize(mp, &pLTI->cbufout, sizeA))return PX_FALSE;
	pLTI->dirty_grdel = PX_TRUE;
	pLTI->a_count = sizeA;
	pLTI->b_count = sizeB;
	pLTI->mp = mp;
	return PX_TRUE;
_ERROR:
	PX_CircularBufferFree(&pLTI->cbuf);
	PX_CircularBufferFree(&pLTI->cbufout);
	return PX_FALSE;
}

px_void PX_LTISetKernel(PX_LTI* pLTI, px_float a[], px_float b[])
{
	
	PX_memcpy(pLTI->a, a, sizeof(pLTI->a_count));
	PX_memcpy(pLTI->b, b, sizeof(pLTI->b_count));
	pLTI->dirty_grdel = PX_TRUE;
}

px_void PX_LTIPush(PX_LTI* pLTI, px_float v)
{
	PX_CircularBufferPush(&pLTI->cbuf, v);
}

px_float PX_LTIFilter(PX_LTI* pLTI, px_float v)
{
	PX_CircularBufferPush(&pLTI->cbuf, v);
	return PX_LTIConvol(pLTI);
}

px_float PX_LTIConvol(PX_LTI* pLTI)
{
	px_float sum = 0;
	px_int i;
	px_int pAl2 = pLTI->cbuf.pointer;
	px_int count = pLTI->b_count - pAl2;
	
	for (i = 0; i < count; i++)
		sum += pLTI->b[i] * PX_CircularBufferGet(&pLTI->cbuf, pAl2 + i);
	for (i = 0; i < pAl2; i++)
		sum += pLTI->b[count + i] * PX_CircularBufferGet(&pLTI->cbuf,  i);

	pAl2 = pLTI->cbufout.pointer;

	count = pLTI->a_count - pAl2;
	for (i = 0; i < count; i++)
		sum -= pLTI->a[i] * PX_CircularBufferGet(&pLTI->cbufout,pAl2 + i);
	for (i = 0; i < pAl2; i++)
		sum -= pLTI->a[count + i] * PX_CircularBufferGet(&pLTI->cbufout,i);

	PX_CircularBufferPush(&pLTI->cbufout,sum);
	return sum;
}

px_float PX_LTIGroupDelay(PX_LTI* pLTI, px_float f, px_float fs)
{
	if (pLTI->dirty_grdel) {
		pLTI->grdel =PX_GroupDelay(f, pLTI->b, pLTI->b_count, pLTI->a, pLTI->a_count, fs);
		pLTI->dirty_grdel = PX_FALSE;
	}
	return pLTI->grdel;
}

px_float PX_LTIPhaseDelay(PX_LTI* pLTI, px_float f, px_float fs)
{
	px_float grpdel = PX_LTIGroupDelay(pLTI,f, fs);
	px_float omega = (px_float)(2.0f * PX_PI * f / fs);
	px_float phdel = grpdel - omega * PX_PhaseDelayDerive(omega, pLTI->b, pLTI->b_count, pLTI->a, pLTI->a_count, 0.0005f);

	return phdel;
}

px_void PX_LTIFree(PX_LTI* pLTI)
{
	PX_CircularBufferFree(&pLTI->cbuf);
	PX_CircularBufferFree(&pLTI->cbufout);
}

static px_int PX_Nchoose(px_int n, px_int k) {
	px_int divisor = 1;
	px_int multiplier = n;
	px_int answer = 1;
	k = (k > n - k)?n-k:k;
	while (divisor <= k)
	{
		answer = (answer * multiplier) / divisor;
		multiplier--;
		divisor++;
	}
	return answer;
}

px_bool PX_ThirianInitialize(PX_Thirian* pThi, px_memorypool* mp, px_int N)
{
	PX_memset(pThi, 0, sizeof(PX_Thirian));
	if (!PX_LTIInitialize(&pThi->LTI,mp,N+1,N))
	{
		return PX_FALSE;
	}
	PX_ThirianSetCoeffs(pThi, 0);
	pThi->N = N;
	return PX_TRUE;
}

px_void PX_ThirianSetCoeffs(PX_Thirian* pThi, px_float D)
{
	int k,n,i;
	if (D < pThi->N - 1)
		D = pThi->N - 1.0f;
	pThi->D = D;
	pThi->LTI.b[pThi->N] = 1;
	for (k = 1; k <= pThi->N; k++) {
		px_double ak = (px_double)PX_Nchoose(pThi->N, k);
		if (k % 2 == 1)
			ak = -ak;
		for (n = 0; n < k; n++)
			ak *= ((px_double)D - ((px_double)pThi->N - n));
		for (i = 0; i < k; i++)
			ak /= ((px_double)D -((px_double)-k + i));
		pThi->LTI.a[k - 1] = (px_float)ak;
		pThi->LTI.b[pThi->N - k] = (px_float)ak;
	}
	pThi->LTI.dirty_grdel = PX_TRUE;
}

px_void PX_ThirianFree(PX_Thirian* pThi)
{
	PX_LTIFree(&pThi->LTI);
}

px_bool PX_FilterC1C3Initialize(PX_FilterC1C3* pFilter, px_memorypool* mp)
{
	PX_memset(pFilter, 0, sizeof(PX_FilterC1C3));
	if (!PX_LTIInitialize(&pFilter->LTI, mp, 1, 1))
	{
		return PX_FALSE;
	}

	return PX_TRUE;
}

px_void PX_FilterC1C3SetCoeffs(PX_FilterC1C3* pFilter, px_float freq, px_float c1, px_float c3)
{
	if (pFilter->freq == freq && pFilter->c1 == c1 && pFilter->c3 == c3)
		return;

	do 
	{
		px_float g = (px_float)(1.0 - c1 / freq);
		px_float b = (px_float)(4.0 * c3 + freq);
		px_float a1 = (px_float)((-b + PX_sqrtd((px_double)b * b - 16.0 * c3 * c3)) / (4.0 * c3));
		pFilter->LTI.b[0] = g * (1 + a1);
		pFilter->LTI.a[0] = a1;
		pFilter->freq = freq;
		pFilter->c1 = c1;
		pFilter->c3 = c3;
		pFilter->LTI.dirty_grdel = PX_TRUE;
	} while (0);

}

px_void PX_FilterC1C3Free(PX_FilterC1C3* pFilter)
{
	PX_LTIFree(&pFilter->LTI);
}

px_bool PX_BiquadInitialize(PX_Biquad* pBiquad, px_memorypool* mp)
{
	PX_memset(pBiquad, 0, sizeof(PX_Biquad));
	if (!PX_LTIInitialize(&pBiquad->LTI, mp, 3, 2))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_void PX_BiquadSetCoeffs(PX_Biquad* pBiquad, px_float f0, px_float fs, px_float Q, PX_BIQUAD_TYPE type)
{
	px_float a = 1 / (2 * (px_float)PX_tand(PX_PI * f0 / fs));
	px_float a2 = a * a;
	px_float aoQ = a / Q;
	px_float d = (4 * a2 + 2 * aoQ + 1);

	pBiquad->LTI.a[0] = -(8 * a2 - 2) / d;
	pBiquad->LTI.a[1] = (4 * a2 - 2 * aoQ + 1) / d;

	switch (type) {
	case PX_BIQUAD_TYPE_PASS:
		pBiquad->LTI.b[0] = 2 * aoQ / d;
		pBiquad->LTI.b[1] = 0;
		pBiquad->LTI.b[2] = -2 * aoQ / d;
		break;
	case PX_BIQUAD_TYPE_LOW:
		pBiquad->LTI.b[0] = 1 / d;
		pBiquad->LTI.b[1] = 2 / d;
		pBiquad->LTI.b[2] = 1 / d;
		break;
	case PX_BIQUAD_TYPE_HIGH:
		pBiquad->LTI.b[0] = 4 * a2 / d;
		pBiquad->LTI.b[1] = -8 * a2 / d;
		pBiquad->LTI.b[2] = 4 * a2 / d;
		break;
	case PX_BIQUAD_TYPE_NOTCH:
		pBiquad->LTI.b[0] = (1 + 4 * a2) / d;
		pBiquad->LTI.b[1] = (2 - 8 * a2) / d;
		pBiquad->LTI.b[2] = (1 + 4 * a2) / d;
		break;
	}
	pBiquad->LTI.dirty_grdel = PX_TRUE;
}

px_void PX_BiquadFree(PX_Biquad* pBiquad)
{
	PX_LTIFree(&pBiquad->LTI);
}


px_bool PX_DelayInitialize(PX_Delay* pdelay, px_memorypool* mp, px_int inv_z, PX_DELAY_DATA_TYPE type)
{
	pdelay->mp = mp;
	pdelay->inv_z = inv_z;
	pdelay->cursor = 0;
	if (type==PX_DELAY_DATA_TYPE_FLOAT)
	{
		pdelay->buffer = MP_Malloc(mp, sizeof(px_float) * inv_z);
		if (!pdelay->buffer)
		{
			return PX_FALSE;
		}
		PX_memset(pdelay->buffer, 0, sizeof(px_float) * inv_z);
	}
	else
	{
		pdelay->buffer = MP_Malloc(mp, sizeof(px_int) * inv_z);
		if (!pdelay->buffer)
		{
			return PX_FALSE;
		}
		PX_memset(pdelay->buffer, 0, sizeof(px_int) * inv_z);
	}
	
	return PX_TRUE;
}

px_void PX_DelayGo_float(PX_Delay* pdelay, px_float in[], px_float out[], px_int size)
{
	px_int i;
	for (i=0;i<size;i++)
	{
		out[i] = ((px_float *)pdelay->buffer)[pdelay->cursor];
		((px_float*)pdelay->buffer)[pdelay->cursor] = in[pdelay->cursor];
		pdelay->cursor++;
		pdelay->cursor %= pdelay->inv_z;
	}
}

px_void PX_DelayGo_int(PX_Delay* pdelay, px_int in[], px_int out[], px_int size)
{
	px_int i;
	for (i = 0; i < size; i++)
	{
		out[i] = ((px_int*)pdelay->buffer)[pdelay->cursor];
		((px_int*)pdelay->buffer)[pdelay->cursor] = in[pdelay->cursor];
		pdelay->cursor++;
		pdelay->cursor %= pdelay->inv_z;
	}
}


px_void PX_DelayFree(PX_Delay* pDelay)
{
	MP_Free(pDelay->mp,pDelay->buffer);
}

