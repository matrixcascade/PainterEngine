#include "../../PainterEngine/Kernel/PX_Kernel.h"
#include "../../PainterEngine/platform/windows/windows_display.h"
#include "stdio.h"
#include "FreeType/ft2build.h"
#include "FreeType/freetype/freetype.h"
#include "windows.h"

#ifdef _DEBUG
#pragma comment(lib,"freetyped.lib")
#else
#pragma comment(lib,"freetype.lib")
#endif


FT_Library				m_library;
FT_Face					m_face; 
FT_GlyphSlot			m_slot;

px_bool LoadFileToMemory(px_char *path,px_byte **bin,px_uint *size)
{
	FILE *pf;
	px_uint fsize,oft=0;
	if (!(pf=fopen(path,"rb")))
	{
		return PX_FALSE;
	}
	fseek(pf,0,SEEK_END);
	fsize=ftell(pf);
	*size=fsize;
	if (fsize==0)
	{
		return PX_FALSE;
	}
	fseek(pf,0,SEEK_SET);
	*bin=(px_byte *)malloc(fsize+1);
	if (*bin==NULL)
	{
		fclose(pf);
		return PX_FALSE;
	}
	while (!feof(pf))
	{
		oft+=fread((*bin)+oft,1,1024,pf);
	}
	fclose(pf);
	(*bin)[fsize]=0;
	return PX_TRUE;
}

px_bool SaveFileToMemory(px_char *path,px_byte *bin,px_int size)
{
	FILE *pf;
	if (!(pf=fopen(path,"wb")))
	{
		return PX_FALSE;
	}
	fwrite(bin,1,size,pf);
	fclose(pf);
	return PX_TRUE;
}

px_bool Font_Open(const px_char *path,px_uint font_size)
{
	FT_Error error = FT_Init_FreeType(&m_library);
	if ( error )
	{
		return PX_FALSE;
	}


	error = FT_New_Face(m_library,
		path,    //字形文件
		0,
		&m_face ); 

	if ( error ) 
	{
		FT_Done_FreeType( m_library );
		return PX_FALSE;
	}

	error = FT_Set_Pixel_Sizes(
		m_face,   /* handle to face object */
		0,      /* pixel_width           */
		font_size );   /* pixel_height          */

	if (error)
	{
		goto _ERROR;
	}
	error=FT_Select_Charmap(m_face,FT_ENCODING_UNICODE);

	if (error)
	{
		goto _ERROR;
	}
	m_slot = m_face->glyph;

	return PX_TRUE;
_ERROR:
	if(m_face)
	{
		FT_Done_Face    ( m_face );
		m_face=NULL;
	}
	if(m_library){
		FT_Done_FreeType( m_library );
		m_library=NULL;
	}
	return PX_FALSE;
}

static px_byte cache_buffer[1024*1024*16];//16M for codec

px_int  Font_BuildFontBuffer(px_dword code,px_dword utf16_le,PX_FONTMODULE_CODEPAGE codePage)
{
	FT_UInt glyph_index;
	FT_Error error;

	px_int gensize;
	px_int BearingX;
	px_int BearingY;
	px_int Advance;
	px_int Font_Width;
	px_int Font_Height;
	px_int x,y;
	PX_FontModule_Charactor_Header *pHeader;
	px_byte *pAlphaBuffer;

	glyph_index = FT_Get_Char_Index( m_face, utf16_le );

	if (glyph_index==0)
	{
		goto _ERROR;
	}

	error = FT_Load_Glyph(
		m_face,
		glyph_index,
		FT_LOAD_DEFAULT);

	if (error)
	{
		goto _ERROR;
	}
	error = FT_Render_Glyph( m_face->glyph,
		FT_RENDER_MODE_NORMAL);
	if (error)
	{
		goto _ERROR;
	}



	BearingX=m_face->glyph->bitmap_left;
	BearingY=m_face->glyph->bitmap_top;
	Advance=m_face->glyph->advance.x>>6;
	Font_Height=m_face->glyph->bitmap.rows;
	Font_Width=m_face->glyph->bitmap.width;

	gensize=Font_Height*Font_Width+sizeof(PX_FontModule_Charactor_Header);
	if (gensize>sizeof(cache_buffer))
	{
		goto _ERROR;
	}
	pHeader=(PX_FontModule_Charactor_Header *)cache_buffer;
	PX_memset(pHeader,0,sizeof(PX_FontModule_Charactor_Header));
	pHeader->c_magic[0]='P';
	pHeader->c_magic[1]='X';
	pHeader->c_magic[2]='F';
	pHeader->c_magic[3]='M';
	pHeader->Advance=Advance;
	pHeader->BearingX=BearingX;
	pHeader->BearingY=BearingY;
	pHeader->charactor_code=code;
	pHeader->codePage=codePage;
	pHeader->Font_Height=Font_Height;
	pHeader->Font_Width=Font_Width;

	pAlphaBuffer=cache_buffer+sizeof(PX_FontModule_Charactor_Header);
	if (Font_Height==0)
	{
		return 0;
	}
	for (y=0;y<Font_Height;y++)
	{
		for (x=0;x<Font_Width;x++)
		{
			pAlphaBuffer[y*Font_Width+x]=m_face->glyph->bitmap.buffer[y*m_face->glyph->bitmap.pitch+x];
		}
	}
	return gensize;	
_ERROR:
	return 0;
}


px_dword Font_GbkToUTF16(px_dword gbk_code)
{
	wchar_t utf16[8]={0};
	px_char gbk[8]={0};
	PX_memcpy(gbk,&gbk_code,4);

	MultiByteToWideChar(CP_ACP,0,gbk,sizeof(gbk),utf16,sizeof(utf16));
	return (px_dword)utf16[0];
}

px_dword Font_UTF8ToUTF16(px_dword utf8_code)
{
	wchar_t utf16[8]={0};
	px_char utf8[8]={0};
	PX_memcpy(utf8,&utf8_code,4);

	MultiByteToWideChar(CP_UTF8,0,utf8,sizeof(utf8),utf16,sizeof(utf16));
	return (px_dword)utf16[0];
}

static px_void Font_ConsolePause()
{
	fflush(stdin);
	getchar();
}


int Font_Build(px_char *font_file,px_char *asc_file,px_char *_out,px_int fontsize,PX_FONTMODULE_CODEPAGE codePage)
{
	px_byte *bin;
	const px_char *pTextCode;
	px_int size,offset=0;
	FILE *pf;
	px_int csize;
	px_dword code,utf16_code;
	px_dword codesize;

	if (!Font_Open(font_file,fontsize))
	{
		printf("无法加载字体文件.\n");
		return 1;
	}

	if(!LoadFileToMemory(asc_file,&bin,&size))
	{
		printf("无法加载字典文件.\n");
		return 1;
	}

	if ((pf=fopen(_out,"wb"))==PX_NULL)
	{
		printf("无法保存目标文件.\n");
		return 1;
	}

	pTextCode=(const px_char *)(bin);

	if (bin[0] == 0xef && bin[1] == 0xbb && bin[2] == 0xbf) 
	{
		pTextCode=(const px_char *)(bin+3);
	}
	
	if (bin[0] == 0xff && bin[1] == 0xfe) 
	{
		pTextCode=(const px_char *)(bin+2);
	}
	
	if (bin[0] == 0xfe && bin[1] == 0xff) 
	{
		pTextCode=(const px_char *)(bin+2);
	}
	
	while (offset<size)
	{
		wchar_t character[8]={0};
		code=0;
		csize=PX_FontModuleGetCharacterCode(codePage,pTextCode+offset,&code);
		
		if (csize==0)
		{
			printf("建立字模:%x 失败\n",code);
			goto _ERROR;
		}

		offset+=csize;

		switch(codePage)
		{
		case PX_FONTMODULE_CODEPAGE_GBK:
			utf16_code=Font_GbkToUTF16(code);
			break;
		case PX_FONTMODULE_CODEPAGE_UTF8:
			utf16_code=Font_UTF8ToUTF16(code);
			break;
		case PX_FONTMODULE_CODEPAGE_UTF16:
			utf16_code=code;
			
			break;
		}
		if (codePage==PX_FONTMODULE_CODEPAGE_UTF16)
		{
			if (code==0xfd00)
			{
				break;
			}
		}
		
		PX_memcpy(character,&utf16_code,4);

		if(!(codesize=Font_BuildFontBuffer(code,utf16_code,codePage)))
			{
				wprintf(L"生成字模:\"%s\" 错误\n",character);
				goto _ERROR;
			}
		if(fwrite(cache_buffer,1,codesize,pf)!=codesize)
			goto _ERROR;

		wprintf(L"建立字模:\"%s\"\n",character);
	}

	free(bin);
	fclose(pf);
	printf("所有字模已生成完成.\n");
	return 0;
_ERROR:
	free(bin);
	fclose(pf);
	printf("字模生成失败.\n");
	Font_ConsolePause();
	return 1;
}

#include "wchar.h"
#include <locale.h>
int main(_In_ int _Argc, _In_count_(_Argc) _Pre_z_ char ** _Argv, _In_z_ char ** _Env)
{
	px_char ttfpath[260]={0};
	px_char ascPath[260]={0};
	px_char outPath[260]={0};
	px_int codepage;
	px_int size;
	px_char *path;

	setlocale(LC_ALL, "chs");

	printf("选择生成字模的字体样式文件(TTF文件)");
	Font_ConsolePause();
	path=PX_OpenFileDialog("TTF文件\0*.ttf\0");
	if(!path||path[0]==0) return;
	printf("%s\n",path);
	PX_strcat(ttfpath,path);


	printf("选择字模的字典文件(TXT文件)");
	Font_ConsolePause();
	path=PX_OpenFileDialog("TXT文件\0*.txt\0");
	if(!path||path[0]==0) return;
	printf("%s\n",path);
	PX_strcat(ascPath,path);


	printf("保存到(PXF文件)");
	Font_ConsolePause();
	path=PX_SaveFileDialog("pxf文件\0*.pxf\0",".pxf");
	if(!path||path[0]==0) return;
	printf("%s\n",path);
	PX_strcat(outPath,path);

	printf("字体大小:");
	scanf("%d",&size);

	printf("编码类型(0-gbk/1-utf8/2-utf16):");
	scanf("%d",&codepage);


	Font_Build(ttfpath,ascPath,outPath,size,(PX_FONTMODULE_CODEPAGE)codepage);
	Font_ConsolePause();
}