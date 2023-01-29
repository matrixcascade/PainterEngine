#ifndef PAINTERENGINEVK_H
#define PAINTERENGINEVK_H

#include <windows.h>
#include <winioctl.h>
#include "tchar.h"
#include "../../../../core/PX_Core.h"

px_bool PX_PainterEngineVKUninstall();
px_bool PX_PainterEngineVKInstall();

px_uint PX_PainterEngineVKGetKeyEvent(px_word scan_code[], px_uint size);
px_bool PX_PainterEngineVKKeyDown(px_uint scan_code);
px_bool PX_PainterEngineVKKeyUp(px_uint scan_code);
#endif

