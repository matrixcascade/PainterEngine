#include "PX_SoundModule.h"

px_void PX_SoundModuleSortSpectrum(PX_SoundModule *SoundModule,px_complex AnalysisFrame[])
{
	px_int i;
	px_double amplitude=0,phase=0;
	PX_FFT(AnalysisFrame,AnalysisFrame,PX_SOUNDMODULE_N);

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
	for (i=1;i<PX_SOUNDMODULE_N/2;i++)
	{
		amplitude=PX_sqrtd(AnalysisFrame[i].re*AnalysisFrame[i].re+AnalysisFrame[i].im*AnalysisFrame[i].im);
		phase=PX_atan2(AnalysisFrame[i].im,AnalysisFrame[i].re);
		AnalysisFrame[i].re=amplitude;
		AnalysisFrame[i].im=phase;
	}

	if (AnalysisFrame[PX_SOUNDMODULE_N/2].re>0)
	{
		AnalysisFrame[PX_SOUNDMODULE_N/2].im=0;
	}
	else
	{
		AnalysisFrame[PX_SOUNDMODULE_N/2].re=PX_ABS(AnalysisFrame[0].re);
		AnalysisFrame[PX_SOUNDMODULE_N/2].im=PX_PI;
	}
}


px_void PX_SoundModuleInitialize(px_memorypool *mp,PX_SoundModule *SoundModule,px_int SampleRate,px_double pitchShift,PX_SoundModule_Formant Formant[],px_int FormantCount)
{
	PX_memset(SoundModule,0,sizeof(PX_SoundModule));
	if (FormantCount>PX_COUNTOF(SoundModule->Formant))
	{
		return;
	}
	PX_memcpy(SoundModule->Formant,Formant,FormantCount*sizeof(PX_SoundModule_Formant));
	SoundModule->mp=mp;
	SoundModule->pitchShift=pitchShift;
	SoundModule->SampleRate=SampleRate;
	SoundModule->searchHzW=PX_SOUNDMODULE_DEFALUT_SEARCH_HZ;
	SoundModule->ZCR_High=100;
	SoundModule->Pitch_threshold_High=800;
	SoundModule->amp_max=32768.0;
}

px_int PX_SoundModuleFilter(PX_SoundModule *SoundModule,_IN px_double _in[],px_int count,_OUT px_double _out[])
{
	px_complex Frame[PX_SOUNDMODULE_N];
	px_double  InstantaneousPhase[PX_SOUNDMODULE_N];
	px_double sincWindow[PX_SOUNDMODULE_N];
	px_double ModuleAmplitude[PX_SOUNDMODULE_FORMANT_COUNT];
	px_double  Pitch;
	px_double  ZCR;
	px_double  *Data;
	px_double durHZ=SoundModule->SampleRate*1.0/PX_SOUNDMODULE_N;
	px_int blkCount,dataCount,remian;
	px_int blk;
	px_int blkSize=PX_SOUNDMODULE_N/2;

	dataCount=count+SoundModule->remainCount;

	Data=(px_double *)MP_Malloc(SoundModule->mp,dataCount*sizeof(px_double));

	PX_memcpy(Data,SoundModule->remian,SoundModule->remainCount*sizeof(px_double));
	PX_memcpy(Data+SoundModule->remainCount,_in,count*sizeof(px_double));

	blkCount=dataCount/(blkSize);

	if (blkCount>1)
	{
		remian=dataCount%(blkSize)+blkSize;
		PX_memcpy(SoundModule->remian,Data+(blkCount-1)*blkSize,remian*sizeof(px_double));
		SoundModule->remainCount=remian;
	}
	else
	{
		remian=dataCount;
		PX_memcpy(SoundModule->remian,Data,remian*sizeof(px_double));
		MP_Free(SoundModule->mp,Data);
		return 0;
	}
	
	PX_WindowFunction_sinc(sincWindow,PX_SOUNDMODULE_N);

	for (blk=0;blk<blkCount-1;blk++)
	{
		px_int i;
		for (i=0;i<PX_COUNTOF(Frame);i++)
		{
			Frame[i].re=Data[blk*blkSize+i]*sincWindow[i];
			Frame[i].im=0;
		}

		ZCR=PX_ZeroCrossingRateComplex(Frame,PX_SOUNDMODULE_N);
		Pitch=PX_PitchEstimation(Frame,PX_SOUNDMODULE_N,SoundModule->SampleRate,100,300);

		for (i=0;i<PX_COUNTOF(Frame);i++)
		{
			Frame[i].re=Data[blk*PX_SOUNDMODULE_N+i]/SoundModule->amp_max*sincWindow[i];
			Frame[i].im=0;
		}

		if (Pitch!=0&&!(ZCR<SoundModule->ZCR_Low||ZCR>SoundModule->ZCR_High||Pitch<SoundModule->Pitch_threshold_Low||Pitch>SoundModule->Pitch_threshold_High))
		{
			PX_SoundModuleSortSpectrum(SoundModule,Frame);

			//Instantaneous frequency
			for (i=0;i<blkSize;i++)
			{
				InstantaneousPhase[i]=Frame[i].im-SoundModule->lastPhase[i];
				SoundModule->lastPhase[i]=Frame[i].im;
			}

			//Build sound module
			for (i=0;i<PX_SOUNDMODULE_FORMANT_COUNT;i++)
			{
				px_int HZindex;
				px_double max;
				

				HZindex=(px_int)((Pitch*i-SoundModule->searchHzW/2)/durHZ);

				if (HZindex<0)
				{
					HZindex=0;
				}

				max=Frame[HZindex].re;

				while (PX_TRUE)
				{
					HZindex++;
					if (HZindex>=PX_SOUNDMODULE_FORMANT_COUNT)
					{
						break;
					}
					if (HZindex*durHZ>Pitch*i+SoundModule->searchHzW/2)
					{
						break;
					}
					if (Frame[HZindex].re>max)
					{
						max=Frame[HZindex].re;
					}
				}

				ModuleAmplitude[HZindex]=max;
			}

			//now rebuild signal 
			
			PX_memset(Frame,0,sizeof(Frame));
			for (i=0;i<PX_SOUNDMODULE_FORMANT_COUNT;i++)
			{
				px_int HZindex;
				HZindex=(px_int)((Pitch*i-SoundModule->searchHzW/2)/durHZ);
				if (HZindex<0)
				{
					HZindex=0;
				}

				while (PX_TRUE)
				{
					px_double distanceHZ;
					if (HZindex>=PX_SOUNDMODULE_FORMANT_COUNT)
					{
						break;
					}
					if (HZindex*durHZ>Pitch*i+SoundModule->Formant[i].W/2)
					{
						break;
					}
					distanceHZ=PX_ABS(HZindex*durHZ-Pitch*i);
					Frame[HZindex].re=SoundModule->Formant[i].A*PX_sind(PX_PI/2*(1-PX_ABS(distanceHZ-Pitch*i)/SoundModule->Formant[i].W*2));

					HZindex++;
				}
			}

			//rebuild instantaneous phase
			for (i=0;i<blkSize;i++)
			{
				Frame[i].im=InstantaneousPhase[i];
			}

			//amplitude phase->re im
			for (i=0;i<blkSize;i++)
			{
				px_double amp,phase;
				amp=Frame[i].re;
				phase=Frame[i].im;

				if (amp<0)
				{
					amp=0;
				}
				//amp,phase->re,im
				Frame[i].re=amp*PX_cosd(phase);
				Frame[i].im=amp*PX_sind(phase);
			}

			PX_FT_Symmetry(Frame,Frame,PX_SOUNDMODULE_N);
			PX_IFFT(Frame,Frame,PX_SOUNDMODULE_N);
		}

		for (i=0;i<blkSize;i++)
		{
			(_out+blk*blkSize)[i]=Frame[i].re*sincWindow[i]+SoundModule->halfRebuild[i];
			if ((_out+blk*blkSize)[i]<-1)
			{
				(_out+blk*blkSize)[i]=-1;
			}
			if ((_out+blk*blkSize)[i]>1)
			{
				(_out+blk*blkSize)[i]=1;
			}
			(_out+blk*blkSize)[i]*=SoundModule->amp_max;
			SoundModule->halfRebuild[i]=Frame[i+blkSize].re*sincWindow[i+blkSize];
		}
	}

	MP_Free(SoundModule->mp,Data);

	return blkCount*blkSize;
}

px_void PX_SoundModulePitchShift(PX_SoundModule *SoundModule,px_double pitchShift)
{
	SoundModule->pitchShift=pitchShift;
}

px_void PX_SoundModuleSetZCR(PX_SoundModule *SoundModule,px_double low,px_double high)
{
	SoundModule->ZCR_Low=low;
	SoundModule->ZCR_High=high;
}
