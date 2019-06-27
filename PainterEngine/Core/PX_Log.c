#include "PX_Typedef.h"

static px_char __PX_LOG_OUT_DATA[1024]; //256 bytes for output log

void PX_ASSERT()
{
#ifdef PX_DEBUG_MODE
<<<<<<< HEAD
	__debugbreak();
=======
// 	_asm
// 	{
// 		int 3
// 	}
>>>>>>> 2214d6d8c82f62f657f6ddfbece3e1e7b45f0b2d
#endif
}

void PX_ERROR(px_char fmt[],...)
{
	_px_va_list ap;
	px_char *p, *sval;
	px_int ival,oft=0;
	px_uint finalLen=0;
	px_double dval;
	px_char dat[32];
    PX_ASSERT();
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
			finalLen +=PX_ftoa((px_float)dval,dat,sizeof dat,2);
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


	if (finalLen>sizeof(__PX_LOG_OUT_DATA))
	{
		return;
	}

	_px_va_start(ap, fmt);
	for (p = fmt; *p; p++) {
		if(*p != '%') {
			__PX_LOG_OUT_DATA[oft++]=*p;
			continue;
		}
		switch(*++p) {
		case 'd':
			ival = _px_va_arg(ap, px_int);
			oft+=PX_itoa(ival,dat,sizeof dat,10);
			px_strcat(__PX_LOG_OUT_DATA,dat);
			break;
		case 'f':
			dval = _px_va_arg(ap, px_double);
			oft+=PX_ftoa((px_float)dval,dat,sizeof dat,2);
			px_strcat(__PX_LOG_OUT_DATA,dat);
			break;
		case 's':
			sval = _px_va_arg(ap, char *); 
			oft+=px_strlen(sval);
			px_strcat(__PX_LOG_OUT_DATA,sval);
			break;
		default:
			__PX_LOG_OUT_DATA[oft++]=*p;
			break;
		}
	}
	_px_va_end(ap);
	__PX_LOG_OUT_DATA[oft]='\0';

	//while(1);
}

void PX_LOG(px_char fmt[],...)
{
	_px_va_list ap;
	px_char *p, *sval;
	px_int ival,oft=0;
	px_uint finalLen=0;
	px_double dval;
	px_char dat[32];

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
			finalLen +=PX_ftoa((px_float)dval,dat,sizeof dat,2);
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


	if (finalLen>sizeof(__PX_LOG_OUT_DATA))
	{
		return;
	}

	_px_va_start(ap, fmt);
	for (p = fmt; *p; p++) {
		if(*p != '%') {
			__PX_LOG_OUT_DATA[oft++]=*p;
			continue;
		}
		switch(*++p) {
		case 'd':
			ival = _px_va_arg(ap, px_int);
			oft+=PX_itoa(ival,dat,sizeof dat,10);
			px_strcat(__PX_LOG_OUT_DATA,dat);
			break;
		case 'f':
			dval = _px_va_arg(ap, px_double);
			oft+=PX_ftoa((px_float)dval,dat,sizeof dat,2);
			px_strcat(__PX_LOG_OUT_DATA,dat);
			break;
		case 's':
			sval = _px_va_arg(ap, char *); 
			oft+=px_strlen(sval);
			px_strcat(__PX_LOG_OUT_DATA,sval);
			break;
		default:
			__PX_LOG_OUT_DATA[oft++]=*p;
			break;
		}
	}
	_px_va_end(ap);
	__PX_LOG_OUT_DATA[oft]='\0';

	//while(1);
}

px_char * PX_GETLOG()
{
	return __PX_LOG_OUT_DATA;
}
