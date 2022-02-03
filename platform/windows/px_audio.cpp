
#include <Windows.h>
#include <dsound.h>
#pragma comment (lib,"dsound.lib")

extern "C"
{
	#include "../../core/PX_Sound.h"
	int PX_AudioInitialize(PX_SoundPlay *soundPlay);
	int PX_AudioInitializeEx();
	void PX_AudioSetVolume( unsigned int Vol );
	void PX_AudioPlay( unsigned long Vol );
	int  PX_AudioGetStandbyBufferSize();
	int  PX_AudioWriteBuffer(void *pBuffer,size_t Size );
	BOOL PX_AudioCaptureOpen(GUID dev_id,int channel);
	void PX_AudioCaptureClose();
	DWORD PX_AudioCaptureReadEx(void *pBuffer,px_int buffersize,px_int align);
	DWORD PX_AudioCaptureRead(void *buffer,px_int buffersize);
	LPDIRECTSOUNDBUFFER PX_AudioGetDirectSoundBuffer();
};

#define DSOUND_BUFFER_SIZE (1764*16)

static LPDIRECTSOUNDBUFFER DSound_lpdbsBuffer;
static DSBUFFERDESC DSound_dsbd;
static LPDIRECTSOUND DSound_lpds;
static WAVEFORMATEX DSound_waveformat;
static PX_SoundPlay *DSound_soundplay;
static px_byte DSound_data[DSOUND_BUFFER_SIZE];
volatile static DWORD DSound_wPosition=0;



DWORD  _stdcall DEMO_DSoundProc(LPVOID p)
{
	px_dword wsize=0;

	while (!DSound_soundplay)
	{
		Sleep(10);
	}

	while(1)
	{
		wsize=PX_AudioGetStandbyBufferSize();

		if (PX_SoundPlayGetDataCount(DSound_soundplay))
		{
			
			if (wsize>DSOUND_BUFFER_SIZE)
			{
				wsize=DSOUND_BUFFER_SIZE;
			}
			PX_SoundPlayRead(DSound_soundplay,DSound_data,wsize);
			PX_AudioWriteBuffer(DSound_data,wsize);
		}
		else
		{
			memset(DSound_data,0,DSOUND_BUFFER_SIZE);
			PX_AudioWriteBuffer(DSound_data,wsize);
		}
		Sleep(10);
	}
}

extern HWND	Win_Hwnd;
int PX_AudioInitialize(PX_SoundPlay *soundPlay)
{
	DSound_soundplay=soundPlay;
	return 1;
}
int PX_AudioInitializeEx()
{
	VOID* pDSLockedBuffer =NULL;
	DWORD dwDSLockedBufferSize =0; 
	HANDLE hThread;
	DWORD  threadId;
	HWND hwnd=Win_Hwnd;
	BOOL startThread=TRUE;
	/*Sound Play*/
	if (!DSound_soundplay)
	{
		return 1;
	}

	//DSound created
	if(DSound_lpds==NULL)
	{
		if(DirectSoundCreate(NULL,&DSound_lpds,NULL) == DS_OK)
		{
			if(DSound_lpds ->SetCooperativeLevel(hwnd,DSSCL_NORMAL)!=DS_OK)
			{
				return FALSE;
			}
		}
	}

	
	///////////////////////////////////////////////////////////////////////////
	//Create Sound buffer
	

	DSound_waveformat.wFormatTag = WAVE_FORMAT_PCM;
	DSound_waveformat.nSamplesPerSec = 44100;//44100;
	DSound_waveformat.wBitsPerSample = 16;//
	DSound_waveformat.nChannels = 2;//
	DSound_waveformat.nBlockAlign = (DSound_waveformat.wBitsPerSample >> 3) * DSound_waveformat.nChannels;;
	DSound_waveformat.nAvgBytesPerSec = DSound_waveformat.nBlockAlign * DSound_waveformat.nSamplesPerSec;
	DSound_waveformat.cbSize = 0;

	memset(&DSound_dsbd,0,sizeof(DSBUFFERDESC));
	DSound_dsbd.dwSize = sizeof(DSBUFFERDESC);
	DSound_dsbd.dwFlags=DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY |DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_CTRLVOLUME ;
	DSound_dsbd.dwBufferBytes =DSOUND_BUFFER_SIZE;
	DSound_dsbd.lpwfxFormat =&DSound_waveformat; 
	DSound_dsbd.dwReserved=0;
	

	HRESULT hr;
	if(FAILED(hr=DSound_lpds ->CreateSoundBuffer(&DSound_dsbd,&DSound_lpdbsBuffer,NULL)))
	{
		return FALSE;
	}

	PX_AudioSetVolume(0);

	if(DSound_lpdbsBuffer ->Lock(0,DSOUND_BUFFER_SIZE,&pDSLockedBuffer,&dwDSLockedBufferSize,NULL,NULL,0))
		return FALSE;

	memset(pDSLockedBuffer,0,dwDSLockedBufferSize); 

	DSound_lpdbsBuffer->Unlock(pDSLockedBuffer,dwDSLockedBufferSize,NULL,0);
	DSound_lpdbsBuffer->Play(0,0,DSBPLAY_LOOPING);

	if(startThread)
		hThread = CreateThread(NULL, 0, DEMO_DSoundProc, 0, 0, &threadId);

	return TRUE;
}

LPDIRECTSOUNDBUFFER PX_AudioGetDirectSoundBuffer()
{
	return DSound_lpdbsBuffer;
}



void PX_AudioSetVolume( unsigned int Vol )
{
	DSound_lpdbsBuffer->SetVolume(Vol);
}

void PX_AudioPlay( unsigned long Vol )
{
	DSound_lpdbsBuffer->Play(0,0,DSBPLAY_LOOPING);
}


int PX_AudioGetStandbyBufferSize()
{
	DWORD PlayCursor,__w;

	if (FAILED(DSound_lpdbsBuffer->GetCurrentPosition(&PlayCursor,&__w)))
	{
		return 0;
	}

	if (DSound_wPosition>PlayCursor)
	{
		return DSOUND_BUFFER_SIZE-(DSound_wPosition-PlayCursor);
	}
	else
	{
		return PlayCursor-DSound_wPosition;
	}
}

int PX_AudioWriteBuffer(void *pBuffer,size_t Size )
{
	DWORD wbuf1_size,wbuf2_size;
	VOID* pDSLockedBuffer =NULL;
	DWORD dwDSLockedBufferSize =0; 

	if (Size>DSOUND_BUFFER_SIZE)
	{
		PX_ASSERT();
	}

	wbuf1_size=DSOUND_BUFFER_SIZE-DSound_wPosition;
	if (wbuf1_size>Size)
	{
		wbuf1_size= (DWORD)Size;
	}

	Size-=wbuf1_size;

	if(DSound_lpdbsBuffer ->Lock(DSound_wPosition,wbuf1_size,&pDSLockedBuffer,&dwDSLockedBufferSize,NULL,NULL,0L))
		return FALSE;

	memcpy(pDSLockedBuffer,pBuffer,dwDSLockedBufferSize); 

	DSound_lpdbsBuffer->Unlock(pDSLockedBuffer,dwDSLockedBufferSize,NULL,0);

	DSound_wPosition+=wbuf1_size;
	
	if (DSound_wPosition>=DSOUND_BUFFER_SIZE)
	{
		DSound_wPosition=0;
	}

	if (Size)
	{
		pBuffer=((char *)pBuffer+wbuf1_size);

		wbuf2_size=(DWORD)Size;

		if(DSound_lpdbsBuffer ->Lock(0,wbuf2_size,&pDSLockedBuffer,&dwDSLockedBufferSize,NULL,NULL,0L))
			return FALSE;

		memcpy(pDSLockedBuffer,pBuffer,dwDSLockedBufferSize); 

		DSound_lpdbsBuffer->Unlock(pDSLockedBuffer,dwDSLockedBufferSize,NULL,0);
		
		DSound_wPosition+=wbuf2_size;
	}

	
	return (DWORD)Size;
}

#define DSOUNDCAPTURE_CHANNEL  1
#define DSOUNDCAPTURE_SAMPLERATE 44100
#define DSOUNDCAPTURE_BITS 16
#define DSOUNDCAPTURE_BUFFER_SIZE (DSOUNDCAPTURE_SAMPLERATE*DSOUNDCAPTURE_CHANNEL*DSOUNDCAPTURE_BITS/8*2) //2 second cache buffer

static LPDIRECTSOUNDCAPTURE8    DSoundCapture_pCapDev ;   //capture device ptr
static LPDIRECTSOUNDCAPTUREBUFFER DSoundCapture_pCapBuf=PX_NULL ;   //capture loop buffer ptr
static GUID    DSoundCapture_guidCapDevId ;  //capture device id
static WAVEFORMATEX      DSoundCapture_wfxInput;   //input wave format description struct
volatile static DWORD DSoundCapture_RPosition;

BOOL PX_AudioCaptureOpen(GUID dev_id,int channel)
{
	HRESULT hr ; 

	DSoundCapture_guidCapDevId = dev_id ;
	
	if (DSoundCapture_pCapBuf||DSoundCapture_pCapDev)
	{
		PX_AudioCaptureClose();
	}


	// Create IDirectSoundCapture using the preferred capture device
	hr = DirectSoundCaptureCreate(&DSoundCapture_guidCapDevId, &DSoundCapture_pCapDev, NULL ) ;
	// init wave format 

	if (FAILED(hr))  return PX_FALSE; 

	// get the default capture wave formate 
	ZeroMemory(&DSoundCapture_wfxInput, sizeof(WAVEFORMATEX)) ; 
	DSoundCapture_wfxInput.wFormatTag = WAVE_FORMAT_PCM;
	// 8KHz, 16 bits PCM, Mono
	DSoundCapture_wfxInput.nSamplesPerSec = 44100 ; 
	DSoundCapture_wfxInput.wBitsPerSample = 16 ; 
	DSoundCapture_wfxInput.nChannels  = channel ;
	DSoundCapture_wfxInput.nBlockAlign = DSoundCapture_wfxInput.nChannels * ( DSoundCapture_wfxInput.wBitsPerSample / 8 ) ; 
	DSoundCapture_wfxInput.nAvgBytesPerSec = DSoundCapture_wfxInput.nBlockAlign * DSoundCapture_wfxInput.nSamplesPerSec;

	//Create CaptureBuffer
	DSCBUFFERDESC dscbd;

	// Create the capture buffer
	ZeroMemory( &dscbd, sizeof(dscbd) );
	dscbd.dwSize        = sizeof(dscbd);
	dscbd.dwBufferBytes = DSOUNDCAPTURE_BUFFER_SIZE;
	dscbd.lpwfxFormat   = &DSoundCapture_wfxInput ; // Set the format during creatation

	

	if( FAILED( hr = DSoundCapture_pCapDev->CreateCaptureBuffer( &dscbd, &DSoundCapture_pCapBuf, NULL ) ) )
		return PX_FALSE ;

	DSoundCapture_RPosition=0;

	DSoundCapture_pCapBuf->Start(DSCBSTART_LOOPING);

	return PX_TRUE;

}

#include "stdio.h"
DWORD PX_AudioCaptureReadEx(void *pBuffer,px_int buffersize,px_int align)
{
	DWORD rbuf1_size,rbuf2_size,Size=0;
	VOID* pDSLockedBuffer =NULL;
	DWORD dwDSLockedBufferSize =0; 

	DWORD RecorderCursor,__r;
	DWORD ReadySize;

	if (FAILED(DSoundCapture_pCapBuf->GetCurrentPosition(&RecorderCursor,&__r)))
	{
		return 0;
	}
	
	if (DSoundCapture_RPosition>RecorderCursor)
	{
		ReadySize= DSOUNDCAPTURE_BUFFER_SIZE-(DSoundCapture_RPosition-RecorderCursor);
	}
	else
	{
		ReadySize= RecorderCursor-DSoundCapture_RPosition;
	}

	if ((DWORD)buffersize<ReadySize)
	{
		ReadySize=buffersize;
	}

	ReadySize=(ReadySize/align)*align;


	if (ReadySize==0)
	{
		return 0;
	}

	rbuf1_size=DSOUNDCAPTURE_BUFFER_SIZE-DSoundCapture_RPosition;

	if (rbuf1_size>ReadySize)
	{
		rbuf1_size=ReadySize;
	}

	ReadySize-=rbuf1_size;

	if(DSoundCapture_pCapBuf ->Lock(DSoundCapture_RPosition,rbuf1_size,&pDSLockedBuffer,&dwDSLockedBufferSize,NULL,NULL,0L))
		return FALSE;

	memcpy(pBuffer,pDSLockedBuffer,dwDSLockedBufferSize); 
	Size+=dwDSLockedBufferSize;
	DSoundCapture_pCapBuf->Unlock(pDSLockedBuffer,dwDSLockedBufferSize,NULL,0);

	DSoundCapture_RPosition+=rbuf1_size;

	if (DSoundCapture_RPosition>=DSOUNDCAPTURE_BUFFER_SIZE)
	{
		DSoundCapture_RPosition=0;
	}

	if (ReadySize)
	{
		pBuffer=((char *)pBuffer+rbuf1_size);

		rbuf2_size=ReadySize;

		if(DSoundCapture_pCapBuf ->Lock(0,rbuf2_size,&pDSLockedBuffer,&dwDSLockedBufferSize,NULL,NULL,0L))
			return FALSE;

		memcpy(pBuffer,pDSLockedBuffer,dwDSLockedBufferSize); 

		Size+=dwDSLockedBufferSize;
		DSoundCapture_pCapBuf->Unlock(pDSLockedBuffer,dwDSLockedBufferSize,NULL,0);

		DSoundCapture_RPosition+=rbuf2_size;
	}
	return Size;
}



DWORD PX_AudioCaptureRead(void *pBuffer,px_int buffersize)
{
	return PX_AudioCaptureReadEx(pBuffer,buffersize,1);
}

void PX_AudioCaptureClose()
{
	if (DSoundCapture_pCapBuf)
	{
		DSoundCapture_pCapBuf->Stop();
		DSoundCapture_pCapBuf->Release();
		DSoundCapture_pCapBuf=PX_NULL;
	}
	if (DSoundCapture_pCapDev)
	{
		DSoundCapture_pCapDev->Release();
		DSoundCapture_pCapDev=PX_NULL;
	}
	
	
	DSoundCapture_RPosition=0;
}
