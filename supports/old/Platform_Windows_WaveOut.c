#include <Windows.h>
#include "../../core/PX_Sound.h"


static HWAVEOUT main_hWaveOut;
static HANDLE main_waveEvent;
static PX_SoundPlay *main_soundplay=NULL;

#define MAIN_WAVEBUFFER_BLOCK_LEN 1764
#define MAIN_WAVEBUFFER_BLOCK_COUNT 8

static unsigned char main_wavebuffer[MAIN_WAVEBUFFER_BLOCK_LEN*MAIN_WAVEBUFFER_BLOCK_COUNT]={0};

DWORD  _stdcall DEMO_waveOutProc(LPVOID p)
{
	WAVEHDR waveheader;
	px_int QueueSize=0;
	px_int wIndex=0;
	
	while (!main_soundplay)
	{
		Sleep(10);
	}

	while(1)
	{
		if (!PX_SoundPlayGetDataCount(main_soundplay))
		{
			QueueSize=0;
		}
		while (QueueSize<MAIN_WAVEBUFFER_BLOCK_COUNT)
		{
			px_byte *data=main_wavebuffer+wIndex*MAIN_WAVEBUFFER_BLOCK_LEN;
			PX_SoundPlayRead(main_soundplay,data,MAIN_WAVEBUFFER_BLOCK_LEN);
			waveheader.lpData=(LPSTR)data;
			waveheader.dwBufferLength=MAIN_WAVEBUFFER_BLOCK_LEN;
			waveheader.dwBytesRecorded=0;
			waveheader.dwUser=0;
			waveheader.dwFlags=WHDR_PREPARED;
			waveheader.dwLoops=0;
			waveheader.lpNext=PX_NULL;
			waveheader.reserved=PX_NULL;
			waveOutWrite (main_hWaveOut, &waveheader, sizeof (WAVEHDR)) ;
			QueueSize++;
			wIndex++;
			if (wIndex>=MAIN_WAVEBUFFER_BLOCK_COUNT)
			{
				wIndex=0;
			}
		}

		WaitForSingleObject(main_waveEvent,INFINITE);
		ResetEvent(main_waveEvent);
		if(QueueSize>0)
			QueueSize--;		
	}
}


BOOL PX_AudioInitialize(HWND hwnd,PX_SoundPlay *soundPlay)
{
	WAVEFORMATEX waveformat;
	HANDLE hThread;
	DWORD  threadId;

	waveformat.wFormatTag = WAVE_FORMAT_PCM;
	waveformat.nSamplesPerSec = 44100;//44100;
	waveformat.wBitsPerSample = 16;//
	waveformat.nChannels = 2;//
	waveformat.nBlockAlign = (waveformat.wBitsPerSample >> 3) * waveformat.nChannels;;
	waveformat.nAvgBytesPerSec = waveformat.nBlockAlign * waveformat.nSamplesPerSec;
	waveformat.cbSize = 0;

	main_waveEvent= CreateEvent(NULL,FALSE,FALSE,"PCM_DONE");

	if(FAILED(waveOutOpen(&main_hWaveOut,WAVE_MAPPER,&waveformat,(DWORD_PTR)main_waveEvent,(DWORD_PTR)NULL,CALLBACK_EVENT)))
		return FALSE;

	hThread = CreateThread(NULL, 0, DEMO_waveOutProc, 0, 0, &threadId);

	main_soundplay=soundPlay;
	return TRUE;
}


void PX_AudioSetVolume(DWORD volume)
{
	waveOutSetVolume(main_hWaveOut,volume);
}
