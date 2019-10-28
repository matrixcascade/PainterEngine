#include "PainterEngine_png.h"

static px_int getRowBytes(px_int width){  
	if((width * 3) % 4 == 0){  
		return width * 3;  
	}else{  
		return ((width * 3) / 4 + 1) * 4;  
	}  
}  

px_bool PX_TextureCreateFromPngFile(px_memorypool *mp,px_texture *pTexture,const char *path)
{
	png_structp png_ptr;  
	png_infop info_ptr;  
	px_int bit_depth;  
	px_uint i,j;  
	FILE *fp;  
	png_bytep* row_pointers;
	unsigned int bufSize = 0;  
	px_int rowBytes;
	px_color *pcolor;
	png_uint_32 width, height;
	px_int color_type; 


	if ((fp = fopen(path, "rb")) == NULL) {  
		return PX_FALSE;  
	}  

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);  

	if (png_ptr == NULL)  
	{  
		fclose(fp);  
		return PX_FALSE;  
	}  
	info_ptr = png_create_info_struct(png_ptr);  
	if (info_ptr == NULL)  
	{  
		fclose(fp);  
		png_destroy_read_struct(&png_ptr, NULL, NULL);  
		return PX_FALSE;  
	}  
	if (setjmp(png_jmpbuf(png_ptr))) {  
		/* Free all of the memory associated with the png_ptr and info_ptr */  
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);  
		fclose(fp);  
		/* If we get here, we had a problem reading the file */  
		return PX_FALSE;  
	}  
	/* Set up the input control if you are using standard C streams */  
	png_init_io(png_ptr, fp);  
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);  
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,  
		NULL, NULL, NULL);  

	row_pointers = png_get_rows(png_ptr, info_ptr);  


	if (color_type == PNG_COLOR_TYPE_RGB) {  
		bufSize = getRowBytes(width) * height;  
	} else if (color_type == PNG_COLOR_TYPE_RGBA) {  
		bufSize = width * height * 4;  
	} else{  
		return PX_FALSE;  
	}  

	if(!PX_TextureCreate(mp,pTexture,width,height)) return PX_FALSE;

	pcolor=(px_color *)((unsigned char *)pTexture->surfaceBuffer);

	rowBytes=getRowBytes(width);

	if(color_type == PNG_COLOR_TYPE_RGB)
	{
		for (i = 0; i < height; i++) 
			for(j=0;j<width;j++)
			{
				(*pcolor)._argb.r=row_pointers[i][j*3];
				(*pcolor)._argb.g=row_pointers[i][j*3+1];
				(*pcolor)._argb.b=row_pointers[i][j*3+2];
				(*pcolor)._argb.a=255;
				pcolor++;
			}
	}
	else if(color_type == PNG_COLOR_TYPE_RGBA)
	{
		for (i = 0; i < height; i++) 
			for(j=0;j<width;j++)
			{
				if (row_pointers[i][j*4+3]!=0)
				{
					(*pcolor)._argb.r=row_pointers[i][j*4];
					(*pcolor)._argb.g=row_pointers[i][j*4+1];
					(*pcolor)._argb.b=row_pointers[i][j*4+2];
					(*pcolor)._argb.a=row_pointers[i][j*4+3];
				}
				else
				{
					(*pcolor)._argb.ucolor=0;
				}

				pcolor++;
			}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);  
	fclose(fp);  

	return PX_TRUE;
}


px_int PX_PixelsToPngFile(const char* png_file_name, unsigned char*  pixels , int width, int height, int bit_depth)
{
	png_structp png_ptr;  
	png_infop info_ptr;  
	png_colorp palette;
	png_bytepp rows;
	int i;
	FILE *png_file = fopen(png_file_name, "wb");  
	if (!png_file)
	{
		return PX_FALSE;
	}
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);  
	if(png_ptr == NULL)  
	{  
		printf("ERROR:png_create_write_struct/n"); 
		fclose(png_file);
		return PX_FALSE;  
	}  
	info_ptr = png_create_info_struct(png_ptr);  
	if(info_ptr == NULL)  
	{  
		printf("ERROR:png_create_info_struct/n");  
		png_destroy_write_struct(&png_ptr, NULL);  
		return PX_FALSE;  
	}  
	png_init_io(png_ptr, png_file);  
	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, /*PNG_COLOR_TYPE_PALETTE*/PNG_COLOR_TYPE_RGB_ALPHA,  
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE); 


	palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
	if (!palette) {
		fclose(png_file);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return PX_FALSE;
	}
	png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);  
	png_write_info(png_ptr, info_ptr);  
	png_set_packing(png_ptr);

	rows = (png_bytepp)png_malloc(png_ptr, height * sizeof(png_bytep));
	for (i = 0; i < height; ++i)
	{
		rows[i] = (png_bytep)(pixels + (i) * width * 4);
	}

	png_write_image(png_ptr, rows);  
	png_free(png_ptr,rows);
	png_write_end(png_ptr, info_ptr);  
	png_free(png_ptr, palette);  
	palette=NULL;  
	png_destroy_write_struct(&png_ptr, &info_ptr);  
	fclose(png_file);  

	return PX_TRUE;  
}

px_bool PX_SurfaceToPngFile(px_surface *psurface,const px_char *filename)
{
	return PX_PixelsToPngFile(filename,(px_byte *)psurface->surfaceBuffer,psurface->width,psurface->height,8);
}
