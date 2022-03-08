#include "PX_Piano.h"

//////////////////////////////////////////////////////////////////////////
/// Algorithm implementation by Clayton Otey, Dec 2007 
/// Code by DBinary,Feb 2022
/// ///////////////////////////////////////////////////////////////////////

px_void PX_Piano_DelayInitialize(PX_Piano_Delay* c,px_memorypool *mp, px_int di)
{
    px_int p2Size = 1;
	c->mp = mp;
    while (p2Size < 2*di) { p2Size <<= 1; }
    c->size = p2Size;
    c->mask = c->size - 1;
    c->x = (px_float*)MP_Malloc(c->mp, sizeof(px_float) * c->size);

    PX_memset(c->x, 0, c->size * sizeof(px_float));

    c->cursor = 0;
    c->di = di;
    c->d1 = (c->size - di) % c->size;
    c->mp = mp;
}

px_void PX_Piano_DelayFree(PX_Piano_Delay* c)
{
    MP_Free(c->mp, c->x);
}

px_float PX_Piano_DoDelay(px_float in, PX_Piano_Delay* c)
{
    px_int cursor = c->cursor;
    px_int d1 = c->d1;
    px_float y0 = c->x[d1];
    c->x[cursor] = in;
    c->d1++;
    c->d1 &= c->mask;
    c->cursor++;
    c->cursor &= c->mask;
    return y0;
}

//////////////////////////////////////////////////////////////////////////
//PX_Piano_Reverb

px_bool PX_Piano_DWGReverbInitialize(PX_Piano_DWGReverb* preverb, px_memorypool* mp)
{
	px_int k;
	for (k=0;k<8;k++)
	{
		if (!PX_FilterC1C3Initialize(&preverb->decay[k], mp))return PX_FALSE;
		if (!PX_CircularBufferInitialize(mp, &preverb->delay[k], 1024))return PX_FALSE;
	}
	preverb->mp = mp;
	return PX_TRUE;
}

void PX_Piano_DWGReverbSetCoeffs(PX_Piano_DWGReverb *preverb, px_float c1, px_float c3, px_float a, px_float mix, px_float Fs) {
	const int lengths[8] = { 37,87,181,271,359,593,688,721 };
	px_float aa[8] = { a,1 + a,a,a,a,a,a,a };
	px_int j, k;
	preverb->mix = mix;

	for ( k = 0; k < 8; k++) {
		preverb->o[k] = 0;
		preverb->b[k] = 1;
		preverb->c[k] = k < 8 ? ((k % 2 == 0) ? 1.0f / 8.0f : -1.0f / 8.0f) : 0;
		PX_FilterC1C3SetCoeffs(&preverb->decay[k],Fs / lengths[k], c1, c3);
	}
	for ( j = 0; j < 8; j++)
		for ( k = 0; k < 8; k++)
			preverb->A[j][k] = aa[(8 + (k - j)) % 8];
}

float PX_Piano_DWGReverbGo(PX_Piano_DWGReverb* preverb, float in)
{
	px_float i[8];
	px_float out;
	const px_int lengths[8] = { 37,87,181,271,359,593,688,721 };
	px_int j, k;
	for ( j = 0; j < 8; j++) {
		i[j] = preverb->b[j] * in;
		for ( k = 0; k < 8; k++) {
			i[j] += preverb->A[j][k] * preverb->o[k];
		}
	}

	out = 0;
	for (j = 0; j < 8; j++) {

		PX_CircularBufferPush(&preverb->delay[j], i[j]);
		preverb->o[j]=PX_LTIFilter(&preverb->decay[j].LTI, PX_CircularBufferDelay(&preverb->delay[j], lengths[j]));

		preverb->o[j]+= 1e-18f;
		preverb->o[j]-= 1e-18f;
		out += preverb->c[j] * preverb->o[j] * 0.5f;
	}

	return preverb->mix * out + (1.0f - preverb->mix) * in;
}


px_void PX_Piano_DWGReverbFree(PX_Piano_DWGReverb* preverb)
{
	px_int k;
	for (k = 0; k < 8; k++)
	{
		PX_CircularBufferFree(&preverb->delay[k]);
		PX_FilterC1C3Free(&preverb->decay[k]);
	}
}

//////////////////////////////////////////////////////////////////////////
//hammer

px_void PX_Piano_IntegratorInitialize(PX_Piano_Integrator* pIntegrator, px_float Fs, px_float inival /*= 0.0*/)
{
    pIntegrator->lastout = inival; 
    pIntegrator->iFs = 1.0f / Fs;
}

px_float PX_Piano_IntegratorGo(PX_Piano_Integrator* pIntegrator, px_float in) {
	px_float out = pIntegrator->lastout + in * pIntegrator->iFs;
    pIntegrator->lastout = out;
	return out;
}



px_void PX_Piano_UnitDelayInitialize(PX_Piano_UnitDelay* pUnitDelay)
{
    pUnitDelay->lastin = 0;
}

px_float PX_Piano_UnitDelayGo(PX_Piano_UnitDelay* pUnitDelay, px_float in) {
	px_float out = pUnitDelay->lastin;
    pUnitDelay->lastin = in;
	return out;
}



px_void PX_Piano_BanksHammerInitialize(PX_Piano_BanksHammer* pBankHammer, px_float f, px_float Fs, px_float m, px_float K, px_float p, px_float Z, px_float alpha)
{
    pBankHammer->Fs = Fs;
    pBankHammer->p = p;
    pBankHammer->K = K;
    pBankHammer->mi = 1.0f / m;
    pBankHammer->Z2i = 1.0f / (2.0f * Z);
    pBankHammer->a = 0.0f;
    pBankHammer->F = 0.0f;

    PX_Piano_IntegratorInitialize(&pBankHammer->intv, Fs * 2, 0);
    PX_Piano_IntegratorInitialize(&pBankHammer->intvh, Fs * 2, 0);
    PX_Piano_UnitDelayInitialize(&pBankHammer->undel);
    pBankHammer->vh = 0;
    pBankHammer->oldvin = 0;
    PX_Piano_IntegratorGo(&pBankHammer->intvh,0);
}

px_void PX_Piano_BanksHammerTrigger(PX_Piano_BanksHammer* pBankHammer, px_float v) {
    pBankHammer->F = 0.0;
    pBankHammer->vh = 0.0;

	PX_Piano_IntegratorInitialize(&pBankHammer->intv, pBankHammer->Fs * 2, 0);
	PX_Piano_IntegratorInitialize(&pBankHammer->intvh, pBankHammer->Fs * 2, 0);
	PX_Piano_UnitDelayInitialize(&pBankHammer->undel);
    PX_Piano_IntegratorGo(&pBankHammer->intvh, v * pBankHammer->Fs * 2);
}

px_float PX_Piano_BanksHammerLoad(PX_Piano_BanksHammer* pBankHammer, px_float vin) {
	px_float rvin = (vin + pBankHammer->oldvin) * 0.5f;
	px_float F1, F2;

	px_float vs = (rvin + pBankHammer->F * pBankHammer->Z2i);
	px_float deltaV = pBankHammer->vh - vs;
	px_float deltaY = PX_Piano_IntegratorGo(&pBankHammer->intv,deltaV);
	px_float up = (deltaY > 0.0f) ? (px_float)PX_pow(deltaY, pBankHammer->p) : 0.0f;
	up = up * pBankHammer->K;
    pBankHammer->a = -pBankHammer->F * pBankHammer->mi;
    pBankHammer->vh = PX_Piano_IntegratorGo(&pBankHammer->intvh, pBankHammer->a);
	F1 = pBankHammer->F = PX_Piano_UnitDelayGo(&pBankHammer->undel,up);

	vs = (vin + pBankHammer->F * pBankHammer->Z2i);
	deltaV = pBankHammer->vh - vs;
	deltaY = PX_Piano_IntegratorGo(&pBankHammer->intv,deltaV);
	up = (deltaY > 0.0f) ? (px_float)PX_pow(deltaY, pBankHammer->p) : 0.0f;
	up = up * pBankHammer->K;
    pBankHammer->a = -pBankHammer->F * pBankHammer->mi;
    pBankHammer->vh = PX_Piano_IntegratorGo(&pBankHammer->intvh, pBankHammer->a);
	F2 = pBankHammer->F = PX_Piano_UnitDelayGo(&pBankHammer->undel,up);

	return (F1 + F2) * 0.5f;
}




px_void PX_Piano_StulovHammerInitialize(PX_Piano_StulovHammer *pStulovHammer,px_float f, px_float Fs, px_float m, px_float K, px_float p, px_float Z, px_float alpha)
{
    pStulovHammer->Fs = Fs;
    pStulovHammer->p = p;
    pStulovHammer->K = K;
    pStulovHammer->mi = 1.0f / m;
    pStulovHammer->Z2i = 1.0f / (2.0f * Z);
    pStulovHammer->a = 0.0f;
    pStulovHammer->F = 0.0f;

    pStulovHammer->S = 3;
    pStulovHammer->alpha = alpha;
    pStulovHammer->dt = 1.0f / (Fs * pStulovHammer->S);
    pStulovHammer->dti = 1.0f / pStulovHammer->dt;
    pStulovHammer->x = 0;
    pStulovHammer->v = 0;
    pStulovHammer->upprev = 0;
}

px_void PX_Piano_StulovHammerTrigger(PX_Piano_StulovHammer* pStulovHammer,px_float v) {
    pStulovHammer->v = v;
    pStulovHammer->x = 0.0;
}

px_float PX_Piano_StulovHammerLoad(PX_Piano_StulovHammer* pStulovHammer, px_float vin)
{
	px_int k;
	for ( k = 0; k < pStulovHammer->S; k++) {
		px_float up;
		px_float v1;
		px_float x1;
		px_float dupdt;
		px_int k;
		up = (pStulovHammer->x > 0) ? (px_float)PX_pow(pStulovHammer->x, pStulovHammer->p) : 0;
		dupdt = (up - pStulovHammer->upprev) * pStulovHammer->dti;
		
		for (k = 0; k < 3; k++) {
			px_float upnew;
			px_float dupdtnew;
			px_float change;
            pStulovHammer->F = pStulovHammer->K * (up + pStulovHammer->alpha * dupdt);
			if (pStulovHammer->F < 0) pStulovHammer->F = 0;
            pStulovHammer->a = -pStulovHammer->F * pStulovHammer->mi;
			v1 = pStulovHammer->v + pStulovHammer->a * pStulovHammer->dt;
			x1 = pStulovHammer->x + (v1 - (vin + pStulovHammer->F * pStulovHammer->Z2i)) * pStulovHammer->dt;
			upnew = (x1 > 0) ? (px_float)PX_pow(x1, pStulovHammer->p) : 0;
			dupdtnew = (upnew - pStulovHammer->upprev) / (2 * pStulovHammer->dt);
			change = dupdtnew - dupdt;
			dupdt = dupdt + (px_float)0.5 * change;
		}
        pStulovHammer->upprev = up;
        pStulovHammer->v = v1;
        pStulovHammer->x = x1;
	}

	return pStulovHammer->F;
}


px_void PX_Piano_dwg_nodeInitialize(PX_Piano_dwg_node* pNode, px_float z)
{
	pNode->a[0] = 0;
	pNode->a[1] = 0;
	pNode->z = z;
	pNode->load = 0;
}




px_void PX_Piano_dwgInitialize(PX_Piano_dwg *pdwg,px_memorypool *mp,px_float z, px_int del1, px_int del2, px_int commute, struct __PX_Piano_dwgs* parent) {
	pdwg->parent = parent;
	pdwg->mp = mp;
	if (del1 > 1)
		PX_Piano_DelayInitialize(&(pdwg->d[0]),mp, del1 - 1);

	if (del2 > 1)
		PX_Piano_DelayInitialize(&(pdwg->d[1]),mp, del2 - 1);

	pdwg->del1 = del1;
	pdwg->del2 = del2;
	pdwg->nl = 0;
	pdwg->nr = 0;
	pdwg->l = PX_Malloc(PX_Piano_dwg_node, mp, sizeof(PX_Piano_dwg_node));
	PX_Piano_dwg_nodeInitialize(pdwg->l, z);
	pdwg->r = PX_Malloc(PX_Piano_dwg_node, mp, sizeof(PX_Piano_dwg_node));
	PX_Piano_dwg_nodeInitialize(pdwg->r, z);
	pdwg->commute = commute;
}

px_void PX_Piano_dwgInitializeData(PX_Piano_dwg* pdwg)
{
	px_float ztot;
	px_int k;

	ztot = pdwg->l->z;
	for (k = 0; k < pdwg->nl; k++) {
		ztot += pdwg->cl[k]->z;
	}
	pdwg->alphalthis = 2.0f * pdwg->l->z / ztot;
	for (k = 0; k < pdwg->nl; k++) {
		pdwg->alphal[k] = 2.0f * pdwg->cl[k]->z / ztot;
	}

	ztot = pdwg->r->z;
	for (k = 0; k < pdwg->nr; k++) {
		ztot += pdwg->cr[k]->z;
	}
	pdwg->alpharthis = 2.0f * pdwg->r->z / ztot;
	for (k = 0; k < pdwg->nr; k++) {
		pdwg->alphar[k] = 2.0f * pdwg->cr[k]->z / ztot;
	}

}

px_void PX_Piano_dwgFree(PX_Piano_dwg* pdwg)
{
	PX_Free(pdwg->mp, pdwg->l);
	PX_Free(pdwg->mp, pdwg->r);
	if (pdwg->del1 > 1)
		PX_Piano_DelayFree(&(pdwg->d[0]));

	if (pdwg->del2 > 1)
		PX_Piano_DelayFree(&(pdwg->d[1]));
}

px_void PX_Piano_dwgConnectLeftEx(PX_Piano_dwg* pdwg,PX_Piano_dwg_node* l, px_int polarity) {
	pdwg->cl[pdwg->nl] = l;
	pdwg->pl[pdwg->nl++] = polarity;
}

px_void PX_Piano_dwgConnectRightEx(PX_Piano_dwg* pdwg, PX_Piano_dwg_node* r, px_int polarity) {
	pdwg->cr[pdwg->nr] = r;
	pdwg->pr[pdwg->nr++] = polarity;
}

px_void PX_Piano_dwgConnectLeft(PX_Piano_dwg* pdwg, PX_Piano_dwg_node* l) {
	PX_Piano_dwgConnectLeftEx(pdwg,l, 0);
}

px_void PX_Piano_dwgConnectRight(PX_Piano_dwg* pdwg, PX_Piano_dwg_node* r) {
	PX_Piano_dwgConnectRightEx(pdwg,r, 0);
}

px_void PX_Piano_dwgDoDelay(PX_Piano_dwg* pdwg ) {
	px_float dar;
	px_float dal;
	if (pdwg->del1 == 1)
		dar = pdwg->r->a[0];
	else
		dar = PX_Piano_DoDelay(pdwg->r->a[0], &(pdwg->d[0]));

	
	if (pdwg->del2 == 1)
		dal = pdwg->l->a[1];
	else
		dal = PX_Piano_DoDelay(pdwg->l->a[1], &(pdwg->d[1]));

	pdwg->l->a[0] = dar;
	pdwg->r->a[1] = dal;
}

px_void PX_Piano_dwgDoLoad(PX_Piano_dwg* pdwg) {
	if (pdwg->nl == 0)
		pdwg->loadl = 0;
	else {
		px_int k;
		pdwg->loadl = pdwg->alphalthis * pdwg->l->a[0];
		for (k = 0; k < pdwg->nl; k++) {
			px_int polarity = pdwg->pl[k] ? 0 : 1;
			pdwg->loadl += pdwg->cl[k]->load;
			pdwg->loadl += pdwg->alphal[k] * pdwg->cl[k]->a[polarity];
		}
	}

	if (pdwg->nr == 0)
		pdwg->loadr = 0;
	else {
		px_int k;
		pdwg->loadr = pdwg->alpharthis * pdwg->r->a[1];
		for (k = 0; k < pdwg->nr; k++) {
			px_int polarity = pdwg->pr[k] ? 1 : 0;
			pdwg->loadr += pdwg->cr[k]->load;
			pdwg->loadr += pdwg->alphar[k] * pdwg->cr[k]->a[polarity];
		}
	}
}

px_void PX_Piano_dwgUpdate(PX_Piano_dwg* pdwg) {
	px_float a = (pdwg->loadl - pdwg->l->a[0]);
	if (pdwg->commute) {
		px_int m;
		for (m = 0; m < pdwg->parent->M; m++) {
			a = PX_LTIFilter(&pdwg->parent->dispersion[m].LTI, a);;
		}
	}
	pdwg->l->a[1] = a;

	a = (pdwg->loadr - pdwg->r->a[1]);
	if (pdwg->commute) {
		
		a = PX_LTIFilter(&pdwg->parent->lowpass.LTI, a);
		PX_CircularBufferPush(&pdwg->parent->fracdelay3.circularbuffer, a);
		a=PX_LagrangeActivingDelay(&pdwg->parent->fracdelay3);
	}
	pdwg->r->a[0] = a;
}




px_void PX_Piano_ThirianDispersion(px_float B, px_float f, px_int M, PX_Thirian* c)
{
    px_int N = 2;
    px_float D;
	px_float C1, C2, k1, k2, k3, logB, kd, Cd, halfstep, Ikey;

	if (M == 4) {
		C1 = 0.069618f;
		C2 = 2.0427f;
		k1 = -0.00050469f;
		k2 = -0.0064264f;
		k3 = -2.8743f;
	}
	else {
		C1 = 0.071089f;
		C2 = 2.1074f;
		k1 = -0.0026580f;
		k2 = -0.014811f;
		k3 = -2.9018f;
	}
	
	logB = (px_float)PX_log(B);
	kd = (px_float)PX_exp(1.0*k1 * logB * logB + 1.0*k2 * logB + k3);
	Cd = (px_float)PX_exp(1.0*C1 * logB + C2);
	halfstep = (px_float)PX_pow(2.0, 1.0 / 12.0);
	Ikey = (px_float)(PX_log(f * halfstep / 27.5f) / PX_log(halfstep));
	D = (px_float)PX_exp(1.0*Cd - 1.0*Ikey * kd);

    if (D <= 1.0) 
    {
        PX_ThirianSetCoeffs(c, 1);
    }
    else 
    {
        PX_ThirianSetCoeffs(c, D);
    }
}

px_bool PX_Piano_dwgsInitialize(PX_Piano_dwgs *pdwgs,px_memorypool *mp,px_float f, px_float Fs, px_float inpos, px_float c1, px_float c3, px_float B, px_float Z, px_float Zb, px_float Zh)
{
	px_float lowpassdelay,dispersiondelay;
	px_int del2;
	px_int del3;
	px_float D;
	px_float tuningdelay;
	px_float deltot = Fs / f;
	px_int del1 = (px_int)(inpos * 0.5 * deltot);
	px_int i;
	PX_memset(pdwgs, 0, sizeof(PX_Piano_dwgs));
	for (i=0;i<PX_COUNTOF(pdwgs->dispersion);i++)
	{
		if (!PX_ThirianInitialize(&pdwgs->dispersion[i], mp, 2))return PX_FALSE;
	}
	if (!PX_FilterC1C3Initialize(&pdwgs->lowpass, mp))return PX_FALSE;
	if (!PX_LagrangeInitialize(&pdwgs->fracdelay3, mp, 1024))return PX_FALSE;

	if (del1 < 2)
		del1 = 1;

	if (f > 400) {
        pdwgs->M = 1;
		PX_Piano_ThirianDispersion(B, f, pdwgs->M, &(pdwgs->dispersion[0]));
	}
	else {
		px_int m;
        pdwgs->M = 4;
		for (m = 0; m < pdwgs->M; m++)
			PX_Piano_ThirianDispersion(B, f, pdwgs->M, &(pdwgs->dispersion[m]));
	}
	//px_float dispersiondelay = M*groupdelay(&(dispersion[0]),f,Fs);
	
	dispersiondelay = pdwgs->M * PX_LTIGroupDelay(&pdwgs->dispersion[0].LTI, f, Fs);

	//loss(f,Fs,c1,c3,&lowpass);
    PX_FilterC1C3SetCoeffs(&pdwgs->lowpass,f, c1, c3);
	
	lowpassdelay = PX_LTIGroupDelay(&pdwgs->lowpass.LTI,f, Fs);
	del2 = (px_int)(0.5 * (deltot - 2.0 * del1) - dispersiondelay);
	del3 = 1;
	if (del2 < 2)
		del2 = 1;
	if (del3 < 2)
		del3 = 1;

	D = (deltot - (px_float)(del1 + del1 + del2 + del3 + dispersiondelay + lowpassdelay));
    PX_LagrangeSetDelay(&pdwgs->fracdelay3, D);
	tuningdelay = D;

    
	
	PX_Piano_dwgInitialize(&pdwgs->d[0], mp,Z, del1, del1, 0, pdwgs);
	PX_Piano_dwgInitialize(&pdwgs->d[1], mp, Z, del2, del3, 1, pdwgs);
	PX_Piano_dwgInitialize(&pdwgs->d[2], mp, Zb, 0, 0, 0, pdwgs);
	PX_Piano_dwgInitialize(&pdwgs->d[3], mp, Zh, 0, 0, 0, pdwgs);

	PX_Piano_dwgConnectRight(&pdwgs->d[0], pdwgs->d[1].l);
	PX_Piano_dwgConnectLeft(&pdwgs->d[1], pdwgs->d[0].r);

	PX_Piano_dwgConnectRight(&pdwgs->d[1], pdwgs->d[2].l);
	PX_Piano_dwgConnectLeft(&pdwgs->d[2], pdwgs->d[1].r);

	PX_Piano_dwgConnectRight(&pdwgs->d[0], pdwgs->d[3].l);
	PX_Piano_dwgConnectLeft(&pdwgs->d[1], pdwgs->d[3].l);
	PX_Piano_dwgConnectLeft(&pdwgs->d[3], pdwgs->d[0].r);
	PX_Piano_dwgConnectLeft(&pdwgs->d[3], pdwgs->d[1].l);

	PX_Piano_dwgInitializeData(&pdwgs->d[0]);
	PX_Piano_dwgInitializeData(&pdwgs->d[1]);
	PX_Piano_dwgInitializeData(&pdwgs->d[2]);
	PX_Piano_dwgInitializeData(&pdwgs->d[3]);
	return PX_TRUE;
}
px_void PX_Piano_dwgsFree(PX_Piano_dwgs* pdwgs)
{
	px_int i;
	for (i = 0; i < PX_COUNTOF(pdwgs->dispersion); i++)
	{
		PX_ThirianFree(&pdwgs->dispersion[i]);
	}
	PX_FilterC1C3Free(&pdwgs->lowpass);
	PX_LagrangeFree(&pdwgs->fracdelay3);

	PX_Piano_dwgFree(&pdwgs->d[0]);
	PX_Piano_dwgFree(&pdwgs->d[1]);
	PX_Piano_dwgFree(&pdwgs->d[2]);
	PX_Piano_dwgFree(&pdwgs->d[3]);

}




px_float PX_Piano_dwgs_input_velocity(PX_Piano_dwgs* pdwgs) {
	return pdwgs->d[1].l->a[0] + pdwgs->d[0].r->a[1];
}

px_float PX_Piano_dwgs_go_hammer(PX_Piano_dwgs* pdwgs, px_float load)
{
	px_int k;
	pdwgs->d[3].l->load = load;
	for (k = 0; k < 2; k++) {
		PX_Piano_dwgDoDelay(&pdwgs->d[k]);
	}

	return pdwgs->d[1].r->a[1];
}

px_float PX_Piano_dwgs_go_soundboard(PX_Piano_dwgs* pdwgs, px_float load) {
	px_int k;
	pdwgs->d[2].l->load = load;
	for (k = 0; k < 3; k++) {
		PX_Piano_dwgDoLoad(&pdwgs->d[k]);
	}

	for (k = 0; k < 3; k++) {
		PX_Piano_dwgUpdate(&pdwgs->d[k]);
	}

	return pdwgs->d[2].l->a[1];
}



static px_float linearmap(px_float s, px_float e, px_float ds, px_float de, px_float v) {
	return ((de - ds) * (v - s) / (e - s)) + ds;
}

static px_float sigmoidal(px_float midi, px_float minV, px_float maxV, px_float ampL, px_float ampR) {

	px_float i = linearmap(21, 108, ampL, ampR, midi);

	px_float minV2 = 1 / (1 + (px_float)PX_exp(ampR));
	px_float maxV2 = 1 / (1 + (px_float)PX_exp(ampL));

	px_float escale = (maxV - minV) / (maxV2 - minV2);

	px_float offset = maxV - maxV2 * escale;

	px_float val = offset + escale / (1 + (px_float)PX_exp(i));
	return val;
}


px_bool PX_PianoKeyInitialize(px_memorypool* mp,PX_PianoKey *pPianoKey,PX_PianoKey_Parameters *param)
{
	px_int k;
	px_float f0,midinote,L,r,rho,logff0,m,alpha,p,K,rcore,E,pos,c1,c3,rhoL,T,B;
	px_float TUNE[3] = { 0, 1, -1 };
	PX_memset(pPianoKey, 0, sizeof(PX_PianoKey));




	pPianoKey->nSampleCount = -1;
	f0 = 27.5;
	midinote = 12 * (px_float)PX_log(param->f / f0) / (px_float)PX_log(2) + 21;
	L = (px_float)sigmoidal(midinote, param->minL, param->maxL, param->ampLl, param->ampLr);
	r = (px_float)sigmoidal(midinote, param->minr, param->maxr, param->amprl, param->amprr) * 0.001f;
	rho = 7850.0f;
	rho *= param->mult_density_string;
	logff0 = (px_float)PX_log(param->f / f0) / (px_float)PX_log(4192 / f0);
	logff0 = (0>logff0)?0: logff0;
	m = 0.06f - 0.058f * (px_float)PX_pow(logff0, 0.1);
	m *= param->mult_mass_hammer;
	alpha = 0.00001f * logff0;
	alpha *= param->mult_hysteresis_hammer;
	p = 2.0f + 1.0f * logff0;
	p *= param->mult_stiffness_exponent_hammer;
	K = 40.0f / (px_float)PX_pow(0.0007, p);
	K *= param->mult_force_hammer;
	pPianoKey->Zb = 4000.0f * param->mult_impedance_bridge;
	pPianoKey->Zh = 1.0f * param->mult_impedance_hammer;
	rcore = (r < 0.0006f) ? r : 0.0006f;
	rcore *= param->mult_radius_core_string;
	E = (px_float)200e9;
	E *= param->mult_modulus_string;
	pos = param->position_hammer;

	pPianoKey->param = *param;
	

	if (param->f < (48.9994278f))
		pPianoKey->nstrings = 1;
	else if (param->f < (87.3070602f))
		pPianoKey->nstrings = 2;
	else
		pPianoKey->nstrings = 3;

	 
	c1 = 0.25f;
	c3 = 5.85f;
	c3 *= param->mult_loss_filter;

	rhoL = (px_float)(PX_PI * r * r * rho);
	T = (2 * L * param->f) * (2 * L * param->f) * rhoL;
	pPianoKey->Z = (px_float)PX_sqrtd(1.0*T * rhoL);
	B = (px_float)((PX_PI * PX_PI * PX_PI) * E * rcore * rcore * rcore * rcore / (4.0 * L * L * T));

	for (k = 0; k < pPianoKey->nstrings; k++) {
		
		if(!PX_Piano_dwgsInitialize(&pPianoKey->string[k], mp, param->f * (1.0f + TUNE[k] * param->detune), param->Fs, pos, c1, c3, B, pPianoKey->Z, pPianoKey->Zb + (pPianoKey->nstrings - 1) * pPianoKey->Z, pPianoKey->Zh))return PX_FALSE;
	}

	switch (param->hammer_type) {
	default:
	case 1:

		PX_Piano_StulovHammerInitialize(&pPianoKey->StulovHammer, param->f, param->Fs, m, K, p, pPianoKey->Z, alpha);
		break;
	case 2:

		PX_Piano_BanksHammerInitialize(&pPianoKey->BanksHammer, param->f, param->Fs, m, K, p, pPianoKey->Z, alpha);
		break;
	}
	return PX_TRUE;
}

px_void PX_PianoKeyTrigger(PX_PianoKey* pPiano,px_float v) {

	pPiano->nSampleCount = 0;
	switch (pPiano->param.hammer_type) {
	default:
	case 1:
		PX_Piano_StulovHammerTrigger(&pPiano->StulovHammer, v*pPiano->param.weight);
		break;
	case 2:
		PX_Piano_BanksHammerTrigger(&pPiano->BanksHammer, v * pPiano->param.weight);
		break;
	}
}
px_int PX_PianoKeyGo(PX_PianoKey* pPiano,px_float* out, px_int samples)
{
	px_int i,n = 0;
	px_float Zx2 = 2 * pPiano->Z;
	px_float facZ = Zx2 / (pPiano->Z * pPiano->nstrings + pPiano->Zb);
	pPiano->nSampleCount += samples;
	for (i = 0; i < samples; i++) {
		px_int k;
		px_float vstring,hload,load,output;
		n++;
		vstring = 0.0;
		for (k = 0; k < pPiano->nstrings; k++) {
			
			vstring += PX_Piano_dwgs_input_velocity(&pPiano->string[k]);
		}
		
	
		switch (pPiano->param.hammer_type) {
		default:
		case 1:
			hload=PX_Piano_StulovHammerLoad(&pPiano->StulovHammer, vstring / pPiano->nstrings);
			break;
		case 2:
			hload=PX_Piano_BanksHammerLoad(&pPiano->BanksHammer, vstring / pPiano->nstrings);
			break;
		}

		load = 0;

		for (k = 0; k < pPiano->nstrings; k++) {
			load += PX_Piano_dwgs_go_hammer(&pPiano->string[k], hload / (Zx2));
			
		}
		load *= facZ;
		output = 0.0;
		for (k = 0; k < pPiano->nstrings; k++) {
			
			output += PX_Piano_dwgs_go_soundboard(&pPiano->string[k], load);
		}

		out[i] = output * 100;
	}
	return n;
}

px_void PX_PianoKeyFree(PX_PianoKey* pPiano)
{
	px_int k;
	for (k = 0; k < pPiano->nstrings; k++) {

		PX_Piano_dwgsFree(&pPiano->string[k]);
	}
}

px_bool PX_PianoSoundBoardInitialize(px_memorypool* mp,PX_PianoSoundBoard* psb,PX_PianoSoundboard_Parameters *param)
{
	PX_memset(psb, 0, sizeof(PX_PianoSoundBoard));
	psb->param.c1 = param->c1;
	psb->param.c3 = param->c3;
	psb->param.eq1 = param->eq1;
	psb->param.eq2 = param->eq2;
	psb->param.eq3 = param->eq3;
	if (!PX_BiquadInitialize(&psb->shaping1, mp))return PX_FALSE;
	if (!PX_BiquadInitialize(&psb->shaping2, mp))return PX_FALSE;
	if (!PX_BiquadInitialize(&psb->shaping3, mp))return PX_FALSE;
	if (!PX_Piano_DWGReverbInitialize(&psb->soundboard, mp))return PX_FALSE;

	PX_BiquadSetCoeffs(&psb->shaping1, param->eq1, 44100, 10, PX_BIQUAD_TYPE_NOTCH);
	PX_BiquadSetCoeffs(&psb->shaping2, param->eq2, 44100, 1, PX_BIQUAD_TYPE_HIGH);
	PX_BiquadSetCoeffs(&psb->shaping3, param->eq3, 44100, 1, PX_BIQUAD_TYPE_LOW);

	PX_Piano_DWGReverbSetCoeffs(&psb->soundboard, param->c1, param->c3, -0.25f, 1, 44100);
	return PX_TRUE;
}

px_void PX_PianoSoundBoardGo(PX_PianoSoundBoard* psb, px_float in[], px_float out[], px_int count)
{
	px_int i;
	px_float signal;
	for (i = 0; i < count; i++) {
		signal = PX_Piano_DWGReverbGo(&psb->soundboard, in[i]);
		signal += PX_LTIFilter(&psb->shaping1.LTI, signal);
		signal = PX_LTIFilter(&psb->shaping2.LTI, signal);
		signal += PX_LTIFilter(&psb->shaping3.LTI, signal);
		out[i] = signal;
	}
}

px_void PX_PianoSoundBoardFree(PX_PianoSoundBoard* psb)
{
	PX_BiquadFree(&psb->shaping1);
	PX_BiquadFree(&psb->shaping2);
	PX_BiquadFree(&psb->shaping3);
	PX_Piano_DWGReverbFree(&psb->soundboard);
}


px_bool PX_PianoInitialize(px_memorypool* mp,PX_Piano* pPiano, PX_PIANO_STYLE style)
{
	px_int i;
	const px_float PX_Piano_KeyFrequency[] = {27.5f,29.135f,30.868f,32.703f,34.648f,36.708f,38.891f,41.203f,43.654f,46.249f,48.999f,51.913f};
	const px_float PX_Piano_AmpMap[] = { 3.047837f,3.967463f,4.068119f,4.147177f,4.195764f,4.250195f,4.312485f,4.366438f,4.393855f,4.413116f,4.429189f,2.233078f,2.242712f,2.258232f,2.275072f,2.294975f,2.309848f,2.336400f,2.353222f,2.383314f,1.599312f,1.599385f,1.591346f,1.592523f,1.585473f,1.583628f,1.589321f,1.600980f,1.613558f,1.629661f,1.648730f,1.692481f,1.736692f,1.780722f,1.832306f,1.832437f,1.894783f,1.970879f,2.062356f,2.171813f,2.171074f,2.304099f,2.305844f,2.468208f,2.663217f,2.666740f,2.902329f,2.909418f,2.915839f,3.198942f,3.208551f,3.565721f,3.574121f,3.584794f,3.595176f,4.090815f,4.099084f,4.108564f,4.884596f,4.886070f,4.888009f,4.888111f,4.884447f,6.323071f,6.307468f,6.285368f,6.258748f,6.230694f,6.195814f,5.837747f,10.067566f,9.370760f,8.710139f,8.128731f,7.626956f,7.191919f,6.811775f,6.468780f,6.176325f,5.900831f,5.542097f,5.172136f,4.831508f,4.520650f,4.243360f,3.995601f,3.753256f,3.500318f, };
	#include "PX_Piano_mod.h"
	PX_memset(pPiano, 0, sizeof(PX_Piano));
	for (i=0;i<PX_COUNTOF(pPiano->keys);i++)
	{
		PX_PianoKey_Parameters *param= (PX_PianoKey_Parameters*)piano_mod_default;
		if(!PX_PianoKeyInitialize(mp, &pPiano->keys[i], &param[i]))return PX_FALSE;
	}
	do 
	{
		PX_PianoSoundboard_Parameters* param = (PX_PianoSoundboard_Parameters*)(piano_mod_default+sizeof(PX_PianoKey_Parameters)*88);
		if (!PX_PianoSoundBoardInitialize(mp, &pPiano->soundboard, param))return PX_FALSE;
	} while (0);
	
	return PX_TRUE;
}

px_bool PX_PianoInitializeEx(px_memorypool* mp, PX_Piano* pPiano, PX_PianoKey_Parameters keyparam[88], PX_PianoSoundboard_Parameters *soundboardparam)
{
	px_int i;
	PX_memset(pPiano, 0, sizeof(PX_Piano));
	for (i = 0; i < PX_COUNTOF(pPiano->keys); i++)
	{
		if (!PX_PianoKeyInitialize(mp, &pPiano->keys[i], &keyparam[i]))return PX_FALSE;
	}
	do
	{
		if (!PX_PianoSoundBoardInitialize(mp, &pPiano->soundboard, soundboardparam))return PX_FALSE;
	} while (0);
	return PX_TRUE;
}

px_int PX_PianoKeyNameToIndex(const px_char keyName[])
{
	const px_char* incstep[] = { "A","#A","bB","B","C","#C","bD","D","#D","bE","E","F","#F","bG","G","#G","bA" };
	const px_char map[] = { 0,1,1,2,3,4,4,5,6,6,7,8,9,9,10,11,11 };
	px_int column = 0, row = 0, i, oft, index = 0;
	for (i = 0; i < 17; i++)
		if (PX_memequ(incstep[i], keyName, oft = PX_strlen(incstep[i])))
			break;
	if (i == 17)return -1; else row = map[i];
	column = keyName[oft] - '1';
	if (i <= 1) column++;
	if (column >= 0 && column < 8 && row + column * 12 < 88)	index = row + column * 12; else return -1;
	return index;
}

px_void PX_PianoIndexToKey(px_int index,px_char keyName[])
{
	const px_char* incstep[] = { "A","#A","B","C","#C","D","#D","E","F","#F","G","#G"};
	keyName[0] = '\0';
	PX_strcat(keyName, incstep[index % 12]);
	PX_itoa(index / 12, keyName + PX_strlen(incstep[index % 12]), 2, 10);
}

px_void PX_PianoTriggerKey(PX_Piano* pPiano, const px_char keyName[], px_float v)
{
	px_int index = PX_PianoKeyNameToIndex(keyName);
	if(index!=-1)
	PX_PianoKeyTrigger(&pPiano->keys[index], v);
}

px_void PX_PianoTriggerIndex(PX_Piano* pPiano, const px_int index, px_float v)
{
	if (index>=0&&index<88)
	{
		PX_PianoKeyTrigger(&pPiano->keys[index], v);
	}
}

px_void PX_PianoGo(PX_Piano* pPiano, px_float* out, px_int samples)
{
	px_int i,j;
	px_float outValue;
	px_float keyOut;
	for (i=0;i<samples;i++)
	{
		outValue = 0;
		for (j=0;j<PX_COUNTOF(pPiano->keys);j++)
		{
			if (pPiano->keys[j].nSampleCount>=0&&pPiano->keys[j].nSampleCount<44100*3)
			{
				PX_PianoKeyGo(&pPiano->keys[j], &keyOut, 1);
				outValue += keyOut;
			}
		}
		outValue /= PX_COUNTOF(pPiano->keys);
		PX_PianoSoundBoardGo(&pPiano->soundboard, &outValue, &out[i], 1);
	}

}

px_void PX_PianoFree(PX_Piano* pPiano)
{
	px_int i;
	for (i = 0; i < PX_COUNTOF(pPiano->keys); i++)
	{
		PX_PianoKeyFree(&pPiano->keys[i]);
	}
	PX_PianoSoundBoardFree(&pPiano->soundboard);
}

px_void PX_PianoModelInitializeData(PX_PianoModel* pReverb)
{
	px_int i;
	PX_memset(pReverb, 0, sizeof(PX_PianoModel));

	pReverb->lock = PX_FALSE;

	for (i = 0; i < PX_COUNTOF(pReverb->pulsation); i++)
	{
		pReverb->pulsation[i].cursor = -1;
		pReverb->pulsation[i].note = -1;
	}
}

px_bool PX_PianoModelInitializeNote(PX_PianoModel* pReverb, px_int i, PX_PianoKey_Parameters *keyparam, PX_PianoSoundboard_Parameters* soundboardparam)
{
	px_int j;
	px_float keyout;
	px_char calcBuffer[64 * 1024] = { 0 };
	px_memorypool mp = MP_Create(calcBuffer, sizeof(calcBuffer));
	PX_PianoSoundBoard soundboard;
	PX_PianoKey key;

#include "PX_Piano_mod.h"

	if (keyparam == PX_NULL)
	{
		keyparam = (PX_PianoKey_Parameters*)piano_mod_default;
	}

	if (soundboardparam == PX_NULL)
	{
		soundboardparam = (PX_PianoSoundboard_Parameters*)(piano_mod_default + sizeof(PX_PianoKey_Parameters) * 88);
	}

	if (!PX_PianoSoundBoardInitialize(&mp, &soundboard, soundboardparam))
		return PX_FALSE;

	if (!PX_PianoKeyInitialize(&mp, &key, &keyparam[i]))
	{
		PX_PianoSoundBoardFree(&soundboard);
		return PX_FALSE;
	}
	PX_PianoKeyTrigger(&key, 1);
	for (j = 0; j < PX_COUNTOF(pReverb->note[i].pcm); j++)
	{
		PX_PianoKeyGo(&key, &keyout, 1);
		PX_PianoSoundBoardGo(&soundboard, &keyout, &pReverb->note[i].pcm[j], 1);
	}
	PX_PianoSoundBoardFree(&soundboard);
	PX_PianoKeyFree(&key);
	return PX_TRUE;
}
px_bool PX_PianoModelInitialize(PX_PianoModel* pReverb, PX_PianoKey_Parameters keyparam[88], PX_PianoSoundboard_Parameters *soundboardparam)
{
	px_int i;
	PX_PianoModelInitializeData(pReverb);
	for (i = 0; i < PX_COUNTOF(pReverb->note); i++)
	{
		PX_PianoModelInitializeNote(pReverb, i, keyparam , soundboardparam);
	}
	return PX_TRUE;
}

px_void PX_PianoModelGo(PX_PianoModel* pReverb, px_float out[], px_int count)
{
	px_int i,j;
	while (pReverb->lock);
	pReverb->lock = PX_TRUE;
	for (j=0;j<count;j++)
	{
		px_float reverb = 0;
		for (i = 0; i < PX_COUNTOF(pReverb->pulsation); i++)
		{
			if (pReverb->pulsation[i].note>=0&&pReverb->pulsation[i].cursor >= 0 && pReverb->pulsation[i].cursor < PX_COUNTOF(pReverb->note[i].pcm))
			{
				reverb += pReverb->note[pReverb->pulsation[i].note].pcm[pReverb->pulsation[i].cursor];
				pReverb->pulsation[i].cursor++;
			}
			else
			{
				pReverb->pulsation[i].note = -1;
				pReverb->pulsation[i].cursor = -1;
			}
		}
		out[j] = reverb / 88;
	}
	pReverb->lock = PX_FALSE;
	
}

px_void PX_PianoModelTrigger(PX_PianoModel* pReverb, px_int index)
{
	px_int i;
	if (index<0||index>=88)
	{
		return;
	}
	for (i=0;i<PX_COUNTOF(pReverb->pulsation);i++)
	{
		if (pReverb->pulsation[i].note==-1|| pReverb->pulsation[i].cursor==-1)
		{
			while (pReverb->lock);
			pReverb->lock = PX_TRUE;
			pReverb->pulsation[i].note = index;
			pReverb->pulsation[i].cursor = 0;
			pReverb->lock = PX_FALSE;
			return;
		}
	}
}

px_void PX_PianoModelFree(PX_PianoModel* pReverb){}

