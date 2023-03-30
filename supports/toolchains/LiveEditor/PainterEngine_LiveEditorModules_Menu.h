#ifndef PX_LIVEFRAMEWORKMODULES_MENU_H
#define PX_LIVEFRAMEWORKMODULES_MENU_H

#include "PainterEngine_Startup.h"

#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_CREATEPROJECT    0x00070000
#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_WINDOW_LAYER 0x00070001
#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_LOADIMAGE    0x00070002
#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_IMPORT    0x00070003
#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_EXPORT    0x00070004
#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_SHOWFPS    0x00070005
#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_MESSAGE    0x00070006
#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_RESETPOSITION    0x00070007
#define PX_LIVEFRAMEWORKMODULES_MENU_EVENT_SHOWHELPERLINE    0x00070008
typedef struct
{
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_Object *Menu;
	PX_Object_Menu_Item *file_new_project,*file_import_live,*file_export_live,*file_import_image_file;
	PX_Object_Menu_Item *view_controller,*view_showfps,*view_resetposition,*view_showhelpline;
	PX_Object_Menu_Item *render_showkeypoints,*render_showlinker,*render_showmesh;
	PX_Object_Menu_Item *animation_resetframe,*animation_resettranslation,*animation_resetvertices,*animation_resetrotation,*animation_resetstretch;
	PX_Object_Menu_Item *animation_resetimpulse,*animation_resettexture;
	PX_Object_Menu_Item *about_painterengine,*about_app;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	px_bool showMesh;
}PX_LiveEditorModule_Menu;

//////////////////////////////////////////////////////////////////////////
//standard
PX_Object * PX_LiveEditorModule_MenuInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_MenuUninstall(PX_Object *pObject);

//////////////////////////////////////////////////////////////////////////
//
px_void PX_LiveEditorModule_MenuStandbyMode(PX_Object *pObject);
px_void PX_LiveEditorModule_MenuEditMode(PX_Object *pObject);
#endif   