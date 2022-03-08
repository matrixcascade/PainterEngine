#ifndef __PX_TYPEDEF_H
#define __PX_TYPEDEF_H

#include	"PX_MathTable.h"

#define PX_DBL_POSITIVE_MAX 1.7976931348623158e+308
#define PX_DBL_POSITIVE_MIN 4.94065645841246544e-324

#define PX_DBL_NEGATIVE_MIN -1.79769313486231570E+308
#define PX_DBL_NEGATIVE_MAX -4.94065645841246544E-324

#define     _IN
#define     _OUT
#define     _LIMIT
#define     PX_FALSE			0
#define     PX_TRUE				1
#define		PX_NULL				0
#define     PX_PI				3.141592653589793238462
#define     PX_e                (2.7182818284590452353602)
#define     PX_e2               (PX_e*PX_e)
#define     PX_RAND_MAX         (0xffffff)


#ifdef _DEBUG
#define PX_DEBUG_MODE _DEBUG
#endif


#define PX_COUNTOF(x) (sizeof(x)/sizeof(x[0]))



typedef		void				px_void;
typedef		int					px_bool;
typedef		unsigned int		px_dword;//typedef     uint32_t		       px_dword;
typedef     short               px_short;
typedef     short				px_int16;
typedef		unsigned short		px_uint16;
typedef     unsigned short		px_word; //typedef     uint16_t		       px_word;
typedef		unsigned short		px_ushort;
typedef     unsigned int		px_uint;
typedef     unsigned int		px_uint32;//typedef     uint32_t		   px_uint32;
typedef     int					px_int;
typedef     int					px_int32;//typedef     int32_t			   px_int32;
typedef		char				px_char;
typedef		unsigned char		px_byte;
typedef		unsigned char		px_uchar;
typedef		unsigned long		px_ulong;
typedef		long				px_long;
typedef     float				px_float;
typedef     float				px_float32;//IEEE 754 little-endian
typedef     double				px_double;
typedef     double				px_double64;
typedef     unsigned long long  px_qword;
typedef     unsigned long long  px_uint64;//typedef     uint64_t			px_uint64;
typedef     long long           px_int64;//typedef      int64_t				px_int64;



typedef struct
{
	px_char data[64];
}PX_RETURN_STRING;

#include	"PX_Log.h"


#define PX_STRUCT_OFFSET(t,m)    ((((t *)0)->m-(px_byte *)0))
#define BigLittleSwap16(A)  ((((px_word)(A) & 0xff00) >> 8)|(((px_word)(A) & 0x00ff) << 8))
#define BigLittleSwap32(A)  ((((px_dword)(A) & 0xff000000) >> 24)|(((px_dword)(A) & 0x00ff0000) >> 8)|(((px_dword)(A) & 0x0000ff00) << 8)|(((px_dword)(A) & 0x000000ff)<<24))

typedef enum
{
	PX_ALIGN_LEFTTOP,
	PX_ALIGN_MIDTOP,
	PX_ALIGN_RIGHTTOP,
	PX_ALIGN_LEFTMID,
	PX_ALIGN_CENTER,
	PX_ALIGN_RIGHTMID,
	PX_ALIGN_LEFTBOTTOM,
	PX_ALIGN_MIDBOTTOM,
	PX_ALIGN_RIGHTBOTTOM,
}PX_ALIGN;


typedef struct _px_matrix 
{
	union {
		struct {
			px_float        _11, _12, _13, _14;
			px_float        _21, _22, _23, _24;
			px_float        _31, _32, _33, _34;
			px_float        _41, _42, _43, _44;
		};
		px_float m[4][4];
	};
}px_matrix;

typedef struct _px_color
{
	union 
	{
		struct
		{
			px_uchar r;
			px_uchar g;
			px_uchar b;
			px_uchar a;
		};
		px_dword ucolor;
	}_argb;
}px_color;

typedef struct _px_color_hsl
{
	px_float a;//Alpha 0--1
	px_float H;//Hue 0-360
	px_float S;//Saturation 0--1
	px_float L;//Lightness 0--1
}px_color_hsl;

typedef struct _px_point
{
	px_float x;
	px_float y;
	px_float z;
}px_point;
typedef px_point px_point32;

typedef struct _px_wpoint
{
	px_double x;
	px_double y;
	px_double z;
}px_wpoint;

typedef struct 
{
	px_float x, y;
}px_point2D;

typedef px_point2D px_vector2D;
typedef px_point px_point3D;
typedef px_point px_vector3D;

typedef struct _px_point4
{
	px_float x;
	px_float y;
	px_float z;
	px_float w;
}px_point4D;

typedef px_point4D px_vertex;
typedef px_point4D px_vector4D;

typedef struct 
{
	px_point3D p0; // point on the plane
	px_vector3D n; // normal to the plane 
}px_plane;

typedef struct _px_rect
{
	px_float x,y,width,height;
}px_rect;

typedef struct __px_complex
{
	px_double re;// really 
	px_double im;// imaginary
}px_complex;



//////////////////////////////////////////////////////////////////////////
//endian

px_dword PX_htonl(px_dword h);
px_dword PX_ntohl(px_dword n);
px_word PX_htons(px_word h);
px_word PX_ntohs(px_word n);


//////////////////////////////////////////////////////////////////////////
//
px_double PX_exp(px_double x);

//////////////////////////////////////////////////////////////////////////
//functions
px_double PX_tanh(px_double x);
px_double PX_sigmoid(px_double x);
px_double PX_ReLU(px_double x);

//////////////////////////////////////////////////////////////////////////
//PMMCLCG
px_void PX_srand(px_uint64 seed);
px_uint32 PX_rand(void);
px_double PX_randRange(px_double min,px_double max);
px_uint32 PX_randEx(px_uint64 seed);
//gauss rand
px_double PX_GaussRand(void);

//ceil
px_double PX_Ceil(px_double v);

//////////////////////////////////////////////////////////////////////////
//file ext
px_void PX_FileGetName(const px_char filefullName[],px_char _out[],px_int outSize);
px_void PX_FileGetPath(const px_char filefullName[],px_char _out[],px_int outSize);
px_void PX_FileGetExt(const px_char filefullName[],px_char _out[],px_int outSize);

//////////////////////////////////////////////////////////////////////////
//CRC
px_uint32 PX_crc32( px_void *buffer, px_uint size);
px_word PX_crc16(px_void *buffer,px_uint size);
//////////////////////////////////////////////////////////////////////////
//Sum
px_uint32 PX_sum32(px_void *buffer, px_uint size);

//////////////////////////////////////////////////////////////////////////
//maths
px_int	PX_pow_ii(px_int i,px_int n);
px_double PX_pow(px_double num,px_double m);
px_float PX_sqrt( px_float number );
px_double PX_sqrtd( px_double number );
px_float PX_SqrtRec( px_float number );
px_double PX_ln(px_double __x);
px_double PX_log(px_double __x);
px_double PX_lg(px_double __x);
px_double PX_log10(px_double __x);

#define  PX_RadianToAngle(radian) ((radian)*180/PX_PI)
#define  PX_AngleToRadian(angle) ((angle)*PX_PI/180)

px_double PX_tand(px_double radian);
px_double PX_sind(px_double radian);
px_double PX_sinc(px_double i);
px_double PX_sinc_interpolate(px_double x[], px_int size, px_double d);
px_double PX_cosd(px_double radian);
px_float PX_sin_radian(px_float radian);
px_float PX_cos_radian(px_float radian);
px_float PX_tan_radian(px_float radian);
px_float PX_sin_angle(px_float angle);
px_float PX_cos_angle(px_float angle);
px_float PX_tan_angle(px_float angle);
px_double PX_atan(px_double x);
px_double PX_atan2(px_double y, px_double x);

px_double PX_asin(px_double x);
px_double PX_acos(px_double x);
px_float PX_Point_sin(px_point v);
px_float PX_Point_cos(px_point v);

#define  PX_APO(x) ((px_int)((x)+0.5f))
#define  PX_ABS(x) ((x)>0?(x):-(x))
#define  PX_SGN(x) ((x)?((x)>0?1:-1):0)
#define  PX_FRAC(x) ((x)-(px_int)(x))
#define  PX_TRUNC(x) ((px_int)(x))

//////////////////////////////////////////////////////////////////////////
//string to others
px_void PX_BufferToHexString(px_byte data[],px_int size,px_char hex_str[]);
px_int PX_HexStringToBuffer(const px_char hex_str[],px_byte data[]);
px_uint PX_htoi(const px_char hex_str[]);
px_int  PX_atoi(const px_char str[]);
px_float PX_atof(const px_char fstr[]);
PX_RETURN_STRING PX_ftos(px_float f, int precision);
PX_RETURN_STRING PX_itos(px_int num,px_int radix);
px_void PX_AscToWord(const px_char *asc,px_word *u16);
px_int PX_ftoa(px_float f, px_char *outbuf, int maxlen, int precision);
px_int PX_itoa(px_int num,px_char *str,px_int MaxStrSize,px_int radix);
px_dword PX_SwapEndian(px_dword val);
px_char *PX_strchr(const px_char *s,int ch);
px_char* PX_strstr(const px_char* dest, const px_char* src);

///////////////////////////////////////////////////////////////////////////
//rectangle circle
px_bool PX_isPointInCircle(px_point p,px_point circle,px_float radius);
px_bool PX_isPoint2DInCircle(px_point2D p,px_point2D circle,px_float radius);
px_bool PX_isPointInRect(px_point p,px_rect rect);
px_bool PX_isXYInRegion(px_float x,px_float y,px_float rectx,px_float recty,px_float width,px_float height);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//cross
px_bool PX_isLineCrossRect(px_point p1,px_point p2,px_rect rect,px_point *cp1,px_point *cp2);
px_bool PX_isRectCrossRect(px_rect rect1,px_rect rect2);
px_bool PX_isRectCrossCircle(px_rect rect1,px_point center,px_float radius);
px_bool PX_isCircleCrossCircle(px_point center1,px_float radius1,px_point center2,px_float radius2);

//////////////////////////////////////////////////////////////////////////
px_double PX_Covariance(px_double x[],px_double y[],px_int n);
px_double PX_Variance(px_double x[],px_int n);

//////////////////////////////////////////////////////////////////////////
//memory
void PX_memset(void *dst,px_byte byte,px_int size);
void PX_memdwordset(void *dst,px_dword dw,px_int count);
#define PX_zeromemory(dst,size) PX_memset(dst,0,size)
px_bool PX_memequ(const void *dst,const void *src,px_int size);
px_void PX_memcpy(px_void *dst,const px_void *src,px_int size);
px_void PX_strcpy(px_char *dst,const px_char *src,px_int size);
px_void PX_wstrcpy(px_word *dst,const px_word *src,px_int size);
px_void PX_strcat(px_char *src,const px_char *cat);
px_void PX_strcat_s(px_char* src, px_int size, const px_char* cat);
px_void PX_wstrcat(px_word *src,const px_word *cat);
px_void PX_strset(px_char *dst,const px_char *src);
px_int PX_strlen(const px_char *dst);
px_int PX_wstrlen(const px_word *dst);
px_int PX_strcmp(const px_char *str1, const px_char *str2);
px_bool PX_strequ(const px_char *src,const px_char *dst);
px_bool PX_strequ2(const px_char* src, const px_char* dst);
px_void PX_strupr(px_char *src);
px_void PX_strlwr(px_char *src);
px_bool PX_strIsNumeric(const px_char *str);
px_bool PX_strIsFloat(const px_char *str);
px_bool PX_strIsInt(const px_char *str);
px_bool PX_charIsNumeric(px_char ch);

typedef enum
{
	PX_STRINGFORMAT_TYPE_INT,
	PX_STRINGFORMAT_TYPE_FLOAT,
	PX_STRINGFORMAT_TYPE_STRING,
}PX_STRINGFORMAT_TYPE;

typedef struct
{
	PX_STRINGFORMAT_TYPE type;
	union
	{
		px_int _int;
		px_float _float;
		const px_char *_pstring;
	};
}px_stringformat;


px_stringformat PX_STRINGFORMAT_INT(px_int _i);
px_stringformat PX_STRINGFORMAT_FLOAT(px_float _f);
px_stringformat PX_STRINGFORMAT_STRING(const px_char *_s);
px_int PX_sprintf8(px_char *str,px_int str_size,const px_char fmt[],\
	px_stringformat _1,\
	px_stringformat _2,\
	px_stringformat _3,\
	px_stringformat _4,\
	px_stringformat _5,\
	px_stringformat _6,\
	px_stringformat _7,\
	px_stringformat _8\
	);
px_int PX_sprintf7(px_char *str,px_int str_size,const px_char fmt[],\
	px_stringformat _1,\
	px_stringformat _2,\
	px_stringformat _3,\
	px_stringformat _4,\
	px_stringformat _5,\
	px_stringformat _6,\
	px_stringformat _7
	);
px_int PX_sprintf6(px_char *str,px_int str_size,const px_char fmt[],\
	px_stringformat _1,\
	px_stringformat _2,\
	px_stringformat _3,\
	px_stringformat _4,\
	px_stringformat _5,\
	px_stringformat _6\
	);
px_int PX_sprintf5(px_char *str,px_int str_size,const px_char fmt[],\
	px_stringformat _1,\
	px_stringformat _2,\
	px_stringformat _3,\
	px_stringformat _4,\
	px_stringformat _5\
	);
px_int PX_sprintf4(px_char *str,px_int str_size,const px_char fmt[],\
	px_stringformat _1,\
	px_stringformat _2,\
	px_stringformat _3,\
	px_stringformat _4\
	);
px_int PX_sprintf3(px_char *str,px_int str_size,const px_char fmt[],\
	px_stringformat _1,\
	px_stringformat _2,\
	px_stringformat _3\
	);
px_int PX_sprintf2(px_char *str,px_int str_size,const px_char fmt[],\
	px_stringformat _1,\
	px_stringformat _2\
	);
px_int PX_sprintf1(px_char *str,px_int str_size,const px_char fmt[],\
	px_stringformat _1\
	);
px_int PX_sprintf0(px_char *str,px_int str_size,const px_char fmt[]);
/*px_int px_sprintf(px_char *str,px_int str_size,px_char fmt[],...);*/
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//matrix
void PX_MatrixZero(px_matrix *Mat);
void PX_MatrixIdentity(px_matrix *Mat);
px_matrix PX_MatrixMultiply(px_matrix Mat1,px_matrix Mat2);
px_matrix PX_MatrixAdd(px_matrix Mat1,px_matrix Mat2);
px_matrix PX_MatrixSub(px_matrix Mat1,px_matrix Mat2);
px_bool PX_MatrixEqual(px_matrix Mat1,px_matrix Mat2);
px_void PX_MatrixRotateVector(px_matrix *mat,px_point v_base,px_point v);
px_void PX_MatrixTranslation(px_matrix *mat,px_float x,px_float y,px_float z);
px_void PX_MatrixRotateX(px_matrix *mat,px_float Angle);
px_void PX_MatrixRotateY(px_matrix *mat,px_float Angle);
px_void PX_MatrixRotateZ(px_matrix *mat,px_float Angle);
px_void PX_MatrixRotateXRadian(px_matrix *mat,px_float rad);
px_void PX_MatrixRotateYRadian(px_matrix *mat,px_float rad);
px_void PX_MatrixRotateZRadian(px_matrix *mat,px_float rad);
px_void PX_MatrixScale(px_matrix *mat,px_float x,px_float y,px_float z);
px_bool PX_MatrixInverse(px_matrix *mat);
px_void PX_MatrixTranspose(px_matrix *matrix);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//color
px_color PX_COLOR(px_uchar a,px_uchar r,px_uchar g,px_uchar b); 
px_color PX_ColorInverse(px_color clr);
px_void  PX_ColorIncrease(px_color* color, px_uchar inc);
px_color PX_ColorAdd(px_color color1,px_color color2);
px_color PX_ColorSub(px_color color1,px_color color2);
px_color PX_ColorMul(px_color color1,px_double muls);
px_bool  PX_ColorEqual(px_color color1,px_color color2);
px_color_hsl PX_ColorRGBToHSL(px_color color_rgb);
px_color PX_ColorHSLToRGB(px_color_hsl color_hsl);
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//point
px_point PX_POINT(px_float x,px_float y,px_float z);
px_point2D PX_POINT2D(px_float x,px_float y);
px_point4D PX_POINT4D(px_float x,px_float y,px_float z);
px_point PX_PointRotate(px_point p,px_float angle);
px_point2D PX_Point2DRotate(px_point2D p,px_float angle);
px_float PX_PointDistance(px_point p1,px_point p2);


px_point PX_PointAdd(px_point p1,px_point p2);
px_point2D PX_Point2DAdd(px_point2D p1,px_point2D p2);
px_point PX_PointSub(px_point p1,px_point p2);
px_point2D PX_Point2DSub(px_point2D p1,px_point2D p2);
px_point4D PX_Point4DSub(px_point4D p1,px_point4D p2);
px_point PX_PointMul(px_point p1,px_float m);
px_point2D PX_Point2DMul(px_point2D p1,px_float m);
px_point PX_PointDiv(px_point p1,px_float m);

px_point2D PX_Point2DRrthonormal(px_point2D v);
px_point2D PX_Point2DBase(px_point2D base1,px_point2D base2,px_point2D target);
px_point2D PX_Point2DDiv(px_point2D p1,px_float m);
px_float PX_PointDot(px_point p1,px_point p2);
px_float PX_Point2DDot(px_point2D p1,px_point2D p2);
px_float PX_Point4DDot(px_point4D p1,px_point4D p2);
px_point PX_PointCross(px_point p1,px_point p2);
px_point4D PX_Point4DCross(px_point4D p1,px_point4D p2);
px_point PX_PointInverse(px_point p1);
px_float PX_PointMod(px_point p);
px_float PX_Point2DMod(px_point2D p);
px_float PX_PointSquare(px_point p);
px_point PX_PointNormalization(px_point p);
px_point2D PX_Point2DNormalization(px_point2D p);
px_point4D PX_Point4DUnit(px_point4D p);
px_point PX_PointReflectX(px_point vector_refer,px_point respoint);

px_point2D PX_Point2DMulMatrix(px_point2D p,px_matrix m);
px_point PX_PointMulMatrix(px_point p,px_matrix m);
px_point4D PX_Point4DMulMatrix(px_point4D p,px_matrix m);

//////////////////////////////////////////////////////////////////////////
//Triangle
typedef struct 
{
	px_point Vertex_1, Vertex_2, Vertex_3;
}px_triangle;
px_bool PX_TriangleIsCross( px_triangle* tri1, px_triangle* tri2 );

//////////////////////////////////////////////////////////////////////////
//plane
px_plane PX_PLANE(px_point3D p,px_vector3D n);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//Rect
px_rect PX_RECT(px_float x,px_float y,px_float width,px_float height);
px_rect PX_RECTPOINT2(px_point p1,px_point p2);


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//complex
px_complex PX_complexBuild(px_float re,px_float im);
px_complex PX_complexAdd(px_complex a,px_complex b);
px_complex PX_complexMult(px_complex a,px_complex b);
px_double  PX_complexMod(px_complex a);
px_complex PX_complexLog(px_complex a);
px_complex PX_complexExp(px_complex a);
px_complex PX_complexSin(px_complex a);

//////////////////////////////////////////////////////////////////////////
//DFT/FFT
void PX_DFT(_IN px_complex x[],_OUT px_complex X[],px_int N);
void PX_DCT(_IN px_double x[],_OUT px_double X[],px_int N);
void PX_IDFT(_IN px_complex X[],_OUT px_complex x[],px_int N);
void PX_IDCT(_IN px_double x[],_OUT px_double X[],px_int N);
void PX_FFT(_IN px_complex x[],_OUT px_complex X[],px_int N);
void PX_IFFT(_IN px_complex X[],_OUT px_complex x[],px_int N);
void PX_FFT_2(_IN px_complex x[],_OUT px_complex X[],px_int N_N);
void PX_IFFT_2(_IN px_complex X[],_OUT px_complex x[],px_int N_N);
void PX_FFT_2_Shift(_IN px_complex _in[],_OUT px_complex _out[],px_int N_N);
void PX_DCT_2_Shift(_IN px_double _in[], _OUT px_double _out[], px_int N_N);


void PX_FT_Symmetry(_IN px_complex x[], _OUT px_complex X[], px_int N);

//////////////////////////////////////////////////////////////////////////
//cepstrum
typedef enum
{
	PX_CEPTRUM_TYPE_REAL,
	PX_CEPSTRUM_TYPE_COMPLEX,
}PX_CEPSTRUM_TYPE;
void PX_Cepstrum(_IN px_complex x[],_OUT px_complex X[],px_int N,PX_CEPSTRUM_TYPE type);

//////////////////////////////////////////////////////////////////////////
//zero-crossing rate,ZCR
px_double PX_ZeroCrossingRate(_IN px_double x[],px_int N);
px_double PX_ZeroCrossingRateComplex(_IN px_complex x[],px_int N);
//////////////////////////////////////////////////////////////////////////
//PitchEstimation
px_int PX_PitchEstimation(_IN px_complex x[],px_int N,px_int sampleRate,px_int low_Hz,px_int high_Hz);
//////////////////////////////////////////////////////////////////////////
//PreEmphasise
void PX_PreEmphasise(const px_double *data, int len, px_double *out, px_double preF);//0.9<preF<1.0 suggest 0.9;

//////////////////////////////////////////////////////////////////////////
//up/down sampling
void PX_LinearInterpolationResample(_IN px_double x[],_OUT px_double X[],px_int N,px_int M);
void PX_SincInterpolationResample(_IN px_double x[], _OUT px_double X[], px_int N, px_int M);
void PX_DownSampled(_IN px_complex x[],_OUT px_complex X[],px_int N,px_int M);
void PX_UpSampled(_IN px_complex x[],_OUT px_complex X[],px_int N,px_int L);

//////////////////////////////////////////////////////////////////////////
//ipv4
typedef struct 
{
	px_dword port;
	union
	{
		px_dword ipv4;
		px_dword ipv6[4];
	};
}PX_Network_Addr;

px_dword PX_inet_addr( const px_char cp[] );
px_dword PX_inet_port(const px_char cp[]);
PX_RETURN_STRING PX_inet_ntoa(px_dword ipv4);
px_bool PX_IsValidIPAddress(const px_char *ip_addr);

//////////////////////////////////////////////////////////////////////////
//Bessel
px_double PX_Bessel(int n,px_double x);
//////////////////////////////////////////////////////////////////////////
//Window Functions
px_void PX_WindowFunction_tukey(px_double data[],px_int N);
px_void PX_WindowFunction_hanning(px_double data[],px_int N);
px_void PX_WindowFunction_blackMan(px_double data[],px_int N);
px_void PX_WindowFunction_hamming(px_double data[],px_int N);
px_void PX_WindowFunction_sinc(px_double data[],px_int N);
px_void PX_WindowFunction_kaiser(px_double beta,px_double data[],px_int N);
px_void PX_WindowFunction_triangular(px_double data[],px_int N);
px_void PX_WindowFunction_Apply(px_double data[],px_double window[],px_int N);

//////////////////////////////////////////////////////////////////////////
//gainc
px_void PX_gain(px_double b[],px_double a[],px_int m,px_int n,px_double x[],px_double y[],px_int len,px_int sign);
px_void PX_gainc(px_double b[],px_double a[],px_int n,px_int ns,px_double x[],px_double y[],px_int len,px_int sign);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//sine
typedef struct
{
	px_double A;
	px_double p;
	px_double f;
}px_sine;//f(t)=Asin(wt+p)

px_sine PX_SINE(px_double A,px_double P,px_double F);

//////////////////////////////////////////////////////////////////////////
//Instantaneous Frequency
px_sine PX_InstantaneousFrequency(px_sine src,px_double p2,px_double delta_t);


//////////////////////////////////////////////////////////////////////////
//

//FIR Filter
typedef enum
{
	PX_FIRFILTER_TYPE_LOWPASS,
	PX_FIRFILTER_TYPE_HIGHPASS,
	PX_FIRFILTER_TYPE_BANDPASS,
	PX_FIRFILTER_TYPE_STOPBANDFILTER,
}PX_FIRFILTER_TYPE;

typedef enum
{
	PX_FIRFILTER_WINDOW_TYPE_RECT,
	PX_FIRFILTER_WINDOW_TYPE_TUKEY,
	PX_FIRFILTER_WINDOW_TYPE_TRIANGULAR,
	PX_FIRFILTER_WINDOW_TYPE_HANNING,
	PX_FIRFILTER_WINDOW_TYPE_HAMMING,
	PX_FIRFILTER_WINDOW_TYPE_BLACKMAN,
	PX_FIRFILTER_WINDOW_TYPE_KAISER,
}PX_FIRFILTER_WINDOW_TYPE;

px_void PX_FIRFilterBuild(PX_FIRFILTER_TYPE bandtype,px_double fln,px_double fhn,PX_FIRFILTER_WINDOW_TYPE wn,px_double h[],px_int n,px_double beta);

///////////////////////////////////////////////////////////////////////////////
//LTI
px_float PX_GroupDelay(px_float f, px_float* B, px_int sizeB, px_float* A, px_int sizeA, px_float FS);
px_float PX_PhaseDelayDerive(px_float omega, px_float* B, px_int sizeB, px_float* A, px_int sizeA, px_float delta);
px_float PX_PhaseDelay(px_float f, px_float* B, px_int sizeB, px_float* A, px_int sizeA, px_float FS);
#endif
