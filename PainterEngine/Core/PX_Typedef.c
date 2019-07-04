#include "PX_Typedef.h"

px_uint PX_htoi(px_char hex_str[])   
{  
	px_char ch;   
	px_uint iret=0;  
	while(ch=*hex_str++)   
	{   
		iret=(iret<<4)|px_hex_to_dex_table[ch];  
	}   
	return iret;   
} 


px_int PX_atoi(px_char s[])
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


px_float PX_atof(px_char fstr[])
{
	px_float temp=10;
	px_bool ispnum=PX_TRUE;
	px_float ans=0;
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
		ans=ans*10+(*fstr-'0');
		fstr++;
	}
	while(*fstr!='\0')
	{
		ans=ans+(*fstr-'0')/temp;
		temp*=10;
		fstr++;
	}
	if(ispnum) return ans;
	else return ans*(-1);
}


#define PUSH_CHAR(x) do { if (ret < maxlen) { *p++ = x; ret++; } } while (0)  

typedef union {  
	px_int L;  
	px_float F;  
} __LF_t;  

px_int  PX_ftoa(px_float f, char *outbuf, px_int maxlen, px_int precision)   
{  
	px_int mantissa, int_part, int_part2, frac_part;  
	px_ushort exp2;  
	px_int sign, i, m, max;  
	__LF_t x;  
	char c, *p = outbuf;  
	px_int ret = 0;  

	if(f<1&&f>0)
	x.F = f+1;  
	else if(f>-1&&f<0)
	x.F=f-1;
	else
	x.F = f;

	//exp2 = (unsigned char)(x.L >> 23);  
	exp2 = (((((px_int)1) << 8) - 1) & (x.L >> 23)); /* JEB fixed for 16-bit char F2xxx */   
	mantissa = (x.L & ((((px_int)1) << 23) - 1));  
	//printf("%x\n", x.L);  
	if (x.L >= 0) sign = +1; else sign = -1;  
	if (exp2 == ((((px_int)1) << 8) - 1)) {  
		if (mantissa == 0) {  
			if (sign == -1)  
				PUSH_CHAR('-');  
			PUSH_CHAR('i');  
			PUSH_CHAR('n');  
			PUSH_CHAR('f');  
			return ret;  
		} else {  
			PUSH_CHAR('n');  
			PUSH_CHAR('a');  
			PUSH_CHAR('n');  
			return ret;  
		}  
	}  

	exp2 -= ((((px_int)1) << 8) >> 1) - 1; //127;  
	mantissa |= (((px_int)1) << 23);  
	frac_part = 0;  
	int_part = 0;  

	if (exp2 >= 23)  
		int_part = mantissa << (exp2 - 23);  
	else if (exp2 >= 0) {  
		int_part = mantissa >> (23 - exp2);  
		frac_part = (mantissa << (exp2 + 1)) & ((((px_int)1) << (23 + 1)) - 1); //0xFFFFFF;  
	}  
	else /* if (exp2 < 0) */  
		frac_part = (mantissa & ((((px_int)1) << (23 + 1)) - 1)) >> -(exp2 + 1);  

	if (int_part == 0) {  
		if (sign == -1)  
			PUSH_CHAR('-');  
		PUSH_CHAR('0');  
	} else {  
		while (int_part != 0) {  
			int_part2 = int_part / 10;  
			PUSH_CHAR((char)(int_part - ((int_part2 << 3) + (int_part2 << 1)) + '0'));  
			int_part = int_part2;  
		}  
		if (sign == -1)  
			PUSH_CHAR('-');  
		// Reverse string  
		for (i = 0; i < ret / 2; i++) {  
			c = outbuf[i];  
			outbuf[i] = outbuf[ret - i - 1];  
			outbuf[ret - i - 1] = c;  
		}  
	}  
	if (precision != 0)  
		PUSH_CHAR('.');  

	max = maxlen - (px_int)(p - outbuf);  
	if (max > precision)  
		max = precision;  
	/* print BCD */  
	for (m = 0; m < max; m++) {  
		/* frac_part *= 10; */  
		frac_part = (frac_part << 3) + (frac_part << 1);  

		PUSH_CHAR((char)((frac_part >> (23 + 1)) + '0'));  
		frac_part &= ((((px_int)1) << (23 + 1)) - 1); //0xFFFFFF;  
	}  
	/* delete ending zeroes */  
	//for (--p; p[0] == '0' && p[-1] != '.'; --p, --ret) ;  

	{  
		px_int digit, roundup = 0;  
		// Rounding  
		// Look at the next digit  
		frac_part = (frac_part << 3) + (frac_part << 1);  
		digit = (frac_part >> (23 + 1));  
		if (digit > 5) roundup = 1;  
		else if (digit == 5) {  
			frac_part &= ((((px_int)1) << (23 + 1)) - 1); //0xFFFFFF;  
			if (frac_part != 0) roundup = 1;  
		}  
		if (roundup) {  
			char d;  
			px_int pos = ret - 1;  
			do {  
				d = outbuf[pos]; // last digit  
				if (d == '-') break;  
				if (d == '.') { pos--; continue; }  
				d++; outbuf[pos] = d;  
				if (d != ':') break;  
				outbuf[pos] = '0';  
				pos--;  
			} while (pos);  
		}  
	}  
	outbuf[ret]='\0';
	if (f<1&&f>0)
	{
		outbuf[0]='0';
	}

	if (f>-1&&f<0)
	{
		outbuf[1]='0';
	}

	while (PX_TRUE)
	{
		if (px_strlen(outbuf)>2&&outbuf[px_strlen(outbuf)-1]=='0'&&outbuf[px_strlen(outbuf)-2]!='.')
		{
			outbuf[px_strlen(outbuf)-1]='\0';
			ret--;
		}
		else{break;}
	}

	return ret;  
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

float PX_sqrt( float number )  
{  
	px_int32 i;  
	float x2, y;  
	const float threehalfs = 1.5F;  
	x2 = number * 0.5F;  
	y  = number;  
	i  = * ( px_int32 * ) &y;       
	i  = 0x5f375a86 - ( i >> 1 );   //ILP32 only
	y  = * ( float * ) &i;  
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );     
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	return number*y;  
} 

float PX_SqrtRec( float number )  
{  
	px_int i;  
	float x2, y;  
	const float threehalfs = 1.5F;  

	x2 = number * 0.5F;  
	y  = number;  
	i  = * ( px_int * ) &y;       
	i  = 0x5f375a86 - ( i >> 1 );   
	y  = * ( float * ) &i;  
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	y  = y * ( threehalfs - ( x2 * y * y ) );     
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	return y;  
} 

px_double PX_exp(px_double x) {
	x = 1.0 + x / (65536);
	x *= x; x *= x; x *= x; x *= x;
	x *= x; x *= x; x *= x; x *= x;
	x *= x; x *= x; x *= x; x *= x;
	x *= x; x *= x; x *= x; x *= x;
	return x;
}

px_double PX_tanh(px_double x)
{
	px_double ex,eix;
	ex=PX_exp(x);
	eix=PX_exp(-x);
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

px_float PX_sin_radian(px_float radius)
{
	px_int radIndex;
	radIndex=PX_TRUNC(radius*1000)%6282;
	if (radIndex<0)
	{
		radIndex+=6282;
	}
	if (radIndex>3141)
	{
		radIndex-=3141;
		if(radIndex<1571)
		{
			return -px_sinx_radius[radIndex%1572];
		}
		else
		{
			return -px_sinx_radius[1570-(radIndex%1571)];
		}
	}

	if(radIndex<1571)
	{
		return px_sinx_radius[radIndex%1572];
	}
	else
	{
		return px_sinx_radius[1570-(radIndex%1571)];
	}
}
px_float PX_cos_radian(px_float radius)
{
	return PX_sin_radian(PX_PI/2-radius);
}
px_float PX_sin_angle(px_float angle)
{
	angle-=((px_int)angle/360)*360;
	return PX_sin_radian((angle*0.0174532925f));
}
px_float PX_cos_angle(px_float angle)
{
	angle-=((px_int)angle/360)*360;
	return PX_cos_radian((angle*0.0174532925f));
}


px_float PX_Point_sin(px_point v)
{
	return v.y/PX_sqrt(v.x*v.x+v.y*v.y);
}


px_float PX_Point_cos(px_point v)
{
	return v.x/PX_sqrt(v.x*v.x+v.y*v.y);
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

px_void PX_MatrixTranslation(px_matrix *mat,float x,float y,float z)
{
	mat->_11=1.0f;	mat->_12=0.0f;	mat->_13=0.0f;	mat->_14=0.0f;
	mat->_21=0.0f;	mat->_22=1.0f;	mat->_23=0.0f;	mat->_24=0.0f;
	mat->_31=0.0f;	mat->_32=0.0f;	mat->_33=1.0f;	mat->_34=0.0f;
	mat->_41=x;	mat->_42=y;	mat->_43=z;	mat->_44=1.0f;
}

px_void PX_MatrixRotateX(px_matrix *mat,float Angle)
{	
	mat->_11=1.0f;						mat->_12=0;								mat->_13=0.0f;								mat->_14=0.0f;
	mat->_21=0.0f;						mat->_22=PX_cos_angle(Angle);			mat->_23=PX_sin_angle(Angle);				mat->_24=0.0f;
	mat->_31=0.0f;						mat->_32=-PX_sin_angle(Angle);			mat->_33=PX_cos_angle(Angle);				mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;								mat->_44=1.0f;
}

px_void PX_MatrixRotateY(px_matrix *mat,float Angle)
{
	mat->_11=PX_cos_angle(Angle);		mat->_12=0.0f;							mat->_13=PX_sin_angle(Angle);			mat->_14=0.0f;
	mat->_21=0.0f;						mat->_22=1.0f;							mat->_23=0.0f;							mat->_24=0.0f;
	mat->_31=-PX_sin_angle(Angle);		mat->_32=0.0f;							mat->_33=PX_cos_angle(Angle);			mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;							mat->_44=1.0f;
}

px_void PX_MatrixRotateZ(px_matrix *mat,float Angle)
{
	
	mat->_11=PX_cos_angle(Angle);		mat->_12=PX_sin_angle(Angle);			mat->_13=0.0f;			mat->_14=0.0f;
	mat->_21=-PX_sin_angle(Angle);		mat->_22=PX_cos_angle(Angle);			mat->_23=0.0f;			mat->_24=0.0f;
	mat->_31=0.0f;						mat->_32=0.0f;							mat->_33=1.0f;			mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;			mat->_44=1.0f;
}


px_void PX_MatrixRotateXRadian(px_matrix *mat,float rad)
{	
	mat->_11=1.0f;						mat->_12=0;								mat->_13=0.0f;								mat->_14=0.0f;
	mat->_21=0.0f;						mat->_22=PX_cos_radian(rad);			mat->_23=PX_sin_radian(rad);				mat->_24=0.0f;
	mat->_31=0.0f;						mat->_32=-PX_sin_radian(rad);			mat->_33=PX_cos_radian(rad);				mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;								mat->_44=1.0f;
}

px_void PX_MatrixRotateYRadian(px_matrix *mat,float rad)
{
	mat->_11=PX_cos_radian(rad);		mat->_12=0.0f;							mat->_13=PX_sin_radian(rad);			mat->_14=0.0f;
	mat->_21=0.0f;						mat->_22=1.0f;							mat->_23=0.0f;							mat->_24=0.0f;
	mat->_31=-PX_sin_radian(rad);		mat->_32=0.0f;							mat->_33=PX_cos_radian(rad);			mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;							mat->_44=1.0f;
}

px_void PX_MatrixRotateZRadian(px_matrix *mat,float rad)
{

	mat->_11=PX_cos_radian(rad);		mat->_12=PX_sin_radian(rad);			mat->_13=0.0f;			mat->_14=0.0f;
	mat->_21=-PX_sin_radian(rad);		mat->_22=PX_cos_radian(rad);			mat->_23=0.0f;			mat->_24=0.0f;
	mat->_31=0.0f;						mat->_32=0.0f;							mat->_33=1.0f;			mat->_34=0.0f;
	mat->_41=0.0f;						mat->_42=0.0f;							mat->_43=0.0f;			mat->_44=1.0f;
}

px_void PX_MatrixScale(px_matrix *mat,float x,float y,float z)
{
	mat->_11=x;	mat->_12=0.0f;	mat->_13=0.0f;	mat->_14=0.0f;
	mat->_21=0.0f;	mat->_22=y;	mat->_23=0.0f;	mat->_24=0.0f;
	mat->_31=0.0f;	mat->_32=0.0f;	mat->_33=z;	mat->_34=0.0f;
	mat->_41=0.0f;	mat->_42=0.0f;	mat->_43=0.0f;	mat->_44=1.0f;
}

static float ptabs(float x){return x>0?x:-x;}
static px_bool Gauss(float A[][4], float B[][4])
{

	px_int i, j, k;
	float max, temp;
	float t[4][4];                
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
			B[i][j] = (i == j) ? (float)1 : 0;
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

px_point PX_PointSub(px_point p1,px_point p2)
{
	p1.x-=p2.x;
	p1.y-=p2.y;
	p1.z-=p2.z;
	return p1;
}

px_point PX_PointMul(px_point p1,px_float m)
{
	p1.x*=m;
	p1.y*=m;
	p1.z*=m;
	return p1;
}
px_point PX_PointDiv(px_point p1,px_float m)
{
	p1.x/=m;
	p1.y/=m;
	p1.z/=m;
	return p1;
}

px_float PX_PointDot(px_point p1,px_point p2)
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


px_point PX_PointInverse(px_point p1)
{
	return PX_POINT(-p1.x,-p1.y,-p1.z);
}

px_float PX_PointMod(px_point p)
{
	return PX_sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
}

px_float  PX_PointSquare(px_point p)
{
	return (p.x*p.x+p.y*p.y+p.z*p.z);
}

px_point PX_PointUnit(px_point p)
{
	if (p.x||p.y||p.z)
	{
		return PX_PointDiv(p,PX_PointMod(p));
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


void px_memset(void *dst,px_byte byte,px_uint size)
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


void px_memdwordset(void *dst,px_dword dw,px_uint count)
{
	px_dword *p=(px_dword *)dst;
	while(count--)*p++=dw;
}


px_bool px_memequ(void *dst,void *src,px_uint size)
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


px_void px_memcpy(px_void *dst,px_void *src,px_uint size)
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



	if ((px_uint)dst>(px_uint)src&&(px_uint)dst<=(px_uint)src+size)
	{
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
			*_4byteMovDst--=*_4byteMovSrc--;
		}
	}
	else
	{
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



px_void px_strcpy(px_char *dst,px_char *src,px_uint size)
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

px_void px_strcat(px_char *src,px_char *cat)
{
	px_int len=px_strlen(cat);
	while(*src)src++;
	while(len--)*src++=*cat++;
	*src='\0';
}

px_int px_strlen(px_char *dst)
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

px_void px_strupr(px_char *src)
{
	while (*src != '\0')  
    {  
        if (*src >= 'a' && *src <= 'z')  
            *src -= 32;  
        src++;  
    }  
}

px_void px_strlwr(px_char *src)
{
	 while (*src != '\0')  
    {  
        if (*src > 'A' && *src <= 'Z'){  
            *src += 32;  
        }  
        src++;  
    } 
}


px_point PX_PointMulMatrix(px_point p,px_matrix mat)
{
	px_point point;
	point.x=p.x*mat._11+p.y*mat._21+p.z*mat._31+1*mat._41;
	point.y=p.x*mat._12+p.y*mat._22+p.z*mat._32+1*mat._42;
	point.z=p.x*mat._13+p.y*mat._23+p.z*mat._33+1*mat._43;
	return point;
}


px_rect PX_RECT(px_float x,px_float y,px_float width,px_float height)
{
	px_rect rect;
	rect.x=x;
	rect.y=y;
	rect.width=width;
	rect.height=height;
	return rect;
}

px_complex complexBuild(float re,float im)
{
	px_complex cx;
	cx.re=re;
	cx.im=im;
	return cx;
}

px_complex complexAdd(px_complex a,px_complex b)
{
	px_complex ret;
	ret.re=a.re+b.re;
	ret.im=a.im+b.im;
	return ret;
}

px_complex complexMult(px_complex a,px_complex b)
{
	px_complex ret;
	ret.re=a.re*b.re-a.im*b.im;
	ret.im=a.im*b.re+a.re*b.im;
	return ret;
}

void PX_DFT(_IN px_complex x[],_OUT px_complex X[],px_int N)
{
	px_int k,n;
	px_complex Wnk;
	for (k=0;k<N;k++)
	{
		X[k].re=0;
		X[k].im=0;
		for (n=0;n<N;n++)
		{
			Wnk.re=(float)PX_cos_radian(2*PX_PI*k*n/N);
			Wnk.im=(float)-PX_sin_radian(2*PX_PI*k*n/N);
			X[k]=complexAdd(X[k],complexMult(x[n],Wnk));
		}
	}
}

void PX_IDFT(_IN px_complex X[],_OUT px_complex x[],px_int N)
{
	px_int k,n;
	float im=0;
	px_complex ejw;
	for (k=0;k<N;k++)
	{
		x[k].re=0;
		x[k].im=0;
		for (n=0;n<N;n++)
		{
			ejw.re=(float)PX_cos_radian(2*PX_PI*k*n/N);
			ejw.im=(float)PX_sin_radian(2*PX_PI*k*n/N);
			x[k]=complexAdd(x[k],complexMult(X[n],ejw));
		}

		x[k].re/=N;x[k].im/=N;
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
			Wnk.re=(float)PX_cos_radian(-2*PX_PI*k/N);
			Wnk.im=(float)PX_sin_radian(-2*PX_PI*k/N);
			cx0=x[k];
			cx1=x[k+(N>>1)];
			x[k]=complexAdd(cx0,complexMult(Wnk,cx1));
			Wnk.re=-Wnk.re;
			Wnk.im=-Wnk.im;
			x[k+(N>>1)]=complexAdd(cx0,complexMult(Wnk,cx1));
		}
	}
	else
	{
		//2 dot DFT
		cx0=x[0];
		cx1=x[1];
		x[0]=complexAdd(cx0,cx1);
		cx1.im=-cx1.im;
		cx1.re=-cx1.re;
		x[1]=complexAdd(cx0,cx1);
	}


}
void PX_FFT(_IN px_complex x[],_OUT px_complex X[],px_int N)
{
	px_memcpy(X,x,sizeof(px_complex)*N);
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
			Wnnk.re=(float)PX_cos_radian(2*PX_PI*n/N);
			Wnnk.im=(float)PX_sin_radian(2*PX_PI*n/N);
			cx0=X[n];
			cx1=X[n+(N>>1)];
			X[n]=complexAdd(cx0,complexMult(Wnnk,cx1));

			Wnnk.re=-Wnnk.re;
			Wnnk.im=-Wnnk.im;
			X[n+(N>>1)]=complexAdd(cx0,complexMult(Wnnk,cx1));

		}
	}
	else
	{
		//2 dot IDFT
		cx0=X[0];
		cx1=X[1];
		X[0]=complexAdd(cx0,cx1);

		cx1.im=-cx1.im;
		cx1.re=-cx1.re;
		X[1]=complexAdd(cx0,cx1);

	}
}
void PX_IFFT(_IN px_complex X[],_OUT px_complex x[],px_int N)
{
	px_int i;
	px_memcpy(X,x,sizeof(px_complex)*N);
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
	px_memcpy(_t,_in,sizeof(px_complex)*N*N);

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

px_point PX_POINT(px_float x,px_float y,px_float z)
{
	px_point p;
	p.x=x;
	p.y=y;
	p.z=z;
	return p;
}

px_bool px_strequ(px_char *src,px_char *dst)
{
	px_int ret = 0;  
	while(!(ret=*(px_uchar*)src-*(px_uchar*)dst) && *src)  
	{  
		src++;  
		dst++; 
	} 
	return !ret;
}


px_bool px_strIsNumeric(px_char *str)
{
	px_bool Dot=PX_FALSE;
	px_int CurrentCharIndex;

	if (str[0]=='\0')
	{
		return PX_FALSE;
	}
	if (!px_charIsNumeric(str[0])&&!(str[0]==('-')))
	{
		if(str[0]!='.')
			return PX_FALSE;
	}
	for (CurrentCharIndex=1;
		CurrentCharIndex<px_strlen(str);
		CurrentCharIndex++)
	{
		if (!px_charIsNumeric(str[CurrentCharIndex]))
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

px_bool px_strIsFloat(px_char *str)
{
	px_bool Dot=PX_FALSE;
	px_int CurrentCharIndex;
	if (str[0]=='\0')
	{
		return PX_FALSE;
	}
	if (!px_charIsNumeric(str[0])&&!(str[0]==('-')))
	{
		if(str[0]!='.')
			return PX_FALSE;
	}
	for (CurrentCharIndex=1;
		CurrentCharIndex<px_strlen(str);
		CurrentCharIndex++)
	{
		if (!px_charIsNumeric(str[CurrentCharIndex]))
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

px_bool px_charIsNumeric(px_char chr)
{
	if (chr>=('0')&&chr<=('9'))
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool px_strIsInt(px_char *str)
{
	if (px_strIsNumeric(str))
	{
		if (px_strIsFloat(str))
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
	for(i=1;i<=n;i++) powint*=num;
	return powint;
}

static px_double __px_pow_f(px_double num,px_double m)
{
	px_int i,j;
	px_double powf=0,x,tmpm=1;
	x=num-1;
	for(i=1;tmpm>0.00001 || tmpm<-0.00001;i++)
	{
		for(j=1,tmpm=1;j<=i;j++) 
			tmpm*=(m-j+1)*x/j;
		powf+=tmpm;
	}
	return powf+1;
}

px_double PX_pow_ff(px_double num,px_double m)
{
	if(num==0 && m!=0) return 0;
	else if(num==0 && m==0) return 1;
	else if(num<0 && m-(px_int)(m)!=0) return 0;
	if(num>2)
	{
		num=1/num;
		m=-m;
	}
	if(m<0) return 1/PX_pow_ff(num,-m);
	if(m-(px_int)(m)==0) return __px_pow_i(num,(px_int)m);
	else return __px_pow_f(num,m-(px_int)(m))*__px_pow_i(num,(px_int)(m));
	return __px_pow_f(num,m);
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


px_double PX_log(px_double __x)
{
	px_int N = 15;
	px_int k,nk;
	px_double x,xx,y;
	x = (__x-1)/(__x+1);
	xx = x*x;
	nk = 2*N+1;
	y = 1.0/nk;
	for(k=N;k>0;k--)
	{
		nk = nk - 2;
		y = 1.0/nk+xx*y;

	}
	return 2.0*x*y;
}

static px_uint64 px_srand_seed=0x31415926;

px_void PX_srand(px_uint64 seed)
{
	  seed = (seed*16807)%(0xefffffff);
	  px_srand_seed=seed;
}

px_uint32 PX_rand()
{
	return  (px_uint32)(px_srand_seed = (px_srand_seed*764261123)%(0xefffffff));
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
		c = (px_double)PX_sqrt((px_float)(-2 * PX_log(r) / r));
		return u * c;
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

PX_PARALLEL_CREATE_FUNC __PX_PARALLEL_CORE_CREATEFUNC=PX_NULL;
PX_PARALLEL_SETSIGNAL __PX_PARALLEL_CORE_SETSIGNAL=PX_NULL;

px_int __PX_PARALLEL_EXTRA_CORE_COUNT=0;

volatile static PX_PARALLEL_EXEC_FUNC __PX_PARALLEL_EXEC_FUNC[PX_PARALLEL_MAX_CORE_COUNT]={0};
static px_void *__PX_PARALLEL_PARAMS[PX_PARALLEL_MAX_CORE_COUNT]={0};

px_int PX_PARALLEL_CYCLE(px_void *p)
{
	px_int index=(px_int)p;
	
	while(1)
	{
		__PX_PARALLEL_CORE_SETSIGNAL(index,PX_TRUE);

		if(__PX_PARALLEL_EXEC_FUNC[index])
		{
			__PX_PARALLEL_EXEC_FUNC[index](__PX_PARALLEL_PARAMS[index]);
			__PX_PARALLEL_EXEC_FUNC[index]=PX_NULL;
		}
	}
	
}


px_void PX_PARALLEL_EXTRA_CORE_SET(PX_PARALLEL_CREATE_FUNC create_func,PX_PARALLEL_SETSIGNAL signal_func,px_int ExtraCoreCount)
{
	px_int i;
	
	__PX_PARALLEL_CORE_CREATEFUNC=create_func;
	__PX_PARALLEL_CORE_SETSIGNAL=signal_func;

	if(ExtraCoreCount<PX_PARALLEL_MAX_CORE_COUNT)
	__PX_PARALLEL_EXTRA_CORE_COUNT=ExtraCoreCount;
	else
		__PX_PARALLEL_EXTRA_CORE_COUNT=ExtraCoreCount;

	for (i=0;i<ExtraCoreCount;i++)
	{
		__PX_PARALLEL_CORE_CREATEFUNC(PX_PARALLEL_CYCLE,(px_void *)i);
	}
	
}

px_void PX_PARALLEL_EXEC(PX_PARALLEL_EXEC_FUNC _func,px_void *param,px_int index)
{
	__PX_PARALLEL_PARAMS[index]=param;
	__PX_PARALLEL_EXEC_FUNC[index]=_func;
	__PX_PARALLEL_CORE_SETSIGNAL(index,PX_FALSE);
}

px_bool PX_PARALLEL_DONE()
{
	px_int i;
	for (i=0;i<PX_PARALLEL_MAX_CORE_COUNT;i++)
	{
		if (__PX_PARALLEL_EXEC_FUNC[i])
		{
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}

static px_bool PX_checkCPUendian()
{
	union{
		px_dword i;
		unsigned char s[4];
	}c;
	c.i = 0x12345678;
	return (0x12 == c.s[0]);

}




px_char *PX_strchr(const char *s,int ch)
{

	while(*s!='\0')
	{
		if(*s-ch==0)
			return (px_char*)s;
		s++;
	}
	return PX_NULL;
}

px_int px_strcmp(const px_char *str1, const px_char *str2)
{
	int ret=0;
	while( !(ret = *(px_uchar*)str1 - *(px_uchar*)str2 ) && *str1 )
	{
		str1++;
		str2++;
	}
	return ret;   
}

px_char* px_strstr(const char* dest, const char* src)
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



px_dword  PX_inet_addr( px_char cp[] )
{
	px_uchar ipBytes[4]={0};
	px_int i;
	for( i=0; i<4; i++, cp++ ){
		ipBytes[i] = (px_uchar)PX_atoi( cp );
		if( !(cp = PX_strchr( cp, '.' )) ){ break; }
	}
	return *(px_dword*)ipBytes;
}


px_char* PX_inet_ntoa(px_dword ipv4)
{
	int b[4];
	static px_char a[17];
	a[0]='\0';
	b[0]=((ipv4 & 0xff000000)>>24);
	b[1]=((ipv4 & 0x00ff0000)>>16);
	b[2]=((ipv4 & 0x0000ff00)>>8);
	b[3]=((ipv4 & 0x000000ff)>>0);

	PX_itoa(b[3],a+px_strlen(a),5,10);
	px_strcat(a,".");
	PX_itoa(b[2],a+px_strlen(a),5,10);
	px_strcat(a,".");
	PX_itoa(b[1],a+px_strlen(a),5,10);
	px_strcat(a,".");
	PX_itoa(b[0],a+px_strlen(a),5,10);
	return a;
}

px_dword px_htonl(px_dword h)
{
	return PX_checkCPUendian() ? h : BigLittleSwap32(h);
}


px_dword px_ntohl(px_dword n)
{
	return PX_checkCPUendian() ? n : BigLittleSwap32(n);
}

px_word  px_htons(px_word h)
{
	return PX_checkCPUendian() ? h : BigLittleSwap16(h);
}

px_word px_ntohs(px_word n)
{
	return PX_checkCPUendian() ? n : BigLittleSwap16(n);
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


//%s %d %f
px_bool px_sprintf(px_char *str,px_int str_size,px_char fmt[],...)
{
	_px_va_list ap;
	px_char *p, *sval;
	px_int ival,oft=0;
	px_int finalLen=0;
	px_double dval;
	px_char dat[32];
	px_uchar shl=0;

	_px_va_start(ap, fmt);
	for (p = fmt; *p; p++) {
		if(*p != '%') {
			finalLen++;
			continue;
		}
		switch(*++p) {
		case 'd':
			ival = _px_va_arg(ap, px_int);
			finalLen +=PX_itoa(ival,dat,sizeof dat,10);
			break;
		case 'f':
			dval = _px_va_arg(ap, px_double);
			finalLen +=PX_ftoa((px_float)dval,dat,sizeof dat,6);
			break;
		case 's':
			sval = _px_va_arg(ap, char *); 
			finalLen +=px_strlen(sval);
			break;
		default:
			finalLen+=1;
			break;
		}
	}
	_px_va_end(ap);
	finalLen++;
	if (str_size<finalLen)
	{
		return PX_FALSE;
	}

	px_memset(str,0,str_size);
	_px_va_start(ap, fmt);
	for (p = fmt; *p; p++) {
		if(*p != '%') {
			str[oft++]=*p;
			continue;
		}
		switch(*++p) {
		case 'd':
			ival = _px_va_arg(ap, px_int);
			oft+=PX_itoa(ival,dat,sizeof dat,10);
			px_strcat(str,dat);
			break;
		case 'f':
			dval = _px_va_arg(ap, px_double);
			oft+=PX_ftoa((px_float)dval,dat,sizeof dat,6);
			px_strcat(str,dat);
			break;
		case 's':
			sval = _px_va_arg(ap, char *); 
			oft+=px_strlen(sval);
			px_strcat(str,sval);
			break;
		default:
			str[oft++]=*p;
			break;
		}
	}
	_px_va_end(ap);
	str[oft]='\0';
	return oft;
}

px_point PX_PointRotate(px_point p,px_float angle)
{
	px_matrix mat;
	PX_MatrixRotateZ(&mat,angle);
	return PX_PointMulMatrix(p,mat);
}

px_float PX_PointDistance(px_point p1,px_point p2)
{
	return PX_sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}

