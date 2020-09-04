#include "PX_Tuning.h"


px_void PX_TuningInitialize(px_memorypool *mp,PX_Tuning *tuning,px_int SampleRate,px_double pitchShift,px_double timeShift,px_double window[],px_double filter[],px_double fix[],PX_TUNING_WINDOW_SIZE windowsize)
{
	px_int i;
	px_int N;

	switch (windowsize)
	{
	case PX_TUNING_WINDOW_SIZE_8:
		N=8;
		break;
	case PX_TUNING_WINDOW_SIZE_128:
		N=128;
		break;
	case PX_TUNING_WINDOW_SIZE_256:
		N=256;
		break;
	case PX_TUNING_WINDOW_SIZE_512:
		N=512;
		break;
	case PX_TUNING_WINDOW_SIZE_1024:
		N=1024;
		break;
	case PX_TUNING_WINDOW_SIZE_2048:
		N=2048;
		break;
	default:
		PX_ASSERT();
		return;
	}

	tuning->mp=mp;
	tuning->pitchShift=pitchShift;
	tuning->timeScale=timeShift;
	tuning->N=N;
	tuning->remainCount=0;
	tuning->previousInterpolationOffset=0;
	tuning->previousInterpolationValue=0;
	tuning->spectrumInterpolationOffset=2;
	tuning->spectrumInterpolationFactor=1/(pitchShift*timeShift);
	tuning->interpolationFactor=pitchShift;
	tuning->ZCR_Low=0;
	tuning->ZCR_High=1;
	tuning->amp_threshold=0;
	tuning->SampleRate=SampleRate;
	PX_memset(tuning->lastphase,0,sizeof(tuning->lastphase));
	PX_memset(tuning->previous_frame,0,sizeof(tuning->previous_frame));
	PX_memset(tuning->half_previous_out,0,sizeof(tuning->half_previous_out));
	PX_memset(tuning->remian,0,sizeof(tuning->remian));

	if(filter)
	{
		PX_memcpy(tuning->filter,filter,sizeof(px_double)*N);	
	}
	else
	{
		for (i=0;i<PX_TUNING_MAX_FRAME_SIZE;i++)
		{
			tuning->filter[i]=1;
		}
	}

	if(window)
	{
		PX_memcpy(tuning->window,window,sizeof(px_double)*N);	
	}
	else
	{
		PX_WindowFunction_sinc(tuning->window,N);
	}

	if(fix)
	{
		PX_memcpy(tuning->fix,fix,sizeof(px_double)*N);
	}
	else
	{
		PX_memset(tuning->fix,0,sizeof(px_double)*PX_TUNING_MAX_FRAME_SIZE);
	}


}

px_void PX_TuningSortSpectrum(PX_Tuning *tuning,px_complex AnalysisFrame[])
{
	px_int i;
	px_double amplitude=0,phase=0;
	PX_FFT(AnalysisFrame,AnalysisFrame,tuning->N);

	//re,im to amplitude,phase

	//DC sort
	if (AnalysisFrame[0].re>0)
	{
		AnalysisFrame[0].im=0;
	}
	else
	{
		AnalysisFrame[0].re=PX_ABS(AnalysisFrame[0].re);
		AnalysisFrame[0].im=PX_PI;
	}

	//others
	for (i=1;i<tuning->N;i++)
	{
		amplitude=PX_sqrtd(AnalysisFrame[i].re*AnalysisFrame[i].re+AnalysisFrame[i].im*AnalysisFrame[i].im);
		phase=PX_atan2(AnalysisFrame[i].im,AnalysisFrame[i].re);
		AnalysisFrame[i].re=amplitude;
		AnalysisFrame[i].im=phase;
	}

	if (AnalysisFrame[tuning->N/2].re>0)
	{
		AnalysisFrame[tuning->N/2].im=0;
	}
	else
	{
		AnalysisFrame[tuning->N/2].re=PX_ABS(AnalysisFrame[0].re);
		AnalysisFrame[tuning->N/2].im=PX_PI;
	}
}



px_int PX_TuningFilter(PX_Tuning *tuning,_IN px_double frame_unit[],px_int Size,_OUT px_double out_unit[])
{
	px_int oft=0,i=0,remainCount=0,outSize=0,resampleAllocSize=0;
	px_double AmpAvg;
	px_double ZCR;
	px_int blockCount;
	px_int blocksize;
	px_complex *AnalysisFrame;
	px_complex *Frame0;
	px_complex *Frame1;
	px_complex *Frame2;
	 
	px_double *resampledFrame;

	blocksize=tuning->N/2;
	AnalysisFrame=(px_complex *)MP_Malloc(tuning->mp,tuning->N*sizeof(px_complex));

	Frame0=(px_complex *)MP_Malloc(tuning->mp,tuning->N*sizeof(px_complex));
	Frame1=(px_complex *)MP_Malloc(tuning->mp,tuning->N*sizeof(px_complex));
	Frame2=(px_complex *)MP_Malloc(tuning->mp,tuning->N*sizeof(px_complex));
	
	if (tuning->pitchShift<1)
	{
		resampleAllocSize+=(px_int)(Size/tuning->pitchShift);
	}
	else
	{
		resampleAllocSize+=(Size);
	}
	
	resampleAllocSize+=tuning->remainCount;
	resampleAllocSize+=blocksize*3;
	resampleAllocSize+=(px_int)((1-PX_FRAC(tuning->previousInterpolationOffset))/tuning->interpolationFactor);
	resampleAllocSize+=1;
	resampleAllocSize*=sizeof(px_double);
	resampledFrame=(px_double *)MP_Malloc(tuning->mp,resampleAllocSize);

	//copy previous 
	PX_memcpy(resampledFrame+oft,tuning->previous_frame,sizeof(px_double)*blocksize*3);
	oft+=blocksize*3;

	//copy remain
	PX_memcpy(resampledFrame+oft,tuning->remian,sizeof(px_double)*tuning->remainCount);
	oft+=tuning->remainCount;
	
	ZCR=PX_ZeroCrossingRate(frame_unit,Size);
	AmpAvg=0;
	for (i=0;i<Size;i++)
	{
		AmpAvg+=PX_ABS(frame_unit[i]);
	}
	AmpAvg/=Size;

	if (ZCR<tuning->ZCR_Low||ZCR>tuning->ZCR_High||AmpAvg<tuning->amp_threshold)
	{
		for (i=0;i<Size;i++)
		{
			resampledFrame[oft]=frame_unit[i];
			oft++;
		}
	}
	else
	{
		//Re-sample
		while (PX_TRUE)
		{
			px_int index;
			px_double d1=0,d2=0,dm,fraction=0;
			tuning->previousInterpolationOffset+=tuning->interpolationFactor;

			if (tuning->previousInterpolationOffset>Size)
			{
				tuning->previousInterpolationOffset-=tuning->interpolationFactor;
				tuning->previousInterpolationOffset=tuning->previousInterpolationOffset-Size;
				break;
			}

			if (oft>PX_TUNING_MAX_EXECUTE_SIZE-1)
			{
				break;
			}

			index=(px_int)tuning->previousInterpolationOffset;

			if (index)
			{
				d1=frame_unit[index-1];
			}
			else
			{
				d1=tuning->previousInterpolationValue;
			}

			d2=frame_unit[index];

			fraction=tuning->previousInterpolationOffset-(px_int)tuning->previousInterpolationOffset;

			dm=d1+fraction*(d2-d1);

			resampledFrame[oft]=dm;

			oft++;
		}
	}
	

	tuning->previousInterpolationValue=frame_unit[Size-1];


	//spectrum
	blockCount=oft/blocksize;
	remainCount=oft%blocksize;

	//save remain
	PX_memcpy(tuning->remian,resampledFrame+oft-remainCount,remainCount*sizeof(px_double));
	tuning->remainCount=remainCount;

	while (PX_TRUE)
	{
		px_double d1=0,d2=0,dm,fraction=0;
		px_int index;
		px_double unitMaxAmp=0;

		if (tuning->spectrumInterpolationOffset>=blockCount-1)
		{
			PX_memcpy(tuning->previous_frame,resampledFrame+(blockCount-3)*blocksize,blocksize*3*sizeof(px_double));
			tuning->spectrumInterpolationOffset=tuning->spectrumInterpolationOffset+3-blockCount;
			break;
		}

		index=(px_int)tuning->spectrumInterpolationOffset;
		
		//frame0

		for (i=0;i<tuning->N;i++)
		{
			AnalysisFrame[i].re=*(resampledFrame+(index-2)*blocksize+i);
			//apply window
			AnalysisFrame[i].re*=tuning->window[i];
			AnalysisFrame[i].im=0;
		}
		
		PX_TuningSortSpectrum(tuning,AnalysisFrame);
		PX_memcpy(Frame0,AnalysisFrame,tuning->N*sizeof(px_complex));

		//frame1

		for (i=0;i<tuning->N;i++)
		{
			AnalysisFrame[i].re=*(resampledFrame+(index-1)*blocksize+i);
			//apply window
			AnalysisFrame[i].re*=tuning->window[i];
			AnalysisFrame[i].im=0;
		}

		PX_TuningSortSpectrum(tuning,AnalysisFrame);
		PX_memcpy(Frame1,AnalysisFrame,tuning->N*sizeof(px_complex));
			
		//frame2
		for (i=0;i<tuning->N;i++)
		{
			AnalysisFrame[i].re=*(resampledFrame+(index)*blocksize+i);
			//apply window
			AnalysisFrame[i].re*=tuning->window[i];
			AnalysisFrame[i].im=0;
		}

		PX_TuningSortSpectrum(tuning,AnalysisFrame);
		PX_memcpy(Frame2,AnalysisFrame,tuning->N*sizeof(px_complex));


		//instantaneous frequency
		for (i=0;i<=tuning->N/2;i++)
		{
			Frame2[i].im-=Frame1[i].im;
			if (Frame2[i].im>PX_PI)
			{
				Frame2[i].im-=PX_PI*2;
			}
			else if(Frame2[i].im<-PX_PI)
			{
				Frame2[i].im+=PX_PI*2;
			}

			Frame1[i].im-=Frame0[i].im;

			if (Frame1[i].im>PX_PI)
			{
				Frame1[i].im-=PX_PI*2;
			}
			else if(Frame1[i].im<-PX_PI)
			{
				Frame1[i].im+=PX_PI*2;
			}
		}
	
		fraction=tuning->spectrumInterpolationOffset-(px_int)tuning->spectrumInterpolationOffset;

		for (i=0;i<=tuning->N/2;i++)
		{
			px_double distance;
			
			//amplitude
			d1=Frame1[i].re;
			d2=Frame2[i].re;
			dm=d1+fraction*(d2-d1);
			AnalysisFrame[i].re=dm;

			//phase
			d1=Frame1[i].im;
			d2=Frame2[i].im;
			
			distance=d2-d1;

			if (distance<-PX_PI)
			{
				d2+=PX_PI*2;
			}
			else if (distance>PX_PI)
			{
				d2-=PX_PI*2;
			}
			

			dm=d1+fraction*(d2-d1);
			dm=tuning->lastphase[i]+dm;

			if (dm>PX_PI)
			{
				dm-=PX_PI*2;
			}
			else if(dm<-PX_PI)
			{
				dm+=PX_PI*2;
			}

			
			//rebuild signal
			AnalysisFrame[i].im=dm;

			//update last phase
			tuning->lastphase[i]=dm;
		}
		
		AmpAvg=0;

		for (i=0;i<=tuning->N/2;i++)
		{
			AmpAvg+=AnalysisFrame[i].re;
		}

		AmpAvg/=(tuning->N/2+1);



		
		//filter & fix
		for (i=0;i<=tuning->N/2;i++)
		{
			px_double amp,phase;
			amp=AnalysisFrame[i].re;
			phase=AnalysisFrame[i].im;

			//filter
			amp*=tuning->filter[i];
			amp+=(tuning->fix[i]*AmpAvg);


			if (amp<0)
			{
				amp=0;
			}

			//amp,phase->re,im
			AnalysisFrame[i].re=amp*PX_cosd(phase);
			AnalysisFrame[i].im=amp*PX_sind(phase);
		}


		AnalysisFrame[0].im=0;
		AnalysisFrame[tuning->N/2].im=0;
		PX_FT_Symmetry(AnalysisFrame,AnalysisFrame,tuning->N);
		PX_IFFT(AnalysisFrame,AnalysisFrame,tuning->N);
		
		for (i=0;i<blocksize;i++)
		{
			out_unit[outSize]=AnalysisFrame[i].re*tuning->window[i]+tuning->half_previous_out[i]*tuning->window[i+blocksize];
			outSize++;
			tuning->half_previous_out[i]=AnalysisFrame[i+tuning->N/2].re;
		}

		tuning->spectrumInterpolationOffset+=tuning->spectrumInterpolationFactor;
	}

	MP_Free(tuning->mp,Frame0);
	MP_Free(tuning->mp,resampledFrame);
	MP_Free(tuning->mp,Frame1);
	MP_Free(tuning->mp,Frame2);
	MP_Free(tuning->mp,AnalysisFrame);
	return outSize;
}

px_void PX_TuningTimeScale(px_double timescale,px_double in[],px_int count,px_memory *out)
{
	px_int i,index,blockcount,blocksize;
	px_double d1,d2,dm,fraction;
	px_double amplitude,phase,lastphase[PX_TUNING_DEFAULT_FRAME_SIZE];
	px_double lastwindow[PX_TUNING_DEFAULT_FRAME_SIZE/2];
	px_complex Frame0[PX_TUNING_DEFAULT_FRAME_SIZE];
	px_complex Frame1[PX_TUNING_DEFAULT_FRAME_SIZE];
	px_complex Frame2[PX_TUNING_DEFAULT_FRAME_SIZE];
	px_complex AnalysisFrame[PX_TUNING_DEFAULT_FRAME_SIZE];
	px_double window[PX_TUNING_DEFAULT_FRAME_SIZE];
	px_double step=0;
	px_double offset=0;;

	PX_memset(lastphase,0,sizeof(lastphase));
	PX_memset(lastwindow,0,sizeof(lastwindow));
	PX_memset(Frame0,0,sizeof(Frame0));
	PX_memset(Frame1,0,sizeof(Frame1));
	PX_memset(Frame2,0,sizeof(Frame2));
	PX_memset(AnalysisFrame,0,sizeof(AnalysisFrame));

	PX_WindowFunction_sinc(window,PX_TUNING_DEFAULT_FRAME_SIZE);



	step=1/timescale;
	offset=2.0;
	blocksize=PX_TUNING_DEFAULT_FRAME_SIZE/2;
	blockcount=count/blocksize;

	while (PX_TRUE)
	{
		index=(px_int)offset;
		if (index>=blockcount-1)
		{
			break;
		}

		//Frame 0
			for (i=0;i<PX_TUNING_DEFAULT_FRAME_SIZE;i++)
			{
				Frame0[i].re=in[(index-2)*blocksize+i]*window[i];
				Frame0[i].im=0;
			}
		//Frame 1
			for (i=0;i<PX_TUNING_DEFAULT_FRAME_SIZE;i++)
			{
				Frame1[i].re=in[(index-1)*blocksize+i]*window[i];
				Frame1[i].im=0;
			}
		//Frame 2
			for (i=0;i<PX_TUNING_DEFAULT_FRAME_SIZE;i++)
			{
				Frame2[i].re=in[(index)*blocksize+i]*window[i];
				Frame2[i].im=0;
			}

		PX_FFT(Frame0,Frame0,PX_TUNING_DEFAULT_FRAME_SIZE);
		PX_FFT(Frame1,Frame1,PX_TUNING_DEFAULT_FRAME_SIZE);
		PX_FFT(Frame2,Frame2,PX_TUNING_DEFAULT_FRAME_SIZE);

		//real im->amplitude phase
		if (Frame0[0].re>=0)
		{
			Frame0[0].im=0;
		}
		else
		{
			Frame0[0].re=-Frame0[0].re;
			Frame0[0].im=PX_PI;
		}
		
		for (i=1;i<PX_TUNING_DEFAULT_FRAME_SIZE/2;i++)
		{
			amplitude=PX_sqrtd(Frame0[i].re*Frame0[i].re+Frame0[i].im*Frame0[i].im);
			phase=PX_atan2(Frame0[i].im,Frame0[i].re);
			Frame0[i].re=amplitude;
			Frame0[i].im=phase;
		}

		
		if (Frame0[i].re>=0)
		{
			Frame0[i].im=0;
		}
		else
		{
			Frame0[i].re=-Frame0[i].re;
			Frame0[i].im=PX_PI;
		}



		
		Frame1[0].im=0;
		if (Frame1[0].re>=0)
		{
			Frame1[0].im=0;
		}
		else
		{
			Frame1[0].re=-Frame1[0].re;
			Frame1[0].im=PX_PI;
		}

		for (i=1;i<PX_TUNING_DEFAULT_FRAME_SIZE/2;i++)
		{
			amplitude=PX_sqrtd(Frame1[i].re*Frame1[i].re+Frame1[i].im*Frame1[i].im);
			phase=PX_atan2(Frame1[i].im,Frame1[i].re);
			Frame1[i].re=amplitude;
			Frame1[i].im=phase;
		}
		
		if (Frame1[i].re>=0)
		{
			Frame1[i].im=0;
		}
		else
		{
			Frame1[i].re=-Frame1[i].re;
			Frame1[i].im=PX_PI;
		}


		
		Frame2[0].im=0;
		if (Frame2[0].re>=0)
		{
			Frame2[0].im=0;
		}
		else
		{
			Frame2[0].re=-Frame2[0].re;
			Frame2[0].im=PX_PI;
		}

		for (i=1;i<PX_TUNING_DEFAULT_FRAME_SIZE/2;i++)
		{
			amplitude=PX_sqrtd(Frame2[i].re*Frame2[i].re+Frame2[i].im*Frame2[i].im);
			phase=PX_atan2(Frame2[i].im,Frame2[i].re);
			Frame2[i].re=amplitude;
			Frame2[i].im=phase;
		}

		
		if (Frame2[i].re>=0)
		{
			Frame2[i].im=0;
		}
		else
		{
			Frame2[i].re=-Frame2[i].re;
			Frame2[i].im=PX_PI;
		}

		//Instantaneous Frequency
		for (i=0;i<=blocksize;i++)
		{
			Frame2[i].im-=Frame1[i].im;
			if (Frame2[i].im<-PX_PI)
			{
				Frame2[i].im+=PX_PI*2;
			}
			else if (Frame2[i].im>PX_PI)
			{
				Frame2[i].im-=PX_PI*2;
			}

			Frame1[i].im-=Frame0[i].im;
			if (Frame1[i].im<-PX_PI)
			{
				Frame1[i].im+=PX_PI*2;
			}
			else if (Frame1[i].im>PX_PI)
			{
				Frame1[i].im-=PX_PI*2;
			}
		}

		fraction=PX_FRAC(offset);

		for (i=0;i<=blocksize;i++)
		{
			px_double distance;
			px_double amp,phase;

			//amplitude
			d1=Frame1[i].re;
			d2=Frame2[i].re;
			dm=d1+fraction*(d2-d1);
			AnalysisFrame[i].re=dm;

			//phase
			d1=Frame1[i].im;
			d2=Frame2[i].im;

			distance=d2-d1;

			if (distance<-PX_PI)
			{
				d2+=PX_PI*2;
			}
			else if (distance>PX_PI)
			{
				d2-=PX_PI*2;
			}


			dm=d1+fraction*(d2-d1);
			dm=lastphase[i]+dm;

			if (dm>PX_PI)
			{
				dm-=PX_PI*2;
			}
			else if(dm<-PX_PI)
			{
				dm+=PX_PI*2;
			}


			//rebuild signal
			AnalysisFrame[i].im=dm;

			//update last phase
			lastphase[i]=dm;

			amp=AnalysisFrame[i].re;
			phase=AnalysisFrame[i].im;


			//amplitude phase->real imaginary
			AnalysisFrame[i].re=amp*PX_cosd(phase);
			AnalysisFrame[i].im=amp*PX_sind(phase);

		}
		AnalysisFrame[0].im=0;
		AnalysisFrame[blocksize].im=0;
		PX_FT_Symmetry(AnalysisFrame,AnalysisFrame,PX_TUNING_DEFAULT_FRAME_SIZE);

		PX_IFFT(AnalysisFrame,AnalysisFrame,PX_TUNING_DEFAULT_FRAME_SIZE);

		for (i=0;i<blocksize;i++)
		{
			px_double val=AnalysisFrame[i].re*window[i]+lastwindow[i]*window[i+blocksize];
			lastwindow[i]=AnalysisFrame[i+blocksize].re;
			if (val>=0.999999)
			{
				val=0.999999;
			}
			PX_MemoryCat(out,&val,sizeof(px_double));
		}

		offset+=step;
	}

}

px_void PX_TuningSetPitchShift(PX_Tuning *tuning,px_double pitchShift)
{
	tuning->pitchShift=pitchShift;
	tuning->spectrumInterpolationFactor=1/(pitchShift*tuning->timeScale);
	tuning->interpolationFactor=pitchShift;
}

px_void PX_TuningSetTimeScale(PX_Tuning *tuning,px_double TimeScale)
{
	tuning->timeScale=TimeScale;
}

px_void PX_TuningSetFilter(PX_Tuning *tuning,px_double filter[])
{
	PX_memcpy(tuning->filter,filter,sizeof(px_double)*tuning->N);
}

px_void PX_TuningSetFix(PX_Tuning *tuning,px_double fix[])
{
	PX_memcpy(tuning->fix,fix,sizeof(px_double)*tuning->N);
}

px_void PX_TuningSetZCR(PX_Tuning *tuning,px_double low,px_double high)
{
	tuning->ZCR_Low=low;
	tuning->ZCR_High=high;
}
