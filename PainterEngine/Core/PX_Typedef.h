#ifndef __PX_TYPEDEF_H
#define __PX_TYPEDEF_H

#include	"PX_MathTable.h"
//#include    "stdint.h"
#define     _IN
#define     _OUT
#define     PX_FALSE			0
#define     PX_TRUE				1
#define		PX_NULL				0
#define     PX_PI				3.14159265359f
#define     PX_RAND_MAX         (0xefffffff)

#ifdef _DEBUG
#define PX_DEBUG_MODE _DEBUG
#endif


typedef char * _px_va_list;

#define __PX_INTSIZEOF(n) ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#define _px_va_start(ap,v) ( ap = (_px_va_list)&v + __PX_INTSIZEOF(v) )
#define _px_va_arg(ap,t) ( *(t *)((ap += __PX_INTSIZEOF(t)) - __PX_INTSIZEOF(t)) )
#define _px_va_end(ap) ( ap = (_px_va_list)0 )
#define px_countof(x) (sizeof(x)/sizeof(x[0]))



typedef		void				px_void;
typedef		int					px_bool;
typedef		unsigned int		px_dword;//typedef     uint32_t		       px_dword;
typedef     short               px_short;
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
typedef     double				px_double;
typedef     unsigned long long  px_qword;
typedef     unsigned long long  px_uint64;//typedef     uint64_t			px_uint64;
typedef     long long           px_int64;//typedef      int64_t				px_int64;



#include	"PX_Log.h"


#define PX_STRUCT_OFFSET(t,m)    ((px_uint)(((t *)0)->m))
#define BigLittleSwap16(A)  ((((px_word)(A) & 0xff00) >> 8)|(((px_word)(A) & 0x00ff) << 8))
#define BigLittleSwap32(A)  ((((px_dword)(A) & 0xff000000) >> 24)|(((px_dword)(A) & 0x00ff0000) >> 8)|(((px_dword)(A) & 0x0000ff00) << 8)|(((px_dword)(A) & 0x000000ff)<<24))


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

typedef struct _px_point
{
	px_float x;
	px_float y;
	px_float z;
}px_point;

typedef struct _px_rect
{
	px_float x,y,width,height;
}px_rect;

typedef struct __px_complex
{
	px_float re;// really 
	px_float im;// imaginary
}px_complex;

//////////////////////////////////////////////////////////////////////////
//endian

px_dword px_htonl(px_dword h);
px_dword px_ntohl(px_dword n);
px_word px_htons(px_word h);
px_word px_ntohs(px_word n);


typedef px_int (*PX_PARALLEL_EXEC_FUNC)(px_void *param);
typedef px_int (*PX_PARALLEL_CREATE_FUNC)(PX_PARALLEL_EXEC_FUNC func,px_void *param);
typedef px_int (*PX_PARALLEL_SETSIGNAL)(px_int index,px_bool setting);


///////////////////////////////////////////////////////////////////////////
//parallel core

#define PX_PARALLEL_MAX_CORE_COUNT 32 //must be 4 byte aligned
extern px_int __PX_PARALLEL_EXTRA_CORE_COUNT;

px_void PX_PARALLEL_EXTRA_CORE_SET(PX_PARALLEL_CREATE_FUNC create_func,PX_PARALLEL_SETSIGNAL signal_func,px_int ExtraCoreCount);
px_void PX_PARALLEL_EXEC(PX_PARALLEL_EXEC_FUNC _func,px_void *param,px_int index);
px_bool PX_PARALLEL_DONE();
#define PX_PARALLEL_ENABLED (__PX_PARALLEL_EXTRA_CORE_COUNT)
#define PX_PARALLEL_COUNT (__PX_PARALLEL_EXTRA_CORE_COUNT)

//////////////////////////////////////////////////////////////////////////
//functions
px_double PX_tanh(px_double x);
px_double PX_sigmoid(px_double x);
px_double PX_ReLU(px_double x);

//////////////////////////////////////////////////////////////////////////
//PMMCLCG
px_void PX_srand(px_uint64 seed);
px_uint32 PX_rand();
px_uint32 PX_randEx(px_uint64 seed);
//gauss rand
px_double PX_GaussRand();



//////////////////////////////////////////////////////////////////////////
//CRC
px_uint32 PX_crc32( px_void *buffer, px_uint size);

//////////////////////////////////////////////////////////////////////////
//Sum
px_uint32 PX_sum32(px_void *buffer, px_uint size);

//////////////////////////////////////////////////////////////////////////
//maths
px_int	PX_pow_ii(px_int i,px_int n);
px_double PX_pow_ff(px_double num,px_double m);
px_float PX_sqrt( px_float number );
px_float PX_SqrtRec( px_float number );
px_double PX_log(px_double __x);

#define  PX_RadianToAngle(angle) ((angle)*PX_PI/180)
#define  PX_AngleToRadian(radian) ((radian)*180/PX_PI)

px_float PX_sin_radian(float radius);
px_float PX_cos_radian(float radius);
px_float PX_sin_angle(float angle);
px_float PX_cos_angle(float angle);
px_float PX_Point_sin(px_point v);
px_float PX_Point_cos(px_point v);

#define  PX_APO(x) ((px_int)((x)+0.5f))
#define  PX_ABS(x) ((x)>0?(x):-(x))
#define  PX_SGN(x) ((x)?((x)>0?1:-1):0)
#define  PX_FRAC(x) ((x)-(px_int)(x))
#define  PX_TRUNC(x) ((px_int)(x))

//////////////////////////////////////////////////////////////////////////
//string to others
px_uint PX_htoi(px_char hex_str[]);
px_int  PX_atoi(px_char str[]);
px_float PX_atof(px_char fstr[]);
int PX_ftoa(float f, char *outbuf, int maxlen, int precision);
int PX_itoa(px_int num,px_char *str,px_int MaxStrSize,px_int radix);
px_char *PX_strchr(const char *s,int ch);
px_char* px_strstr(const char* dest, const char* src);

///////////////////////////////////////////////////////////////////////////
//rectangle
px_bool PX_isPointInRect(px_point p,px_rect rect);
px_bool PX_isRectCrossRect(px_rect rect1,px_rect rect2);
px_bool PX_isXYInRegion(px_float x,px_float y,px_float rectx,px_float recty,px_float width,px_float height);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//memory
void px_memset(void *dst,px_byte byte,px_uint size);
void px_memdwordset(void *dst,px_dword dw,px_uint count);
px_bool px_memequ(void *dst,void *src,px_uint size);
px_void px_memcpy(px_void *dst,px_void *src,px_uint size);
px_void px_strcpy(px_char *dst,px_char *src,px_uint size);
px_void px_strcat(px_char *src,px_char *cat);
px_int px_strlen(px_char *dst);
px_int px_strcmp(const px_char *str1, const px_char *str2);
px_bool px_strequ(px_char *src,char *dst);
px_void px_strupr(px_char *src);
px_void px_strlwr(px_char *src);
px_bool px_strIsNumeric(px_char *str);
px_bool px_strIsFloat(px_char *str);
px_bool px_strIsInt(px_char *str);
px_bool px_charIsNumeric(px_char ch);
px_int px_sprintf(px_char *str,px_int str_size,px_char fmt[],...);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//matrix
void PX_MatrixZero(px_matrix *Mat);
void PX_MatrixIdentity(px_matrix *Mat);
px_matrix PX_MatrixMultiply(px_matrix Mat1,px_matrix Mat2);
px_matrix PX_MatrixAdd(px_matrix Mat1,px_matrix Mat2);
px_matrix PX_MatrixSub(px_matrix Mat1,px_matrix Mat2);
px_bool PX_MatrixEqual(px_matrix Mat1,px_matrix Mat2);
px_void PX_MatrixTranslation(px_matrix *mat,float x,float y,float z);
px_void PX_MatrixRotateX(px_matrix *mat,float Angle);
px_void PX_MatrixRotateY(px_matrix *mat,float Angle);
px_void PX_MatrixRotateZ(px_matrix *mat,float Angle);
px_void PX_MatrixRotateXRadian(px_matrix *mat,float rad);
px_void PX_MatrixRotateYRadian(px_matrix *mat,float rad);
px_void PX_MatrixRotateZRadian(px_matrix *mat,float rad);
px_void PX_MatrixScale(px_matrix *mat,float x,float y,float z);
px_bool PX_MatrixInverse(px_matrix *mat);
px_void PX_MatrixTranspose(px_matrix *matrix);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//color
px_color PX_COLOR(px_uchar a,px_uchar r,px_uchar g,px_uchar b); 
px_void  PX_ColorIncrease(px_color *color,px_uchar inc);
px_color PX_ColorAdd(px_color color1,px_color color2);
px_color PX_ColorSub(px_color color1,px_color color2);
px_color PX_ColorMul(px_color color1,px_double muls);
px_bool  PX_ColorEqual(px_color color1,px_color color2);
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//point
px_point PX_POINT(px_float x,px_float y,px_float z);
px_point PX_PointRotate(px_point p,px_float angle);
px_point PX_PointAdd(px_point p1,px_point p2);
px_point PX_PointSub(px_point p1,px_point p2);
px_point PX_PointMul(px_point p1,px_float m);
px_point PX_PointDiv(px_point p1,px_float m);
px_float PX_PointDot(px_point p1,px_point p2);
px_point PX_PointCross(px_point p1,px_point p2);
px_float PX_PointMod(px_point p);
px_float PX_PointSquare(px_point p);
px_point PX_PointUnit(px_point p);
px_point PX_PointReflectX(px_point vector_refer,px_point respoint);

px_point PX_PointMulMatrix(px_point p,px_matrix m);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//Rect
px_rect PX_RECT(px_float x,px_float y,px_float width,px_float height);


//////////////////////////////////////////////////////////////////////////
//DFT/FFT
void PX_DFT(_IN px_complex x[],_OUT px_complex X[],px_int N);
void PX_IDFT(_IN px_complex X[],_OUT px_complex x[],px_int N);
void PX_FFT(_IN px_complex x[],_OUT px_complex X[],px_int N);
void PX_IFFT(_IN px_complex X[],_OUT px_complex x[],px_int N);
void PX_FFT_2(_IN px_complex x[],_OUT px_complex X[],px_int N_N);
void PX_IFFT_2(_IN px_complex X[],_OUT px_complex x[],px_int N_N);
void PX_FFT_2_Shift(_IN px_complex _in[],_OUT px_complex _out[],px_int N_N);

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//ipv4
px_dword PX_inet_addr( px_char cp[] );
px_char* PX_inet_ntoa( px_dword ipv4 );

#endif
