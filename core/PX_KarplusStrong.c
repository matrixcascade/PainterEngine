#include "PX_KarplusStrong.h"

px_void PX_KarplusStrong(px_int sampleRate, px_int pitch, px_float alpha, px_float* out, px_int outCount)
{
	px_int pitch_period=sampleRate/pitch;
	px_int i;
	PX_memset(out, 0, outCount * sizeof(px_float));
	if (pitch_period>outCount)
	{
		return;
	}
	for (i=0;i< pitch_period;i++)
	{
		out[i] =(px_float)(PX_randRange(-1, 1));
	}
	for (i=0;i<outCount;i++)
	{
		px_int iIdx = i / pitch_period;
		px_int iIndex = i % pitch_period;
		if (iIdx==0)
		{
			if (iIndex==0)
			{
				out[i] = (px_float)((out[i] + out[i + pitch_period - 1]) * 0.5f * alpha);
			}
			else
			{
				out[i] = (px_float)((out[i] + out[i-1]) * 0.5f * alpha);
			}
		}
		else
		{
			if (iIndex == 0)
			{
				out[i] = (px_float)((out[i- pitch_period] + out[i - 1]) * 0.5f * alpha);
			}
			else
			{
				out[i] = (px_float)((out[i- pitch_period] + out[i - 1]) * 0.5f * alpha);
			}
		}
	}

}

