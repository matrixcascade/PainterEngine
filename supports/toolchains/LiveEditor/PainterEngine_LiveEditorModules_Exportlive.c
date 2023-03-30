#include "PainterEngine_LiveEditorModules_ExportLive.h"


static  px_int PX_ExportLive_ExplorerGetPathFolderCount(const px_char *path,const char *filter)
{
	return PX_FileGetDirectoryFileCount(path,PX_FILEENUM_TYPE_FOLDER,filter);
}
static px_int PX_ExportLive_ExplorerGetPathFileCount(const px_char *path,const char *filter)
{
	return 0;
}
static px_int PX_ExportLive_ExplorerGetPathFolderName(const char path[],int count,char FileName[][260],const char *filter)
{
	return PX_FileGetDirectoryFileName(path,count,FileName,PX_FILEENUM_TYPE_FOLDER,filter);
}
static px_int PX_ExportLive_ExplorerGetPathFileName(const char path[],int count,char FileName[][260],const char *filter)
{
	return 0;
}


px_void PX_Application_OnExportLive_OnConfirm(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_char path[260];
	px_int index=0;
	PX_Object *pExportLiveObject=(PX_Object *)ptr;
	PX_LiveEditorModule_ExportLive *pExportLive=(PX_LiveEditorModule_ExportLive *)pExportLiveObject->pObject;

		PX_Object_ExplorerGetPath(pExportLive->explorer,path,index);
		if (!path[0])
		{
			return;
		}
		PX_strcat(path,"\\release.live");
		do 
		{
			px_byte *cacheBuffer=(px_byte *)malloc(64*1024*1024);
			px_memorypool mp=MP_Create(cacheBuffer,64*1024*1024);
			px_memory data;
			PX_MemoryInitialize(&mp,&data);
			if(!PX_LiveFrameworkExport(pExportLive->pLiveFramework,&data))
			{
				px_char content[260];
				PX_sprintf2(content,sizeof(content),"%1 %2",PX_STRINGFORMAT_STRING(PX_JsonGetString(pExportLive->pLanguageJson,"exportlive.out of memory")),PX_STRINGFORMAT_STRING((path)));
				PX_ObjectPostEvent(pExportLiveObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEEDITORMODULE_EXPORTLIVE_EVENT_MESSAGE,content));
			}

			if(!PX_SaveDataToFile(data.buffer,data.usedsize,path))
			{
				px_char content[260];
				PX_sprintf2(content,sizeof(content),"%1 %2",PX_STRINGFORMAT_STRING(PX_JsonGetString(pExportLive->pLanguageJson,"exportlive.could not export file")),PX_STRINGFORMAT_STRING((path)));
				PX_ObjectPostEvent(pExportLiveObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEEDITORMODULE_EXPORTLIVE_EVENT_MESSAGE,content));
			}

			do 
			{
				px_char content[260];
				PX_sprintf2(content,sizeof(content),"%1 %2",PX_STRINGFORMAT_STRING(PX_JsonGetString(pExportLive->pLanguageJson,"exportlive.succeeded")),PX_STRINGFORMAT_STRING((path)));
				PX_ObjectPostEvent(pExportLiveObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEEDITORMODULE_EXPORTLIVE_EVENT_MESSAGE,content));
			} while (0);
		} while (0);
	

}


PX_Object * PX_LiveEditorModule_ExportLiveInstall(PX_Object *parent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_ExportLive ExportLive,*pExportLive;
	PX_memset(&ExportLive,0,sizeof(ExportLive));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,parent,0,0,0,0,0,0,0,0,0,0,&ExportLive,sizeof(ExportLive));
	pExportLive=(PX_LiveEditorModule_ExportLive *)pObject->pObject;

	pExportLive->fontmodule=fm;
	pExportLive->pLanguageJson=pLanguageJson;
	pExportLive->pLiveFramework=pLiveFramework;
	pExportLive->pruntime=pruntime;
	pExportLive->explorer=PX_Object_ExplorerCreate(&pruntime->mp_ui,pObject,0,0,pruntime->surface_width,pruntime->surface_height,fm,\
	PX_ExportLive_ExplorerGetPathFolderCount,\
	PX_ExportLive_ExplorerGetPathFileCount,\
	PX_ExportLive_ExplorerGetPathFolderName,\
	PX_ExportLive_ExplorerGetPathFileName,\
	"");
	PX_Object_ExplorerSetFilter(pExportLive->explorer,".live\0.LIVE\0.Live");
	PX_Object_ExplorerSetMaxSelectCount(pExportLive->explorer,1);

	PX_ObjectRegisterEvent(pExportLive->explorer,PX_OBJECT_EVENT_EXECUTE,PX_Application_OnExportLive_OnConfirm,pObject);
	return pObject;
}

px_void PX_LiveEditorModule_ExportLiveUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_ExportLiveEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_ExportLive *pExportLive=(PX_LiveEditorModule_ExportLive *)pObject->pObject;
	PX_Object_ExplorerOpen(pExportLive->explorer);
}

px_void PX_LiveEditorModule_ExportLiveDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_ExportLive *pExportLive=(PX_LiveEditorModule_ExportLive *)pObject->pObject;
	PX_ObjectReleaseFocus(pObject);
	PX_Object_ExplorerClose(pExportLive->explorer);
}
