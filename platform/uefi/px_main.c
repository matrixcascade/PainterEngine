#include "../../runtime/PainterEngine_Application.h"
#include "px_uefi.h"

int  _fltused = 1;

EFI_GRAPHICS_OUTPUT_PROTOCOL *mEfiGop = NULL;

EFI_HANDLE                   mImageHandle = NULL;

px_bool PX_AudioInitialize(PX_SoundPlay *soundPlay)
{
  return FALSE;
}

typedef struct {
  EFI_ABSOLUTE_POINTER_PROTOCOL  *AbsPointer;
  EFI_SIMPLE_POINTER_PROTOCOL    *SimplePointer;
  INTN                           LastCursorX;
  INTN                           LastCursorY;
  BOOLEAN                        LeftButton;
} PX_UEFI_MOUSE_DATA;

PX_UEFI_MOUSE_DATA  mPxUefiMouseData;

EFI_STATUS
EfiMouseInit (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_ABSOLUTE_POINTER_PROTOCOL  *AbsPointer = NULL;
  EFI_SIMPLE_POINTER_PROTOCOL    *SimplePointer = NULL;
  EFI_HANDLE                     *HandleBuffer = NULL;
  UINTN                          HandleCount, Index;
  EFI_DEVICE_PATH_PROTOCOL       *DevicePath;
  BOOLEAN                        AbsPointerSupport = FALSE;
  BOOLEAN                        SimplePointerSupport = FALSE;

  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiAbsolutePointerProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (!EFI_ERROR (Status)) {
      AbsPointerSupport = TRUE;
      break;
    }
  }
  if (HandleBuffer!= NULL) {
    FreePool (HandleBuffer);
    HandleBuffer = NULL;
  }

  if (AbsPointerSupport) {
    goto StartInit;
  }

  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimplePointerProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (!EFI_ERROR (Status)) {
      SimplePointerSupport = TRUE;
      break;
    }
  }
  if (HandleBuffer!= NULL) {
    FreePool (HandleBuffer);
    HandleBuffer = NULL;
  }

  if (!AbsPointerSupport && !SimplePointerSupport) {
    return EFI_UNSUPPORTED;
  }

StartInit:

  if (AbsPointerSupport) {
    Status = gBS->HandleProtocol (gST->ConsoleInHandle, &gEfiAbsolutePointerProtocolGuid, (VOID **)&AbsPointer);
    if (!EFI_ERROR (Status) && AbsPointer != NULL) {
      mPxUefiMouseData.AbsPointer = AbsPointer;
    }
  }

  if (SimplePointerSupport && !AbsPointerSupport) {
    Status = gBS->HandleProtocol (gST->ConsoleInHandle, &gEfiSimplePointerProtocolGuid, (VOID **)&SimplePointer);
    if (!EFI_ERROR (Status) && SimplePointer != NULL) {
      mPxUefiMouseData.SimplePointer = SimplePointer;
    }
  }

  mPxUefiMouseData.LeftButton = FALSE;
  mPxUefiMouseData.LastCursorX = 0;
  mPxUefiMouseData.LastCursorY = 0;

  return Status;
}


EFI_STATUS
CheckUefiMouse (
  VOID
  )
{
  EFI_STATUS                     Status = EFI_NOT_READY;
  EFI_ABSOLUTE_POINTER_STATE     AbsState;
  EFI_SIMPLE_POINTER_STATE       SimpleState;
  INTN                           LastCursorX, LastCursorY;
  BOOLEAN                        LastLeft;
  INT32                          Hor, Ver;
  PX_Object_Event                e;

  if (!mPxUefiMouseData.AbsPointer && !mPxUefiMouseData.SimplePointer) {
    return EFI_UNSUPPORTED;
  }

  Hor = mEfiGop->Mode->Info->HorizontalResolution;
  Ver = mEfiGop->Mode->Info->VerticalResolution;

  LastCursorX = mPxUefiMouseData.LastCursorX;
  LastCursorY = mPxUefiMouseData.LastCursorY;
  LastLeft    = mPxUefiMouseData.LeftButton;
  if (mPxUefiMouseData.AbsPointer != NULL) {
    Status = mPxUefiMouseData.AbsPointer->GetState (mPxUefiMouseData.AbsPointer, &AbsState);
    if (!EFI_ERROR (Status)) {
      mPxUefiMouseData.LastCursorX = (AbsState.CurrentX * Hor) / (mPxUefiMouseData.AbsPointer->Mode->AbsoluteMaxX - mPxUefiMouseData.AbsPointer->Mode->AbsoluteMinX);
      if (mPxUefiMouseData.LastCursorX > Hor - 1) {
        mPxUefiMouseData.LastCursorX = Hor - 1;
      }
      mPxUefiMouseData.LastCursorY = (AbsState.CurrentY * Ver) / (mPxUefiMouseData.AbsPointer->Mode->AbsoluteMaxY - mPxUefiMouseData.AbsPointer->Mode->AbsoluteMinY);
      if (mPxUefiMouseData.LastCursorY > Ver - 1) {
        mPxUefiMouseData.LastCursorY = Ver - 1;
      }
      mPxUefiMouseData.LeftButton = AbsState.ActiveButtons & BIT0;
    }
  } else if (mPxUefiMouseData.SimplePointer != NULL) {
    Status = mPxUefiMouseData.SimplePointer->GetState (mPxUefiMouseData.SimplePointer, &SimpleState);
    if (!EFI_ERROR (Status)) {
      mPxUefiMouseData.LastCursorX += (SimpleState.RelativeMovementX * Hor) / (INT32)(50 * mPxUefiMouseData.SimplePointer->Mode->ResolutionX);
      if (mPxUefiMouseData.LastCursorX > Hor - 1) {
        mPxUefiMouseData.LastCursorX = Hor - 1;
      }
      if (mPxUefiMouseData.LastCursorX < 0) {
        mPxUefiMouseData.LastCursorX = 0;
      }
      mPxUefiMouseData.LastCursorY += (SimpleState.RelativeMovementY * Ver) / (INT32)(50 * mPxUefiMouseData.SimplePointer->Mode->ResolutionY);
      if (mPxUefiMouseData.LastCursorY > Ver - 1) {
        mPxUefiMouseData.LastCursorY = Ver - 1;
      }
      if (mPxUefiMouseData.LastCursorY < 0) {
        mPxUefiMouseData.LastCursorY = 0;
      }

      mPxUefiMouseData.LeftButton = SimpleState.LeftButton;
    }
  }

  if (!LastLeft && !mPxUefiMouseData.LeftButton && LastCursorX == mPxUefiMouseData.LastCursorX && LastCursorY == mPxUefiMouseData.LastCursorY) {
    return EFI_NOT_READY;
  }

  PX_Object_Event_SetCursorX (&e, mPxUefiMouseData.LastCursorX);
  PX_Object_Event_SetCursorY (&e, mPxUefiMouseData.LastCursorY);
  if (LastCursorX != mPxUefiMouseData.LastCursorX || LastCursorY != mPxUefiMouseData.LastCursorY) {
    if (mPxUefiMouseData.LeftButton) {
      e.Event = PX_OBJECT_EVENT_CURSORDRAG;
    } else {
      e.Event = PX_OBJECT_EVENT_CURSORMOVE;
    }
  } else if (LastCursorX == mPxUefiMouseData.LastCursorX && LastCursorY == mPxUefiMouseData.LastCursorY) {
    if (mPxUefiMouseData.LeftButton) {
      e.Event = PX_OBJECT_EVENT_CURSORDOWN;
    } else {
      if (LastLeft) {
        e.Event = PX_OBJECT_EVENT_CURSORCLICK;
        PX_ApplicationPostEvent(&App,e);
      }
      e.Event = PX_OBJECT_EVENT_CURSORUP;
    }
  }
  PX_ApplicationPostEvent(&App,e);

  return EFI_SUCCESS;
}

VOID
CheckUefiKeyboard (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *TxtInEx;
  EFI_KEY_DATA                       KeyData;
  px_char                            str[2]={0};
  PX_Object_Event                    e;

  Status = gBS->HandleProtocol (gST->ConsoleInHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID **)&TxtInEx);
  Status = TxtInEx->ReadKeyStrokeEx (TxtInEx, &KeyData);
  if (!EFI_ERROR (Status)) {
    e.Event = PX_OBJECT_EVENT_KEYDOWN;
    switch (KeyData.Key.UnicodeChar) {
      case CHAR_CARRIAGE_RETURN:
        str[0] = PX_VK_RETURN;
        break;

      case CHAR_BACKSPACE:
        str[0] = PX_VK_BACK;
        break;

      case CHAR_TAB:
        str[0] = PX_VK_TAB;
        break;

      case CHAR_NULL:
        switch (KeyData.Key.ScanCode) {
        case SCAN_UP:
          str[0] = PX_VK_UP;
          break;
        
        case SCAN_DOWN:
          str[0] = PX_VK_DOWN;
          break;

        case SCAN_RIGHT:
          str[0] = PX_VK_RIGHT;
          break;

        case SCAN_LEFT:
          str[0] = PX_VK_LEFT;
          break;

        case SCAN_ESC:
          str[0] = PX_VK_ESCAPE;
          break;

        case SCAN_DELETE:
          str[0] = PX_VK_DELETE;
          break;

        case SCAN_PAGE_DOWN:
          str[0] = PX_VK_NEXT;
          break;

        case SCAN_PAGE_UP:
          str[0] = PX_VK_PRIOR;
          break;

        case SCAN_HOME:
          str[0] = PX_VK_HOME;
          break;

        case SCAN_END:
          str[0] = PX_VK_END;
          break;

        default:
          break;
        }
        break;

      case CHAR_LINEFEED:
        break;

      default:
        e.Event = PX_OBJECT_EVENT_STRING;
        str[0] = (CHAR8)(KeyData.Key.UnicodeChar);
        break;
    }
  } else {
    return;
  }

  if (str[0]) {
    if (e.Event == PX_OBJECT_EVENT_KEYDOWN) {
      PX_Object_Event_SetKeyDown(&e,str[0]);
    } else if (e.Event == PX_OBJECT_EVENT_STRING) {
      PX_Object_Event_SetStringPtr(&e,&str[0]);
    }
    PX_ApplicationPostEvent(&App,e);
  }

}

EFI_STATUS
EFIAPI
CheckEfiInputEvent (
  IN  EFI_EVENT    Event,
  IN  VOID         *Context
  )
{
  CheckUefiMouse();
  CheckUefiKeyboard();

  return EFI_SUCCESS;
}


void uefi_gop_render(px_color* gram, px_int width, px_int height)
{
  EFI_STATUS  Status;
  UINTN       Delta;

  Delta = mEfiGop->Mode->Info->HorizontalResolution * 4;

  Status = mEfiGop->Blt (
                      mEfiGop,
                      (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)(gram),
                      EfiBltBufferToVideo,
                      0,
                      0,
                      0,
                      0,
                      width,
                      height,
                      Delta
                      );
}


EFI_STATUS
EFIAPI
PainterEngineUefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  px_int screen_width=240;
  px_int screen_height=320;
#ifdef REAL_TIMER_LIB
  px_dword timelasttime = 0;
#endif
  EFI_STATUS  Status;
  EFI_EVENT   MouseEvent;

  Status = gBS->LocateProtocol (&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &mEfiGop);
  if (!EFI_ERROR(Status)) {
    screen_width = mEfiGop->Mode->Info->HorizontalResolution;
    screen_height = mEfiGop->Mode->Info->VerticalResolution;
  }

  mImageHandle = ImageHandle;

  if (!PX_ApplicationInitialize(&App,screen_width,screen_height))
  {
    return 0;
  }

  EfiMouseInit();
  PX_Object_MouseCursorCreate();

  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  (EFI_EVENT_NOTIFY) CheckEfiInputEvent,
                  (VOID*)NULL,
                  &MouseEvent
                  );

  Status = gBS->SetTimer (MouseEvent, TimerPeriodic, 10 * 10000); // 10ms

#ifdef REAL_TIMER_LIB
  timelasttime=PX_TimeGetTime();
#endif
  while (1)
  {
#ifdef REAL_TIMER_LIB
    px_dword now=PX_TimeGetTime();
    px_dword elapsed=now-timelasttime;
    timelasttime= now;
#else
    px_dword elapsed = 100;
#endif
    PX_ApplicationUpdate(&App,elapsed);
    PX_ApplicationRender(&App,elapsed);
    uefi_gop_render(App.runtime.RenderSurface.surfaceBuffer, App.runtime.RenderSurface.width, App.runtime.RenderSurface.height);
  }
  
  return 0;
}

