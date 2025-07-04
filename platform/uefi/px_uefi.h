#ifndef __PX_UEFI_H__
#define __PX_UEFI_H__

#include <Uefi.h>
#include <Guid/FileInfo.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleTextInEx.h>

extern EFI_HANDLE                   mImageHandle;

void PX_Object_MouseCursorCreate();

#endif
