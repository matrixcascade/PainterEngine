#include "PainterEngine_LiveEditorModules_ImportImage.h"


static  px_int PX_ImportImage_ExplorerGetPathFolderCount(const px_char *path,const char *filter)
{
	return PX_FileGetDirectoryFileCount(path,PX_FILEENUM_TYPE_FOLDER,filter);
}
static px_int PX_ImportImage_ExplorerGetPathFileCount(const px_char *path,const char *filter)
{
	return PX_FileGetDirectoryFileCount(path,PX_FILEENUM_TYPE_FILE,filter);
}
static px_int PX_ImportImage_ExplorerGetPathFolderName(const char path[],int count,char FileName[][260],const char *filter)
{
	return PX_FileGetDirectoryFileName(path,count,FileName,PX_FILEENUM_TYPE_FOLDER,filter);
}
static px_int PX_ImportImage_ExplorerGetPathFileName(const char path[],int count,char FileName[][260],const char *filter)
{
	return PX_FileGetDirectoryFileName(path,count,FileName,PX_FILEENUM_TYPE_FILE,filter);
}

px_void PX_Application_OnImportImage_OnConfirm(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_char path[260];
	px_int index=0;
	PX_Object *pImportImageObject=(PX_Object *)ptr;
	PX_LiveEditorModule_ImportImage *pImportImage=(PX_LiveEditorModule_ImportImage *)pImportImageObject->pObject;
	while (PX_TRUE)
	{
		px_char id[PX_LIVE_ID_MAX_LEN]={0};
		PX_LiveTexture *pLiveTexture=PX_NULL,LiveTextureInstance;
		PX_Object_ExplorerGetPath(pImportImage->explorer,path,index);
		if (!path[0])
		{
			break;
		}
		PX_FileGetName(path,id,sizeof(id));
		pLiveTexture=PX_LiveFrameworkGetLiveTextureById(pImportImage->pLiveFramework,id);
		if (pLiveTexture)
		{
			PX_TextureFree(&pLiveTexture->Texture);
			PX_memset(pLiveTexture,0,sizeof(PX_LiveTexture));
		}
		else
		{
			PX_memset(&LiveTextureInstance,0,sizeof(LiveTextureInstance));
			PX_VectorPushback(&pImportImage->pLiveFramework->livetextures,&LiveTextureInstance);
			pLiveTexture=PX_VECTORLAST(PX_LiveTexture,&pImportImage->pLiveFramework->livetextures);
		}

		do
		{
			px_texture loadtexture;

			//texture
			if (PX_LoadTextureFromFile(&pImportImage->pruntime->mp_resources,&loadtexture,path))
			{
				px_int left,right,top,bottom;

				if (loadtexture.width!=pImportImage->pLiveFramework->width||loadtexture.height!=pImportImage->pLiveFramework->height)
				{
					px_char content[260];
					PX_sprintf2(content,sizeof(content),"%1 %2",PX_STRINGFORMAT_STRING(PX_JsonGetString(pImportImage->pLanguageJson,"importimage.Invalid image size")),PX_STRINGFORMAT_STRING((path)));
					PX_TextureFree(&loadtexture);
					PX_ObjectExecuteEvent(pImportImageObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_MESSAGE,content));
					PX_ObjectExecuteEvent(pImportImageObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_EXIT));
					return;
				}
				PX_TextureGetVisibleRange(&loadtexture,&left,&right,&top,&bottom);
				PX_TextureCreate(&pImportImage->pruntime->mp_resources,&pLiveTexture->Texture,right-left+1,bottom-top+1);
				PX_TextureRegionCopy(&pLiveTexture->Texture,&loadtexture,0,0,left,top,right,bottom,PX_ALIGN_LEFTTOP,PX_NULL);
				pLiveTexture->textureOffsetX=left;
				pLiveTexture->textureOffsetY=top;
				PX_strcpy(pLiveTexture->id,id,sizeof(pLiveTexture->id));
				

				PX_TextureFree(&loadtexture);
			}
			else
			{
				px_char content[260];
				PX_sprintf2(content,sizeof(content),"%1 %2",PX_STRINGFORMAT_STRING(PX_JsonGetString(pImportImage->pLanguageJson,"importimage.Could not load")),PX_STRINGFORMAT_STRING((path)));
				PX_ObjectExecuteEvent(pImportImageObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_MESSAGE,content));
				PX_ObjectExecuteEvent(pImportImageObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_EXIT));
				return;
			}
		}while(0);
		index++;
	}
	PX_LiveFrameworkUpdateSourceVerticesUV(pImportImage->pLiveFramework);
	PX_ObjectExecuteEvent(pImportImageObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_EXIT));
}

px_void PX_Application_OnImportImage_OnCancel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pImportImageObject=(PX_Object *)ptr;
	PX_ObjectExecuteEvent(pImportImageObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_EXIT));
}

PX_Object * PX_LiveEditorModule_ImportImageInstall(PX_Object *parent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_ImportImage ImportImage,*pImportImage;
	PX_memset(&ImportImage,0,sizeof(ImportImage));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,parent,0,0,0,0,0,0,0,0,0,0,&ImportImage,sizeof(ImportImage));
	pImportImage=(PX_LiveEditorModule_ImportImage *)pObject->pObject;

	pImportImage->fontmodule=fm;
	pImportImage->pLanguageJson=pLanguageJson;
	pImportImage->pLiveFramework=pLiveFramework;
	pImportImage->pruntime=pruntime;
	pImportImage->explorer=PX_Object_ExplorerCreate(&pruntime->mp_ui,pObject,0,0,pruntime->surface_width,pruntime->surface_height,fm,\
	PX_ImportImage_ExplorerGetPathFolderCount,\
	PX_ImportImage_ExplorerGetPathFileCount,\
	PX_ImportImage_ExplorerGetPathFolderName,\
	PX_ImportImage_ExplorerGetPathFileName,\
	"");
	PX_Object_ExplorerSetFilter(pImportImage->explorer,".png\0.PNG\0.Png");
	PX_Object_ExplorerSetMaxSelectCount(pImportImage->explorer,128);

	PX_ObjectRegisterEvent(pImportImage->explorer,PX_OBJECT_EVENT_EXECUTE,PX_Application_OnImportImage_OnConfirm,pObject);
	PX_ObjectRegisterEvent(pImportImage->explorer,PX_OBJECT_EVENT_CANCEL,PX_Application_OnImportImage_OnCancel,pObject);
	return pObject;
}

px_void PX_LiveEditorModule_ImportImageUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_ImportImageEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_ImportImage *pImportImage=(PX_LiveEditorModule_ImportImage *)pObject->pObject;
	PX_Object_ExplorerOpen(pImportImage->explorer);
	PX_ObjectSetFocus(pObject);
}

px_void PX_LiveEditorModule_ImportImageDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_ImportImage *pImportImage=(PX_LiveEditorModule_ImportImage *)pObject->pObject;
	PX_ObjectReleaseFocus(pObject);
	PX_Object_ExplorerClose(pImportImage->explorer);
}
