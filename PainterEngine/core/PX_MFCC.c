#include "PX_MFCC.h"


static px_double PX_HzToMel(px_double  freq)
{
	return 1127*PX_log(1.0 + freq/700);
}

static px_double PX_MelToHz(px_double  freq)
{
	return  700 * (PX_exp(freq/1127) - 1);
}


void PX_liftwindow(px_double* p, int m)
{
	int i;
	px_double  max_value = 0.0f;
	for(i = 1; i <= m; i++)
	{
		p[i-1] = 1+ 0.5 * m * PX_sind( PX_PI * i/m );
		if( p[i-1] > max_value)
		{
			max_value = p[i-1];
		}
	}
	for(i = 1; i <= m; i++)
	{
		p[i-1] /= max_value;
	}
}

px_void PX_MFCCInitialize(PX_MFCC *mfcc,px_int framesize,px_int sampleRate,px_int low,px_int high)
{
	px_int i,j;
	px_double  fre_bin = (px_double)sampleRate / framesize;
	px_double  low_mel = PX_HzToMel(low);
	px_double  high_mel = PX_HzToMel(high);
	px_double  mel_bw  = (high_mel - low_mel)/(PX_MFCC_DCT_FACTOR_SIZE + 1);
	px_int  valid_nfft = framesize/2 + 1;

	mfcc->N=framesize;
	PX_memset(mfcc->filter,0,sizeof(mfcc->filter));

	for(j = 1; j <= PX_MFCC_DCT_FACTOR_SIZE; j++)
	{  
		px_double  mel_cent  = j * mel_bw + low_mel;
		px_double  mel_left  = mel_cent - mel_bw;
		px_double  mel_right = mel_cent + mel_bw;
		px_double  freq_cent =  PX_MelToHz(mel_cent);
		px_double  freq_left =  PX_MelToHz(mel_left);
		px_double  freq_bw_left = freq_cent - freq_left;
		px_double  freq_right = PX_MelToHz(mel_right);
		px_double  freq_bw_right = freq_right - freq_cent;
		for(i = 1; i <= valid_nfft; i++)
		{          
			px_double freq = (i-1) * fre_bin ;
			if( freq > freq_left && freq < freq_right )
			{
				if( freq <= freq_cent)
				{
					mfcc->filter[j-1].w[i-1] = (freq - freq_left) / freq_bw_left;
				}
				else
				{
					mfcc->filter[j-1].w[i-1] = (freq_right - freq) / freq_bw_right;
				}
			}  
		}  
	} 

}

px_bool PX_MFCCParse(PX_MFCC *mfcc,px_double *data,PX_MFCC_FEATURE *out)
{
	px_int i,j;
	px_complex cal[PX_MFCC_FRAME_MAX_SIZE];
	px_double window[PX_MFCC_FRAME_MAX_SIZE];
	px_double dct_data[PX_MFCC_DCT_FACTOR_SIZE];
	px_double dct_rdata[PX_MFCC_DCT_FACTOR_SIZE];

	PX_WindowFunction_hamming(window,mfcc->N);

	for(i = mfcc->N - 1; i >= 1; i--)
	{
		cal[i].re = data[i] - 0.9375 * data[i-1];
		cal[i].im=0;
	}
	cal[0].re = data[0];
	cal[0].im=0;

	for (i=0;i<mfcc->N;i++)
	{
		cal[i].re*=window[i];
	}

	PX_FFT(cal,cal,mfcc->N);

	//re,im->power,phase
	for (i=0;i<mfcc->N;i++)
	{
		px_double pw,phase;
		pw=(cal[i].re*cal[i].re+cal[i].im*cal[i].im)/mfcc->N;
		phase=PX_atan2(cal[i].re,cal[i].im);
		cal[i].re=pw;
		cal[i].im=phase;
	}

	PX_memset(dct_data,0,sizeof(dct_data));

	//filter bank
	for (i=0;i<PX_MFCC_DCT_FACTOR_SIZE;i++)
	{
		for (j=0;j<mfcc->N;j++)
		{
			dct_data[i]+=cal[i].re*mfcc->filter[i].w[j];
		}
		if (dct_data[i]>1)
		{
			dct_data[i]=PX_ln(dct_data[i]);
		}
		else
		{
			dct_data[i]=0;
		}
	}

	
	//dct
	PX_DCT(dct_data,dct_rdata,PX_MFCC_DCT_FACTOR_SIZE);

	for (i=0;i<PX_MFCC_DCT_FACTOR_SIZE;i++)
	{
		out->factor[i]=dct_rdata[i];
	}

	return PX_TRUE;
}
