#ifndef PX_SIGNAL_H
#define PX_SIGNAL_H

#include "PX_Typedef.h"


px_void PX_SignalDelay(_IN _OUT px_float x[], px_int size, px_int delay);
px_void PX_SignalForwardDifference(_IN px_float x[], px_float out[], px_int size);
px_void PX_SignalAvg(_IN px_float in[], _OUT px_float out[], px_int size, px_int filter);
#endif