#include "PX_Typedef.h"

static px_bool PX_isBigEndianCPU()
{
	union{
		px_dword i;
		px_uchar s[4];
	}c;
	c.i = 0x12345678;
	return (0x12 == c.s[0]);

}

px_uint PX_htoi(const px_char hex_str[])   
{  
	px_char ch;   
	px_uint iret=0;  
	while((ch=*hex_str++)!=0)
	{   
		iret=(iret<<4)|px_hex_to_dex_table[ch];  
	}   
	return iret;   
} 


px_int PX_atoi(const px_char s[])
{
	px_int i,n,sign=1;
	for(i=0;s[i]==' ';i++);
	if(s[i]=='+'||s[i]=='-')
	{
		sign=(s[i]=='-')?-1:1;
		i++;
	}
	for(n=0;s[i]>='0'&&s[i]<='9';i++)
		n=10*n+(s[i]-'0');
	return sign *n;
}


px_float PX_atof(const px_char fstr[])
{
	px_double temp=10;
	px_bool ispnum=PX_TRUE;
	px_double ans=0;
	px_bool be=PX_FALSE;
	if(*fstr=='-')
	{
		ispnum=PX_FALSE;
		fstr++;
	}
	else if(*fstr=='+')
	{
		fstr++;
	}

	while(*fstr!='\0')
	{
		if(*fstr=='.'){ fstr++;break;}
		if(*fstr=='e'||*fstr=='E'){ fstr++;be=PX_TRUE;break;}
		ans=ans*10+(*fstr-'0');
		fstr++;
	}
	if (be)
	{
		px_int e=PX_atoi(fstr);
		px_float e10=1;
		if (e>0)
		{
			while (e)
			{
				e10*=10;
				e--;
			}
		}
		else
		{
			while(e)
			{
				e10/=10;
				e++;
			}
		}
		ans*=e10;
	}
	else
	{
		while(*fstr!='\0')
		{
			ans=ans+(*fstr-'0')/temp;
			temp*=10;
			fstr++;
		}
	}
	
	if(ispnum) return (px_float)ans;
	else return (px_float)ans*(-1);
}


PX_RETURN_STRING PX_ftos(px_float f, px_int precision)
{
	PX_RETURN_STRING str;
	PX_ftoa(f,str.data,sizeof(str.data),precision);
	return str;
}


PX_RETURN_STRING PX_itos(px_int num,px_int radix)
{
	PX_RETURN_STRING str;
	PX_itoa(num,str.data,sizeof(str.data),radix);
	return str;
}


px_void PX_AscToWord(const px_char *asc,px_word *u16)
{
	while(*asc)
	{
		*u16=*asc;
		u16++;
		asc++;

	}
	*u16=0;
}

px_int  PX_ftoa(px_float f, px_char *outbuf, px_int maxlen, px_int precision)   
{  
	px_int i_value;
	px_int f_value;
	px_int shl=10;
	px_int len;
	px_int zero_oft=0;
	if (maxlen==0)
	{
		return 0;
	}
	shl=PX_pow_ii(shl,precision);
	i_value=(px_int)f;
	f_value=(px_int)PX_ABS(shl*(f-(px_int)f));
	if (f_value<0)
	{
		outbuf[0] = 'N';
		outbuf[1] = 'a';
		outbuf[2] = 'N';
		outbuf[3] = '\0';
		return 0;
	}
	if (i_value==0&&f<0)
	{
		outbuf[0]='-';
		PX_itoa(i_value,outbuf+1,maxlen-1,10);
	}
	else
	{
		PX_itoa(i_value,outbuf,maxlen,10);
	}
	
	len=PX_strlen(outbuf);

	if (precision==0)
	{
		return len; 
	}

	if (len>maxlen-3)
	{
		return len; 
	}
	outbuf[len]='.';
	outbuf[len+1]='\0';
	
	while (f_value<shl)
	{
		*(outbuf+len+1+zero_oft)='0';
		shl/=10;
		zero_oft++;
	}

	PX_itoa(f_value,outbuf+len+zero_oft,maxlen-len-1,10);
	*(outbuf+len+1+precision)='\0';
	len+=PX_strlen(outbuf+len);
	return len;
}  



px_int PX_itoa(px_int num,px_char *str,px_int MaxStrSize,px_int radix) 
{  
	px_char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	unsigned unum; 
	px_char temp; 
	px_int i=0,j,l; 
	if (MaxStrSize==0)
	{
		return 0;
	}

	if(radix==10&&num<0) 
	{ 
		unum=(unsigned)-num; 
		str[i++]='-'; 
	} 
	else unum=(unsigned)num; 

	do  
	{ 
		if (MaxStrSize<=i+1)
		{
			return 0;
		}
		str[i++]=index[unum%(unsigned)radix]; 
		unum/=radix; 
	}while(unum); 
	str[i]='\0'; 
	l=i;
	if(str[0]=='-')
	{
		for(j=1,i=i-1;j<i;j++,i--) 
		{ 
			temp=str[j]; 
			str[j]=str[i]; 
			str[i]=temp; 
		} 
	}
	else
	{
		for(j=0,i--;j<i;j++,i--) 
		{ 
			temp=str[j]; 
			str[j]=str[i]; 
			str[i]=temp; 
		} 
	}
	
	

	return l; 
} 



static px_int32 PX_i32SwapEndian(px_int32 val){
	val = ((val << 8)&0xFF00FF00) | ((val >> 8)&0x00FF00FF);
	return (val << 16)|(val >> 16);
}

px_dword PX_SwapEndian(px_dword val)
{
	val = ((val << 8)&0xFF00FF00) | ((val >> 8)&0x00FF00FF);
	return (val << 16)|(val >> 16);
}

static px_int64 PX_i64SwapEndian(px_int64 val)
{
	px_int32 u32_host_h, u32_host_l;
	px_int64 u64_net;
	u32_host_l = val & 0xffffffff;
	u32_host_h = (val >> 32) & 0xffffffff;

	u64_net = PX_i32SwapEndian(u32_host_l);
	u64_net = ( u64_net << 32 ) | PX_i32SwapEndian(u32_host_h);
	return u64_net;
}



px_float PX_sqrt( px_float number )  
{  
	px_int32 i;  
	px_float x2, y;  
	const px_float threehalfs = 1.5F;  
	x2 = number * 0.5F;  
	y  = number;  
	i  = * ( px_int32 * ) &y;

	if (PX_isBigEndianCPU())
	{
		i=PX_i32SwapEndian(i);
	}
	
	i  = 0x5f375a86 - ( i >> 1 );
	y  = * ( px_float * ) &i;  
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );     
	y  = y * ( threehalfs - ( x2 * y * y ) );
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );     
	y  = y * ( threehalfs - ( x2 * y * y ) );      
	return number*y;  
} 

//0x5fe6ec85e7de30da

px_double PX_sqrtd( px_double number )  
{  
	px_int64 i;  
	px_double x2, y;  
	const px_double threehalfs = 1.5;  
	x2 = number * 0.5;  
	y  = number;  
	i  = * ( px_int64 * ) &y;

	if (PX_isBigEndianCPU())
	{
		i=PX_i64SwapEndian(i);
	}

	i  = 0x5fe6ec85e7de30da - ( i / 2 ); 
	y  = * ( px_double * ) &i;  
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );     
	y  = y * ( threehalfs - ( x2 * y * y ) );
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );     
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );        
	return number*y;  
} 

px_float PX_SqrtRec( px_float number )  
{  
	px_int i;  
	px_float x2, y;  
	const px_float threehalfs = 1.5F;  

	x2 = number * 0.5F;  
	y  = number;  
	i  = * ( px_int * ) &y;   
	if (PX_isBigEndianCPU())
	{
		i=PX_i32SwapEndian(i);
	}
	i  = 0x5f375a86 - ( i >> 1 );   
	y  = * ( px_float * ) &i;  
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );     
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	return y;  
} 

px_double PX_fast_exp(px_double x) {
	x = 1.0 + x / (65536);
	x *= x; x *= x; x *= x; x *= x;
	x *= x; x *= x; x *= x; x *= x;
	x *= x; x *= x; x *= x; x *= x;
	x *= x; x *= x; x *= x; x *= x;
	return x;
}

/* PX_pow_dd & PX_scalbn & PX_log function(s)
 * ====================================================
 * Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
px_double64 PX_copysign(px_double64 x, px_double64 y)
{
	*(1 + (px_int32*)&x) = (*(1 + (px_int32*)&x) & 0x7fffffff) | (*(1 + (px_int32*)&y) & 0x80000000);
	return x;
}

px_double64 PX_scalbn(px_double64 x, px_double64 n)
{
	px_int32  k, hx, lx;
	static px_double64	two54 = 1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
		twom54 = 5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
		huge = 1.0e+300,
		tiny = 1.0e-300;

	hx = *(1 + (px_int32*)&x);
	lx = *(px_int32*)&x;
	k = (hx & 0x7ff00000) >> 20;		/* extract exponent */
	if (k == 0) {				/* 0 or subnormal x */
		if ((lx | (hx & 0x7fffffff)) == 0) return x; /* +-0 */
		x *= two54;
		hx = *(1 + (px_int32*)&x);
		k = ((hx & 0x7ff00000) >> 20) - 54;
		if (n < -50000) return tiny * x; 	/*underflow*/
	}
	if (k == 0x7ff) return x + x;		/* NaN or Inf */
	k = (px_int32)(k + n);
	if (k > 0x7fe) return huge * PX_copysign(huge, x); /* overflow  */
	if (k > 0) 				/* normal result */
	{
		*(1 + (px_int32*)&x) = (hx & 0x800fffff) | (k << 20); return x;
	}
	if (k <= -54)
	{
		if (n > 50000) 	/* in case integer overflow in n+k */
			return huge * PX_copysign(huge, x);	/*overflow*/
		else
			return tiny * PX_copysign(tiny, x); 	/*underflow*/
	}
	k += 54;				/* subnormal result */
	*(1 + (px_int32*)&x) = (hx & 0x800fffff) | (k << 20);
	return x * twom54;
}

px_double64 __px_pow_dd(px_double64 x, px_double64 y)
{
	px_double64 z, ax, z_h, z_l, p_h, p_l;
	px_double64 y1, t1, t2, r, s, t, u, v, w;
	px_int32 i0, i1, i, j, k, yisint, n;
	px_int32 hx, hy, ix, iy;
	unsigned lx, ly;

	static px_double64 bp[] = { 1.0, 1.5, },
		dp_h[] = { 0.0, 5.84962487220764160156e-01, }, /* 0x3FE2B803, 0x40000000 */
		dp_l[] = { 0.0, 1.35003920212974897128e-08, }, /* 0x3E4CFDEB, 0x43CFD006 */
		zero = 0.0,
		one = 1.0,
		two = 2.0,
		two53 = 9007199254740992.0,	/* 0x43400000, 0x00000000 */
		huge = 1.0e300,
		tiny = 1.0e-300,
		/* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
		L1 = 5.99999999999994648725e-01, /* 0x3FE33333, 0x33333303 */
		L2 = 4.28571428578550184252e-01, /* 0x3FDB6DB6, 0xDB6FABFF */
		L3 = 3.33333329818377432918e-01, /* 0x3FD55555, 0x518F264D */
		L4 = 2.72728123808534006489e-01, /* 0x3FD17460, 0xA91D4101 */
		L5 = 2.30660745775561754067e-01, /* 0x3FCD864A, 0x93C9DB65 */
		L6 = 2.06975017800338417784e-01, /* 0x3FCA7E28, 0x4A454EEF */
		P1 = 1.66666666666666019037e-01, /* 0x3FC55555, 0x5555553E */
		P2 = -2.77777777770155933842e-03, /* 0xBF66C16C, 0x16BEBD93 */
		P3 = 6.61375632143793436117e-05, /* 0x3F11566A, 0xAF25DE2C */
		P4 = -1.65339022054652515390e-06, /* 0xBEBBBD41, 0xC5D26BF1 */
		P5 = 4.13813679705723846039e-08, /* 0x3E663769, 0x72BEA4D0 */
		lg2 = 6.93147180559945286227e-01, /* 0x3FE62E42, 0xFEFA39EF */
		lg2_h = 6.93147182464599609375e-01, /* 0x3FE62E43, 0x00000000 */
		lg2_l = -1.90465429995776804525e-09, /* 0xBE205C61, 0x0CA86C39 */
		ovt = 8.0085662595372944372e-0017, /* -(1024-log2(ovfl+.5ulp)) */
		cp = 9.61796693925975554329e-01, /* 0x3FEEC709, 0xDC3A03FD =2/(3ln2) */
		cp_h = 9.61796700954437255859e-01, /* 0x3FEEC709, 0xE0000000 =(float)cp */
		cp_l = -7.02846165095275826516e-09, /* 0xBE3E2FE0, 0x145B01F5 =tail of cp_h*/
		ivln2 = 1.44269504088896338700e+00, /* 0x3FF71547, 0x652B82FE =1/ln2 */
		ivln2_h = 1.44269502162933349609e+00, /* 0x3FF71547, 0x60000000 =24b 1/ln2*/
		ivln2_l = 1.92596299112661746887e-08; /* 0x3E54AE0B, 0xF85DDF44 =1/ln2 tail*/


	i0 = ((*(px_int32*)&one) >> 29) ^ 1; i1 = 1 - i0;
	hx = *(1 + (px_int32*)&x); lx = *(px_int32*)&x;
	hy = *(1 + (px_int32*)&y); ly = *(px_int32*)&y;
	ix = hx & 0x7fffffff;  iy = hy & 0x7fffffff;

	/* y==zero: x**0 = 1 */
	if ((iy | ly) == 0) return one;

	/* +-NaN return x+y */
	if (ix > 0x7ff00000 || ((ix == 0x7ff00000) && (lx != 0)) ||
		iy > 0x7ff00000 || ((iy == 0x7ff00000) && (ly != 0)))
		return x + y;

	/* determine if y is an odd px_int32 when x < 0
	 * yisint = 0	... y is not an integer
	 * yisint = 1	... y is an odd px_int32
	 * yisint = 2	... y is an even px_int32
	 */
	yisint = 0;
	if (hx < 0) {
		if (iy >= 0x43400000) yisint = 2; /* even integer y */
		else if (iy >= 0x3ff00000) {
			k = (iy >> 20) - 0x3ff;	   /* exponent */
			if (k > 20) {
				j = ly >> (52 - k);
				if ((j << (52 - k)) == ly) yisint = 2 - (j & 1);
			}
			else if (ly == 0) {
				j = iy >> (20 - k);
				if ((j << (20 - k)) == iy) yisint = 2 - (j & 1);
			}
		}
	}

	/* special value of y */
	if (ly == 0) {
		if (iy == 0x7ff00000) {	/* y is +-inf */
			if (((ix - 0x3ff00000) | lx) == 0)
				return  y - y;	/* inf**+-1 is NaN */
			else if (ix >= 0x3ff00000)/* (|x|>1)**+-inf = inf,0 */
				return (hy >= 0) ? y : zero;
			else			/* (|x|<1)**-,+inf = inf,0 */
				return (hy < 0) ? -y : zero;
		}
		if (iy == 0x3ff00000) {	/* y is  +-1 */
			if (hy < 0) return one / x; else return x;
		}
		if (hy == 0x40000000) return x * x; /* y is  2 */
		if (hy == 0x3fe00000) {	/* y is  0.5 */
			if (hx >= 0)	/* x >= +0 */
				return PX_sqrtd(x);
		}
	}

	ax = PX_ABS(x);
	/* special value of x */
	if (lx == 0) {
		if (ix == 0x7ff00000 || ix == 0 || ix == 0x3ff00000) {
			z = ax;			/*x is +-0,+-inf,+-1*/
			if (hy < 0) z = one / z;	/* z = (1/|x|) */
			if (hx < 0) {
				if (((ix - 0x3ff00000) | yisint) == 0) {
					z = (z - z) / (z - z); /* (-1)**non-int is NaN */
				}
				else if (yisint == 1)
					z = -z;		/* (x<0)**odd = -(|x|**odd) */
			}
			return z;
		}
	}

	n = (hx >> 31) + 1;

	/* (x<0)**(non-int) is NaN */
	if ((n | yisint) == 0) return (x - x) / (x - x);

	s = one; /* s (sign of result -ve**odd) = -1 else = 1 */
	if ((n | (yisint - 1)) == 0) s = -one;/* (-ve)**(odd int) */

	/* |y| is huge */
	if (iy > 0x41e00000) { /* if |y| > 2**31 */
		if (iy > 0x43f00000) {	/* if |y| > 2**64, must o/uflow */
			if (ix <= 0x3fefffff) return (hy < 0) ? huge  : tiny ;
			if (ix >= 0x3ff00000) return (hy > 0) ? huge  : tiny ;
		}
		/* over/underflow if x is not close to one */
		if (ix < 0x3fefffff) return (hy < 0) ? s * huge * huge : s * tiny * tiny;
		if (ix > 0x3ff00000) return (hy > 0) ? s * huge * huge : s * tiny * tiny;
		/* now |1-x| is tiny <= 2**-20, suffice to compute
		   log(x) by x-x^2/2+x^3/3-x^4/4 */
		t = ax - one;		/* t has 20 trailing zeros */
		w = (t * t) * (0.5 - t * (0.3333333333333333333333 - t * 0.25));
		u = ivln2_h * t;	/* ivln2_h has 21 sig. bits */
		v = t * ivln2_l - w * ivln2;
		t1 = u + v;
		*(px_int32*)&t1 = 0;
		t2 = v - (t1 - u);
	}
	else {
		px_double64 ss, s2, s_h, s_l, t_h, t_l;
		n = 0;
		/* take care subnormal number */
		if (ix < 0x00100000)
		{
			ax *= two53; n -= 53; ix = *(1 + (px_int32*)&ax);
		}
		n += ((ix) >> 20) - 0x3ff;
		j = ix & 0x000fffff;
		/* determine interval */
		ix = j | 0x3ff00000;		/* normalize ix */
		if (j <= 0x3988E) k = 0;		/* |x|<sqrt(3/2) */
		else if (j < 0xBB67A) k = 1;	/* |x|<sqrt(3)   */
		else { k = 0; n += 1; ix -= 0x00100000; }
		*(1 + (px_int32*)&ax) = ix;

		/* compute ss = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
		u = ax - bp[k];		/* bp[0]=1.0, bp[1]=1.5 */
		v = one / (ax + bp[k]);
		ss = u * v;
		s_h = ss;
		*(px_int32*)&s_h = 0;
		/* t_h=ax+bp[k] High */
		t_h = zero;
		*(1 + (px_int32*)&t_h) = ((ix >> 1) | 0x20000000) + 0x00080000 + (k << 18);
		t_l = ax - (t_h - bp[k]);
		s_l = v * ((u - s_h * t_h) - s_h * t_l);
		/* compute log(ax) */
		s2 = ss * ss;
		r = s2 * s2 * (L1 + s2 * (L2 + s2 * (L3 + s2 * (L4 + s2 * (L5 + s2 * L6)))));
		r += s_l * (s_h + ss);
		s2 = s_h * s_h;
		t_h = 3.0 + s2 + r;
		*(px_int32*)&t_h = 0;
		t_l = r - ((t_h - 3.0) - s2);
		/* u+v = ss*(1+...) */
		u = s_h * t_h;
		v = s_l * t_h + t_l * ss;
		/* 2/(3log2)*(ss+...) */
		p_h = u + v;
		*(px_int32*)&p_h = 0;
		p_l = v - (p_h - u);
		z_h = cp_h * p_h;		/* cp_h+cp_l = 2/(3*log2) */
		z_l = cp_l * p_h + p_l * cp + dp_l[k];
		/* log2(ax) = (ss+..)*2/(3*log2) = n + dp_h + z_h + z_l */
		t = (px_double64)n;
		t1 = (((z_h + z_l) + dp_h[k]) + t);
		*(px_int32*)&t1 = 0;
		t2 = z_l - (((t1 - t) - dp_h[k]) - z_h);
	}

	/* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
	y1 = y;
	*(px_int32*)&y1 = 0;
	p_l = (y - y1) * t1 + y * t2;
	p_h = y1 * t1;
	z = p_l + p_h;
	j = *(1 + (px_int32*)&z);
	i = *(px_int32*)&z;
	if (j >= 0x40900000) {				/* z >= 1024 */
		if (((j - 0x40900000) | i) != 0)			/* if z > 1024 */
			return s * huge * huge;			/* overflow */
		else {
			if (p_l + ovt > z - p_h) return s * huge * huge;	/* overflow */
		}
	}
	else if ((j & 0x7fffffff) >= 0x4090cc00) {	/* z <= -1075 */
		if (((j - 0xc090cc00) | i) != 0) 		/* z < -1075 */
			return s * tiny * tiny;		/* underflow */
		else {
			if (p_l <= z - p_h) return s * tiny * tiny;	/* underflow */
		}
	}
	/*
	 * compute 2**(p_h+p_l)
	 */
	i = j & 0x7fffffff;
	k = (i >> 20) - 0x3ff;
	n = 0;
	if (i > 0x3fe00000) {		/* if |z| > 0.5, set n = [z+0.5] */
		n = j + (0x00100000 >> (k + 1));
		k = ((n & 0x7fffffff) >> 20) - 0x3ff;	/* new k for n */
		t = zero;
		*(1 + (px_int32*)&t) = (n & ~(0x000fffff >> k));
		n = ((n & 0x000fffff) | 0x00100000) >> (20 - k);
		if (j < 0) n = -n;
		p_h -= t;
	}
	t = p_l + p_h;
	*(px_int32*)&t = 0;
	u = t * lg2_h;
	v = (p_l - (t - p_h)) * lg2 + t * lg2_l;
	z = u + v;
	w = v - (z - u);
	t = z * z;
	t1 = z - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
	r = (z * t1) / (t1 - two) - (w + z * w);
	z = one - (r - z);
	j = *(1 + (px_int32*)&z);
	j += (n << 20);
	if ((j >> 20) <= 0) z = PX_scalbn(z, n);	/* subnormal output */
	else *(1 + (px_int32*)&z) += (n << 20);
	return s * z;
}

px_double PX_exp(px_double x)
{
	return __px_pow_dd(PX_e, x);
}

px_double PX_tanh(px_double x)
{
	px_double ex,eix;
	ex=PX_exp(x);
	eix=1/ex;
	return (ex-eix)/(ex+eix);
}


px_double PX_sigmoid(px_double x)
{
	px_double eix;
	eix=PX_exp(-x);
	return 1/(1+eix);
}

px_double PX_ReLU(px_double x)
{
	return x<=0?0:x;
}

px_double PX_sind(px_double x)
{
	px_double it;
	px_double term;
	px_double result;

	it=x/(2*PX_PI);
	x=x-(2*PX_PI)*PX_TRUNC(it);

	term=x;
	result=0;

	result+=term;
	term*=(-x*x)/(2*3);
	result+=term;
	term*=(-x*x)/(4*5);
	result+=term;
	term*=(-x*x)/(6*7);
	result+=term;
	term*=(-x*x)/(8*9);
	result+=term;
	term*=(-x*x)/(10*11);
	result+=term;
	term*=(-x*x)/(12*13);
	result+=term;
	term*=(-x*x)/(14*15);
	result+=term;
	term*=(-x*x)/(16*17);
	result+=term;
	term*=(-x*x)/(18*19);
	result+=term;
	term*=(-x*x)/(20*21);
	result+=term;
	term*=(-x*x)/(22*23);
	result+=term;
	term*=(-x*x)/(24*25);
	result+=term;
	term*=(-x*x)/(26*27);
	result+=term;
	term*=(-x*x)/(28*29);
	result+=term;
	term*=(-x*x)/(30*31);
	result+=term;
	term*=(-x*x)/(32*33);
	return result;

}

px_double PX_sinc(px_double i)
{
	return i?PX_sind(i * PX_PI) / (i * PX_PI):1;
}

px_double PX_sinc_interpolate(px_double x[], px_int size, px_double d)
{
	px_int i;
	px_double sum = 0;
	for (i = 0; i < size; i++)
		sum += x[i] * PX_sinc(d - i);
	return sum;
}

px_double PX_cosd(px_double radian)
{
	return PX_sind((PX_PI/2-radian));
}

px_float PX_sin_radian(px_float radian)
{
	return (px_float)PX_sind(radian);
}


px_float PX_cos_radian(px_float radian)
{
	return PX_sin_radian((px_float)(PX_PI/2-radian));
}

px_float PX_tan_radian(px_float radian)
{
	return PX_sin_radian(radian)/PX_cos_radian(radian);
}

px_float PX_sin_angle(px_float angle)
{
	angle-=((px_int)angle/360)*360;
	return (px_float)PX_sin_radian((angle*0.0174532925f));
}
px_float PX_cos_angle(px_float angle)
{
	angle-=((px_int)angle/360)*360;
	return PX_cos_radian((angle*0.0174532925f));
}


px_float PX_tan_angle(px_float angle)
{
	return PX_sin_angle(angle)/PX_cos_angle(angle);
}

px_float PX_Point_sin(px_point v)
{
	return v.y/PX_sqrt(v.x*v.x+v.y*v.y);
}


px_float PX_Point_cos(px_point v)
{
	return v.x/PX_sqrt(v.x*v.x+v.y*v.y);
}

px_void PX_BufferToHexString(px_byte data[],px_int size,px_char hex_str[])
{
	px_int i;
	hex_str[0]=0;
	for (i=0;i<size;i++)
	{
		if (data[i]==0)
		{
			PX_strcat(hex_str+i*2,"00");
		}
		else if (data[i]<0x10)
		{
			*(hex_str+i*2)='0';
			PX_itoa(data[i],hex_str+i*2+1,3,16);
		}
		else
		PX_itoa(data[i],hex_str+i*2,3,16);
	}
	hex_str[i*2]=0;
}

px_int PX_HexStringToBuffer(const px_char hex_str[],px_byte data[])
{
	px_int i=0;
	px_char hex[3]={0};
	if (PX_strlen(hex_str)&1)
	{
		return PX_FALSE;
	}
	while (*hex_str)
	{
		if (*hex_str==' ')
		{
			hex_str++;
			continue;
		}

		hex[0]=*hex_str;
		hex_str++;
		hex[1]=*hex_str;
		hex_str++;

		data[i]=PX_htoi(hex);
		i++;
	}
	return i;
}

static volatile const px_double Tiny =(px_double)2.225073858507201383e-308 ;

static px_double Tail(px_double x)
{
	static const px_double HalfPi = PX_PI/2;
	static const px_double p03 = (px_double)(-0.3333333333331767734);
	static const px_double p05 = (px_double)(0.1999999999714804158);
	static const px_double p07 = (px_double)(-0.1428571408980792989);
	static const px_double p09 = (px_double)(0.1111110405917124038);
	static const px_double p11 = (px_double)(-0.09090757063599989862);
	static const px_double p13 = (px_double)(0.07690189054748748643);
	static const px_double p15 = (px_double)(-0.06646850218576683123);
	static const px_double p17 = (px_double)(0.0575572605769638368);
	static const px_double p19 = (px_double)(-0.04709779890322960544);
	static const px_double p21 = (px_double)(0.03130684024963894391);
	static const px_double p23 = (px_double)(-0.01183640847691868649);

	static const px_double p000 = (px_double)(1.570796326794896558);
	static const px_double p001 = (px_double)(6.123233995736766036e-17);
	px_double y ;
	// Square y.
	px_double y2 ;

	if ((9007199254740992.0) <= x)
		return HalfPi + Tiny;

	 y = 1 / x;
	 y2 = y * y;

	return p001 - ((((((((((((
		+p23) * y2
		+ p21) * y2
		+ p19) * y2
		+ p17) * y2
		+ p15) * y2
		+ p13) * y2
		+ p11) * y2
		+ p09) * y2
		+ p07) * y2
		+ p05) * y2
		+ p03) * y2 * y + y) + p000;
}

static px_double atani0(px_double x)
{
	static const px_double p03 = (px_double)(-0.3333333333333300952);
	static const px_double p05 = (px_double)(0.1999999999990851318);
	static const px_double p07 = (px_double)(-0.1428571427670465022);
	static const px_double p09 = (px_double)(0.11111111066822130927);
	static const px_double p11 = (px_double)(-0.09090896357643575565);
	static const px_double p13 = (px_double)(0.07692074439069175595);
	static const px_double p15 = (px_double)(-0.06663806168831017118);
	static const px_double p17 = (px_double)(0.058582174411949621);
	static const px_double p19 = (px_double)(-0.05121467553745503998);
	static const px_double p21 = (px_double)(0.0418461372066637749);
	static const px_double p23 = (px_double)(-0.02739990312425184121);
	static const px_double p25 = (px_double)(0.01008979802853119875);

	// Square x.
	px_double x2 = x * x;

	return ((((((((((((
		+p25) * x2
		+ p23) * x2
		+ p21) * x2
		+ p19) * x2
		+ p17) * x2
		+ p15) * x2
		+ p13) * x2
		+ p11) * x2
		+ p09) * x2
		+ p07) * x2
		+ p05) * x2
		+ p03) * x2 * x + x;
}

static px_double atani1(px_double x)
{
	static const px_double p00 = (px_double)(0.5585993153435655501);
	static const px_double p01 = (px_double)(0.7191011235955026004);
	static const px_double p02 = (px_double)(-0.3231915162226940974);
	static const px_double p03 = (px_double)(0.02130401005855760144);
	static const px_double p04 = (px_double)(0.1018414372498045034);
	static const px_double p05 = (px_double)(-0.08242613472076004699);
	static const px_double p06 = (px_double)(0.01291956298376459658);
	static const px_double p07 = (px_double)(0.03238363329851735167);
	static const px_double p08 = (px_double)(-0.03243829816327323257);
	static const px_double p09 = (px_double)(0.00780078683156177656);
	static const px_double p10 = (px_double)(0.01236133736686602196);
	static const px_double p11 = (px_double)(-0.01442112851466732272);
	static const px_double p12 = (px_double)(0.004164927187425533986);

	px_double y = x - (px_double)(0.6250000000000043299);

	return ((((((((((((
		+p12) * y
		+ p11) * y
		+ p10) * y
		+ p09) * y
		+ p08) * y
		+ p07) * y
		+ p06) * y
		+ p05) * y
		+ p04) * y
		+ p03) * y
		+ p02) * y
		+ p01) * y
		+ p00;
}

static px_double atani2(px_double x)
{
	static const px_double p00 = (px_double)(0.7188299996845015638);
	static const px_double p01 = (px_double)(0.5663716813536087136);
	static const px_double p02 = (px_double)(-0.2806797712949650192);
	static const px_double p03 = (px_double)(0.07853829253223243434);
	static const px_double p04 = (px_double)(0.02110208940610767275);
	static const px_double p05 = (px_double)(-0.04342139500642782507);
	static const px_double p06 = (px_double)(0.0278965408671968812);
	static const px_double p07 = (px_double)(-0.006133549533512984951);
	static const px_double p08 = (px_double)(-0.006532089191404124792);
	static const px_double p09 = (px_double)(0.008457549704878185304);
	static const px_double p10 = (px_double)(-0.004491743889132696586);
	static const px_double p11 = (px_double)(0.0001093120021936477832);

	px_double y = x - (px_double)(0.8750000001110173065);

	return (((((((((((
		+p11) * y
		+ p10) * y
		+ p09) * y
		+ p08) * y
		+ p07) * y
		+ p06) * y
		+ p05) * y
		+ p04) * y
		+ p03) * y
		+ p02) * y
		+ p01) * y
		+ p00;
}

static px_double atani3(px_double x)
{
	static const px_double p00 = (px_double)(0.8621700546672244059);
	static const px_double p01 = (px_double)(0.4235294117647077639);
	static const px_double p02 = (px_double)(-0.2092733564013857794);
	static const px_double p03 = (px_double)(0.07808182373302373358);
	static const px_double p04 = (px_double)(-0.0135556997637514675);
	static const px_double p05 = (px_double)(-0.009124992591382986157);
	static const px_double p06 = (px_double)(0.01134219138389198391);
	static const px_double p07 = (px_double)(-0.00684699873146459112);
	static const px_double p08 = (px_double)(0.002317836143188779178);
	static const px_double p09 = (px_double)(0.0002192642833474152939);
	static const px_double p10 = (px_double)(-0.0009808674228592557075);
	static const px_double p11 = (px_double)(0.0008098046818543229095);
	static const px_double p12 = (px_double)(-0.0003740695734151908039);

	px_double y = x - (px_double)(1.16666666666662078);

	return ((((((((((((
		+p12) * y
		+ p11) * y
		+ p10) * y
		+ p09) * y
		+ p08) * y
		+ p07) * y
		+ p06) * y
		+ p05) * y
		+ p04) * y
		+ p03) * y
		+ p02) * y
		+ p01) * y
		+ p00;
}

static px_double atani4(px_double x)
{
	static const px_double p00 = (px_double)(0.9827937232473292761);
	static const px_double p01 = (px_double)(0.3076923076923072653);
	static const px_double p02 = (px_double)(-0.142011834319542396);
	static const px_double p03 = (px_double)(0.05583371263871451939);
	static const px_double p04 = (px_double)(-0.01680613422985677979);
	static const px_double p05 = (px_double)(0.002102921348931583931);
	static const px_double p06 = (px_double)(0.0018297786863776509);
	static const px_double p07 = (px_double)(-0.001909912753511534573);
	static const px_double p08 = (px_double)(0.001120490304029380041);
	static const px_double p09 = (px_double)(-0.0004624685412818255181);
	static const px_double p10 = (px_double)(0.0001041619583992129452);
	static const px_double p11 = (px_double)(3.081575322765031172e-05);

	px_double y = x - 1.50000000000000666;

	return (((((((((((
		+p11) * y
		+ p10) * y
		+ p09) * y
		+ p08) * y
		+ p07) * y
		+ p06) * y
		+ p05) * y
		+ p04) * y
		+ p03) * y
		+ p02) * y
		+ p01) * y
		+ p00;
}

static px_double atani5(px_double x)
{
	static const px_double p00 = (px_double)(1.071449605063853356);
	static const px_double p01 = (px_double)(0.2292993631001272459);
	static const px_double p02 = (px_double)(-0.09639336283821990647);
	static const px_double p03 = (px_double)(0.03650333485983356385);
	static const px_double p04 = (px_double)(-0.01196655808839080896);
	static const px_double p05 = (px_double)(0.003026728804287181791);
	static const px_double p06 = (px_double)(-0.0002913554434445434329);
	static const px_double p07 = (px_double)(-0.0002857253601689876445);
	static const px_double p08 = (px_double)(0.0002603252202535161078);
	static const px_double p09 = (px_double)(-0.0001450883505235225923);
	static const px_double p10 = (px_double)(6.120525496089561119e-05);

	px_double y = x - (px_double)(1.833333333111296204);

	return ((((((((((
		+p10) * y
		+ p09) * y
		+ p08) * y
		+ p07) * y
		+ p06) * y
		+ p05) * y
		+ p04) * y
		+ p03) * y
		+ p02) * y
		+ p01) * y
		+ p00;
}

// See documentation above.
px_double PX_atan(px_double x)
{
	if (x < 0)
		if (x < -1)
			if (x < -5 / 3.)
				if (x < -2)
					return -Tail(-x);
				else
					return -atani5(-x);
			else
				if (x < -4 / 3.)
					return -atani4(-x);
				else
					return -atani3(-x);
		else
			if (x < -.5)
				if (x < -.75)
					return -atani2(-x);
				else
					return -atani1(-x);
			else
				if (x < (px_double)(1.353860343122586362e-8))
					return atani0(x);
				else
					if (x <= (px_double)(2.225073858507201383e-308))
						// Generate inexact and return x.
						return (Tiny + 1) * x;
					else
						if (x == 0)
							return x;
						else
							// Generate underflow and return x.
							return x * Tiny + x;
	else
		if (x <= +1)
			if (x <= +.5)
				if (x <= (px_double)(1.353860343122586362e-8))
					if (x < (px_double)(2.225073858507201383e-308))
						if (x == 0)
							return x;
						else
							// Generate underflow and return x.
							return x * Tiny + x;
					else
						// Generate inexact and return x.
						return (Tiny + 1) * x;
				else
					return atani0(x);
			else
				if (x <= +.75)
					return +atani1(+x);
				else
					return +atani2(+x);
		else
			if (x <= +5 / 3.)
				if (x <= +4 / 3.)
					return +atani3(+x);
				else
					return +atani4(+x);
			else
				if (x <= +2)
					return +atani5(+x);
				else
					return +Tail(+x);
}

px_double PX_atan2(px_double y, px_double x)
{
	if (x>0)
	{
		return PX_atan(y/x);;
	}
	else if (y >= 0 && x < 0)
	{
		return PX_atan(y/x) + PX_PI;
	}
	else if (y < 0 && x < 0)
	{
		return PX_atan(y/x) - PX_PI;
	}
	else if (y > 0 && x == 0)
	{
		return PX_PI / 2;
	}
	else if (y < 0 && x == 0)
	{
		return -1 * PX_PI / 2;
	}
	else
	{
		return 0;
	}

}

px_double PX_asin(px_double x)
{
	return PX_atan2 (x, PX_sqrtd (1.0 - x * x));
}

px_double PX_acos(px_double x)
{
	return PX_atan2 (PX_sqrtd (1.0 - x * x), x);
}

px_stringformat PX_STRINGFORMAT_INT(px_int _i)
{
	px_stringformat fmt;
	fmt.type=PX_STRINGFORMAT_TYPE_INT;
	fmt._int=_i;
	return fmt;
}


px_stringformat PX_STRINGFORMAT_FLOAT(px_float _f)
{
	px_stringformat fmt;
	fmt.type=PX_STRINGFORMAT_TYPE_FLOAT;
	fmt._float=_f;
	return fmt;
}


px_stringformat PX_STRINGFORMAT_STRING(const px_char *_s)
{
	px_stringformat fmt;
	fmt.type=PX_STRINGFORMAT_TYPE_STRING;
	fmt._pstring=_s;
	return fmt;
}


px_int PX_sprintf8(px_char *_out_str,px_int str_size,const px_char fmt[], px_stringformat _1, px_stringformat _2, px_stringformat _3, px_stringformat _4,px_stringformat _5, px_stringformat _6, px_stringformat _7, px_stringformat _8)
{
	px_int length=0;
	const px_char *p=PX_NULL;
	PX_RETURN_STRING tret;
	px_stringformat pstringfmt;
	px_int precision=3;

	if (!_out_str||!str_size)
	{
		for (p = fmt;*p; p++) 
		{
			if(*p != '%') {
				length++;
				continue;
			}
			switch(*(p+1)) 
			{
			case '1': pstringfmt=_1; break;
			case '2': pstringfmt=_2; break;
			case '3': pstringfmt=_3; break;
			case '4': pstringfmt=_4; break;
			case '5': pstringfmt=_5; break;
			case '6': pstringfmt=_6; break;
			case '7': pstringfmt=_7; break;
			case '8': pstringfmt=_8; break;
			default:
				length++;
				continue;
			}

			if (*(p+2)=='.'&&PX_charIsNumeric(*(p+3)))
			{
				precision=*(p+3)-'0';
				p+=3;
			}
			else
			{
				p++;
			}
			
			switch (pstringfmt.type)
			{
			case PX_STRINGFORMAT_TYPE_INT:
				length+=PX_strlen(PX_itos(pstringfmt._int,10).data);
				break;
			case PX_STRINGFORMAT_TYPE_FLOAT:
				length+=PX_strlen(PX_ftos(pstringfmt._float,precision).data);
				break;
			case PX_STRINGFORMAT_TYPE_STRING:
				length+=PX_strlen(pstringfmt._pstring);
				break;
			default:
				return 0;
			}
			
		}
		return length;
	}
	PX_memset(_out_str,0,str_size);
	for (p = fmt;*p; p++) 
	{
		if(*p != '%') {
			_out_str[length]=*p;
			length++;
			if (length >= str_size)
			{
				PX_ASSERT();
			}
			continue;
		}
		switch(*(p+1)) 
		{
		case '1': pstringfmt=_1; break;
		case '2': pstringfmt=_2; break;
		case '3': pstringfmt=_3; break;
		case '4': pstringfmt=_4; break;
		case '5': pstringfmt=_5; break;
		case '6': pstringfmt=_6; break;
		case '7': pstringfmt=_7; break;
		case '8': pstringfmt=_8; break;
		default:
			_out_str[length]=*p;
			length++;
			if (length >= str_size)
			{
				PX_ASSERT();
			}
			continue;
		}

		if (*(p+2)=='.'&&PX_charIsNumeric(*(p+3)))
		{
			precision=*(p+3)-'0';
			p+=3;
		}
		else
		{
			p++;
		}

		switch (pstringfmt.type)
		{
		case PX_STRINGFORMAT_TYPE_INT:
			tret=PX_itos(pstringfmt._int,10);
			if(length+PX_strlen(tret.data)<str_size)
			{
				PX_strcat(_out_str,tret.data);
				length+=PX_strlen(tret.data);
			}
			else
				return length;
			break;
		case PX_STRINGFORMAT_TYPE_FLOAT:
			tret=PX_ftos(pstringfmt._float,precision);
			if(length+PX_strlen(tret.data)<str_size)
			{
				PX_strcat(_out_str,tret.data);
				length+=PX_strlen(tret.data);
			}
			else
			{
				return length;
			}
			break;
		case PX_STRINGFORMAT_TYPE_STRING:
			if(length+PX_strlen(pstringfmt._pstring)<str_size)
			{
				PX_strcat(_out_str,pstringfmt._pstring);
				length+=PX_strlen(pstringfmt._pstring);
			}
			else
			{
				return length;
			}
			break;
		default:
			return 0;
		}
	}
	if (length >= str_size)
	{
		PX_ASSERT();
	}
	return length;
}


px_int PX_sprintf7(px_char *str,px_int str_size,const px_char fmt[],px_stringformat _1,px_stringformat _2,px_stringformat _3,px_stringformat _4,px_stringformat _5,px_stringformat _6,px_stringformat _7)
{
	return PX_sprintf8(str,str_size,fmt,_1,_2,_3,_4,_5,_6,_7,PX_STRINGFORMAT_INT(0));
}


px_int PX_sprintf6(px_char *str,px_int str_size,const px_char fmt[],px_stringformat _1,px_stringformat _2,px_stringformat _3,px_stringformat _4,px_stringformat _5,px_stringformat _6)
{
	return PX_sprintf8(str,str_size,fmt,_1,_2,_3,_4,_5,_6,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}


px_int PX_sprintf5(px_char *str,px_int str_size,const px_char fmt[],px_stringformat _1,px_stringformat _2,px_stringformat _3,px_stringformat _4,px_stringformat _5)
{
	return PX_sprintf8(str,str_size,fmt,_1,_2,_3,_4,_5,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}


px_int PX_sprintf4(px_char *str,px_int str_size,const px_char fmt[],px_stringformat _1,px_stringformat _2,px_stringformat _3,px_stringformat _4)
{
	return PX_sprintf8(str,str_size,fmt,_1,_2,_3,_4,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}


px_int PX_sprintf3(px_char *str,px_int str_size,const px_char fmt[],px_stringformat _1,px_stringformat _2,px_stringformat _3)
{
	return PX_sprintf8(str,str_size,fmt,_1,_2,_3,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}


px_int PX_sprintf2(px_char *str,px_int str_size,const px_char fmt[], px_stringformat _1,px_stringformat _2)
{
	return PX_sprintf8(str,str_size,fmt,_1,_2,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}


px_int PX_sprintf1(px_char *str,px_int str_size,const px_char fmt[], px_stringformat _1)
{
	return PX_sprintf8(str,str_size,fmt,_1,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}

px_int PX_sprintf0(px_char *str,px_int str_size,const px_char fmt[])
{
	return PX_sprintf8(str,str_size,fmt,PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0),PX_STRINGFORMAT_INT(0));
}

void PX_MatrixZero(px_matrix *Mat)
{
	Mat->m[0][0]=0;Mat->m[0][1]=0;Mat->m[0][2]=0;Mat->m[0][3]=0;
	Mat->m[1][0]=0;Mat->m[1][1]=0;Mat->m[1][2]=0;Mat->m[1][3]=0;
	Mat->m[2][0]=0;Mat->m[2][1]=0;Mat->m[2][2]=0;Mat->m[2][3]=0;
	Mat->m[3][0]=0;Mat->m[3][1]=0;Mat->m[3][2]=0;Mat->m[3][3]=0;
}

void PX_MatrixIdentity(px_matrix *Mat)
{
	Mat->_11=1.0f;	Mat->_12=0.0f;	Mat->_13=0.0f;	Mat->_14=0.0f;
	Mat->_21=0.0f;	Mat->_22=1.0f;	Mat->_23=0.0f;	Mat->_24=0.0f;
	Mat->_31=0.0f;	Mat->_32=0.0f;	Mat->_33=1.0f;	Mat->_34=0.0f;
	Mat->_41=0.0f;	Mat->_42=0.0f;	Mat->_43=0.0f;	Mat->_44=1.0f;
}

px_matrix PX_MatrixMultiply(px_matrix Mat1,px_matrix Mat2)
{
	px_matrix ptmat;
	ptmat.m[0][0]=Mat1.m[0][0]*Mat2.m[0][0]/**/+Mat1.m[0][1]*Mat2.m[1][0]/**/+Mat1.m[0][2]*Mat2.m[2][0]/**/+Mat1.m[0][3]*Mat2.m[3][0];
	ptmat.m[0][1]=Mat1.m[0][0]*Mat2.m[0][1]/**/+Mat1.m[0][1]*Mat2.m[1][1]/**/+Mat1.m[0][2]*Mat2.m[2][1]/**/+Mat1.m[0][3]*Mat2.m[3][1];
	ptmat.m[0][2]=Mat1.m[0][0]*Mat2.m[0][2]/**/+Mat1.m[0][1]*Mat2.m[1][2]/**/+Mat1.m[0][2]*Mat2.m[2][2]/**/+Mat1.m[0][3]*Mat2.m[3][2];
	ptmat.m[0][3]=Mat1.m[0][0]*Mat2.m[0][3]/**/+Mat1.m[0][1]*Mat2.m[1][3]/**/+Mat1.m[0][2]*Mat2.m[2][3]/**/+Mat1.m[0][3]*Mat2.m[3][3];

	ptmat.m[1][0]=Mat1.m[1][0]*Mat2.m[0][0]/**/+Mat1.m[1][1]*Mat2.m[1][0]/**/+Mat1.m[1][2]*Mat2.m[2][0]/**/+Mat1.m[1][3]*Mat2.m[3][0];
	ptmat.m[1][1]=Mat1.m[1][0]*Mat2.m[0][1]/**/+Mat1.m[1][1]*Mat2.m[1][1]/**/+Mat1.m[1][2]*Mat2.m[2][1]/**/+Mat1.m[1][3]*Mat2.m[3][1];
	ptmat.m[1][2]=Mat1.m[1][0]*Mat2.m[0][2]/**/+Mat1.m[1][1]*Mat2.m[1][2]/**/+Mat1.m[1][2]*Mat2.m[2][2]/**/+Mat1.m[1][3]*Mat2.m[3][2];
	ptmat.m[1][3]=Mat1.m[1][0]*Mat2.m[0][3]/**/+Mat1.m[1][1]*Mat2.m[1][3]/**/+Mat1.m[1][2]*Mat2.m[2][3]/**/+Mat1.m[1][3]*Mat2.m[3][3];

	ptmat.m[2][0]=Mat1.m[2][0]*Mat2.m[0][0]/**/+Mat1.m[2][1]*Mat2.m[1][0]/**/+Mat1.m[2][2]*Mat2.m[2][0]/**/+Mat1.m[2][3]*Mat2.m[3][0];
	ptmat.m[2][1]=Mat1.m[2][0]*Mat2.m[0][1]/**/+Mat1.m[2][1]*Mat2.m[1][1]/**/+Mat1.m[2][2]*Mat2.m[2][1]/**/+Mat1.m[2][3]*Mat2.m[3][1];
	ptmat.m[2][2]=Mat1.m[2][0]*Mat2.m[0][2]/**/+Mat1.m[2][1]*Mat2.m[1][2]/**/+Mat1.m[2][2]*Mat2.m[2][2]/**/+Mat1.m[2][3]*Mat2.m[3][2];
	ptmat.m[2][3]=Mat1.m[2][0]*Mat2.m[0][3]/**/+Mat1.m[2][1]*Mat2.m[1][3]/**/+Mat1.m[2][2]*Mat2.m[2][3]/**/+Mat1.m[2][3]*Mat2.m[3][3];

	ptmat.m[3][0]=Mat1.m[3][0]*Mat2.m[0][0]/**/+Mat1.m[3][1]*Mat2.m[1][0]/**/+Mat1.m[3][2]*Mat2.m[2][0]/**/+Mat1.m[3][3]*Mat2.m[3][0];
	ptmat.m[3][1]=Mat1.m[3][0]*Mat2.m[0][1]/**/+Mat1.m[3][1]*Mat2.m[1][1]/**/+Mat1.m[3][2]*Mat2.m[2][1]/**/+Mat1.m[3][3]*Mat2.m[3][1];
	ptmat.m[3][2]=Mat1.m[3][0]*Mat2.m[0][2]/**/+Mat1.m[3][1]*Mat2.m[1][2]/**/+Mat1.m[3][2]*Mat2.m[2][2]/**/+Mat1.m[3][3]*Mat2.m[3][2];
	ptmat.m[3][3]=Mat1.m[3][0]*Mat2.m[0][3]/**/+Mat1.m[3][1]*Mat2.m[1][3]/**/+Mat1.m[3][2]*Mat2.m[2][3]/**/+Mat1.m[3][3]*Mat2.m[3][3];
	return ptmat;
}

px_matrix PX_MatrixAdd(px_matrix Mat1,px_matrix Mat2)
{
	px_matrix ptmat;
	ptmat._11=Mat1._11+Mat2._11;ptmat._12=Mat1._12+Mat2._12;ptmat._13=Mat1._13+Mat2._13;ptmat._14=Mat1._14+Mat2._14;
	ptmat._21=Mat1._21+Mat2._21;ptmat._22=Mat1._22+Mat2._22;ptmat._23=Mat1._23+Mat2._23;ptmat._24=Mat1._24+Mat2._24;
	ptmat._31=Mat1._31+Mat2._31;ptmat._32=Mat1._32+Mat2._32;ptmat._33=Mat1._33+Mat2._33;ptmat._34=Mat1._34+Mat2._34;
	ptmat._41=Mat1._41+Mat2._41;ptmat._42=Mat1._42+Mat2._42;ptmat._43=Mat1._43+Mat2._43;ptmat._44=Mat1._44+Mat2._44;
	return ptmat;
}

px_matrix PX_MatrixSub(px_matrix Mat1,px_matrix Mat2)
{
	px_matrix ptmat;
	ptmat._11=Mat1._11-Mat2._11;ptmat._12=Mat1._12-Mat2._12;ptmat._13=Mat1._13-Mat2._13;ptmat._14=Mat1._14-Mat2._14;
	ptmat._21=Mat1._21-Mat2._21;ptmat._22=Mat1._22-Mat2._22;ptmat._23=Mat1._23-Mat2._23;ptmat._24=Mat1._24-Mat2._24;
	ptmat._31=Mat1._31-Mat2._31;ptmat._32=Mat1._32-Mat2._32;ptmat._33=Mat1._33-Mat2._33;ptmat._34=Mat1._34-Mat2._34;
	ptmat._41=Mat1._41-Mat2._41;ptmat._42=Mat1._42-Mat2._42;ptmat._43=Mat1._43-Mat2._43;ptmat._44=Mat1._44-Mat2._44;
	return ptmat;
}

px_bool PX_MatrixEqual(px_matrix Mat1,px_matrix Mat2)
{
	px_int i,j;
	for (i=0;i<4;i++)
	{
		for (j=0;j<4;j++)
		{
			if (Mat1.m[i][j]!=Mat2.m[i][j])
			{
				return PX_FALSE;
			}
		}
	}
	return PX_TRUE;
}

px_void PX_MatrixRotateVector(px_matrix *mat,px_point v_base,px_point v)
{
	px_point cross=PX_PointCross(v_base,v);
	px_float cosv=PX_PointDot(v_base,v)/PX_PointMod(v_base)/PX_PointMod(v);
	px_float sinv=(px_float)PX_sqrtd(1-cosv*cosv);
	if(cross.z<0) sinv=-sinv;
	mat->_11=cosv;		mat->_12=sinv;			mat->_13=0.0f;			mat->_14=0.0f;
	mat->_21=-sinv;		mat->_22=cosv;			mat->_23=0.0f;			mat->_24=0.0f;
	mat->_31=0.0f;		mat->_32=0.0f;			mat->_33=1.0f;			mat->_34=0.0f;
	mat->_41=0.0f;		mat->_42=0.0f;			mat->_43=0.0f;			mat->_44=1.0f;
}

px_void PX_MatrixTranslation(px_matrix *mat,px_float x,px_float y,px_float z)
{
	mat->_11=1.0f;	mat->_12=0.0f;	mat->_13=0.0f;	mat->_14=0.0f;
	mat->_21=0.0f;	mat->_22=1.0f;	mat->_23=0.0f;	mat->_24=0.0f;
	mat->_31=0.0f;	mat->_32=0.0f;	mat->_33=1.0f;	mat->_34=0.0f;
	mat->_41=x;	mat->_42=y;	mat->_43=z;	mat->_44=1.0f;
}

px_void PX_MatrixRotateX(px_matrix *mat,px_float Angle)
{	
	mat->_11=1.0f;						mat->_12=0;								mat->_13=0.0f;								mat->_14=0.0f;
	mat->_21=0.0f;						mat->_22=PX_cos_angle(Angle);			mat->_23=PX_sin_angle(Angle);				mat->_24=0.0f;
	mat->_31=0.0f;						mat->_32=-PX_sin_angle(Angle);			mat->_33=PX_cos_angle(Angle);				mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;								mat->_44=1.0f;
}

px_void PX_MatrixRotateY(px_matrix *mat,px_float Angle)
{
	mat->_11=PX_cos_angle(Angle);		mat->_12=0.0f;							mat->_13=PX_sin_angle(Angle);			mat->_14=0.0f;
	mat->_21=0.0f;						mat->_22=1.0f;							mat->_23=0.0f;							mat->_24=0.0f;
	mat->_31=-PX_sin_angle(Angle);		mat->_32=0.0f;							mat->_33=PX_cos_angle(Angle);			mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;							mat->_44=1.0f;
}

px_void PX_MatrixRotateZ(px_matrix *mat,px_float Angle)
{
	
	mat->_11=PX_cos_angle(Angle);		mat->_12=PX_sin_angle(Angle);			mat->_13=0.0f;			mat->_14=0.0f;
	mat->_21=-PX_sin_angle(Angle);		mat->_22=PX_cos_angle(Angle);			mat->_23=0.0f;			mat->_24=0.0f;
	mat->_31=0.0f;						mat->_32=0.0f;							mat->_33=1.0f;			mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;			mat->_44=1.0f;
}


px_void PX_MatrixRotateXRadian(px_matrix *mat,px_float rad)
{	
	mat->_11=1.0f;						mat->_12=0;								mat->_13=0.0f;								mat->_14=0.0f;
	mat->_21=0.0f;						mat->_22=PX_cos_radian(rad);			mat->_23=PX_sin_radian(rad);				mat->_24=0.0f;
	mat->_31=0.0f;						mat->_32=-PX_sin_radian(rad);			mat->_33=PX_cos_radian(rad);				mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;								mat->_44=1.0f;
}

px_void PX_MatrixRotateYRadian(px_matrix *mat,px_float rad)
{
	mat->_11=PX_cos_radian(rad);		mat->_12=0.0f;							mat->_13=PX_sin_radian(rad);			mat->_14=0.0f;
	mat->_21=0.0f;						mat->_22=1.0f;							mat->_23=0.0f;							mat->_24=0.0f;
	mat->_31=-PX_sin_radian(rad);		mat->_32=0.0f;							mat->_33=PX_cos_radian(rad);			mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;							mat->_44=1.0f;
}

px_void PX_MatrixRotateZRadian(px_matrix *mat,px_float rad)
{

	mat->_11=PX_cos_radian(rad);		mat->_12=PX_sin_radian(rad);			mat->_13=0.0f;			mat->_14=0.0f;
	mat->_21=-PX_sin_radian(rad);		mat->_22=PX_cos_radian(rad);			mat->_23=0.0f;			mat->_24=0.0f;
	mat->_31=0.0f;						mat->_32=0.0f;							mat->_33=1.0f;			mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;			mat->_44=1.0f;
}

px_void PX_MatrixScale(px_matrix *mat,px_float x,px_float y,px_float z)
{
	mat->_11=x;	mat->_12=0.0f;	mat->_13=0.0f;	mat->_14=0.0f;
	mat->_21=0.0f;	mat->_22=y;	mat->_23=0.0f;	mat->_24=0.0f;
	mat->_31=0.0f;	mat->_32=0.0f;	mat->_33=z;	mat->_34=0.0f;
	mat->_41=0.0f;	mat->_42=0.0f;	mat->_43=0.0f;	mat->_44=1.0f;
}

static px_float ptabs(px_float x){return x>0?x:-x;}
static px_bool Gauss(px_float A[][4], px_float B[][4])
{

	px_int i, j, k;
	px_float max, temp;
	px_float t[4][4];                
	for (i = 0; i < 4; i++)        
	{
		for (j = 0; j < 4; j++)
		{
			t[i][j] = A[i][j];
		}
	}
	for (i = 0; i < 4; i++)        
	{
		for (j = 0; j < 4; j++)
		{
			B[i][j] = (i == j) ? (px_float)1 : 0;
		}
	}
	for (i = 0; i < 4; i++)
	{
		max = t[i][i];
		k = i;
		for (j = i+1; j < 4; j++)
		{
			if (ptabs(t[j][i]) > ptabs(max))
			{
				max = t[j][i];
				k = j;
			}
		}
		if (k != i)
		{
			for (j = 0; j < 4; j++)
			{
				temp = t[i][j];
				t[i][j] = t[k][j];
				t[k][j] = temp;
				temp = B[i][j];
				B[i][j] = B[k][j];
				B[k][j] = temp; 
			}
		}
		if (t[i][i] == 0)
		{
			return PX_FALSE;
		}
		temp = t[i][i];
		for (j = 0; j < 4; j++)
		{
			t[i][j] = t[i][j] / temp;        
			B[i][j] = B[i][j] / temp;        
		}
		for (j = 0; j < 4; j++)       
		{
			if (j != i)             
			{
				temp = t[j][i];
				for (k = 0; k < 4; k++)       
				{
					t[j][k] = t[j][k] - t[i][k]*temp;
					B[j][k] = B[j][k] - B[i][k]*temp;
				}
			}
		}
	}
	return PX_TRUE;
}


px_bool PX_MatrixInverse(px_matrix *mat)
{
	return Gauss(mat->m,mat->m);
}

px_void PX_MatrixTranspose(px_matrix *matrix)
{
	px_matrix mat=*matrix;
	matrix->_11=mat._11;	matrix->_12=mat._21;	matrix->_13=mat._31;	matrix->_14=mat._41;
	matrix->_21=mat._12;	matrix->_22=mat._22;	matrix->_23=mat._32;	matrix->_24=mat._42;
	matrix->_31=mat._13;	matrix->_32=mat._23;	matrix->_33=mat._33;	matrix->_34=mat._43;
	matrix->_41=mat._14;	matrix->_42=mat._24;	matrix->_43=mat._34;	matrix->_44=mat._44;
}


px_color PX_COLOR(px_uchar a,px_uchar r,px_uchar g,px_uchar b)
{
	px_color color;
	color._argb.a=a;
	color._argb.r=r;
	color._argb.g=g;
	color._argb.b=b;
	return color;
}

px_color PX_ColorInverse(px_color clr)
{
	px_color color;
	color._argb.a = clr._argb.a;
	color._argb.r = 255-clr._argb.r;
	color._argb.g = 255 - clr._argb.g;
	color._argb.b = 255 - clr._argb.b;
	return color;
}

px_void PX_ColorIncrease(px_color *color,px_uchar inc)
{
	if(color->_argb.r+inc>=0xff)
	{
		color->_argb.r=0xff;
	}
	else
	{
		color->_argb.r+=inc;
	}

	if(color->_argb.g+inc>=0xff)
	{
		color->_argb.g=0xff;
	}
	else
	{
		color->_argb.g+=inc;
	}

	if(color->_argb.b+inc>=0xff)
	{
		color->_argb.b=0xff;
	}
	else
	{
		color->_argb.b+=inc;
	}
};

px_color PX_ColorAdd(px_color color1,px_color color2)
{
	if(color1._argb.r+color2._argb.r>=0xff)
	{
		color1._argb.r=0xff;
	}
	else
	{
		color1._argb.r+=color2._argb.r;
	}

	if(color1._argb.g+color2._argb.g>=0xff)
	{
		color1._argb.g=0xff;
	}
	else
	{
		color1._argb.g+=color2._argb.g;
	}

	if(color1._argb.b+color2._argb.b>=0xff)
	{
		color1._argb.b=0xff;
	}
	else
	{
		color1._argb.b+=color2._argb.b;
	}

	if(color1._argb.a+color2._argb.a>=0xff)
	{
		color1._argb.a=0xff;
	}
	else
	{
		color1._argb.a+=color2._argb.a;
	}
	return color1;
};

px_color PX_ColorSub(px_color color1,px_color color2)
{
	if(color1._argb.r-color2._argb.r<0)
	{
		color1._argb.r=0;
	}
	else
	{
		color1._argb.r-=color2._argb.r;
	}

	if(color1._argb.g-color2._argb.g<0)
	{
		color1._argb.g=0;
	}
	else
	{
		color1._argb.g-=color2._argb.g;
	}

	if(color1._argb.b-color2._argb.b<0)
	{
		color1._argb.b=0;
	}
	else
	{
		color1._argb.b-=color2._argb.b;
	}

	if(color1._argb.a-color2._argb.a<0)
	{
		color1._argb.a=0;
	}
	else
	{
		color1._argb.a-=color2._argb.a;
	}
	return color1;
}

px_bool PX_ColorEqual(px_color color1,px_color color2)
{
	return color1._argb.ucolor==color2._argb.ucolor;
}

px_color_hsl PX_ColorRGBToHSL(px_color color_rgb)
{
	px_color_hsl hsl;
	px_float max=0,min=0;
	px_float r,g,b;

	r=color_rgb._argb.r/255.f;
	g=color_rgb._argb.g/255.f;
	b=color_rgb._argb.b/255.f;

	max=r;
	max=max<g?g:max;
	max=max<b?b:max;

	min=r;
	min=min>g?g:min;
	min=min>b?b:min;


	hsl.L=(max+min)/2.0f;
	if (max==min)
	{
		hsl.S=0;
	}
	else
	{
		hsl.S=(hsl.L<0.5f?(max-min)/(max + min):(max-min)/(2.0f-max-min));
	}
	if(r==max) hsl.H=(g-b)*1.0f/(max-min);
	if(g==max) hsl.H=2.0f+(b-r)*1.0f/(max-min);
	if(b==max) hsl.H=4.0f+(r-g)*1.0f/(max-min);
	hsl.H=hsl.H*60;
	if (hsl.H<0)
	{
		hsl.H+=360;
	}
	hsl.a=color_rgb._argb.a/255.f;
	return hsl;
}

px_color PX_ColorHSLToRGB(px_color_hsl color_hsl)
{
	px_float r,g,b,temp1,temp2,temp3,H,S,L;
	H=color_hsl.H;
	S=color_hsl.S;
	L=color_hsl.L;

	if (color_hsl.S==0)
	{
		r=g=b=color_hsl.L;
	}
	else
	{
		if(L<0.5)
			temp2=L*(1.0f+S);
		else
			temp2=L+S-L*S;

		temp1 = 2.0f * L - temp2;
		H /= 360;

		r = temp3 = H + 1.0f / 3.0f;
		if (temp3 < 0)
			temp3 = temp3 + 1.0f;
		if (temp3 > 1)
			temp3 = temp3 - 1.0f;

		if (6.0f * temp3 < 1)r = temp1 + (temp2 - temp1) * 6.0f * temp3;
		else if (2.0f * temp3 < 1)r = temp2;
		else if (3.0f * temp3 < 2)r = temp1 + (temp2 - temp1) * ((2.0f / 3.0f) - temp3) * 6.0f;
		else r = temp1;

		temp3 = H;
		if (temp3 < 0)
			temp3 = temp3 + 1.0f;
		if (temp3 > 1)
			temp3 = temp3 - 1.0f;
		if (6.0f * temp3 < 1)g = temp1 + (temp2 - temp1) * 6.0f * temp3;
		else if (2.0f * temp3 < 1)g = temp2;
		else if (3.0f * temp3 < 2)g = temp1 + (temp2 - temp1) * ((2.0f / 3.0f) - temp3) * 6.0f;
		else g = temp1;

		temp3 = H - 1.0f / 3.0f;
		if (temp3 < 0)
			temp3 = temp3 + 1.0f;
		if (temp3 > 1)
			temp3 = temp3 - 1.0f;
		if (6.0f * temp3 < 1)b = temp1 + (temp2 - temp1) * 6.0f * temp3;
		else if (2.0f * temp3 < 1)b = temp2;
		else if (3.0f * temp3 < 2)b = temp1 + (temp2 - temp1) * ((2.0f / 3.0f) - temp3) * 6.0f;
		else b = temp1;
	}

	return PX_COLOR((px_uchar)(color_hsl.a*255),(px_uchar)(r*255),(px_uchar)(g*255),(px_uchar)(b*255));
}

px_color PX_ColorMul(px_color color1,px_double muls)
{
	color1._argb.a=(px_uchar)(color1._argb.a*muls);
	color1._argb.r=(px_uchar)(color1._argb.r*muls);
	color1._argb.g=(px_uchar)(color1._argb.g*muls);
	color1._argb.b=(px_uchar)(color1._argb.b*muls);
	return color1;
}



px_point PX_PointAdd(px_point p1,px_point p2)
{
	p1.x+=p2.x;
	p1.y+=p2.y;
	p1.z+=p2.z;
	return p1;
}

px_point2D PX_Point2DAdd(px_point2D p1,px_point2D p2)
{
	p1.x+=p2.x;
	p1.y+=p2.y;
	return p1;
}


px_point PX_PointSub(px_point p1,px_point p2)
{
	p1.x-=p2.x;
	p1.y-=p2.y;
	p1.z-=p2.z;
	return p1;
}

px_point2D PX_Point2DSub(px_point2D p1,px_point2D p2)
{
	p1.x-=p2.x;
	p1.y-=p2.y;
	return p1;
}


px_point4D PX_Point4DSub(px_point4D p1,px_point4D p2)
{
	px_point4D v;
	v.x=p1.x-p2.x;
	v.y=p1.y-p2.y;
	v.z=p1.z-p2.z;
	v.w=1;
	return v;
}

px_point PX_PointMul(px_point p1,px_float m)
{
	p1.x*=m;
	p1.y*=m;
	p1.z*=m;
	return p1;
}

px_point2D PX_Point2DMul(px_point2D p1,px_float m)
{
	p1.x*=m;
	p1.y*=m;
	return p1;
}

px_point PX_PointDiv(px_point p1,px_float m)
{
	p1.x/=m;
	p1.y/=m;
	p1.z/=m;
	return p1;
}

px_point2D PX_Point2DRrthonormal(px_point2D v)
{
	return PX_Point2DNormalization(PX_POINT2D(v.y,-v.x));
}

px_point2D PX_Point2DBase(px_point2D base1,px_point2D base2,px_point2D target)
{
	base1=PX_Point2DNormalization(base1);
	base2=PX_Point2DNormalization(base2);

	return PX_POINT2D(
		(target.x*base2.y-base2.x*target.y)/(base1.x*base2.y-base2.x*base1.y),
		(target.x*base1.y-base1.x*target.y)/(base2.x*base1.y-base1.x*base2.y)
		);
}

px_point2D PX_Point2DDiv(px_point2D p1,px_float m)
{
	p1.x/=m;
	p1.y/=m;
	return p1;
}

px_float PX_PointDot(px_point p1,px_point p2)
{
	return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
}

px_float PX_Point2DDot(px_point2D p1,px_point2D p2)
{
	return p1.x*p2.x+p1.y*p2.y;
}

px_float PX_Point4DDot(px_point4D p1,px_point4D p2)
{
	return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
}

px_point PX_PointCross(px_point p1,px_point p2)
{
	px_point pt;
	pt.x=p1.y*p2.z-p2.y*p1.z;
	pt.y=p1.z*p2.x-p2.z*p1.x;
	pt.z=p1.x*p2.y-p2.x*p1.y;
	return pt;
}


px_point4D PX_Point4DCross(px_point4D p1,px_point4D p2)
{
	px_point4D pt;
	pt.x=p1.y*p2.z-p2.y*p1.z;
	pt.y=p1.z*p2.x-p2.z*p1.x;
	pt.z=p1.x*p2.y-p2.x*p1.y;
	pt.w=1;
	return pt;
}

px_point PX_PointInverse(px_point p1)
{
	return PX_POINT(-p1.x,-p1.y,-p1.z);
}

px_float PX_PointMod(px_point p)
{
	return PX_sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
}

px_float PX_Point2DMod(px_point2D p)
{
	return PX_sqrt(p.x*p.x+p.y*p.y);
}


px_float  PX_PointSquare(px_point p)
{
	return (p.x*p.x+p.y*p.y+p.z*p.z);
}

px_point PX_PointNormalization(px_point p)
{
	if (p.x||p.y||p.z)
	{
		return PX_PointDiv(p,PX_PointMod(p));
	}
	return p;
}

px_point2D PX_Point2DNormalization(px_point2D p)
{
	if (p.x||p.y)
	{
		return PX_Point2DDiv(p,PX_Point2DMod(p));
	}
	return p;
}



px_point4D PX_Point4DUnit(px_point4D p)
{
	px_point4D pt;
	if (p.x||p.y||p.z)
	{
		px_float sqare=PX_sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
		pt.x=p.x/sqare;
		pt.y=p.y/sqare;
		pt.z=p.z/sqare;
		pt.w=1;
		return pt;
	}
	return p;
}

px_point PX_PointReflectX(px_point vector_refer,px_point respoint)
{
	px_point ret;
	px_float cosx,sinx;
	px_float mod=PX_PointMod(vector_refer);
	cosx=vector_refer.x/mod;
	sinx=-vector_refer.y/mod;

	ret.x=respoint.x*cosx+respoint.y*sinx;
	ret.y=respoint.y*cosx-respoint.x*sinx;
	ret.z=respoint.z;
	return ret;
}



px_bool PX_isRectCrossRect(px_rect rect1,px_rect rect2)
{
	px_float disx,disy;
	disx=PX_ABS(rect1.x+rect1.width/2-rect2.x-rect2.width/2);
	disy=PX_ABS(rect1.y+rect1.height/2-rect2.y-rect2.height/2);
	if (disx*2<rect1.width+rect2.width&&disy*2<rect1.height+rect2.height)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}


px_bool PX_isRectCrossCircle(px_rect rect1,px_point center,px_float radius)
{
	px_float dx=PX_ABS(rect1.x+rect1.width/2-center.x);
	px_float dy=PX_ABS(rect1.y+rect1.height/2-center.y);
	return (dx<radius+rect1.width/2)&&(dy<radius+rect1.height/2);
}


px_bool PX_isCircleCrossCircle(px_point center1,px_float radius1,px_point center2,px_float radius2)
{
	px_float dis=PX_PointDistance(center1,center2);
	return (dis<radius1+radius2);
}


px_double PX_Covariance(px_double x[],px_double y[],px_int n)
{
	px_int i;
	px_double x_average,y_average;
	px_double sum;
	sum=0;

	if (n-1<=0)
	{
		return 0;
	}


	for (i=0;i<n;i++)
	{
		sum+=x[i];
	}

	x_average=sum/(n);

	sum=0;
	for (i=0;i<n;i++)
	{
		sum+=y[i];
	}
	y_average=sum/(n);

	sum=0; 
	for (i=0;i<n;i++)
		sum+=(x[i]-x_average)*(y[i]-y_average);

	return (px_double)sum/(n-1);
}


px_double PX_Variance(px_double x[],px_int n)
{
	px_int i;
	px_double average;
	px_double sum;


	if (n-1<=0)
	{
		return 0;
	}

	sum=0;
	for (i=0;i<n;i++)
	{
		sum+=x[i];
	}
	average=sum/(n);

	for (i=0;i<n;i++)
	{
		sum+=(x[i]-average)*(x[i]-average);
	}

	return (px_double)sum/(n-1);
}

px_bool PX_isPointInRect(px_point p,px_rect rect)
{
		if (p.x<rect.x)
		{
			return PX_FALSE;
		}
		if (p.x>rect.x+rect.width)
		{
			return PX_FALSE;
		}
		if (p.y<rect.y)
		{
			return PX_FALSE;
		}
		if (p.y>rect.y+rect.height)
		{
			return PX_FALSE;
		}
		return PX_TRUE;
}


px_bool PX_isXYInRegion(px_float x,px_float y,px_float rectx,px_float recty,px_float width,px_float height)
{
	if (x<rectx)
	{
		return PX_FALSE;
	}
	if (x>rectx+width)
	{
		return PX_FALSE;
	}
	if (y<recty)
	{
		return PX_FALSE;
	}
	if (y>recty+height)
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_bool PX_isLineCrossRect(px_point p1,px_point p2,px_rect rect,px_point *cp1,px_point *cp2)
{
	px_float k,b;
	px_float calx,caly;
	px_bool bcross=PX_FALSE;
	
	if (!cp1||!cp2)
	{
		return PX_FALSE;
	}
	if (cp1&&cp2)
	{
		*cp1=p1;
		*cp2=p2;
	}

	if (PX_isPointInRect(p1,rect)&&PX_isPointInRect(p2,rect))
	{
		return PX_TRUE;
	}

	if (PX_ABS(p1.x-p2.x)<0.000001f)
	{
		if (p2.y<p1.y)
		{
			px_point p;
			p=p2;
			p2=p1;
			p1=p;
		}
		*cp1=p1;
		*cp2=p2;


		if (p1.x>rect.x&&p1.x<rect.x+rect.width)
		{
			if (p1.y>rect.y+rect.height)
			{
				return PX_FALSE;
			}
			if (p2.y<rect.y)
			{
				return PX_FALSE;
			}

			if (p1.y<rect.y)
			{
				cp1->y=rect.y;
			}
			if (p2.y>rect.y+rect.height)
			{
				cp2->y=rect.y+rect.height;
			}
			return PX_TRUE;
		}
		return PX_FALSE;
	}


	if (PX_ABS(p1.y-p2.y)<0.000001f)
	{
		if (p2.x<p1.x)
		{
			px_point p;
			p=p2;
			p2=p1;
			p1=p;
		}

		*cp1=p1;
		*cp2=p2;

		if (p1.y>rect.y&&p1.y<rect.y+rect.height)
		{
			if (p1.x>rect.x+rect.width)
			{
				return PX_FALSE;
			}
			if (p2.x<rect.x)
			{
				return PX_FALSE;
			}

			if (p1.x<rect.x)
			{
				cp1->x=rect.x;
			}
			if (p2.x>rect.x+rect.width)
			{
				cp2->x=rect.x+rect.width;
			}
			return PX_TRUE;
		}
		return PX_FALSE;
	}


	if (p2.x<p1.x)
	{
		px_point p;
		p=p2;
		p2=p1;
		p1=p;
	}

	

	k=(p2.y-p1.y)/(p2.x-p1.x);
	b=p2.y-k*p2.x;

	calx=rect.x;
	caly=calx*k+b;
	if (PX_isPointInRect(PX_POINT(calx,caly,0),rect))
	{
		if (calx>p1.x&&calx<p2.x)
		{
			cp1->x=calx;
			cp1->y=caly;
			bcross=PX_TRUE;
		}
	}

	calx=rect.x+rect.width;
	caly=calx*k+b;
	if (PX_isPointInRect(PX_POINT(calx,caly,0),rect))
	{
		if (calx>p1.x&&calx<p2.x)
		{
			if (cp2)
			{
				cp2->x=calx;
				cp2->y=caly;
			}
			bcross=PX_TRUE;
		}
	}

	caly=rect.y;
	calx=(caly-b)/k;
	if (PX_isPointInRect(PX_POINT(calx,caly,0),rect))
	{
		if (calx>p1.x&&calx<p2.x)
		{
			if (k>0)
			{
				if (cp1)
				{
					cp1->x=calx;
					cp1->y=caly;
				}
			}
			else
			{
				if (cp2)
				{
					cp2->x=calx;
					cp2->y=caly;
				}
			}
			
			
			bcross=PX_TRUE;
		}
	}

	caly=rect.y+rect.height;
	calx=(caly-b)/k;
	if (PX_isPointInRect(PX_POINT(calx,caly,0),rect))
	{
		if (calx>p1.x&&calx<p2.x)
		{
			if (k>0)
			{
				if (cp2)
				{
					cp2->x=calx;
					cp2->y=caly;
				}
			}
			else
			{
				if (cp1)
				{
					cp1->x=calx;
					cp1->y=caly;
				}
			}
			
			bcross=PX_TRUE;
		}
	}

	return bcross;
}

void PX_memset(void *dst,px_byte byte,px_int size)
{
	px_dword dw=byte?(byte<<24)|(byte<<16)|(byte<<8)|byte:0;
	px_dword *_4byteMovDst=(px_dword *)dst;
	px_uchar *pdst=(px_uchar *)dst+(size&~3);
	px_uint _movTs=size>>2;
	while (_movTs--)
	{
		*_4byteMovDst++=dw;
	}
	_movTs=size&3;
	while (_movTs--)
		*(pdst++)=byte;
}


void PX_memdwordset(void *dst,px_dword dw,px_int count)
{
	px_dword *p=(px_dword *)dst;
	while(count--)*p++=dw;
}


px_bool PX_memequ(const void *dst,const void *src,px_int size)
{
	px_dword *_4byteMovSrc=(px_dword *)src;
	px_dword *_4byteMovDst=(px_dword *)dst;
	px_uchar *psrc=(px_uchar *)src+(size&~3);
	px_uchar *pdst=(px_uchar *)dst+(size&~3);
	px_uint _movTs=size>>2;
	if (dst==PX_NULL||src==PX_NULL)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	while (_movTs--)
	{
		if(*_4byteMovDst++!=*_4byteMovSrc++)
			return PX_FALSE;
	}
	_movTs=size&3;
	while (_movTs--)
		if(*(pdst++)!=*(psrc++))
			return PX_FALSE;
	return PX_TRUE;
}


px_void PX_memcpy(px_void *dst,const px_void *src,px_int size)
{
	typedef struct
	{
		px_byte m[16];
	}PX_MEMCPY_16;

	typedef struct
	{
		px_byte m[32];
	}PX_MEMCPY_32;

	typedef struct
	{
		px_byte m[64];
	}PX_MEMCPY_64;

	typedef struct
	{
		px_byte m[128];
	}PX_MEMCPY_128;

	typedef struct
	{
		px_byte m[256];
	}PX_MEMCPY_256;

	typedef struct
	{
		px_byte m[512];
	}PX_MEMCPY_512;

	typedef struct
	{
		px_byte m[1024];
	}PX_MEMCPY_1024;

	typedef struct
	{
		px_byte m[2048];
	}PX_MEMCPY_2048;

	typedef struct
	{
		px_byte m[4096];
	}PX_MEMCPY_4096;

	px_dword *_4byteMovSrc;
	px_dword *_4byteMovDst;
	px_uchar *psrc;
	px_uchar *pdst;
	PX_MEMCPY_4096 *_4kbyteMovSrc,*_4kbyteMovDst;
	PX_MEMCPY_2048 *_2kbyteMovSrc,*_2kbyteMovDst;
	PX_MEMCPY_1024 *_1kbyteMovSrc,*_1kbyteMovDst;
	PX_MEMCPY_512 *_512byteMovSrc,*_512byteMovDst;
	PX_MEMCPY_256 *_256byteMovSrc,*_256byteMovDst;
	PX_MEMCPY_128 *_128byteMovSrc,*_128byteMovDst;
	PX_MEMCPY_64 *_64byteMovSrc,*_64byteMovDst;
	PX_MEMCPY_32 *_32byteMovSrc,*_32byteMovDst;
	PX_MEMCPY_16 *_16byteMovSrc,*_16byteMovDst;
	px_uint _movTs;

	if (size<=0)
	{
		return;
	}
	//is overlap?
	if (dst>src&&(px_char *)dst<(px_char *)src+size)
	{
		px_dword _4byteMov=0;//4bytes unit-copy
		//backward overlap
		psrc=(px_uchar *)src+size-1;
		pdst=(px_uchar *)dst+size-1;

		_movTs=size&3;

		while (_movTs--)
			*(pdst--)=*(psrc--);

		pdst-=3;
		psrc-=3;
		_4byteMovDst=(px_dword *)pdst;
		_4byteMovSrc=(px_dword *)psrc;
		_movTs=size>>2;
		while (_movTs--)
		{
			_4byteMov=*_4byteMovSrc;
			_4byteMovSrc--;
			*_4byteMovDst=_4byteMov;
			_4byteMovDst--;
		}
	}
	else if (src>dst&&(px_char *)src<(px_char *)dst+size)
	{
		px_dword _4byteMov=0;//4bytes unit-copy

		//forward overlap
		psrc=(px_uchar *)src;
		pdst=(px_uchar *)dst;

		_4byteMovDst=(px_dword *)pdst;
		_4byteMovSrc=(px_dword *)psrc;
		_movTs=size>>2;
		while (_movTs--)
		{
			_4byteMov=*_4byteMovSrc;
			_4byteMovSrc++;
			*_4byteMovDst=_4byteMov;
			_4byteMovDst++;
		}
		_movTs=size&3;

		psrc=(px_uchar *)_4byteMovSrc;
		pdst=(px_uchar *)_4byteMovDst;

		while (_movTs--)
			*(pdst++)=*(psrc++);

		pdst+=3;
		psrc+=3;
	}
	else
	{
		//no overlap,using block-copy
		//high->low
		_movTs=size>>12;
		if(_movTs)
		{
			_4kbyteMovSrc=(PX_MEMCPY_4096 *)src;
			_4kbyteMovDst=(PX_MEMCPY_4096 *)dst;
			while(_movTs--)*_4kbyteMovDst++=*_4kbyteMovSrc++;
			src=_4kbyteMovSrc;
			dst=_4kbyteMovDst;
		}

		_movTs=size&(1<<11);
		if(_movTs)
		{
			_2kbyteMovSrc=(PX_MEMCPY_2048 *)src;
			_2kbyteMovDst=(PX_MEMCPY_2048 *)dst;
			*_2kbyteMovDst++=*_2kbyteMovSrc++;
			src=_2kbyteMovSrc;
			dst=_2kbyteMovDst;
		}

		_movTs=size&(1<<10);
		if(_movTs)
		{
			_1kbyteMovSrc=(PX_MEMCPY_1024 *)src;
			_1kbyteMovDst=(PX_MEMCPY_1024 *)dst;
			*_1kbyteMovDst++=*_1kbyteMovSrc++;
			src=_1kbyteMovSrc;
			dst=_1kbyteMovDst;
		}

		_movTs=size&(1<<9);
		if(_movTs)
		{
			_512byteMovSrc=(PX_MEMCPY_512 *)src;
			_512byteMovDst=(PX_MEMCPY_512 *)dst;
			*_512byteMovDst++=*_512byteMovSrc++;
			src=_512byteMovSrc;
			dst=_512byteMovDst;
		}

		_movTs=size&(1<<8);
		if(_movTs)
		{
			_256byteMovSrc=(PX_MEMCPY_256 *)src;
			_256byteMovDst=(PX_MEMCPY_256 *)dst;
			*_256byteMovDst++=*_256byteMovSrc++;
			src=_256byteMovSrc;
			dst=_256byteMovDst;
		}

		_movTs=size&(1<<7);
		if(_movTs)
		{
			_128byteMovSrc=(PX_MEMCPY_128 *)src;
			_128byteMovDst=(PX_MEMCPY_128 *)dst;
			*_128byteMovDst++=*_128byteMovSrc++;
			src=_128byteMovSrc;
			dst=_128byteMovDst;
		}

		_movTs=size&(1<<6);
		if(_movTs)
		{
			_64byteMovSrc=(PX_MEMCPY_64 *)src;
			_64byteMovDst=(PX_MEMCPY_64 *)dst;
			*_64byteMovDst++=*_64byteMovSrc++;
			src=_64byteMovSrc;
			dst=_64byteMovDst;
		}

		_movTs=size&(1<<5);
		if(_movTs)
		{
			_32byteMovSrc=(PX_MEMCPY_32 *)src;
			_32byteMovDst=(PX_MEMCPY_32 *)dst;
			*_32byteMovDst++=*_32byteMovSrc++;
			src=_32byteMovSrc;
			dst=_32byteMovDst;
		}

		_movTs=size&(1<<4);
		if(_movTs)
		{
			_16byteMovSrc=(PX_MEMCPY_16 *)src;
			_16byteMovDst=(PX_MEMCPY_16 *)dst;
			*_16byteMovDst++=*_16byteMovSrc++;
			src=_16byteMovSrc;
			dst=_16byteMovDst;
		}

		_movTs=size&0x0F;
		if(_movTs>=12)
		{
			_4byteMovSrc=(px_dword *)src;
			_4byteMovDst=(px_dword *)dst;
			*_4byteMovDst++=*_4byteMovSrc++;
			*_4byteMovDst++=*_4byteMovSrc++;
			*_4byteMovDst++=*_4byteMovSrc++;
			_movTs-=12;
			psrc=(px_uchar *)_4byteMovSrc;
			pdst=(px_uchar *)_4byteMovDst;
			while(_movTs--)*pdst++=*psrc++;
		}
		else if(_movTs>=8)
		{
			_4byteMovSrc=(px_dword *)src;
			_4byteMovDst=(px_dword *)dst;
			*_4byteMovDst++=*_4byteMovSrc++;
			*_4byteMovDst++=*_4byteMovSrc++;
			_movTs-=8;
			psrc=(px_uchar *)_4byteMovSrc;
			pdst=(px_uchar *)_4byteMovDst;
			while(_movTs--)*pdst++=*psrc++;
		}
		else if(_movTs>=4)
		{
			_4byteMovSrc=(px_dword *)src;
			_4byteMovDst=(px_dword *)dst;
			*_4byteMovDst++=*_4byteMovSrc++;
			_movTs-=4;
			psrc=(px_uchar *)_4byteMovSrc;
			pdst=(px_uchar *)_4byteMovDst;
			while(_movTs--)*pdst++=*psrc++;
		}
		else
		{
			psrc=(px_uchar *)src;
			pdst=(px_uchar *)dst;
			while(_movTs--)*pdst++=*psrc++;
		}
		
	}
}



px_void PX_strcpy(px_char *dst,const px_char *src,px_int size)
{
	while(size--)
		if(*src)
			*(dst++)=*(src++);
		else
			{
				*dst='\0';
				return;
			}
	*(dst-1)='\0';
}

px_void PX_wstrcpy(px_word *dst,const px_word *src,px_int size)
{
	while(size--)
		if(*src)
			*(dst++)=*(src++);
		else
		{
			*dst='\0';
			return;
		}
		*(dst-1)='\0';
}

px_void PX_strcat(px_char *src,const px_char *cat)
{
	px_int len=PX_strlen(cat);
	while(*src)src++;
	while(len--)*src++=*cat++;
	*src='\0';
}

px_void PX_strcat_s(px_char* src, px_int size, const px_char* cat)
{
	if (PX_strlen(src)+PX_strlen(cat)<size)
	{
		PX_strcat(src, cat);
	}
}

px_void PX_wstrcat(px_word *src,const px_word *cat)
{
	px_int len=PX_wstrlen(cat);
	while(*src)src++;
	while(len--)*src++=*cat++;
	*src='\0';
}

px_void PX_strset(px_char *dst,const px_char*src)
{
	dst[0]=0;
	PX_strcat(dst,src);
}


px_int PX_strlen(const px_char *dst)
{
	px_int len=0;
	if(!dst)
	{
		PX_ERROR("NULL pointer assert!");
		return 0;
	}
	while(dst[len++]);
	return len-1;
}

px_int PX_wstrlen(const px_word *dst)
{
	px_int len=0;
	if(!dst)
	{
		PX_ERROR("NULL pointer assert!");
		return 0;
	}
	while(dst[len++]);
	return len-1;
}


px_bool PX_strequ2(const px_char* src, const px_char* dst)
{
	px_char _l, _r;
	while (PX_TRUE)
	{
		_l = *src;
		_r = *dst;
		if (_l>='a'&&_l<='z')
		{
			_l += 'A' - 'a';
		}
		if (_r >= 'a' && _r <= 'z')
		{
			_r += 'A' - 'a';
		}
		if (_l == _r)
		{
			if (_l == 0)return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
		src++;
		dst++;
	}
	return PX_FALSE;
}

px_void PX_strupr(px_char *src)
{
	while (*src != '\0')  
    {  
        if (*src >= 'a' && *src <= 'z')  
            *src -= 32;  
        src++;  
    }  
}

px_void PX_strlwr(px_char *src)
{
	 while (*src != '\0')  
    {  
        if (*src > 'A' && *src <= 'Z'){  
            *src += 32;  
        }  
        src++;  
    } 
}


px_point2D PX_Point2DMulMatrix(px_point2D p,px_matrix mat)
{
	px_point2D point;
	point.x=p.x*mat._11+p.y*mat._21+1*mat._41;
	point.y=p.x*mat._12+p.y*mat._22+1*mat._42;
	return point;
}

px_point PX_PointMulMatrix(px_point p,px_matrix mat)
{
	px_point point;
	point.x=p.x*mat._11+p.y*mat._21+p.z*mat._31+1*mat._41;
	point.y=p.x*mat._12+p.y*mat._22+p.z*mat._32+1*mat._42;
	point.z=p.x*mat._13+p.y*mat._23+p.z*mat._33+1*mat._43;
	return point;
}


px_point4D PX_Point4DMulMatrix(px_point4D p,px_matrix mat)
{
	px_point4D point;
	point.x=p.x*mat._11+p.y*mat._21+p.z*mat._31+1*mat._41;
	point.y=p.x*mat._12+p.y*mat._22+p.z*mat._32+1*mat._42;
	point.z=p.x*mat._13+p.y*mat._23+p.z*mat._33+1*mat._43;
	point.w=p.x*mat._14+p.y*mat._24+p.z*mat._34+1*mat._44;
	return point;
}


px_plane PX_PLANE(px_point3D p,px_vector3D n)
{
	px_plane pl;
	pl.n=n;
	pl.p0=p;
	return pl;
}

px_rect PX_RECT(px_float x,px_float y,px_float width,px_float height)
{
	px_rect rect;
	rect.x=x;
	rect.y=y;
	rect.width=width;
	rect.height=height;

	if (width<0)
	{
		rect.width=-width;
		rect.x+=width;
	}

	if (height<0)
	{
		rect.height=-height;
		rect.y+=height;
	}
	return rect;
}


px_rect PX_RECTPOINT2(px_point p1,px_point p2)
{
	px_rect rect;
	rect.width=PX_ABS(p2.x-p1.x);
	rect.height=PX_ABS(p2.y-p1.y);

	if (p1.x<p2.x)
	{
		rect.x=p1.x;
	}
	else
	{
		rect.x=p2.x;
	}

	if (p1.y<p2.y)
	{
		rect.y=p1.y;
	}
	else
	{
		rect.y=p2.y;
	}
	return rect;
}

px_complex PX_complexBuild(px_float re,px_float im)
{
	px_complex cx;
	cx.re=re;
	cx.im=im;
	return cx;
}

px_complex PX_complexAdd(px_complex a,px_complex b)
{
	px_complex ret;
	ret.re=a.re+b.re;
	ret.im=a.im+b.im;
	return ret;
}

px_complex PX_complexMult(px_complex a,px_complex b)
{
	px_complex ret;
	ret.re=a.re*b.re-a.im*b.im;
	ret.im=a.im*b.re+a.re*b.im;
	return ret;
}

px_double PX_complexMod(px_complex a)
{
	return PX_sqrtd(a.re*a.re+a.im*a.im);
}

px_complex PX_complexLog(px_complex a)
{
	px_complex ret;
	ret.re=PX_sqrtd(a.re*a.re+a.im*a.im);
	ret.im=PX_atan2(a.im,a.re);
	return ret;
}

px_complex PX_complexExp(px_complex a)
{
	px_double p;
	px_complex ret;
	p=PX_exp(a.re);
	ret.re=p*PX_cosd(a.im);
	ret.im=p*PX_cosd(a.re);
	return ret;
}

px_complex PX_complexSin(px_complex a)
{
	px_double64 q,p;
	px_complex ret;
	p=PX_exp(a.im);
	q=1/p;
	ret.re=PX_sind(a.re)*(q+p)/2.0;
	ret.im=PX_cosd(a.re)*(p-q)/2.0;
	return ret;
}


void PX_DFT(_IN px_complex x[],_OUT px_complex X[],px_int N)
{
	px_int k,n;
	px_complex Wnk;
	if (x==X)
	{
		//overlap error
		PX_ASSERT();
	}

	for (k=0;k<N;k++)
	{
		X[k].re=0;
		X[k].im=0;
		for (n=0;n<N;n++)
		{
			Wnk.re=(px_float)PX_cosd((2*PX_PI*k*n/N));
			Wnk.im=(px_float)-PX_sind((2*PX_PI*k*n/N));
			X[k]=PX_complexAdd(X[k],PX_complexMult(x[n],Wnk));
		}
	}
}

void PX_DCT(_IN px_double x[],_OUT px_double X[],px_int N)
{
	px_int n,m;
	px_double v;
	if (x == X)
	{
		//overlap error
		PX_ASSERT();
	}
	PX_memset(X,0,sizeof(px_double)*N);
	for (n=0;n<N;n++)
	{
		for (m=0;m<N;m++)
		{
			X[n]+=x[m]*PX_cosd((2*m+1)*PX_PI*n/(2*N));
		}	
	}

	v=PX_sqrtd(2.0/N);
	X[0]*=PX_sqrtd(1.0/N);
	
	for (n=1;n<N;n++)
	{
		X[n]*=v;
	}
}

void PX_IDFT(_IN px_complex X[],_OUT px_complex x[],px_int N)
{
	px_int k,n;
	px_float im=0;
	px_complex ejw;
	
	if (x==X)
	{
		//overlap error
		PX_ASSERT();
	}

	for (k=0;k<N;k++)
	{
		x[k].re=0;
		x[k].im=0;
		for (n=0;n<N;n++)
		{
			ejw.re=(px_float)PX_cosd((2*PX_PI*k*n/N));
			ejw.im=(px_float)PX_sind((2*PX_PI*k*n/N));
			x[k]=PX_complexAdd(x[k],PX_complexMult(X[n],ejw));
		}

		x[k].re/=N;x[k].im/=N;
	}
}   

void PX_IDCT(_IN px_double X[],_OUT px_double x[],px_int N)
{
	px_int n,m;
	px_double v0,v;
	PX_memset(x,0,sizeof(px_double)*N);

	v=PX_sqrtd(2.0/N);
	v0=PX_sqrtd(1.0/N);

	for (m=0;m<N;m++)
	{
		for (n=0;n<N;n++)
		{
			if (n==0)
			{
				x[m]+=v0*X[n]*PX_cosd(PX_PI*n*(2*m+1)/(2*N));
			}
			else
			{
				x[m]+=v*X[n]*PX_cosd(PX_PI*n*(2*m+1)/(2*N));
			}
		}
	}	
}

void FFT_Base2(_IN _OUT px_complex x[],px_int N)
{
	px_int exbase,exrang,i,j,k;
	px_complex excomplex,Wnk,cx0,cx1;
	if (N>>2)
	{
		// x[] 4 base odd/even Sort
		exbase=1;
		exrang=0;
		while (exrang<N)
		{
			exrang=exbase<<2;

			for (i=0;i<N/exrang;i++)//for each token
			{
				for (j=0;j<exbase;j++)//for each atom in token
				{
					excomplex=x[exrang*i+exbase+j];
					x[exrang*i+exbase+j]=x[exrang*i+exbase*2+j];
					x[exrang*i+exbase*2+j]=excomplex;
				}
			}
			exbase<<=1;
		}
		FFT_Base2(x,N>>1);
		FFT_Base2(x+(N>>1),N>>1);

		for(k=0;k<N>>1;k++)
		{
			Wnk.re=(px_float)PX_cosd((-2*PX_PI*k/N));
			Wnk.im=(px_float)PX_sind((-2*PX_PI*k/N));
			cx0=x[k];
			cx1=x[k+(N>>1)];
			x[k]=PX_complexAdd(cx0,PX_complexMult(Wnk,cx1));
			Wnk.re=-Wnk.re;
			Wnk.im=-Wnk.im;
			x[k+(N>>1)]=PX_complexAdd(cx0,PX_complexMult(Wnk,cx1));
		}
	}
	else
	{
		//2 dot DFT
		cx0=x[0];
		cx1=x[1];
		x[0]=PX_complexAdd(cx0,cx1);
		cx1.im=-cx1.im;
		cx1.re=-cx1.re;
		x[1]=PX_complexAdd(cx0,cx1);
	}


}
void PX_FFT(_IN px_complex x[],_OUT px_complex X[],px_int N)
{
	PX_memcpy(X,x,sizeof(px_complex)*N);
	FFT_Base2(X,N);
}

void IFFT_Base2(_IN _OUT px_complex X[],px_int N)
{
	px_int exbase,exrang,i,j,n;
	px_complex excomplex,Wnnk,cx0,cx1;
	if (N>>2)
	{
		// x[] 4 base odd/even Sort
		exbase=1;
		exrang=0;
		while (exrang<N)
		{
			exrang=exbase<<2;

			for (i=0;i<N/exrang;i++)//for each token
			{
				for (j=0;j<exbase;j++)//for each atom in token
				{
					excomplex=X[exrang*i+exbase+j];
					X[exrang*i+exbase+j]=X[exrang*i+exbase*2+j];
					X[exrang*i+exbase*2+j]=excomplex;
				}
			}
			exbase<<=1;
		}
		IFFT_Base2(X,N>>1);
		IFFT_Base2(X+(N>>1),N>>1);

		for(n=0;n<N>>1;n++)
		{
			Wnnk.re=(px_float)PX_cosd((2*PX_PI*n/N));
			Wnnk.im=(px_float)PX_sind((2*PX_PI*n/N));
			cx0=X[n];
			cx1=X[n+(N>>1)];
			X[n]=PX_complexAdd(cx0,PX_complexMult(Wnnk,cx1));

			Wnnk.re=-Wnnk.re;
			Wnnk.im=-Wnnk.im;
			X[n+(N>>1)]=PX_complexAdd(cx0,PX_complexMult(Wnnk,cx1));

		}
	}
	else
	{
		//2 dot IDFT
		cx0=X[0];
		cx1=X[1];
		X[0]=PX_complexAdd(cx0,cx1);

		cx1.im=-cx1.im;
		cx1.re=-cx1.re;
		X[1]=PX_complexAdd(cx0,cx1);

	}
}
void PX_IFFT(_IN px_complex X[],_OUT px_complex x[],px_int N)
{
	px_int i;
	PX_memcpy(X,x,sizeof(px_complex)*N);
	IFFT_Base2(X,N);
	// 1/N operate
	for (i=0;i<N;i++)
	{
		X[i].re/=N;
		X[i].im/=N;
	}
}


void PX_FFT_2(_IN px_complex x[],_OUT px_complex X[],px_int N)
{
	px_int i,cx,cy;
	px_complex _t;
	for (i=0;i<N;i++)
	{
		PX_FFT(&x[i*N],&X[i*N],N);
	}
	//Matrix transpose
	for (cy=0;cy<N;cy++)
	{
		for (cx=cy+1;cx<N;cx++)
		{
			_t=X[cy*N+cx];
			X[cy*N+cx]=X[cx*N+cy];
			X[cx*N+cy]=_t;
		}
	}

	for(i=0;i<N;i++)
	{
		PX_FFT(&X[i*N],&X[i*N],N);
	}

	//Matrix transpose again

	for (cy=0;cy<N;cy++)
	{
		for (cx=cy+1;cx<N;cx++)
		{
			_t=X[cy*N+cx];
			X[cy*N+cx]=X[cx*N+cy];
			X[cx*N+cy]=_t;
		}
	}
}
void PX_IFFT_2(_IN px_complex X[],_OUT px_complex x[],px_int N)
{
	px_int cx,cy,i;
	px_complex _t;
	//Matrix transpose
	for (cy=0;cy<N;cy++)
	{
		for (cx=cy+1;cx<N;cx++)
		{
			_t=X[cy*N+cx];
			X[cy*N+cx]=X[cx*N+cy];
			X[cx*N+cy]=_t;
		}
	}

	for(i=0;i<N;i++)
	{
		PX_IFFT(&x[i*N],&X[i*N],N);
	}

	//Matrix transpose again
	for (cy=0;cy<N;cy++)
	{
		for (cx=cy+1;cx<N;cx++)
		{
			_t=X[cy*N+cx];
			X[cy*N+cx]=X[cx*N+cy];
			X[cx*N+cy]=_t;
		}
	}

	for (i=0;i<N;i++)
	{
		PX_IFFT(&X[i*N],&X[i*N],N);
	}
}

void PX_FFT_2_Shift(_IN px_complex _in[],_OUT px_complex _out[],px_int N)
{
	px_int x,y;
	px_complex *_t=_out;
	px_complex _ext;
	PX_memcpy(_t,_in,sizeof(px_complex)*N*N);

	for (y=0;y<N/2;y++)
	{
		for (x=0;x<N/2;x++)
		{
			_ext=_t[y*N+x];
			_t[y*N+x]=_t[y*N+N*N/2+x+N/2];
			_t[y*N+N*N/2+x+N/2]=_ext;
		}
	}

	for (y=0;y<N/2;y++)
	{
		for (x=N/2;x<N;x++)
		{
			_ext=_t[y*N+x];
			_t[y*N+x]=_t[y*N+N*N/2+x-N/2];
			_t[y*N+N*N/2+x-N/2]=_ext;
		}
	}
}

void PX_DCT_2_Shift(_IN px_double _in[], _OUT px_double _out[], px_int N)
{
	px_int x, y;
	px_double* _t = _out;
	px_double _ext;
	PX_memcpy(_t, _in, sizeof(px_double) * N * N);

	for (y = 0; y < N / 2; y++)
	{
		for (x = 0; x < N / 2; x++)
		{
			_ext = _t[y * N + x];
			_t[y * N + x] = _t[y * N + N * N / 2 + x + N / 2];
			_t[y * N + N * N / 2 + x + N / 2] = _ext;
		}
	}

	for (y = 0; y < N / 2; y++)
	{
		for (x = N / 2; x < N; x++)
		{
			_ext = _t[y * N + x];
			_t[y * N + x] = _t[y * N + N * N / 2 + x - N / 2];
			_t[y * N + N * N / 2 + x - N / 2] = _ext;
		}
	}
}

void PX_FT_Symmetry(_IN px_complex x[],_OUT px_complex X[],px_int N)
{
	px_int l=1,r=N-1;
	PX_memcpy(X,x,N/2*sizeof(px_complex)+1);
	while (l<r)
	{
		X[r]=X[l];
		X[r].im=-X[r].im;
		l++;
		r--;
	}
}

void PX_Cepstrum(_IN px_complex x[],_OUT px_complex X[],px_int N,PX_CEPSTRUM_TYPE type)
{
	px_int i;
	PX_FFT(x,X,N);
	if (type==PX_CEPSTRUM_TYPE_COMPLEX)
	{
		for (i=0;i<N;i++)
		{
			X[i]=PX_complexLog(X[i]);
		}
		PX_IFFT(X,X,N);
	}
	else
	{
		for (i=0;i<N;i++)
		{
			X[i].re=PX_log((X[i].re*X[i].re+X[i].im*X[i].im)+1);
			X[i].im=0;
		}
		PX_IFFT(X,X,N);
		for (i=0;i<N;i++)
		{
			X[i].re=(X[i].re*X[i].re+X[i].im*X[i].im);
			X[i].im=0;
		}
	}
	
}

px_double PX_ZeroCrossingRate(_IN px_double x[],px_int N)
{
	px_int i,zeroCross;
	zeroCross=0;
	for (i=1;i<N;i++)
	{
		if (x[i]*x[i-1]<0)
		{
			zeroCross++;
		}
	}
	return zeroCross*1.0/N;
}

px_double PX_ZeroCrossingRateComplex(_IN px_complex x[],px_int N)
{
	px_int i,zeroCross;
	zeroCross=0;
	for (i=1;i<N;i++)
	{
		if (x[i].re*x[i-1].re<0)
		{
			zeroCross++;
		}
	}
	return zeroCross*1.0/N;
}

px_int PX_PitchEstimation(_IN px_complex x[],px_int N,px_int sampleRate,px_int low_Hz,px_int high_Hz)
{
	px_int low=0,high=0,i,idx=0;
	px_double max=0;
	px_int zeroCross=0;
	low=sampleRate/high_Hz;
	high=sampleRate/low_Hz;
	for (i=1;i<N;i++)
	{
		if (x[i].re*x[i-1].re<0)
		{
			zeroCross++;
		}
	}
	if (zeroCross*1.0f/N>0.15)
	{
		return 0;
	}

	PX_Cepstrum(x,x,N,PX_CEPTRUM_TYPE_REAL);

	if (high>=N||high==low)
	{
		return 0;
	}

	for (i=low;i<=high;i++)
	{
		if(x[i].re>max)
		{
			max=x[i].re;
			idx=i;
		}
	}
	if (idx==0)
	{
		return 0;
	}
	return sampleRate/idx;
}

void PX_PreEmphasise(const px_double *data, px_int len, px_double *out, px_double preF)//0.9<preF<1.0 suggest 0.9
{
	px_int i;
	for(i = len - 1; i >= 1; i--)
	{
		out[i] = data[i] - preF * data[i-1];
	}
	out[0] = data[0];
}

void PX_LinearInterpolationResample(_IN px_double x[],_OUT px_double X[],px_int N,px_int M)
{
	px_int k,m=0;
	px_double d1=0,d2=0;
	px_double freqscale=N*1.0/M;
	for (k=0;k<M;k++)
	{
		/* linear interpolation */
		d1=0.0;
		if (m<N){
			d1+=(1.0-d2)*x[m];
		}
		if (m+1<N){
			d1+=d2*x[m+1];
		}

		/* propagate in inbuf */
		d2+=freqscale;
		if (d2>=1.0){
			m+=(px_int)(d2);
			d2-=(px_int)(d2);
			
		}
		X[k]=d1;
	}
}

void PX_SincInterpolationResample(_IN px_double x[], _OUT px_double X[], px_int N, px_int M)
{
	px_double step = 1 / (M-1);
	px_int i;
	for ( i = 0; i < M; i++)
	{
		X[i] = PX_sinc_interpolate(x, N, step * i);
	}
}

void PX_UpSampled(_IN px_complex x[],_OUT px_complex X[],px_int N,px_int L)
{
	px_int i,j;
	for (i=0;i<N;i++)
	{
		X[i*L]=x[i];
		for (j=1;j<L;j++)
		{
			X[i*L+j].re=0;
			X[i*L+j].im=0;
		}
	}

	if (((N*L)&(N*L-1))==0)
	{
		PX_FFT(X,X,N*L);
	}
	else
	{
		PX_DFT(X,X+N*L,N*L);
		PX_memcpy(X,X+N*L,sizeof(px_complex)*N*L);
	}

	for (i=N/2+1;i<N*L/2+1;i++)
	{
		X[i].re=0;
		X[i].im=0;
	}

	PX_FT_Symmetry(X,X,N*L);


	if (((N*L)&(N*L-1))==0)
	{
		PX_IFFT(X,X,N*L);
	}
	else
	{
		PX_IDFT(X,X+N*L,N*L);
		PX_memcpy(X,X+N*L,sizeof(px_complex)*N*L);
	}
}

void PX_DownSampled(_IN px_complex x[],_OUT px_complex X[],px_int N,px_int M)
{
	px_int i;
	if (((N)&(N-1))==0)
	{
		PX_FFT(x,X,N);
	}
	else
	{
		PX_DFT(x,X+N,N);
		PX_memcpy(X,X+N,sizeof(px_complex)*N);
	}

	for (i=N/(2*M)+1;i<N/2+1;i++)
	{
		X[i].re=0;
		X[i].im=0;
	}
	PX_FT_Symmetry(X,X,N);

	if (((N)&(N-1))==0)
	{
		PX_IFFT(X,X,N);
	}
	else
	{
		PX_IDFT(X,X+N,N);
		PX_memcpy(X,X+N,sizeof(px_complex)*N);
	}
	for (i=0;i<N/M;i++)
	{
		X[i]=X[i*M];
	}
}





px_point PX_POINT(px_float x,px_float y,px_float z)
{
	px_point p;
	p.x=x;
	p.y=y;
	p.z=z;
	return p;
}

px_point2D PX_POINT2D(px_float x,px_float y)
{
	px_point2D p;
	p.x=x;
	p.y=y;
	return p;
}

px_point4D PX_POINT4D(px_float x,px_float y,px_float z)
{
	px_point4D p;
	p.x=x;
	p.y=y;
	p.z=z;
	p.w=1;
	return p;
}

px_bool PX_strequ(const px_char *src,const px_char *dst)
{
	px_int ret = 0;  
	while(!(ret=*(px_uchar*)src-*(px_uchar*)dst) && *src)  
	{  
		src++;  
		dst++; 
	} 
	return !ret;
}


px_bool PX_strIsNumeric(const px_char *str)
{
	px_bool Dot=PX_FALSE;
	px_int CurrentCharIndex;

	if (str[0]=='\0')
	{
		return PX_FALSE;
	}
	if (!PX_charIsNumeric(str[0])&&!(str[0]==('-')))
	{
		if(str[0]!='.')
			return PX_FALSE;
	}
	for (CurrentCharIndex=1;
		CurrentCharIndex<PX_strlen(str);
		CurrentCharIndex++)
	{
		if (!PX_charIsNumeric(str[CurrentCharIndex]))
		{
			if (str[CurrentCharIndex]=='e'&&(str[CurrentCharIndex+1]=='+'||str[CurrentCharIndex+1]=='-'))
			{
				CurrentCharIndex++;
				continue;
			}

			if (str[CurrentCharIndex]=='.')
			{
				if (Dot)
				{
					return PX_FALSE;
				}
				else
				{
					Dot=PX_TRUE;
					continue;
				}
			}
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}

px_bool PX_strIsFloat(const px_char *str)
{
	px_bool Dot=PX_FALSE;
	px_int CurrentCharIndex;
	if (str[0]=='\0')
	{
		return PX_FALSE;
	}
	if (!PX_charIsNumeric(str[0])&&!(str[0]==('-')))
	{
		if(str[0]!='.')
			return PX_FALSE;
	}
	for (CurrentCharIndex=1;
		CurrentCharIndex<PX_strlen(str);
		CurrentCharIndex++)
	{
		if (!PX_charIsNumeric(str[CurrentCharIndex]))
		{
			if (str[CurrentCharIndex]=='e'&&(str[CurrentCharIndex+1]=='+'||str[CurrentCharIndex+1]=='-'))
			{
				CurrentCharIndex++;
				continue;
			}

			if (str[CurrentCharIndex]=='.')
			{
				if (Dot)
				{
					return PX_FALSE;
				}
				else
				{
					Dot=PX_TRUE;
					continue;
				}
			}
			return PX_FALSE;
		}
	}
	return Dot? PX_TRUE:PX_FALSE;
}

px_bool PX_charIsNumeric(px_char chr)
{
	if (chr>=('0')&&chr<=('9'))
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_strIsInt(const px_char *str)
{
	if (PX_strIsNumeric(str))
	{
		if (PX_strIsFloat(str))
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	return PX_FALSE;
}

static px_double __px_pow_i(px_double num,px_int n)
{
	px_double powint=1;
	px_int i;
	for(i=1;i<=n;i++) 
	{
		powint*=num;
		if (powint==0)
		{
			break; 
		}
		if (powint>0)
		{
			if (powint>=PX_DBL_POSITIVE_MAX||powint<=PX_DBL_POSITIVE_MIN)
			{
				PX_ASSERT();
				break;
			}
		}
		else
		{
			if (powint<=PX_DBL_NEGATIVE_MIN||powint>=PX_DBL_NEGATIVE_MAX)
			{
				PX_ASSERT();
				break;
			}
		}
	}
	return powint;
}

static px_double __px_pow_f(px_double num,px_double m)
{
	return PX_exp(m*PX_log(num));
}

px_double PX_pow(px_double num,px_double m)
{

	if(m-(px_int)(m)==0) 
		return __px_pow_i(num,(px_int)m);
	else 
		return __px_pow_dd(num,m);
	//return __px_pow_f(num,m);
}

px_int PX_pow_ii(px_int i,px_int n)
{
	px_int fin=1;
	if (n==0)
	{
		return 1;
	}
	if (n<0)
	{
		return 0;
	}
	while(n--)
		fin*=i;
	return fin;
}

static px_float __int_as_float(px_int32 a)
{
	px_float r; 
	PX_memcpy(&r, &a, sizeof (r)); 
	return r; 
}
static px_int32 __float_as_int(px_float a) 
{ 
	px_int32 r; 
	PX_memcpy(&r, &a, sizeof(px_int32));
	return r; 
}


px_double64 PX_ln(px_double x)
{
	px_double64 hfsq, f, s, z, R, w, t1, t2, dk;
	int k, hx, i, j;
	unsigned lx;
	static px_double64	ln2_hi = 6.93147180369123816490e-01,	/* 3fe62e42 fee00000 */
		ln2_lo = 1.90821492927058770002e-10,	/* 3dea39ef 35793c76 */
		two54 = 1.80143985094819840000e+16,  /* 43500000 00000000 */
		Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
		Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
		Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
		Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
		Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
		Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
		Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

	static px_double64 zero = 0.0;

	hx = *(1 + (px_int32*)&x);
	lx = *(px_int32*)&x;

	k = 0;
	if (hx < 0x00100000) {			/* x < 2**-1022  */
		if (((hx & 0x7fffffff) | lx) == 0)
			return -two54 / zero;		/* log(+-0)=-inf */
		if (hx < 0) return (x - x) / zero;	/* log(-#) = NaN */
		k -= 54; x *= two54; /* subnormal number, scale up x */
		hx = *(1 + (px_int32*)&x);		/* high word of x */
	}
	if (hx >= 0x7ff00000) return x + x;
	k += (hx >> 20) - 1023;
	hx &= 0x000fffff;
	i = (hx + 0x95f64) & 0x100000;
	*(1 + (px_int32*)&x) = hx | (i ^ 0x3ff00000);	/* normalize x or x/2 */
	k += (i >> 20);
	f = x - 1.0;
	if ((0x000fffff & (2 + hx)) < 3) {	/* |f| < 2**-20 */
		if (f == zero)
		{
			if (k == 0)
			{
				return zero;
			}
			else
			{
				dk = (px_double64)k;
				return dk * ln2_hi + dk * ln2_lo;
			}
		}
		R = f * f * (0.5 - 0.33333333333333333 * f);
		if (k == 0) return f - R; else {
			dk = (px_double64)k;
			return dk * ln2_hi - ((R - dk * ln2_lo) - f);
		}
	}
	s = f / (2.0 + f);
	dk = (px_double64)k;
	z = s * s;
	i = hx - 0x6147a;
	w = z * z;
	j = 0x6b851 - hx;
	t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
	t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
	i |= j;
	R = t2 + t1;
	if (i > 0) {
		hfsq = 0.5 * f * f;
		if (k == 0) return f - (hfsq - s * (hfsq + R)); else
			return dk * ln2_hi - ((hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
	}
	else {
		if (k == 0) return f - s * (f - R); else
			return dk * ln2_hi - ((s * (f - R) - dk * ln2_lo) - f);
	}
}

px_double PX_log(px_double __x)
{
	return PX_ln(__x);
}

px_double PX_lg(px_double __x)
{
	return PX_log10(__x);
}

px_double PX_log10(px_double __x)
{
	return PX_ln(__x)/2.30258509299404568401799145468;
}



px_double PX_tand(px_double radian)
{
	return PX_tan_radian((px_float)radian);
}

static px_uint64 px_srand_seed=0x31415926;

px_void PX_srand(px_uint64 seed)
{
	  seed = (seed*16807)%(0xefffffff);
	  px_srand_seed=seed;
}

px_uint32 PX_rand()
{
	return  ((px_uint32)(px_srand_seed = (px_srand_seed*764261123)%(0xefffffff)))&PX_RAND_MAX;
}


px_double PX_randRange(px_double min,px_double max)
{
	return min+PX_rand()*1.0/PX_RAND_MAX*(max-min);
}

px_uint32 PX_randEx(px_uint64 seed)
{
	return  (px_uint32)(seed = (seed*764261123)%(0xefffffff));
}


px_double PX_GaussRand()
{
	px_double u ;
	px_double v;
	px_double r;
	px_double c;
	while (PX_TRUE)
	{
		u = ((px_double) PX_rand() / (PX_RAND_MAX)) * 2 - 1;
		v = ((px_double) PX_rand() / (PX_RAND_MAX)) * 2 - 1;
		r = u * u + v * v;
		if (r == 0 || r > 1)  continue;
		c = (px_double)PX_sqrt((px_float)(-2 * PX_ln(r) / r));
		return u * c;
	}
}

px_double PX_Ceil(px_double v)
{
	return	v-(px_int)v?(px_double)((px_int)(v+1)):v;
}

px_void PX_FileGetName(const px_char filefullName[],px_char _out[],px_int outSize)
{
	px_int s;
	if (outSize==0)
	{
		return;
	}
	_out[0]=0;
	s=PX_strlen(filefullName);
	if (s==0)
	{
		return;
	}
	s--;
	while (s)
	{
		if (filefullName[s]=='/'||filefullName[s]=='\\')
		{
			s++;
			break;
		}
		s--;
	}

	while (outSize>1&&filefullName[s]&&filefullName[s]!='.')
	{
		outSize--;
		*_out=filefullName[s++];
		_out[1]='\0';
		_out++;
	}

}
px_void PX_FileGetPath(const px_char filefullName[],px_char _out[],px_int outSize)
{
	px_int s,i;
	if (outSize==0)
	{
		return;
	}
	_out[0]=0;
	s=PX_strlen(filefullName);
	if (s==0)
	{
		return;
	}
	s--;
	while (s)
	{
		if (filefullName[s]=='/'||filefullName[s]=='\\')
		{
			break;
		}
		s--;
	}

	for (i=0;i<s;i++)
	{
		_out[i]=filefullName[i];
		if (i>=outSize-1)
		{
			_out[0]=0;
			return;
		}
	}
	if (i>0&&_out[i-1]==':')
	{
		_out[i]='/';
		i++;
	}
	_out[i]=0;

}

px_void PX_FileGetExt(const px_char filefullName[],px_char _out[],px_int outSize)
{
	px_int s;
	px_bool bDot=PX_FALSE;
	if (outSize==0)
	{
		return;
	}
	_out[0]=0;
	s=PX_strlen(filefullName);
	if (s==0)
	{
		return;
	}
	s--;
	while (s)
	{
		if (filefullName[s]=='.')
		{
			s++;
			bDot=PX_TRUE;
			break;
		}
		s--;
	}

	while (outSize>1&&filefullName[s])
	{
		outSize--;
		*_out=filefullName[s++];
		_out[1]='\0';
		_out++;
	}
}

static const px_uint32 crc32tab[] = {  
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,  
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,  
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,  
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,  
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,  
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,  
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,  
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,  
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,  
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,  
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,  
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,  
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,  
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,  
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,  
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,  
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,  
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,  
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,  
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,  
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,  
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,  
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,  
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,  
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,  
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,  
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,  
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,  
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,  
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,  
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,  
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,  
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,  
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,  
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,  
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,  
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,  
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,  
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,  
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,  
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,  
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,  
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,  
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,  
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,  
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,  
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,  
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,  
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,  
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,  
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,  
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,  
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,  
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,  
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,  
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,  
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,  
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,  
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,  
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,  
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,  
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,  
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,  
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d  
};  


px_uint32 PX_crc32( px_void *buffer, px_uint size)  
{  
	px_uint32 i, crc;  
	crc = 0xFFFFFFFF;  


	for (i = 0; i < size; i++)  
		crc = crc32tab[(crc ^ ((px_byte *)buffer)[i]) & 0xff] ^ (crc >> 8);  

	return crc^0xFFFFFFFF;  
}  




px_word PX_crc16(px_void *buffer,px_uint size)
{
	px_uint pos,i;
	px_word crc = 0xFFFF;
	px_byte *p_data=(px_byte *)buffer;
	for (pos = 0; pos < size; pos++)
	{
		crc ^= (px_word)p_data[pos]; // XOR byte into least sig. byte of crc
		for (i = 8; i != 0; i--)   // Loop over each bit
		{
			if ((crc & 0x0001) != 0)   // If the LSB is set
			{
				crc >>= 1; // Shift right and XOR 0xA001
				crc ^= 0xA001;
			}
			else // Else LSB is not set
			{
				crc >>= 1;    // Just shift right
			}
		}
	}

	crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
	return crc;
	
}

px_char *PX_strchr(const px_char *s,px_int ch)
{

	while(*s!='\0')
	{
		if(*s-ch==0)
			return (px_char*)s;
		s++;
	}
	return PX_NULL;
}

px_int PX_strcmp(const px_char *str1, const px_char *str2)
{
	px_int ret=0;
	while( !(ret = *(px_uchar*)str1 - *(px_uchar*)str2 ) && *str1 )
	{
		str1++;
		str2++;
	}
	return ret;   
}

px_char* PX_strstr(const px_char* dest, const px_char* src)
{
	px_char* start = (px_char*)dest;
	px_char* substart = (px_char*)src;
	px_char* cp = (px_char*)dest;
	while (*cp)
	{
		start = cp;
		while (*start != '\0' && *substart !='\0' && *start == *substart)
		{
			start++;
			substart++;
		}
		if (*substart == '\0')
		{
			return cp;
		}
		substart = (px_char*)src;
		cp++;
	}
	return PX_NULL;
}



px_bool PX_isPointInCircle(px_point p,px_point circle,px_float radius)
{
	if ((p.x-circle.x)*(p.x-circle.x)+(p.y-circle.y)*(p.y-circle.y)<=radius*radius)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}


px_bool PX_isPoint2DInCircle(px_point2D p,px_point2D circle,px_float radius)
{
	if ((p.x-circle.x)*(p.x-circle.x)+(p.y-circle.y)*(p.y-circle.y)<=radius*radius)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_dword  PX_inet_addr( const px_char cp[] )
{
	px_uchar ipBytes[4]={0};
	px_int i;
	for( i=0; i<4; i++, cp++ ){
		ipBytes[i] = (px_uchar)PX_atoi( cp );
		if( !(cp = PX_strchr( cp, '.' )) ){ break; }
	}
	return *(px_dword*)ipBytes;
}

px_dword  PX_inet_port(const px_char cp[])
{
	const px_char *p=cp;
	while (*p)
	{
		if (*p==':')
		{
			p++;
			break;
		}
		p++;
	}
	return (px_dword)PX_atoi(p);
}



PX_RETURN_STRING PX_inet_ntoa(px_dword ipv4)
{
	px_int b[4];
	PX_RETURN_STRING ret = {0};
	px_char *a=ret.data;
	a[0]='\0';
	b[0]=((ipv4 & 0xff000000)>>24);
	b[1]=((ipv4 & 0x00ff0000)>>16);
	b[2]=((ipv4 & 0x0000ff00)>>8);
	b[3]=((ipv4 & 0x000000ff)>>0);

	PX_itoa(b[3],a+PX_strlen(a),5,10);
	PX_strcat(a,".");
	PX_itoa(b[2],a+PX_strlen(a),5,10);
	PX_strcat(a,".");
	PX_itoa(b[1],a+PX_strlen(a),5,10);
	PX_strcat(a,".");
	PX_itoa(b[0],a+PX_strlen(a),5,10);
	
	return ret;
}

px_bool PX_IsValidIPAddress(const px_char *ip_addr)
{
	// Only supports Ipv4
	px_int len;
	px_int position = 0; // Current parser position
	px_int bitsNumber = 0; // how many bits in ip_addr,The valid ip address should be 4 bits
	px_int bitsLen; // The length of a single bit
	px_int bits;// The value of a single bit
	px_int n, i;

	len = PX_strlen(ip_addr);
	if (len < 7 || len > 15) return PX_FALSE; // The valid IP address length should be 7 to 15

	for (i = 0; i < len; i++) 
	{
		if (ip_addr[i] == '.' || i == len - 1) 
		{
			bitsLen = i - position;
			if (i == len - 1) bitsLen++; // at the end, Need to add an extra length 1
			if (bitsLen > 3 || bitsLen == 0) return PX_FALSE;
			if (bitsLen != 1 && ip_addr[position] == '0') return PX_FALSE;

			for (n = 0, bits = 0; n < bitsLen; n++, position++) bits = 10 * bits + (ip_addr[position] - '0');
			if (bits > 255) return PX_FALSE;
			position++;
			bitsNumber++;
		} 
		else if (ip_addr[i] >= '0' && ip_addr[i] <= '9') continue;
		else return PX_FALSE;
	}
	if (bitsNumber != 4) return PX_FALSE;
	return PX_TRUE;
}

px_dword PX_htonl(px_dword h)
{
	return PX_isBigEndianCPU() ? h : BigLittleSwap32(h);
}


px_dword PX_ntohl(px_dword n)
{
	return PX_isBigEndianCPU() ? n : BigLittleSwap32(n);
}

px_word  PX_htons(px_word h)
{
	return PX_isBigEndianCPU() ? h : BigLittleSwap16(h);
}

px_word PX_ntohs(px_word n)
{
	return PX_isBigEndianCPU() ? n : BigLittleSwap16(n);
}

px_uint32 PX_sum32(px_void *buffer, px_uint size)
{
	px_uint32 _sum32=0;
	px_uint i;
	px_byte *pbuffer=(px_byte *)buffer;
	for (i=0;i<size;i++)
	{
		_sum32+=pbuffer[i];
	}
	return _sum32;
}



px_point PX_PointRotate(px_point p,px_float angle)
{
	px_matrix mat;
	PX_MatrixRotateZ(&mat,angle);
	return PX_PointMulMatrix(p,mat);
}

px_point2D PX_Point2DRotate(px_point2D p,px_float angle)
{
	px_matrix mat;
	PX_MatrixRotateZ(&mat,angle);
	return PX_Point2DMulMatrix(p,mat);
}

px_float PX_PointDistance(px_point p1,px_point p2)
{
	return PX_sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}


px_void  PX_WindowFunction_triangular(px_double data[],px_int N)
{
	px_int i;
	if((N%2)==1)
	{
		for(i=0;i<((N-1)/2);i++)
		{
			data[i]=2*(px_double)(i+1)/(N+1);
		}
		for(i=((N-1)/2);i<N;i++)
		{
			data[i]=2*(px_double)(N-i)/(N+1);
		}
	}

	else
	{
		for(i=0;i<(N/2);i++)
		{
			data[i]=(i+i+1)*(px_double)1/N;
		}

		for(i=(N/2);i<N;i++)
		{
			data[i]=data[N-1-i];
		}
	}
}

px_void PX_WindowFunction_Apply(px_double data[],px_double window[],px_int N)
{
	px_int i;
	for (i=0;i<N;i++)
	{
		data[i]*=window[i];
	}
}

px_void PX_gain(px_double b[],px_double a[],px_int m,px_int n,px_double x[],px_double y[],px_int len,px_int sign) 
{
	px_int i,k;
	px_double ar,ai,br,bi,zr,zi,im,re,den,numr,numi,freq,temp;
	for (k=0;k<len;k++)
	{
		freq=k*0.5/(len-1);
		zr=PX_cosd((-8.0*PX_atan(1.0)*freq));
		zi=PX_sind((-8.0*PX_atan(1.0)*freq));
		br=0;
		bi=0;
		for (i=m;i>0;i--)
		{
			re=br;
			im=bi;
			br=(re+b[i])*zr-im*zi;
			bi=(re+b[i])*zi+im*zr;
		}
		ar=0.0;
		ai=0;
		for(i=n;i>0;i--)
		{
			re=ar;
			im=ai;
			ar=(re+a[i])*zr-im*zi;
			ai=(re+a[i])*zi+im*zr;
		}
		br=br+b[0];
		ar=ar+1.0;
		numr=ar*br+ai*bi;
		numi=ar*bi-ai*br;
		den=ar*ar+ai*ai;
		x[k]=numr/den;
		y[k]=numi/den;
		switch(sign)
		{
		case 1:
			{
				temp=PX_sqrtd(x[k]*x[k]+y[k]*y[k]);
				y[k]=PX_atan2(y[k],x[k]);
				x[k]=temp;
			}
			break;
		case 2:
			{
				temp=(x[k]*x[k]+y[k]*y[k]);
				y[k]=PX_atan2(y[k],x[k]);
				x[k]=10*PX_log10(temp);
			}
			break;
		}

	}
}

px_void PX_gainc(px_double b[],px_double a[],px_int n,px_int ns,px_double x[],px_double y[],px_int len,px_int sign)
{
	px_int i,j,k,nl;
	px_double ar,ai,br,bi,zr,zi,im,re,den,numr,numi,freq,temp;
	px_double hr,hi,tr,ti;
	nl=n+1;
	for (k=0;k<len;k++)
	{
		freq=k*0.5/(len-1);
		zr=PX_cosd((-8.0*PX_atan(1.0)*freq));
		zi=PX_sind((-8.0*PX_atan(1.0)*freq));
		x[k]=1.0;
		y[k]=0.0;
		for (j=0;j<ns;j++)
		{
			br=0;
			bi=0;
			for (i=n;i>0;i--)
			{
				re=br;
				im=bi;
				br=(re+b[j*nl+i])*zr-im*zi;
				bi=(re+b[j*nl+i])*zi+im*zr;
			}
			ar=0;
			ai=0;
			for (i=n;i>0;i--)
			{
				re=ar;
				im=ai;
				ar=(re+a[j*nl+i])*zr-im*zi;
				ai=(re+b[j*nl+i])*zi+im*zr;
			}
			br=br+b[j*nl+0];
			ar=ar+1.0;
			numr=ar*br+ai*bi;
			numi=ar*bi-ai*br;
			den=ar*ar+ai*ai;
			hr=numr/den;
			hi=numi/den;
			tr=x[k]*hr-y[k]*hi;
			ti=x[k]*hi+y[k]*hr;
			x[k]=tr;
			y[k]=ti;
		}
		switch(sign)
		{
		case 1:
			{
				temp=PX_sqrtd(x[k]*x[k]+y[k]*y[k]);
				if (temp!=0)
				{
					y[k]=PX_atan2(y[k],x[k]);
				}
				else
				{
					y[k]=0;
				}
				x[k]=temp;
			}
			break;
		case 2:
			{
				temp=(x[k]*x[k]+y[k]*y[k]);
				if (temp!=0)
				{
					y[k]=PX_atan2(y[k],x[k]);
				}
				else
				{
					temp=1.0e-40;
					y[k]=0;
				}
				x[k]=10*PX_log10(temp);
			}
			break;
		}
	}
}



px_void  PX_WindowFunction_blackMan(px_double data[],px_int N)
{
	px_int n;
	for(n=0;n<N;n++)
	{
		data[n]=0.42-0.5*PX_cosd((px_float)(2*PX_PI*(px_double)n/(N-1))+0.08f*PX_cos_radian((px_float)(4*PX_PI*(px_double)n/(N-1))));
	}
}

px_void  PX_WindowFunction_hamming(px_double data[],px_int N)
{
	px_int n;
	for(n=0;n<N;n++)
	{
		data[n]=0.54-0.46*PX_cosd((px_float)(2*PX_PI*(px_double)n/(N-1)));
	}
}

px_void PX_WindowFunction_sinc(px_double data[],px_int N)
{
	px_int n;
	for(n=0;n<N;n++)
	{
		data[N-n-1]=(data[n]=PX_sind(PX_PI*(((px_double)n)+0.5)/((px_double)N)));
	}
}

px_double PX_Bessel(px_int n,px_double x)
{
	px_int i,m;
	px_double t,y,p=0,b0,b1,q;
	static px_double a[7]={ 1.0,3.5156229,3.0899424,1.2067492,	0.2659732,0.0360768,0.0045813};
	static px_double b[7]={ 0.5,0.87890594,0.51498869,	0.15084934,0.02658773,0.00301532,0.00032411};
	static px_double c[9]={ 0.39894228,0.01328592,0.00225319,-0.00157565,0.00916281,-0.02057706,0.02635537,-0.01647633,0.00392377};	
	static px_double d[9]={ 0.39894228,-0.03988024,-0.00362018,0.00163801,-0.01031555,0.02282967,-0.02895312,0.01787654,-0.00420059};
	if (n<0) n=-n;
	t=PX_ABS(x);
	if (n!=1)
	{ 
		if (t<3.75)
		{ 
			y=(x/3.75)*(x/3.75); p=a[6];
			for (i=5; i>=0; i--)
				p=p*y+a[i];
		}
		else
		{ 
			y=3.75/t; p=c[8];
			for (i=7; i>=0; i--)
				p=p*y+c[i];
			p=p*PX_exp(t)/PX_sqrtd(t);
		}
	}
	if (n==0) return(p);
	q=p;
	if (t<3.75)
	{ 
		y=(x/3.75)*(x/3.75); p=b[6];
		for (i=5; i>=0; i--) p=p*y+b[i];
		p=p*t;
	}
	else
	{ 
		y=3.75/t; p=d[8];
		for (i=7; i>=0; i--) p=p*y+d[i];
		p=p*PX_exp(t)/PX_sqrtd(t);
	}
	if (x<0.0) p=-p;
	if (n==1) return(p);
	if (x==0.0) return(0.0);
	y=2.0/t; t=0.0; b1=1.0; b0=0.0;
	m=n+(px_int)PX_sqrtd(40.0*n);
	m=2*m;

	for (i=m; i>0; i--)
	{ 
		p=b0+i*y*b1; b0=b1; b1=p;
		if (PX_ABS(b1)>1.0e+10)
		{ 
			t=t*1.0e-10; b0=b0*1.0e-10;
			b1=b1*1.0e-10;
		}
		if (i==n) t=b0;
	}
	p=t*q/b1;
	if ((x<0.0)&&(n%2==1)) p=-p;
	return(p);
}

px_void PX_WindowFunction_tukey(px_double data[],px_int N)
{
	px_int n;
	if (N<16)
	{
		return;
	}
	for (n=0;n<=(N-2)/10;n++)
	{
		data[n]=0.5*(1-PX_cosd((px_float)(10*PX_PI*n/(N+8))));
	}
	for (n=(N-2)/10;n<=9*(N-2)/10;n++)
	{
		data[n]=1;
	}
	for (n=9*(N-2)/10;n<=N-1;n++)
	{
		data[n]=0.5*(1-PX_cosd((px_float)(10*PX_PI*(N-n-1)/(N+8))));;
	}
}

px_void PX_WindowFunction_hanning(px_double data[],px_int N)
{
	px_int n;
	for(n=0;n<N;n++)
	{
		data[n]=0.5-0.5*PX_cosd((px_float)(2*PX_PI*(px_double)n/(N-1)));
	}
}

px_void   PX_WindowFunction_kaiser(px_double beta,px_double data[],px_int N)
{
	px_int n;
	px_double theta;
	for(n = 0; n < N; n++)
	{
		theta = beta * PX_sqrtd( 1 - ( (2 * (px_double)n/(N -1)) - 1)*( (2 * (px_double)n/(N -1)) - 1) );
		data[n]= (px_double)PX_Bessel(0, theta ) / PX_Bessel(0,beta);
	}
}


static px_double PX_FIRKaiser(px_int i,px_int n,px_double beta)
{
	px_double a,w,a2,b1,b2,beta1;
	b1=PX_Bessel(0,beta);
	a=2.0*i/(px_double)(n-1.0)-1.0;
	a2=a*a;
	beta1=beta*PX_sqrtd(1.0-a2);
	b2=PX_Bessel(0,beta1);
	w=b2/b1;
	return w;
}

static px_double PX_FIRWindow(PX_FIRFILTER_WINDOW_TYPE type,px_int n,px_int i,px_double beta)
{
	px_int k;
	px_double pi,w;
	pi=4.0*PX_atan(1.0);
	w=1.0;
	switch(type)
	{
	case PX_FIRFILTER_WINDOW_TYPE_RECT:
		{
			w=1.0;
			break;
		}
	case PX_FIRFILTER_WINDOW_TYPE_TUKEY:
		{
			k=(n-2)/10;
			if (i<=k)
			{
				w=0.5*(1.0-PX_cos_radian((px_float)(i*pi/(k+1.0f))));
			}
			if (i>n-k-2)
			{
				w=0.5*((px_float)(1.0-PX_cos_radian((px_float)((n-i-1.0f)*pi/(k+1.0f)))));
			}
			break;
		}
	case PX_FIRFILTER_WINDOW_TYPE_TRIANGULAR:
		{
			w=1.0-PX_ABS(1.0-2*i/(n-1.0));
			break;
		}
	case PX_FIRFILTER_WINDOW_TYPE_HANNING:
		{
			w=0.5*(1.0-PX_cos_radian((px_float)(2.0f*i*pi/(n-1.0f))));
			break;
		}
	case PX_FIRFILTER_WINDOW_TYPE_HAMMING:
		{
			w=0.54-0.46*PX_cos_radian((px_float)(2.0f*i*pi/(n-1.0)));
			break;
		}
	case PX_FIRFILTER_WINDOW_TYPE_BLACKMAN:
		{
			w=0.42-0.5*PX_cos_radian((px_float)(2.0f*i*pi/(n-1.0f)))+0.08*PX_cos_radian((px_float)(4.0f*i*pi/(n-1.0f)));
			break;
		}
	case PX_FIRFILTER_WINDOW_TYPE_KAISER:
		{
			w=PX_FIRKaiser(i,n,beta);
			break;
		}
	}
	return w;
}

px_sine PX_SINE(px_double A,px_double P,px_double F)
{
	px_sine s;
	s.A=A;
	s.p=P;
	s.f=F;
	return s;
}

px_sine PX_InstantaneousFrequency(px_sine src,px_double p2,px_double delta_t)
{
	px_double delta=(p2-src.p);
	delta=delta-(px_int)delta-1/2;
	delta/=delta_t;
	src.f+=delta;
	return src;
}


px_void PX_FIRFilterBuild(PX_FIRFILTER_TYPE bandtype,px_double fln,px_double fhn,PX_FIRFILTER_WINDOW_TYPE wn,px_double h[],px_int n,px_double beta)
{
	px_int i,n2,mid;
	px_double s,pi,wc1,wc2=0,delay;
	
	pi=4.0*PX_atan(1.0);
	if ((n%2)==0)
	{
		n2=n/2-1;
		mid=1;
	}
	else
	{
		n2=n/2;
		mid=0;
	}
	delay=n/2.0;
	wc1=2.0*pi*fln;
	if (bandtype>=3)
	{
		wc2=2.0*pi*fhn;
	}
	switch(bandtype)
	{
	case PX_FIRFILTER_TYPE_LOWPASS:
		{
			for (i=0;i<=n2;i++)
			{
				s=i-delay;
				h[i]=(PX_sind((wc1*s))/(pi*s))*PX_FIRWindow(wn,n+1,i,beta);
				h[n-i]=h[i];
			}
			if (mid==1)
			{
				h[n/2]=wc1/pi;
			}
		}
		break;
	case PX_FIRFILTER_TYPE_HIGHPASS:
		{
			for (i=0;i<=n2;i++)
			{
				s=i-delay;
				h[i]=(PX_sind((pi*s))-PX_sind((wc1*s)))/(pi*s);
				h[i]=h[i]*PX_FIRWindow(wn,n+1,i,beta);
				h[n-i]=h[i];
			}
			if (mid==1)
			{
				h[n/2]=1.0-wc1/pi;
			}
		}
		break;
	case PX_FIRFILTER_TYPE_BANDPASS:
		{
			for (i=0;i<=n2;i++)
			{
				s=i-delay;
				h[i]=(PX_sind((wc2*s))-PX_sind((wc1*s)))/(pi*s);
				h[i]=h[i]*PX_FIRWindow(wn,n+1,i,beta);
				h[n-i]=h[i];
			}
			if (mid==1)
			{
				h[n/2]=(wc2-wc1)/pi;
			}
		}
		break;
	case PX_FIRFILTER_TYPE_STOPBANDFILTER:
		{
			for (i=0;i<=n2;i++)
			{
				s=i-delay;
				h[i]=(PX_sind((wc1*s))-PX_sind((wc2*s)))/(pi*s);
				h[i]=h[i]*PX_FIRWindow(wn,n+1,i,beta);
				h[n-i]=h[i];
			}
			if (mid==1)
			{
				h[n/2]=(wc1+pi-wc2)/pi;
			}
		}
		break;
	}
}


px_float PX_GroupDelay(px_float f, px_float* B, px_int sizeB, px_float* A, px_int sizeA, px_float FS) 
{
	px_int i;
	px_float omega;
	px_float Br[2] = { 0 }; 
	px_float Be[2] = { 0 }; 
	px_float Ar[2] = { 0 }; 
	px_float Ae[2] = { 1,0 };
	px_float BrxAe[2] = { 0 };
	px_float BexAr[2] = { 0 }; 
	px_float AxB[2] = { 0 }; 
	px_float Num[2] = { 0 };
	px_float c[2] = { 0 }; 
	px_float magd2;

	omega = (px_float)(2.0f * PX_PI * f / FS);

	for (i = 1; i < sizeB; i++) 
	{
		Br[0] += i * (px_float)PX_cosd(i *1.0* omega) * B[i];
		Br[1] -= i * (px_float)PX_sind(i *1.0* omega) * B[i];
	}

	for (i = 0; i < sizeB; i++) 
	{
		Be[0] += (px_float)PX_cosd(i *1.0* omega) * B[i];
		Be[1] -= (px_float)PX_sind(i *1.0* omega) * B[i];
	}

	for (i = 0; i < sizeA; i++) {
		Ar[0] += (i + 1) * (px_float)PX_cosd((i + 1.0) * omega) * A[i];
		Ar[1] -= (i + 1) * (px_float)PX_sind((i + 1.0) * omega) * A[i];
	}

	for (i = 0; i < sizeA; i++) {
		Ae[0] += (px_float)PX_cosd((i + 1.0) * omega) * A[i];
		Ae[1] -= (px_float)PX_sind((i + 1.0) * omega) * A[i];
	}

	BrxAe[0] = Br[0] * Ae[0] - Br[1] * Ae[1];
	BrxAe[1] = Br[0] * Ae[1] + Br[1] * Ae[0];

	BexAr[0] = Be[0] * Ar[0] - Be[1] * Ar[1];
	BexAr[1] = Be[0] * Ar[1] + Be[1] * Ar[0];

	AxB[0] = Ae[0] * Be[0] - Ae[1] * Be[1];
	AxB[1] = Ae[0] * Be[1] + Ae[1] * Be[0];

	Num[0] = BrxAe[0] - BexAr[0]; 
	Num[1] = BrxAe[1] - BexAr[1];

	magd2 = AxB[0] * AxB[0] + AxB[1] * AxB[1];
	c[0] = (Num[0] * AxB[0] + Num[1] * AxB[1]) / magd2;
	c[1] = (Num[1] * AxB[0] - Num[0] * AxB[1]) / magd2;
	
	return c[0];
}


static px_float PX_PhaseIIR(px_float omega, px_float* B, px_int sizeB, px_float* A, px_int sizeA) {
	px_float C[2];
	px_float HB[2] = { 0 };
	px_float HA[2] = { 0 };
	px_float magd2;
	px_int i;
	for (i = 0; i < sizeB; i++) {
		HB[0] += (px_float)PX_cosd((px_float)i * omega) * B[i];
		HB[1] -= (px_float)PX_sind((px_float)i * omega) * B[i];
	}

	for (i = 0; i < sizeA; i++) {
		HA[0] += (px_float)PX_cosd((i + 1.0) * omega) * A[i];
		HA[1] -= (px_float)PX_sind((i + 1.0) * omega) * A[i];
	}
	magd2 = HA[0] * HA[0] + HA[1] * HA[1];

	C[0] = (HB[0] * HA[0] + HB[1] * HA[1]) / magd2;
	C[1] = (HB[1] * HA[0] - HB[0] * HA[1]) / magd2;

	return (px_float)PX_atan2(C[1], C[0]);
}
px_float PX_PhaseDelayDerive(px_float omega, px_float* B, px_int sizeB, px_float* A, px_int sizeA, px_float delta) {
	px_float omega1 = omega - delta;
	px_float omega2 = omega + delta;
	px_float p1 = PX_PhaseIIR(omega1, B, sizeB, A, sizeA);
	px_float p2 = PX_PhaseIIR(omega2, B, sizeB, A, sizeA);
	return (-omega1 * p2 + omega2 * p1) / (2 * delta * omega1 * omega2);
}
px_float PX_PhaseDelay(px_float f, px_float* B, px_int sizeB, px_float* A, px_int sizeA, px_float FS) {
	px_float grpdel = PX_GroupDelay(f, B, sizeB, A, sizeA, FS);
	px_float omega = (px_float)(2.0 * PX_PI * f / FS);
	return grpdel - omega * PX_PhaseDelayDerive(omega, B, sizeB, A, sizeA, 0.0005f);
}