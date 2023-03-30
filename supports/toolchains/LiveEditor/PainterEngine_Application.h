#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#include "PainterEngine_Startup.h"


#include "PainterEngine_LiveEditorModules_Menu.h"
#include "PainterEngine_LiveEditorModules_CreateProject.h"
#include "PainterEngine_LiveEditorModules_ImportImage.h"
#include "PainterEngine_LiveEditorModules_LiveController.h"
#include "PainterEngine_LiveEditorModules_Display.h"
#include "PainterEngine_LiveEditorModules_Translation.h"
#include "PainterEngine_LiveEditorModules_BuildMesh.h"
#include "PainterEngine_LiveEditorModules_SetKey.h"
#include "PainterEngine_LiveEditorModules_LinkKey.h"
#include "PainterEngine_LiveEditorModules_AnimationController.h"
#include "PainterEngine_LiveEditorModules_FrameController.h"
#include "PainterEngine_LiveEditorModules_TimeStampEditor.h"
#include "PainterEngine_LiveEditorModules_NewFrameEditor.h"
#include "PainterEngine_LiveEditorModules_KeyTransform.h"
#include "PainterEngine_LiveEditorModules_VertexTransform.h"
#include "PainterEngine_LiveEditorModules_FrameLiveTextureEditor.h"
#include "PainterEngine_LiveEditorModules_Importlive.h"
#include "PainterEngine_LiveEditorModules_Exportlive.h"
#include "PainterEngine_LiveEditorModules_GlobalRotation.h"
#include "PainterEngine_LiveEditorModules_ImpulseEditor.h"
#include "PainterEngine_LiveEditorModules_Panc.h"
typedef struct
{
	//////////////////////////////////////////////////////////////////////////
	//modules
	//////////////////////////////////////////////////////////////////////////
	//global
	PX_Object *module_messagebox;
	//others
	PX_Object *module_display;
	PX_Object *module_menu;
	PX_Object *module_createproject;	
	PX_Object *module_importimage;
	PX_Object *module_importlive;
	PX_Object *module_exportlive;
	PX_Object *module_livecontroller;
	PX_Object *module_translation;
	PX_Object *module_buildmesh;
	PX_Object *module_setkey;
	PX_Object *module_linkkey;
	PX_Object *module_animationcontroller;
	PX_Object *module_framecontroller;
	PX_Object *module_timestampeditor;
	PX_Object *module_translationeditor;
	PX_Object *module_newframeeditor;
	PX_Object *module_keytransform;
	PX_Object *module_vertextransform;
	PX_Object *module_framelivetextureeditor;
	PX_Object *module_globalrotation;
	PX_Object *module_impulse;
	PX_Object* module_panc;
	//////////////////////////////////////////////////////////////////////////
	//
	PX_Object *root;
	PX_Object *explorer;
	PX_Object *messagebox;

	PX_Json languageJson;
	PX_LiveFramework liveFramework;
	PX_FontModule fontmodule;
	PX_Runtime runtime;
}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *App,px_int screen_Width,px_int screen_Height);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased);
px_void PX_ApplicationRender(PX_Application *App,px_dword elpased);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);

#endif
