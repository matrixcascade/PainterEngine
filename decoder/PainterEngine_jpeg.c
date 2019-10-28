#include "PainterEngine_jpeg.h"

px_bool PX_TextureCreateFromJpegFile(px_memorypool *mp,px_texture *pTexture,const char *path)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *input_file;
	JSAMPARRAY buffer;
	int row_width;
	px_color *pColor;

	cinfo.err = jpeg_std_error(&jerr);

	if ((input_file = fopen(path, "rb")) == NULL) {
		return PX_FALSE;
	}

	// Initialization of JPEG compression objects
	jpeg_create_decompress(&cinfo);

	/* Specify data source for decompression */
	jpeg_stdio_src(&cinfo, input_file);

	(void) jpeg_read_header(&cinfo, TRUE);
	(void) jpeg_start_decompress(&cinfo);

	row_width = cinfo.output_width * cinfo.output_components;

	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_width, 1);

	if(!PX_TextureCreate(mp,pTexture,cinfo.output_width,cinfo.output_height)) return PX_FALSE;

	pColor = pTexture->surfaceBuffer;


	while (cinfo.output_scanline < cinfo.output_height) 
	{
		px_uint i;
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		for (i = 0; i < cinfo.output_width; ++i)
		{
			pColor[i]._argb.r=(*buffer)[i*3];
			pColor[i]._argb.g=(*buffer)[i*3+1];
			pColor[i]._argb.b=(*buffer)[i*3+2];
			pColor[i]._argb.a=255;
		}
		pColor+=pTexture->width;
	}


	(void) jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	fclose(input_file);
	return PX_TRUE;
}

px_bool PX_SurfaceToJpegFile(px_surface *psurface,const px_char *filename)
{
	int nComponent = 3; 
	struct jpeg_compress_struct jcs; 
	struct jpeg_error_mgr jem; 
	FILE* f;
	JSAMPROW row_pointer[1];        
	int row_stride;     
	px_byte *data;

	data=(px_byte *)malloc(psurface->width*3);

	jcs.err = jpeg_std_error(&jem);  
	jpeg_create_compress(&jcs); 

	f=fopen(filename,"wb"); 
	if (f==NULL)  
	{ 
		free(data);
		return PX_FALSE; 
	} 
	jpeg_stdio_dest(&jcs, f); 
	jcs.image_width = psurface->width;            
	jcs.image_height = psurface->height; 
	jcs.input_components = nComponent;         
	if (nComponent==1) 
		jcs.in_color_space = JCS_GRAYSCALE;
	else  
		jcs.in_color_space = JCS_RGB; 

	jpeg_set_defaults(&jcs);     
	jpeg_set_quality (&jcs, 80, PX_TRUE); 

	jpeg_start_compress(&jcs, TRUE); 



	row_stride = jcs.image_width*nComponent;      

	while (jcs.next_scanline < jcs.image_height) 
	{ 
		px_int i;
		for (i=0;i<psurface->width;i++)
		{
			data[i*3]=psurface->surfaceBuffer[jcs.next_scanline*psurface->width+i]._argb.r;
			data[i*3+1]=psurface->surfaceBuffer[jcs.next_scanline*psurface->width+i]._argb.g;
			data[i*3+2]=psurface->surfaceBuffer[jcs.next_scanline*psurface->width+i]._argb.b;
		}
		row_pointer[0] =data; 
		jpeg_write_scanlines(&jcs, row_pointer, 1); 
	} 

	jpeg_finish_compress(&jcs); 
	jpeg_destroy_compress(&jcs); 
	fclose(f); 

	free(data);
	return PX_TRUE;
}

