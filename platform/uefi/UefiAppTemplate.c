#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#include "../../PainterEngine.h"

PX_OBJECT_EVENT_FUNCTION(OnButtonClick)
{
  PX_Object_PushButtonSetText (pObject, "按钮被点击了");
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_EditOnTextChanged)
{
}

// aka px_main()
int main (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *EfiGop = NULL;

  Status = gBS->LocateProtocol (&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &EfiGop);
  if (!EFI_ERROR(Status)) {
    screen_width = EfiGop->Mode->Info->HorizontalResolution;
    screen_height = EfiGop->Mode->Info->VerticalResolution;
  }

  PainterEngine_Initialize (screen_width, screen_height);

  PainterEngine_LoadFontModule("font/unifont.ttf", PX_FONTMODULE_CODEPAGE_UTF8, 20);

  PainterEngine_DrawText (screen_width/2, screen_height/2, "你好 PainterEngine", PX_ALIGN_CENTER, PX_COLOR(255, 255, 0, 0));

  PX_Object* myButtonObject;
  myButtonObject=PX_Object_PushButtonCreate(mp,root,screen_width/2-200,screen_height/2+50,200,80,"按钮", PainterEngine_GetFontModule());
  PX_ObjectRegisterEvent(myButtonObject,PX_OBJECT_EVENT_EXECUTE,OnButtonClick,0);

  PX_Object* pObject;
  pObject=PX_Object_EditCreate(mp,root,screen_width/2,screen_height/2+50,200,80,0);
  PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_VALUECHANGED, PX_Object_EditOnTextChanged,PX_NULL);

  return EFI_SUCCESS;
}

