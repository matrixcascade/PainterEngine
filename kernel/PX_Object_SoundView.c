#include "PX_Object_SoundView.h"

#define SOUNDVIEW_FRAME_SIZE 1024

PX_OBJECT_RENDER_FUNCTION(PX_Object_SoundViewRender)
{
	px_int lheight;
	px_complex frame[SOUNDVIEW_FRAME_SIZE];
	px_double hanning[SOUNDVIEW_FRAME_SIZE];
	px_int16  uframe[SOUNDVIEW_FRAME_SIZE];
	PX_SurfaceLimitInfo linfo;
	PX_Object_SoundView* pDesc = PX_ObjectGetDesc(PX_Object_SoundView, pObject);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);
	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	linfo=PX_SurfaceGetLimit(psurface);

	PX_SurfaceSetLimit(psurface, (px_int)objx, (px_int)objy, (px_int)objWidth, (px_int)objHeight);
	if (PX_SoundPlayReadCurrentPlayingData(pDesc->pSoundPlay, 0, 0, uframe, SOUNDVIEW_FRAME_SIZE))
	{
		px_int i;

		PX_WindowFunction_hanning(hanning, SOUNDVIEW_FRAME_SIZE);
		for (i = 0; i < SOUNDVIEW_FRAME_SIZE; i++)
		{
			frame[i].re = uframe[i] * 1.0 / 32768.0 * hanning[i];
			frame[i].im = 0;
		}

		PX_FFT(frame, frame, SOUNDVIEW_FRAME_SIZE);
		for (i = 0; i < SOUNDVIEW_FRAME_SIZE / 2; i++)
		{
			frame[i].re = PX_sqrtd(frame[i].re * frame[i].re + frame[i].im * frame[i].im) / 50;
		}


		for (i = 0; i < SOUNDVIEW_FRAME_SIZE / 2; i++)
		{
			px_int j;
			lheight = (px_int)(frame[i].re > 1 ? objHeight : objHeight * frame[i].re);
			lheight = (px_int)(lheight * 0.85);
			for (j = 0; j < lheight; j++)
			{
				px_uchar r = (px_uchar)(j * 255 / lheight);
				r = (r > 255 ? 255 : r);
				PX_SurfaceDrawPixel(psurface, (px_int)(objx+ objWidth / 2 + i), (px_int)(objy+ objHeight - 1 - j), PX_COLOR(255 - r, r, 255 - r, 255 - r));
				PX_SurfaceDrawPixel(psurface, (px_int)(objx+objWidth / 2 - i), (px_int)(objy+objHeight - 1 - j), PX_COLOR(255 - r, r, 255 - r, 255 - r));

			}
		}
	}
	PX_SurfaceSetLimitInfo(psurface, linfo);

}
PX_Object* PX_Object_SoundViewCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float width, px_float height, PX_SoundPlay* pSoundPlay)
{
	PX_Object* pObject;
	PX_Object_SoundView* pDesc;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y,0, width, height, 0, PX_OBJECT_TYPE_SOUNDVIEW, 0, PX_Object_SoundViewRender, PX_NULL, 0, sizeof(PX_Object_SoundView));
pDesc = PX_ObjectGetDesc(PX_Object_SoundView, pObject);
	pDesc->pSoundPlay = pSoundPlay;
	return pObject;
}
