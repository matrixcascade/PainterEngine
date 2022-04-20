#include "PX_Signal.h"

px_void PX_SignalDelay(_IN _OUT px_float x[], px_int size, px_int delay)
{
	px_int i;
	if (!size)
	{
		return;
	}
	if (delay>size)
	{
		delay = size;
	}
	if (delay>0)
	{
		px_float fill = x[0];
		PX_memcpy(x + delay, x , size - delay);
		for (i=0;i<delay;i++)
		{
			x[i] = fill;
		}
	}
	else if(delay<0)
	{
		px_float fill = x[size-1];
		PX_memcpy(x, x - delay, size + delay);
		for (i = size + delay; i < size; i++)
		{
			x[i] = fill;
		}
	}
	
}

px_void PX_SignalForwardDifference(_IN px_float x[], px_float out[], px_int size)
{
	px_int i;
	for (i=1;i<size;i++)
	{
		out[i] = x[i] - x[i - 1];
	}
	if(size)
		out[0] = out[1];
}

px_void PX_SignalAvg(_IN  px_float in[], _OUT px_float out[], px_int size, px_int filter)
{
	px_int i;
	px_int l, r;
	l = filter / 2+ ((filter & 1)?1:0);
	r = filter / 2;
	for (i=0;i< l;i++)
	{
		out[i] = in[i];
	}
	for (;i<size- r;i++)
	{
		px_int j;
		px_float sum=0;
		for (j=0;j<filter;j++)
		{
			sum += in[i + j];
		}
		out[i] = sum/filter;
	}
	for (;i<size;i++)
	{
		out[i] = in[i];
	}
	
}
