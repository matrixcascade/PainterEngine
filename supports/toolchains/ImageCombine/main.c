#include "architecture/PainterEngine.h"
#include "platform/modules/px_file.h"

px_byte runtime_cache[256 * 1024 * 1024];
px_memorypool mp;
px_vector texturedatas;
px_char enumonepath[1][260];

typedef struct
{
	px_int left, right, top, bottom;
	px_int width, height;
	px_texture texture;	
}PX_TextureData;

px_void PX_MP_ERROR_CATCH( px_void* ptr, PX_MEMORYPOOL_ERROR error)
{
	printf("Out of memory!\n");
	exit(0);
}

px_bool PX_LoadTextureFromFile(px_memorypool* mp, px_texture* tex, const px_char path[])
{
	PX_IO_Data io;

	io = PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_TextureCreateFromMemory(mp, io.buffer, io.size, tex))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_void PX_TextureGetRegion(PX_TextureData* pTexData)
{
	px_int left, right, top, bottom;
	px_int x, y;
	px_texture *ptex=&pTexData->texture;
	left = ptex->width;
	right=0;
	top=ptex->height;
	bottom=0;

	for (y = 0; y < ptex->height; y++)
	{
		for (x = 0; x < ptex->width; x++)
		{
			if (PX_SurfaceGetPixel(ptex, x, y)._argb.a)
			{
				if (x < left)
				{
					left = x;
				}
				if (x > right)
				{
					right = x;
				}
				if (y < top)
				{
					top = y;
				}
				if (y > bottom)
				{
					bottom = y;
				}
			}
		}
	}
	if (right<left)
	{
		pTexData->width = 0;
		pTexData->height =0 ;
	}
	else
	{
		pTexData->left  =left;
		pTexData->right  =right;
		pTexData->top  =top;
		pTexData->bottom  =bottom;
		pTexData->width  =right-left+1;
		pTexData->height  =bottom-top+1;
	}

}


px_int main(px_int argc,px_char **argv)
{
	px_int i,index,temp,tex_index,append_zero,errcounter=0;
	px_int numericlen=0;
	px_int pngfilecount;
	px_int minwidth=0, minheight=0;
	px_float scalefactor=1.0f;
	px_char content[32] = {0};
	px_char prename[256]={0};
	px_char numbercontent[8];
	px_char loadname[256] = { 0 };
	px_char outname[256] = { 0 };
	px_memory outbin;
	px_texture tex_clip,tex_scale,tex_out;
	printf("scale factor(1.0):");
	gets_s(content,sizeof(content));
	if(content[0]!='\0')
	scalefactor = PX_atof(content);

	mp=MP_Create(runtime_cache,sizeof(runtime_cache));
	MP_ErrorCatch(&mp,PX_MP_ERROR_CATCH,0);
	pngfilecount=PX_FileGetDirectoryFileCount(".", PX_FILEENUM_TYPE_FILE, "png\0Png\0PNG");
	if (pngfilecount==0)
	{
		return;
	}

	PX_FileGetDirectoryFileName(".", 1, enumonepath, PX_FILEENUM_TYPE_FILE, "png\0Png\0PNG");
	PX_FileGetName(enumonepath[0], prename, sizeof(prename));
	index = PX_strlen(prename)-1;
	while (index>0)
	{
		if (PX_charIsNumeric(prename[index]))
		{
			numericlen++;
			index--;
		}
		else
		break;
	}
	temp=PX_atoi(prename+index+1);
	PX_itoa(temp,numbercontent,sizeof(numbercontent),10);
	
	if (numericlen==PX_strlen(numbercontent))
	{
		numericlen = 0;
	}
	prename[index + 1] = 0;
	tex_index = 0;
	PX_VectorInitialize(&mp, &texturedatas, sizeof(PX_TextureData), 256);
	while(PX_TRUE)
	{
		PX_TextureData textureData;
		PX_memset(&textureData, 0, sizeof(PX_TextureData));
		loadname[0]=0;
		PX_strcat(loadname,prename);
		PX_itoa(tex_index, numbercontent, sizeof(numbercontent), 10);
		if (PX_strlen(numbercontent)<numericlen)
		{
			append_zero = numericlen - PX_strlen(numbercontent);
			for (i = 0; i < append_zero; i++)
			{
				PX_strcat(loadname, "0");
			}
		}
		PX_strcat(loadname, numbercontent);
		PX_strcat(loadname, ".png");

		if (PX_LoadTextureFromFile(&mp, &textureData.texture, loadname))
		{
			printf("loading-->%s\n", loadname);
			PX_TextureGetRegion(&textureData);
			PX_VectorPushback(&texturedatas, &textureData);
			errcounter = 0;
			if (textureData.width>minwidth)
			{
				minwidth = textureData.width;
			}
			if (textureData.height > minheight)
			{
				minheight  =textureData.height;
			}
		}
		else
		{
			errcounter++;
		}
		tex_index++;

		if (errcounter>10)
		{
			break;
		}
	}
	if (texturedatas.size==0)
	{
		printf("no texture found!\n");
		return 0;
	}
	printf("ClipFrame w:%d h:%d count:%d\n", (px_int)(minwidth * scalefactor), (px_int)(minheight * scalefactor), texturedatas.size);
	printf("packing....\n");
	
	PX_TextureCreate(&mp, &tex_clip, minwidth, minheight);
	PX_TextureCreate(&mp, &tex_scale, (px_int)(minwidth*scalefactor), (px_int)(minheight * scalefactor));
	PX_TextureCreate(&mp, &tex_out, (px_int)(minwidth * scalefactor) * texturedatas.size, (px_int)(minheight * scalefactor));
	PX_MemoryInitialize(&mp, &outbin);

	printf("processing...\n");
	for (i = 0; i < texturedatas.size; i++)
	{
		PX_TextureData *pData=PX_VECTORAT(PX_TextureData,&texturedatas,i);

		PX_SurfaceClearAll(&tex_clip, PX_COLOR(0, 255, 255, 255));
		PX_TextureRenderClip(&tex_clip, &pData->texture, tex_clip.width / 2, tex_clip.height / 2, pData->left, pData->top, pData->width, pData->height, PX_ALIGN_CENTER, 0);
		PX_TextureScaleToTexture(&tex_clip, &tex_scale);
		PX_TextureRender(&tex_out, &tex_scale, i * tex_scale.width, 0, PX_ALIGN_LEFTTOP, 0);
		PX_TextureFree(&pData->texture);
	}
	printf("encoding...\n");
	PX_PngSurfaceToBuffer(&tex_out, &outbin);
	PX_sprintf3(outname, sizeof(outname), "release %1x%2x%3.png", PX_STRINGFORMAT_INT(tex_scale.width), PX_STRINGFORMAT_INT(tex_scale.height), PX_STRINGFORMAT_INT(texturedatas.size));
	PX_SaveDataToFile( outbin.buffer, outbin.usedsize, outname);
	printf("done\n");

}