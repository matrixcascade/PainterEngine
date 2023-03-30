#include "PainterEngine_LiveEditorModules_ImportLive.h"


static  px_int PX_ImportLive_ExplorerGetPathFolderCount(const px_char *path,const char *filter)
{
	return PX_FileGetDirectoryFileCount(path,PX_FILEENUM_TYPE_FOLDER,filter);
}
static px_int PX_ImportLive_ExplorerGetPathFileCount(const px_char *path,const char *filter)
{
	return PX_FileGetDirectoryFileCount(path,PX_FILEENUM_TYPE_FILE,filter);
}
static px_int PX_ImportLive_ExplorerGetPathFolderName(const char path[],int count,char FileName[][260],const char *filter)
{
	return PX_FileGetDirectoryFileName(path,count,FileName,PX_FILEENUM_TYPE_FOLDER,filter);
}
static px_int PX_ImportLive_ExplorerGetPathFileName(const char path[],int count,char FileName[][260],const char *filter)
{
	return PX_FileGetDirectoryFileName(path,count,FileName,PX_FILEENUM_TYPE_FILE,filter);
}

px_void PX_Application_OnImportLive_OnConfirm(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_char path[260];
	px_int index=0;
	PX_Object *pImportLiveObject=(PX_Object *)ptr;
	PX_LiveEditorModule_ImportLive *pImportLive=(PX_LiveEditorModule_ImportLive *)pImportLiveObject->pObject;
	do
	{
		PX_Object_ExplorerGetPath(pImportLive->explorer,path,index);
		if (!path[0])
		{
			break;
		}
		do 
		{
			PX_IO_Data io;
			io=PX_LoadFileToIOData(path);
			if (io.size)
			{
				PX_LiveFrameworkFree(pImportLive->pLiveFramework);
				PX_memset(pImportLive->pLiveFramework,0,sizeof(PX_LiveFramework));
				if(!PX_LiveFrameworkImport(&pImportLive->pruntime->mp_resources,pImportLive->pLiveFramework,io.buffer,io.size))
				{
					px_char content[260];
					PX_sprintf2(content,sizeof(content),"%1 %2",PX_STRINGFORMAT_STRING(PX_JsonGetString(pImportLive->pLanguageJson,"importlive.Invalid live file")),PX_STRINGFORMAT_STRING((path)));
					PX_ObjectPostEvent(pImportLiveObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_MESSAGE,content));
				}
				PX_FreeIOData(&io);
			}
			
		} while (0);
	}while(0);
	PX_ObjectExecuteEvent(pImportLiveObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_CONFIRM));
}

px_void PX_Application_OnImportLive_OnCancel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pImportLiveObject=(PX_Object *)ptr;
	PX_LiveEditorModule_ImportLive *pImportLive=(PX_LiveEditorModule_ImportLive *)pImportLiveObject->pObject;
	PX_ObjectExecuteEvent(pImportLiveObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_EXIT));
}


PX_Object * PX_LiveEditorModule_ImportLiveInstall(PX_Object *parent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_ImportLive ImportLive,*pImportLive;
	PX_memset(&ImportLive,0,sizeof(ImportLive));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,parent,0,0,0,0,0,0,0,0,0,0,&ImportLive,sizeof(ImportLive));
	pImportLive=(PX_LiveEditorModule_ImportLive *)pObject->pObject;

	pImportLive->fontmodule=fm;
	pImportLive->pLanguageJson=pLanguageJson;
	pImportLive->pLiveFramework=pLiveFramework;
	pImportLive->pruntime=pruntime;
	pImportLive->explorer=PX_Object_ExplorerCreate(&pruntime->mp_ui,pObject,0,0,pruntime->surface_width,pruntime->surface_height,fm,\
	PX_ImportLive_ExplorerGetPathFolderCount,\
	PX_ImportLive_ExplorerGetPathFileCount,\
	PX_ImportLive_ExplorerGetPathFolderName,\
	PX_ImportLive_ExplorerGetPathFileName,\
	"");
	PX_Object_ExplorerSetFilter(pImportLive->explorer,".live\0.LIVE\0.Live");
	PX_Object_ExplorerSetMaxSelectCount(pImportLive->explorer,1);

	PX_ObjectRegisterEvent(pImportLive->explorer,PX_OBJECT_EVENT_EXECUTE,PX_Application_OnImportLive_OnConfirm,pObject);
	PX_ObjectRegisterEvent(pImportLive->explorer,PX_OBJECT_EVENT_CANCEL,PX_Application_OnImportLive_OnCancel,pObject);
	return pObject;
}

px_void PX_LiveEditorModule_ImportLiveUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_ImportLiveEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_ImportLive *pImportLive=(PX_LiveEditorModule_ImportLive *)pObject->pObject;
	PX_Object_ExplorerOpen(pImportLive->explorer);
}

px_void PX_LiveEditorModule_ImportLiveDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_ImportLive *pImportLive=(PX_LiveEditorModule_ImportLive *)pObject->pObject;
	PX_ObjectReleaseFocus(pObject);
	PX_Object_ExplorerClose(pImportLive->explorer);
}
