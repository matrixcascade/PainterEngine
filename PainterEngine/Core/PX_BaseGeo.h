#ifndef PIXELSES_BASEGEO
#define PIXELSES_BASEGEO

#include "PX_Surface.h"

px_void PX_GeoDrawLine(px_surface *psurface, px_int x0, px_int y0, px_int x1, px_int y1 ,px_int lineWidth, px_color color);
px_void PX_GeoDrawBorder(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom ,px_int lineWidth,px_color color);
px_void PX_GeoDrawRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_color color);
px_void PX_GeoDrawSolidCircle(px_surface *psurface, px_int x,px_int y,px_int Radius,px_color color );
px_void PX_GeoDrawCircle(px_surface *psurface,px_int x,px_int y,px_int Radius,px_int lineWidth,px_color color);
px_void PX_GeoDrawRing(px_surface *psurface, px_int x,px_int y,px_int Radius,px_int lineWidth,px_color color,px_uint start_angle,px_uint end_angle);
px_void PX_GeoDrawSector(px_surface *psurface, px_int x,px_int y,px_int Radius_outside,px_int Radius_inside,px_color color,px_uint start_angle,px_uint end_angle);
px_void PX_GeoDrawSolidRoundRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_float roundRaduis,px_color color);
px_void PX_GeoDrawRoundRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_float roundRaduis,px_float linewidth,px_color color);
#endif
