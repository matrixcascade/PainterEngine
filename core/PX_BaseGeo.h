#ifndef PIXELSES_BASEGEO
#define PIXELSES_BASEGEO

#include "PX_Surface.h"

px_void PX_GeoDrawLine(px_surface *psurface, px_int x0, px_int y0, px_int x1, px_int y1 ,px_int lineWidth, px_color color);
px_void PX_GeoDrawPenSamplesLine(px_surface* psurface, px_point _samples[], px_int samplescount, px_float radius, px_color color, px_float v, px_float filter_factor);
px_void PX_GeoDrawPath(px_surface* psurface, px_point path[], px_int pathCount, px_float linewidth, px_color color);
px_void PX_GeoDrawPenLine(px_surface* psurface, px_float x0, px_float y0, px_float x1, px_float y1, px_float lineWidth, px_color color);
px_void PX_GeoDrawDashLine(px_surface* psurface, px_int x0, px_int y0, px_int x1, px_int y1, px_int lineWidth,px_float dash_width, px_color color);
px_void PX_GeoDrawPenDots(px_surface* psurface, px_point _samples[], px_int samplescount, px_float radius, px_color color);
px_void PX_GeoDrawBorder(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom ,px_int lineWidth,px_color color);
px_void PX_GeoDrawRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_color color);
px_void PX_GeoDrawSolidCircle(px_surface *psurface, px_int x,px_int y,px_int Radius,px_color color );
px_void PX_GeoDrawSolidCircleFast(px_surface* psurface, px_int x, px_int y, px_int Radius, px_color color);
px_void PX_GeoDrawPenCircle(px_surface *psurface, px_float x,px_float y,px_float Radius,px_color color );
px_void PX_GeoDrawPenCircleDecay(px_surface* psurface, px_float x, px_float y, px_float Radius, px_color color, px_float Decay);
px_void PX_GeoDrawPenRing(px_surface* psurface, px_float x, px_float y, px_float Radius, px_float linewidth_inside, px_color color);
px_void PX_GeoDrawPenCircleEraser(px_surface* psurface, px_float x, px_float y, px_float Radius, px_bool white_background);
px_void PX_GeoDrawSpray(px_surface* psurface, px_float x, px_float y, px_float Radius, px_color color);
px_void PX_GeoDrawBall(px_surface* psurface, px_float x, px_float y, px_float Radius, px_color color);
px_void PX_GeoDrawPath2D(px_surface* psurface, px_point2D path[], px_int pathCount, px_float linewidth, px_color color);
px_void PX_GeoDrawCircle(px_surface *psurface,px_int x,px_int y,px_int Radius,px_int lineWidth,px_color color);
px_void PX_GeoDrawRing(px_surface* psurface, px_int x, px_int y, px_float Radius, px_float lineWidth, px_color color, px_int start_angle, px_int end_angle);
px_void PX_GeoDrawSector(px_surface* psurface, px_int x, px_int y, px_float Radius_outside, px_float Radius_inside, px_color color, px_int start_angle, px_int end_angle);
px_void PX_GeoDrawSolidRoundRect(px_surface* psurface, px_int left, px_int top, px_int right, px_int bottom, px_float roundRaduis, px_color color);
px_void PX_GeoDrawRoundRect(px_surface *psurface, px_int left, px_int top, px_int right, px_int bottom,px_float roundRaduis,px_float linewidth,px_color color);
px_void PX_GeoDrawBezierCurvePoint(px_surface *rendersurface,px_point pt[],px_int pt_count,px_float t,px_float radius,px_color clr);
px_point PX_GeoGetBezierCurvePoint(px_point pt[], px_int pt_count, px_float t);
px_void PX_GeoDrawBresenhamLine(px_surface* psurface, px_int x0, px_int y0, px_int x1, px_int y1, px_color clr);
px_void PX_GeoDrawTriangle(px_surface *psurface,px_point2D p0,px_point2D p1,px_point2D p2,px_color color);
px_void PX_GeoDrawArrow(px_surface *psurface,px_point2D p0,px_point2D p1,px_float size,px_color color);
px_void PX_GeoBSpline3(px_point2D controlPoints[4], px_point2D _samples[], px_int samplescount);

#endif
