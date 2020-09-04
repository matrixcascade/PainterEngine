#include "PX_TRaw.h"

px_bool PX_TRawVerify(px_void *data,px_int size)
{
	PX_TRaw_Header *pHeader=(PX_TRaw_Header *)data;
	if (size<sizeof(PX_TRaw_Header))
	{
		return PX_FALSE;
	}
	if (pHeader->Magic[0]!='T'||pHeader->Magic[1]!='R'||pHeader->Magic[2]!='A'||pHeader->Magic[3]!='W')
	{
		return PX_FALSE;
	}

	if (size-sizeof(PX_TRaw_Header)<pHeader->Height*pHeader->Width*sizeof(px_color))
	{
		return PX_FALSE;
	} 

	return PX_TRUE;
}

px_int PX_TRawGetWidth(px_void *data)
{
	PX_TRaw_Header *pHeader=(PX_TRaw_Header *)data;
	return pHeader->Width;
}

px_int PX_TRawGetHeight(px_void *data)
{
	PX_TRaw_Header *pHeader=(PX_TRaw_Header *)data;
	return pHeader->Height;
}

px_void PX_TRawRender(px_surface *psurface,px_void *data,px_int x,px_int y)
{
	px_int i,j;
	px_int Height;
	px_int Width;
	px_color  *pdataColor=(px_color *)((px_char *)data+sizeof(PX_TRaw_Header));
	if (x>psurface->width||y>psurface->height)
	{
		return;
	}

	Height=PX_TRawGetHeight(data);
	Width=PX_TRawGetWidth(data);

	if (Height>0&&Width>0)
	{
		if (x<-Width||y<-Height)
		{
			return;
		}
	}
	else
	{
		return;
	}

	for (i=0;i<Height;i++)
	{
		for (j=0;j<Width;j++)
		{
			PX_SurfaceDrawPixel(psurface,j+x,i+y,pdataColor[Width*i+j]);
		}
	}
	

	//Other bit format is no support
	return;
}



px_bool PX_TRawBuild(px_surface *psurface,px_byte *TRawBuffer,px_int *size)
{
	px_int _outsize;
	PX_TRaw_Header header;
	_outsize=psurface->height*psurface->width*sizeof(px_color)+sizeof(header);
	*size=_outsize;
	header.Magic[0]='T';
	header.Magic[1]='R';
	header.Magic[2]='A';
	header.Magic[3]='W';

	header.Height=psurface->height;
	header.Width=psurface->width;

	if (TRawBuffer)
	{
		PX_memcpy(TRawBuffer,&header,sizeof(header));
		PX_memcpy(TRawBuffer+sizeof(header),psurface->surfaceBuffer,psurface->height*psurface->width*sizeof(px_color));
		return PX_TRUE;
	}
	else
		return PX_FALSE;
}

px_uint PX_TRawGetSize(PX_TRaw_Header *header)
{
	return header->Height*header->Width*sizeof(px_color)+sizeof(PX_TRaw_Header);
}
