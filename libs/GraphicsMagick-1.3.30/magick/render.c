/*
% Copyright (C) 2003-2018 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                 RRRR   EEEEE  N   N  DDDD   EEEEE  RRRR                     %
%                 R   R  E      NN  N  D   D  E      R   R                    %
%                 RRRR   EEE    N N N  D   D  EEE    RRRR                     %
%                 R R    E      N  NN  D   D  E      R R                      %
%                 R  R   EEEEE  N   N  DDDD   EEEEE  R  R                     %
%                                                                             %
%                                                                             %
%                   GraphicsMagick Image Drawing Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1998                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Bill Radcliffe of Corbis (www.corbis.com) contributed the polygon
% rendering code based on Paul Heckbert's "Concave Polygon Scan Conversion",
% Graphics Gems, 1990.  Leonard Rosenthal and David Harr of Appligent
% (www.appligent.com) contributed the dash pattern, linecap stroking
% algorithm, and minor rendering improvements.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/alpha_composite.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/constitute.h"
#include "magick/enhance.h"
#include "magick/enum_strings.h"
#include "magick/gem.h"
#include "magick/gradient.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/omp_data_view.h"
#include "magick/paint.h"
#include "magick/pixel_cache.h"
#include "magick/render.h"
#include "magick/transform.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define BezierQuantum  200

/*
  Typedef declarations.
*/

typedef struct _EdgeInfo
{
  SegmentInfo
    bounds;

  double
    scanline;

  PointInfo
    *points;

  long
    number_points,
    direction,
    highwater;

  MagickBool
    ghostline;

} EdgeInfo;

typedef struct _PolygonInfo
{
  EdgeInfo
    *edges;

  long
    number_edges;
} PolygonInfo;

typedef enum
{
  MoveToCode,
  OpenCode,
  GhostlineCode,
  LineToCode,
  EndCode
} PathInfoCode;

typedef struct _PathInfo
{
  PointInfo
    point;

  PathInfoCode
    code;
} PathInfo;


/*
  A data structure to help manage automatically growing a PrimitiveInfo array.
  Intended to be used in DrawImage() and the TraceXXX() functions.  See static
  function PrimitiveInfoRealloc().  Motivated by ticket #516.
*/
typedef struct _PrimitiveInfoMgr
{
  PrimitiveInfo ** pp_PrimitiveInfo;  /* address of a pointer to the start of the PrimitiveInfo array */
                                      /* (updated when the array is reallocated) */

  size_t *         p_AllocCount;      /* address of a location containing the number of elements allocated */
                                      /* (updated when the array is reallocated) */

  size_t           StoreStartingAt;   /* index to the element at which to start storing new elements */
                                      /* (must be updated before calling PrimitiveInfoRealloc()) */

  ExceptionInfo *  p_Exception;       /* for when reallocation fails */
} PrimitiveInfoMgr;


/*
  DrawInfoExtra allows for expansion of DrawInfo without increasing its
  size.  The internals are defined only in this source file.  Clients
  outside of this source file can access the internals via the provided
  access functions (see below).
*/
typedef struct _DrawInfoExtra
{
  char
    /*
      clip_path is (typically) the name of the attribute whose value contains
      the clipping path's graphical elements.
    */
    *clip_path,
    /*
      composite_path is (typically) the name of the attribute whose value contains
      the composite path's graphical elements.
    */
    *composite_path;
} DrawInfoExtra;

/* provide public access to the clip_path member of DrawInfo */
MagickExport char **
DrawInfoGetClipPath(const DrawInfo * draw_info)
{
  return(&draw_info->extra->clip_path);
}

/* provide public access to the composite_path member of DrawInfo */
MagickExport char **
DrawInfoGetCompositePath(const DrawInfo * draw_info)
{
  return(&draw_info->extra->composite_path);
}

/*
  Forward declarations.
*/
static PrimitiveInfo  /* added Image* param so DrawInfo::stroke_width can be clamped */
  *TraceStrokePolygon(const Image *,const DrawInfo *,const PrimitiveInfo *);

static MagickPassFail
  DrawPrimitive(Image *,const DrawInfo *,const PrimitiveInfo *),
  DrawStrokePolygon(Image *,const DrawInfo *,const PrimitiveInfo *);

static MagickBool
  /*IsDrawInfoClippingPath(const DrawInfo *),*/           /* is DrawInfo a clipping path */
  IsDrawInfoSVGCompliant(const DrawInfo *),               /* is DrawInfo drawn as SVG compliant */
  IsDrawInfoSVGCompliantClippingPath(const DrawInfo *);   /* is DrawInfo drawn as SVG compliant clipping path */

static void
  SetDrawInfoClippingPath(DrawInfo *, MagickBool ClippingPath),   /* tag DrawInfo as clipping path or not */
  SetDrawInfoSVGCompliant(DrawInfo *, MagickBool SVGCompliant);   /* tag DrawInfo as SVG compliant or not */

static unsigned long
  TracePath(Image *image, PrimitiveInfoMgr *,const char *);

static void
#if 0
  DestroyGradientInfo(GradientInfo *),
#endif
  DestroyPolygonInfo(void *polygon_info_void),
  TraceArc(PrimitiveInfoMgr *,const PointInfo,const PointInfo,const PointInfo),
  TraceArcPath(PrimitiveInfoMgr *,const PointInfo,const PointInfo,const PointInfo,
    const double,const unsigned int,const unsigned int),
  TraceBezier(PrimitiveInfoMgr *,const unsigned long),
  TraceCircle(PrimitiveInfoMgr *,const PointInfo,const PointInfo),
  TraceEllipse(PrimitiveInfoMgr *,const PointInfo,const PointInfo,const PointInfo),
  TraceLine(PrimitiveInfo *,const PointInfo,const PointInfo),
  TracePoint(PrimitiveInfo *,const PointInfo),
  TraceRectangle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceRoundRectangle(PrimitiveInfoMgr *,const PointInfo,const PointInfo,
    PointInfo),
  TraceSquareLinecap(PrimitiveInfo *,const unsigned long,const double);

static MagickBool
  PrimitiveInfoRealloc(PrimitiveInfoMgr *p_PIMgr,const size_t Needed);
/*
  Ticket #515 showed how an excessively big DrawInfo::stroke_width can cause writes
  beyond the end of an array of points when computing a stroked polygon.  So we want
  to clamp it to some reasonable value before using it in computations.  The value
  sqrt(2)*max(width,height) will always be >= the diagonal size of any image.  The
  computed upper limit will be twice this value.  Note that 1.415 is just slightly
  bigger than sqrt(2).
*/

#define STROKE_WIDTH_LIMIT(image) ((2.0*1.415) * Max((image)->columns,(image)->rows))

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e D r a w I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneDrawInfo() makes a copy of the given draw info structure.  If NULL
%  is specified, a new DrawInfo structure is created initialized to
%  default values.
%
%  The format of the CloneDrawInfo method is:
%
%      DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
%        const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o draw_info: The draw info.
%
%
*/
MagickExport DrawInfo *
CloneDrawInfo(const ImageInfo *image_info,const DrawInfo *draw_info)
{
  DrawInfo
    *clone_info;

  clone_info=MagickAllocateMemory(DrawInfo *,sizeof(DrawInfo));
  if (clone_info == (DrawInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateDrawInfo);
  GetDrawInfo(image_info,clone_info);
  if (draw_info == (DrawInfo *) NULL)
    return(clone_info);
  if (clone_info->primitive != (char *) NULL)
    (void) CloneString(&clone_info->primitive,draw_info->primitive);
  if (draw_info->geometry != (char *) NULL)
    clone_info->geometry=AllocateString(draw_info->geometry);
  clone_info->affine=draw_info->affine;
  clone_info->gravity=draw_info->gravity;
  clone_info->fill=draw_info->fill;
  clone_info->stroke=draw_info->stroke;
  clone_info->stroke_width=draw_info->stroke_width;
  if (draw_info->fill_pattern != (Image *) NULL)
    clone_info->fill_pattern=CloneImage(draw_info->fill_pattern,0,0,True,
      &draw_info->fill_pattern->exception);
  else
    if (draw_info->tile != (Image *) NULL)
      clone_info->fill_pattern=CloneImage(draw_info->tile,0,0,True,
        &draw_info->tile->exception);
  clone_info->tile=(Image *) NULL;  /* tile is deprecated */
  if (draw_info->stroke_pattern != (Image *) NULL)
    clone_info->stroke_pattern=CloneImage(draw_info->stroke_pattern,0,0,True,
      &draw_info->stroke_pattern->exception);
  clone_info->stroke_antialias=draw_info->stroke_antialias;
  clone_info->text_antialias=draw_info->text_antialias;
  clone_info->fill_rule=draw_info->fill_rule;
  clone_info->linecap=draw_info->linecap;
  clone_info->linejoin=draw_info->linejoin;
  clone_info->miterlimit=draw_info->miterlimit;
  clone_info->dash_offset=draw_info->dash_offset;
  clone_info->decorate=draw_info->decorate;
  clone_info->compose=draw_info->compose;
  if (draw_info->text != (char *) NULL)
    clone_info->text=AllocateString(draw_info->text);
  if (draw_info->font != (char *) NULL)
    (void) CloneString(&clone_info->font,draw_info->font);
  if (draw_info->family != (char *) NULL)
    clone_info->family=AllocateString(draw_info->family);
  clone_info->style=draw_info->style;
  clone_info->stretch=draw_info->stretch;
  clone_info->weight=draw_info->weight;
  if (draw_info->encoding != (char *) NULL)
    clone_info->encoding=AllocateString(draw_info->encoding);
  clone_info->pointsize=draw_info->pointsize;
  if (draw_info->density != (char *) NULL)
    (void) CloneString(&clone_info->density,draw_info->density);
  clone_info->align=draw_info->align;
  clone_info->undercolor=draw_info->undercolor;
  clone_info->border_color=draw_info->border_color;
  if (draw_info->server_name != (char *) NULL)
    (void) CloneString(&clone_info->server_name,draw_info->server_name);
  if (draw_info->dash_pattern != (double *) NULL)
    {
      register long
        x;

      for (x=0; draw_info->dash_pattern[x] != 0.0; x++);
      clone_info->dash_pattern=
        MagickAllocateArray(double *,(x+1),sizeof(double));
      if (clone_info->dash_pattern == (double *) NULL)
        MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
          UnableToAllocateDashPattern);
      (void) memcpy(clone_info->dash_pattern,draw_info->dash_pattern,
        (x+1)*sizeof(double));
    }
  if (draw_info->extra->clip_path != (char *) NULL)
    clone_info->extra->clip_path=AllocateString(draw_info->extra->clip_path);
  if (draw_info->extra->composite_path != (char *) NULL)
    clone_info->extra->composite_path=AllocateString(draw_info->extra->composite_path);
  clone_info->bounds=draw_info->bounds;
  clone_info->clip_units=draw_info->clip_units;
  clone_info->render=draw_info->render;
  clone_info->opacity=draw_info->opacity;
  clone_info->element_reference=draw_info->element_reference;
  clone_info->flags=draw_info->flags;   /* contains SVG compliance bit, etc. */
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n v e r t P a t h T o P o l y g o n                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvertPathToPolygon converts a path to the more efficient sorted
%  rendering form.
%
%  The format of the ConvertPathToPolygon method is:
%
%      PolygonInfo *ConvertPathToPolygon(const DrawInfo *draw_info,
%        const PathInfo *path_info)
%
%  A description of each parameter follows:
%
%    o Method ConvertPathToPolygon returns the path in a more efficient sorted
%      rendering form of type PolygonInfo.
%
%    o draw_info: Specifies a pointer to an DrawInfo structure.
%
%    o path_info: Specifies a pointer to an PathInfo structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Ticket #562: Inconsistent results from qsort callback.

  NOTE: The right-handed coordinate system is: x right postive, y down positive,
  z into the plane positive.

  Prior to fixing ticket #562, the compare procedure in CompareEdges() looked at the
  first segment in each edge (edges can have multiple segments) and compared them as
  follows:

    (1) Within MagickEpsilon, the edge with the smaller starting y coordinate should come
        first.  Otherwise:
    (2) Within MagickEpsilon, the edge with the smaller starting x coordinate should come
        first.  Otherwise:
    (3) As indicated by the cross product v0 X v1, the edge that rotates counter-clockwise
        (as viewed) into the other edge through the smaller angle between them should come
        first.  I.e., if v0 X v1 is negative, v0 should come first.

  This procedure fails to properly differentiate between parallel and anti-parallel edges
  whose starting x and y coordinates are within MagickEpsilon of each other (the cross
  product of such edges is zero).  For these cases, the result for (v0,v1) says v0 comes
  first, but the result for (v1,v0) says v1 comes first (the problem reported by ticket #562).

  It's unclear to me why the x and y coordinate comparisons should use MagickEpsilon, but to
  avoid changing existing behavior this part of the code has not been changed.

  The new compare procedure is as follows:

    (1) Within MagickEpsilon, the edge with the smaller starting y coordinate should come
        first (UNCHANGED).  Otherwise:
    (2) Within MagickEpsilon, the edge with the smaller starting x coordinate should come
        first (UNCHANGED).  Otherwise:
    (3) As indicated by the cross product v0 X v1, the edge that rotates counter-clockwise
        (as viewed) into the other edge through the smaller angle between them should come
        first AS LONG AS THE ANGLE BETWEEN THEM IS NON-ZERO.  I.e., if v0 X v1 is STRICTLY
        NEGATIVE, v0 should come first.  If v0 X v1 IS STRICTLY POSITIVE, v1 should come
        first.  Otherwise:
    (4) (If we get here, the edges are either parallel or anti-parallel, but their starting
        points may or may not be identical.)  The edge with the smaller starting y coordinate
        should come first (DO NOT use MagickEpsilon).  Otherwise:
    (5) The edge with the smaller starting x coordinate should come first (DO NOT use
        MagickEpsilon).  Otherwise:
    (6) (If we get here, the edges starting points are identical) The edge with the smaller
        ending y coordinate should come first.  Otherwise:
    (7) The edge with the smaller ending x coordinate should come first.  Otherwise:
    (8) The edges are identical with respect to their first segments; return 0 to indicate
        this.  Note that previously CompareEdges() never returned a 0.

  While it may seem that steps 4 - 8 are a lot of additional code, note that this code will
  be executed very infrequently, since most cases will be resolved by steps 1 - 3.

  The new code subtracts the two coordinate values and then compares the result to +/- MagickEpsilon.
  When using finite precision floating point arithmetic, this computation is numerically more
  accurate that adding +/- MagickEpsilon to one coordinate and then comparing that result to
  the other coordinate.
*/

static int
CompareEdges(const void *edge0,const void *edge1)
{
  register const PointInfo
    *v0Points,
    *v1Points;

  double
    v0CROSSv1,
    v0x0,
    v0y0,
    v1x0,
    v1y0,
    DeltaYStart,
    DeltaXStart,
    DeltaYEnd,
    DeltaXEnd,
    NegMagickEpsilon = -MagickEpsilon;

  /*
    Compare two edges.
  */
  v0Points=((const EdgeInfo *) edge0)->points;
  v1Points=((const EdgeInfo *) edge1)->points;

  /* (1) the edge with the smaller starting y coordinate should come first */
  DeltaYStart = (v0y0 = v0Points[0].y) - (v1y0 = v1Points[0].y);
  if  ( DeltaYStart < NegMagickEpsilon )
    return(-1);  /* v0y0 + epsi < v1y0 */
  if  ( DeltaYStart > MagickEpsilon )
    return(1);  /* v0y0 - epsi > v1y0 */

  /* y start coordinates are the same within MagickEpsilon */

  /* (2) the edge with the smaller starting x coordinate should come first */
  DeltaXStart = (v0x0 = v0Points[0].x) - (v1x0 = v1Points[0].x);
  if  ( DeltaXStart < NegMagickEpsilon )
    return(-1);  /* v0x0 + epsi < v1x0 */
  if  ( DeltaXStart > MagickEpsilon )
    return(1);  /* v0x0 - epsi > v1x0 */

  /* y and x start coordinates are the same within MagickEpsilon */

  /* compute cross product v0 X v1 */
  v0CROSSv1 = (v0Points[1].x-v0x0)*(v1Points[1].y-v1y0)-(v0Points[1].y-v0y0)*(v1Points[1].x-v1x0);

  /* (3) the edge that rotates ccwise through the smaller angle between them should come first */
  if  ( v0CROSSv1 < 0.0 )
  return(-1);
  if  ( v0CROSSv1 > 0.0 )
    return(1);

  /* edges are either parallel or anti-parallel; check (4) y and (5) x start coords again w/o MagickEpsilon */
  if  ( DeltaYStart < 0.0 )
    return(-1);  /* v0y0 < v1y0 */
  if  ( DeltaYStart > 0.0 )
    return(1);  /* v0y0 > v1y0 */
  if  ( DeltaXStart < 0.0 )
    return(-1);  /* v0x0 < v1x0 */
  if  ( DeltaXStart > 0.0 )
    return(1);  /* v0x0 > v1x0 */

  /* starting points are the same; compare (6) y and (7) x ending points */
  DeltaYEnd = v0Points[1].y - v1Points[1].y;
  if  ( DeltaYEnd < 0.0 )
    return(-1);  /* v0y1 < v1y1 */
  if  ( DeltaYEnd > 0.0 )
    return(1);  /* v0y1 > v1y1 */
  DeltaXEnd = v0Points[1].x - v1Points[1].x;
  if  ( DeltaXEnd < 0.0 )
    return(-1);  /* v0x1 < v1x1 */
  if  ( DeltaXEnd > 0.0 )
    return(1);  /* v0x1 > v1x1 */

  /* (8) the edges are identical with respect to their first segments */
  return(0);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static void
LogPolygonInfo(const PolygonInfo *polygon_info)
{
  register EdgeInfo
    *p;

  register long
    i,
    j;

  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    begin active-edge");
  p=polygon_info->edges;
  for (i=0; i < polygon_info->number_edges; i++)
  {
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),"      edge %lu:",i);
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),"      direction: %s",
      p->direction ? "down" : "up");
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),"      ghostline: %s",
      p->ghostline ? "transparent" : "opaque");
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),
      "      bounds: %g,%g - %g,%g",p->bounds.x1,p->bounds.y1,p->bounds.x2,
      p->bounds.y2);
    for (j=0; j < p->number_points; j++)
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"        %g,%g",
        p->points[j].x,p->points[j].y);
    p++;
  }
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end active-edge");
}

static void
ReversePoints(PointInfo *points,const int number_points)
{
  PointInfo
    point;

  register long
    i;

  for (i=0; i < (number_points >> 1); i++)
  {
    point=points[i];
    points[i]=points[number_points-(i+1)];
    points[number_points-(i+1)]=point;
  }
}

static PolygonInfo *
ConvertPathToPolygon(const PathInfo *path_info)
{
  long
    direction,
    edge,
    next_direction,
    number_edges,
    number_points;

  PointInfo
    point,
    *points;

  PolygonInfo
    *polygon_info;

  SegmentInfo
    bounds;

  register long
    i,
    n;

  MagickBool
    ghostline;

  /*
    Convert a path to the more efficient sorted rendering form.
  */
  polygon_info=MagickAllocateMemory(PolygonInfo *,sizeof(PolygonInfo));
  if (polygon_info == (PolygonInfo *) NULL)
    return((PolygonInfo *) NULL);
  number_edges=16;
  polygon_info->edges=
    MagickAllocateArray(EdgeInfo *,number_edges,sizeof(EdgeInfo));
  if (polygon_info->edges == (EdgeInfo *) NULL)
    {
      DestroyPolygonInfo(polygon_info);
      return((PolygonInfo *) NULL);
    }
  direction=0;
  edge=0;
  ghostline=MagickFalse;
  n=0;
  number_points=0;
  points=(PointInfo *) NULL;
  (void) memset(&point,0,sizeof(PointInfo));
  (void) memset(&bounds,0,sizeof(SegmentInfo));
  polygon_info->edges[edge].number_points=n;
  polygon_info->edges[edge].scanline=0.0;
  polygon_info->edges[edge].highwater=0;
  polygon_info->edges[edge].ghostline=ghostline;
  polygon_info->edges[edge].direction=direction;
  polygon_info->edges[edge].points=points;
  polygon_info->edges[edge].bounds=bounds;
  polygon_info->number_edges=0;
  for (i=0; path_info[i].code != EndCode; i++)
  {
    if ((path_info[i].code == MoveToCode) || (path_info[i].code == OpenCode) ||
        (path_info[i].code == GhostlineCode))
      {
        /*
          Move to.
        */
        if ((points != (PointInfo *) NULL) && (n >= 2))
          {
            if (edge == number_edges)
              {
                number_edges<<=1;
                MagickReallocMemory(EdgeInfo *,polygon_info->edges,
                                    MagickArraySize(number_edges,sizeof(EdgeInfo)));
                if (polygon_info->edges == (EdgeInfo *) NULL)
                  {
                    DestroyPolygonInfo(polygon_info);
                    return((PolygonInfo *) NULL);
                  }
              }
            polygon_info->edges[edge].number_points=n;
            polygon_info->edges[edge].scanline=(-1.0);
            polygon_info->edges[edge].highwater=0;
            polygon_info->edges[edge].ghostline=ghostline;
            polygon_info->edges[edge].direction=direction > 0;
            if (direction < 0)
              ReversePoints(points,n);
            polygon_info->edges[edge].points=points;
            polygon_info->edges[edge].bounds=bounds;
            polygon_info->edges[edge].bounds.y1=points[0].y;
            polygon_info->edges[edge].bounds.y2=points[n-1].y;
            points=(PointInfo *) NULL;
            ghostline=MagickFalse;
            edge++;
          }
        if (points == (PointInfo *) NULL)
          {
            number_points=16;
            points=
              MagickAllocateArray(PointInfo *,number_points,sizeof(PointInfo));
            if (points == (PointInfo *) NULL)
              {
                DestroyPolygonInfo(polygon_info);
                return((PolygonInfo *) NULL);
              }
          }
        ghostline=path_info[i].code == GhostlineCode ? MagickTrue : MagickFalse;
        point=path_info[i].point;
        points[0]=point;
        bounds.x1=point.x;
        bounds.x2=point.x;
        direction=0;
        n=1;
        continue;
      }
    /*
      Line to.
    */
    next_direction=((path_info[i].point.y > point.y) ||
      ((path_info[i].point.y == point.y) &&
       (path_info[i].point.x > point.x))) ? 1 : -1;
    if ((points != (PointInfo *) NULL) && (direction != 0) &&
        (direction != next_direction))
      {
        /*
          New edge.
        */
        point=points[n-1];
        if (edge == number_edges)
          {
            number_edges<<=1;
            MagickReallocMemory(EdgeInfo *,polygon_info->edges,
                                MagickArraySize(number_edges,sizeof(EdgeInfo)));
            if (polygon_info->edges == (EdgeInfo *) NULL)
              {
                DestroyPolygonInfo(polygon_info);
                return((PolygonInfo *) NULL);
              }
          }
        polygon_info->edges[edge].number_points=n;
        polygon_info->edges[edge].scanline=(-1.0);
        polygon_info->edges[edge].highwater=0;
        polygon_info->edges[edge].ghostline=ghostline;
        polygon_info->edges[edge].direction=direction > 0;
        if (direction < 0)
          ReversePoints(points,n);
        polygon_info->edges[edge].points=points;
        polygon_info->edges[edge].bounds=bounds;
        polygon_info->edges[edge].bounds.y1=points[0].y;
        polygon_info->edges[edge].bounds.y2=points[n-1].y;
        number_points=16;
        points=
          MagickAllocateArray(PointInfo *,number_points,sizeof(PointInfo));
        if (points == (PointInfo *) NULL)
          {
            DestroyPolygonInfo(polygon_info);
            return((PolygonInfo *) NULL);
          }
        n=1;
        ghostline=MagickFalse;
        points[0]=point;
        bounds.x1=point.x;
        bounds.x2=point.x;
        edge++;
      }
    direction=next_direction;
    if (points == (PointInfo *) NULL)
      continue;
    if (n == number_points)
      {
        number_points<<=1;
        MagickReallocMemory(PointInfo *,points,MagickArraySize(number_points,sizeof(PointInfo)));
        if (points == (PointInfo *) NULL)
          {
            DestroyPolygonInfo(polygon_info);
            return((PolygonInfo *) NULL);
          }
      }
    point=path_info[i].point;
    points[n]=point;
    if (point.x < bounds.x1)
      bounds.x1=point.x;
    if (point.x > bounds.x2)
      bounds.x2=point.x;
    n++;
  }
  if (points != (PointInfo *) NULL)
    {
      if (n < 2)
        {
          MagickFreeMemory(points);
        }
      else
        {
          if (edge == number_edges)
            {
              number_edges<<=1;
              MagickReallocMemory(EdgeInfo *,polygon_info->edges,
                                  MagickArraySize(number_edges,sizeof(EdgeInfo)));
              if (polygon_info->edges == (EdgeInfo *) NULL)
                {
                  DestroyPolygonInfo(polygon_info);
                  return((PolygonInfo *) NULL);
                }
            }
          polygon_info->edges[edge].number_points=n;
          polygon_info->edges[edge].scanline=(-1.0);
          polygon_info->edges[edge].highwater=0;
          polygon_info->edges[edge].ghostline=ghostline;
          polygon_info->edges[edge].direction=direction > 0;
          if (direction < 0)
            ReversePoints(points,n);
          polygon_info->edges[edge].points=points;
          polygon_info->edges[edge].bounds=bounds;
          polygon_info->edges[edge].bounds.y1=points[0].y;
          polygon_info->edges[edge].bounds.y2=points[n-1].y;
          ghostline=MagickFalse;
          edge++;
        }
    }
  polygon_info->number_edges=edge;
  qsort(polygon_info->edges,polygon_info->number_edges,sizeof(EdgeInfo),
    CompareEdges);
  if (IsEventLogging())
    LogPolygonInfo(polygon_info);
  return(polygon_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n v e r t P r i m i t i v e T o P a t h                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvertPrimitiveToPath converts a PrimitiveInfo structure into a
%  vector path structure.
%
%  The format of the ConvertPrimitiveToPath method is:
%
%      PathInfo *ConvertPrimitiveToPath(const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o Method ConvertPrimitiveToPath returns a vector path structure of type
%      PathInfo.
%
%    o draw_info: a structure of type DrawInfo.
%
%    o primitive_info: Specifies a pointer to an PrimitiveInfo structure.
%
%
*/

static void
LogPathInfo(const PathInfo *path_info)
{
  register const PathInfo
    *p;

  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    begin vector-path");
  for (p=path_info; p->code != EndCode; p++)
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),
      "      %g,%g %s",p->point.x,p->point.y,p->code == GhostlineCode ?
      "moveto ghostline" : p->code == OpenCode ? "moveto open" :
      p->code == MoveToCode ? "moveto" : p->code == LineToCode ? "lineto" :
      "?");
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end vector-path");
}

static PathInfo *
ConvertPrimitiveToPath(const DrawInfo *draw_info,
                       const PrimitiveInfo *primitive_info)
{
  PathInfo
    *path_info;

  PathInfoCode
    code;

  PointInfo
    p,  /* first point in subpath (i.e., just did a "moveto" to this point) */
    q;  /* previous point in subpath */

  register long
    i,
    n;

  long
    coordinates,  /* number of points in subpath */
    start;        /* index to start of subpath in path_info */

  MagickBool
    IsClosedSubPath;

  ARG_NOT_USED(draw_info);

  /*
    Converts a PrimitiveInfo structure into a vector path structure.
  */
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    case ColorPrimitive:
    case MattePrimitive:
    case TextPrimitive:
    case ImagePrimitive:
      return((PathInfo *) NULL);
    default:
      break;
  }
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++);
  path_info=MagickAllocateArray(PathInfo *,(2*i+3),sizeof(PathInfo));
  if (path_info == (PathInfo *) NULL)
    return((PathInfo *) NULL);
  coordinates=0;
  n=0;
  p.x=(-1.0);
  p.y=(-1.0);
  q.x=(-1.0);
  q.y=(-1.0);
  start=0;
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    code=LineToCode;
    if (coordinates <= 0)
      {
        /* start of a new subpath */
        coordinates=(long) primitive_info[i].coordinates;
        p=primitive_info[i].point;  /* first point in subpath */
        start=n;  /* index to start of subpath in path_info */
        code=MoveToCode;
        IsClosedSubPath=PRIMINF_GET_IS_CLOSED_SUBPATH(&primitive_info[i]);
      }
    coordinates--;
    /*
      Do not put the current point into path_info if it is a duplicate of
      (i.e. "too close" to) the previous point.  However, the current point
      is always put into path_info when it is the first (code==MoveToCode)
      or last point in a subpath.  Note that for the first point the "previous
      point" (q) is not valid (usually a leftover from the previous subpath),
      so the first point test should be done first.  W.r.t. the last point,
      sometimes eliminating the last point as a "duplicate" will result in a
      polygon (path) not being completely closed, causing small "nubs" to
      protrude from the filled shape at that point.
    */
    if ((code == MoveToCode) || (coordinates <= 0) ||
        (fabs(q.x-primitive_info[i].point.x) > MagickEpsilon) ||
        (fabs(q.y-primitive_info[i].point.y) > MagickEpsilon))
      {
        /* put current point into path_info*/
        path_info[n].code=code;
        path_info[n].point=primitive_info[i].point;
        q=primitive_info[i].point;  /* will be "previous point" for next iteration */
        n++;
      }
    if (coordinates > 0)
      continue;   /* go process next point in current subpath */
    /*
      The current point is the last point in the subpath.  If it closes
      the subpath, go on to the next subpath.
    */
    if  ( IsClosedSubPath )
      {
        IsClosedSubPath = MagickFalse;
        continue;
      }
    /*
      The just completed subpath is not closed.  Mark it as "open" and add two
      more points (repeat of current point + subpath start point) to "virtually"
      close it (this is a "ghost line").
    */
    path_info[start].code=OpenCode;
    path_info[n].code=GhostlineCode;
    path_info[n].point=primitive_info[i].point;
    n++;
    path_info[n].code=LineToCode;
    path_info[n].point=p;
    n++;
  }
  path_info[n].code=EndCode;
  path_info[n].point.x=0.0;
  path_info[n].point.y=0.0;
  if (IsEventLogging())
    LogPathInfo(path_info);
  return(path_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D r a w I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyDrawInfo() deallocates memory associated with an DrawInfo
%  structure.
%
%  The format of the DestroyDrawInfo method is:
%
%      void DestroyDrawInfo(DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o draw_info: The draw info.
%
%
*/
MagickExport void
DestroyDrawInfo(DrawInfo *draw_info)
{
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  MagickFreeMemory(draw_info->primitive);
  MagickFreeMemory(draw_info->text);
  MagickFreeMemory(draw_info->geometry);
  if (draw_info->tile != 0)
    DestroyImage(draw_info->tile);
  if (draw_info->fill_pattern != (Image *) NULL)
    DestroyImage(draw_info->fill_pattern);
  if (draw_info->stroke_pattern != (Image *) NULL)
    DestroyImage(draw_info->stroke_pattern);
  MagickFreeMemory(draw_info->font);
  MagickFreeMemory(draw_info->family);
  MagickFreeMemory(draw_info->encoding);
  MagickFreeMemory(draw_info->density);
  MagickFreeMemory(draw_info->server_name);
  MagickFreeMemory(draw_info->dash_pattern);
  MagickFreeMemory(draw_info->extra->clip_path);
  MagickFreeMemory(draw_info->extra->composite_path);
  MagickFreeMemory(draw_info->extra);
  (void) memset((void *)draw_info,0xbf,sizeof(DrawInfo));
  MagickFreeMemory(draw_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y E d g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyEdge destroys the specified polygon edge.
%
%  The format of the DestroyEdge method is:
%
%      long DestroyEdge(PolygonInfo *polygon_info,const int edge)
%
%  A description of each parameter follows:
%
%    o polygon_info: Specifies a pointer to an PolygonInfo structure.
%
%    o edge: the polygon edge number to destroy.
%
%
*/
static long
DestroyEdge(PolygonInfo * restrict polygon_info,const long edge)
{
  assert(edge >= 0);
  assert(edge < polygon_info->number_edges);
  MagickFreeMemory(polygon_info->edges[edge].points);
  polygon_info->number_edges--;
  if (edge < polygon_info->number_edges)
    (void) memmove(polygon_info->edges+edge,polygon_info->edges+edge+1,
      (polygon_info->number_edges-edge)*sizeof(EdgeInfo));
  return(polygon_info->number_edges);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y G r a d i e n t I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyGradientInfo() deallocates memory associated with the GradientInfo
%  list.
%
%  The format of the DestroyGradientInfo method is:
%
%      DestroyGradientInfo(GradientInfo *gradient_info)
%
%  A description of each parameter follows:
%
%    o gradient_info: The gradient info.
%
%
*/
#if 0
static void
DestroyGradientInfo(GradientInfo *gradient_info)
{
  register GradientInfo
    *p;

  if (gradient_info == (GradientInfo *) NULL)
    return;
  assert(gradient_info->signature == MagickSignature);
  for (p=gradient_info; p->previous != (GradientInfo *) NULL; p=p->previous);
  for (gradient_info=p; p != (GradientInfo *) NULL; gradient_info=p)
  {
    p=p->next;
    MagickFreeMemory(gradient_info);
  }
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y P o l y g o n I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPolygonInfo destroys the PolygonInfo data structure.
%
%  The format of the DestroyPolygonInfo method is:
%
%      void DestroyPolygonInfo(PolygonInfo *polygon_info)
%
%  A description of each parameter follows:
%
%    o polygon_info: Specifies a pointer to an PolygonInfo structure.
%
%
*/
static void
DestroyPolygonInfo(void *polygon_info_void)
{
  PolygonInfo
    *polygon_info = (PolygonInfo *) polygon_info_void;

  if (polygon_info != (PolygonInfo *) NULL)
    {
      register long
        i;

      for (i=0; i < polygon_info->number_edges; i++)
        MagickFreeMemory(polygon_info->edges[i].points);
      MagickFreeMemory(polygon_info->edges);
      MagickFreeMemory(polygon_info);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     D r a w A f f i n e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawAffineImage() composites the source over the destination image as
%  dictated by the affine transform.
%
%  The format of the DrawAffineImage method is:
%
%      unsigned int DrawAffineImage(Image *image,const Image *composite,
%        const AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o image: The composite image.
%
%    o affine: The affine transform.
%
%
*/

static SegmentInfo
AffineEdge(const Image *image,const AffineMatrix *affine,
           const long y,const SegmentInfo *edge)
{
  double
    intercept,
    z;

  register long
    x;

  SegmentInfo
    inverse_edge;

  /*
    Determine left and right edges.
  */
  inverse_edge.x1=edge->x1;
  inverse_edge.y1=0;
  inverse_edge.x2=edge->x2;
  inverse_edge.y2=0;
  z=affine->ry*(y+0.5)+affine->tx;
  if (affine->sx > MagickEpsilon)
    {
      intercept=(-z/affine->sx);
      x=(long) ceil(intercept+MagickEpsilon-0.5);
      if (x > inverse_edge.x1)
        inverse_edge.x1=x;
      intercept=(-z+image->columns)/affine->sx;
      x=(long) ceil(intercept-MagickEpsilon-0.5);
      if (x < inverse_edge.x2)
        inverse_edge.x2=x;
    }
  else
    if (affine->sx < -MagickEpsilon)
      {
        intercept=(-z+image->columns)/affine->sx;
        x=(long) ceil(intercept+MagickEpsilon-0.5);
        if (x > inverse_edge.x1)
          inverse_edge.x1=x;
        intercept=(-z/affine->sx);
        x=(long) ceil(intercept-MagickEpsilon-0.5);
        if (x < inverse_edge.x2)
          inverse_edge.x2=x;
      }
    else
      if ((z < 0.0) || (z >= image->columns))
        {
          inverse_edge.x2=edge->x1;
          return(inverse_edge);
        }
  /*
    Determine top and bottom edges.
  */
  z=affine->sy*(y+0.5)+affine->ty;
  if (affine->rx > MagickEpsilon)
    {
      intercept=(-z /affine->rx);
      x=(long) ceil(intercept+MagickEpsilon-0.5);
      if (x > inverse_edge.x1)
        inverse_edge.x1=x;
      intercept=(-z+image->rows)/affine->rx;
      x=(long) ceil(intercept-MagickEpsilon-0.5);
      if (x < inverse_edge.x2)
        inverse_edge.x2=x;
    }
  else
    if (affine->rx < -MagickEpsilon)
      {
        intercept=(-z+image->rows)/affine->rx;
        x=(long) ceil(intercept+MagickEpsilon-0.5);
        if (x > inverse_edge.x1)
          inverse_edge.x1=x;
        intercept=(-z/affine->rx);
        x=(long) ceil(intercept-MagickEpsilon-0.5);
        if (x < inverse_edge.x2)
          inverse_edge.x2=x;
      }
    else
      if ((z < 0) || (z >= image->rows))
        {
          inverse_edge.x2=edge->x1;
          return(inverse_edge);
        }
  return(inverse_edge);
}

static AffineMatrix
InverseAffineMatrix(const AffineMatrix *affine)
{
  AffineMatrix
    inverse_affine;

  double
    determinant;

  determinant=1.0/(affine->sx*affine->sy-affine->rx*affine->ry);
  inverse_affine.sx=determinant*affine->sy;
  inverse_affine.rx=determinant*(-affine->rx);
  inverse_affine.ry=determinant*(-affine->ry);
  inverse_affine.sy=determinant*affine->sx;
  inverse_affine.tx=
    (-affine->tx)*inverse_affine.sx-affine->ty*inverse_affine.ry;
  inverse_affine.ty=
    (-affine->tx)*inverse_affine.rx-affine->ty*inverse_affine.sy;
  return(inverse_affine);
}

#define AffineDrawImageText "[%s] Affine composite..."
MagickExport MagickPassFail
DrawAffineImage(Image *image,const Image *composite,
                const AffineMatrix *affine)
{
  MagickPassFail
    status = MagickPass;

  unsigned long
    row_count=0;

  AffineMatrix
    inverse_affine;

  long
    y,
    y_max,
    y_min;

  PointInfo
    extent[4],
    min,
    max;

  register long
    i;

  SegmentInfo
    edge;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(composite != (const Image *) NULL);
  assert(composite->signature == MagickSignature);
  assert(affine != (AffineMatrix *) NULL);

  /*
    Determine bounding box.
  */
  extent[0].x=0;
  extent[0].y=0;
  extent[1].x=composite->columns;
  extent[1].y=0;
  extent[2].x=composite->columns;
  extent[2].y=composite->rows;
  extent[3].x=0;
  extent[3].y=composite->rows;
  for (i=0; i < 4; i++)
    {
      register long
        x;

      x=(long) extent[i].x;
      y=(long) extent[i].y;
      extent[i].x=x*affine->sx+y*affine->ry+affine->tx;
      extent[i].y=x*affine->rx+y*affine->sy+affine->ty;
    }
  min=extent[0];
  max=extent[0];
  for (i=1; i < 4; i++)
    {
      if (min.x > extent[i].x)
        min.x=extent[i].x;
      if (min.y > extent[i].y)
        min.y=extent[i].y;
      if (max.x < extent[i].x)
        max.x=extent[i].x;
      if (max.y < extent[i].y)
        max.y=extent[i].y;
    }
  /*
    Affine transform image.
  */
  (void) SetImageType(image,TrueColorType);
  edge.x1=min.x;
  edge.y1=min.y;
  edge.x2=max.x;
  edge.y2=max.y;
  inverse_affine=InverseAffineMatrix(affine);
  if (edge.y1 < 0)
    edge.y1=0.0;
  if (edge.y2 > image->rows - 1)
    edge.y2=image->rows-1;
  y_min=(long) ceil(edge.y1-0.5);
  y_max=(long) floor(edge.y2+0.5);
#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static) shared(row_count, status)
#  endif
#endif
  for (y=y_min; y <= y_max; y++)
    {
      MagickBool
        thread_status;

      long
        start,
        stop;

      SegmentInfo
        inverse_edge;

      register PixelPacket
        *q;

      register long
        x;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_DrawAffineImage)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      inverse_edge=AffineEdge(composite,&inverse_affine,y,&edge);
      if (inverse_edge.x2 < inverse_edge.x1)
        continue;
      if (inverse_edge.x1 < 0)
        inverse_edge.x1=0.0;
      if (inverse_edge.x2 > image->columns-1)
        inverse_edge.x2=image->columns-1;
      start=(long) ceil(inverse_edge.x1-0.5);
      stop=(long) floor(inverse_edge.x2+0.5);
      if (stop >= start)
        x=start;
      else
        x=stop;
      q=GetImagePixelsEx(image,x,y,stop-x+1,1,&image->exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;
      if (thread_status != MagickFail)
        {
          for ( ; x <= stop; x++)
            {
              PointInfo
                point;

              PixelPacket
                pixel;

              point.x=x*inverse_affine.sx+y*inverse_affine.ry+inverse_affine.tx;
              point.y=x*inverse_affine.rx+y*inverse_affine.sy+inverse_affine.ty;
              if (InterpolateViewColor(AccessDefaultCacheView(composite),
                                       &pixel,
                                       point.x,
                                       point.y,
                                       &image->exception) == MagickFail)
                {
                  thread_status=MagickFail;
                  break;
                }
              if (!composite->matte)
                pixel.opacity=OpaqueOpacity;
              AlphaCompositePixel(q,&pixel,pixel.opacity,q,q->opacity);
              q++;
            }
          if (thread_status != MagickFail)
            if (!SyncImagePixelsEx(image,&image->exception))
              thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_DrawAffineImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,y_max-y_min+1))
          if (!MagickMonitorFormatted(row_count,y_max-y_min+1,&image->exception,
                                      AffineDrawImageText,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w B o u n d i n g R e c t a n g l e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawBoundingRectangles draws the bounding rectangles on the image.
%  This is only useful for developers debugging the rendering algorithm.
%
%  The format of the DrawBoundingRectangles method is:
%
%      void DrawBoundingRectangles(Image *image,const DrawInfo *draw_info,
%        PolygonInfo *polygon_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o polygon_info: Specifies a pointer to a PolygonInfo structure.
%
%
*/
static void
DrawBoundingRectangles(Image *image,const DrawInfo *draw_info,
                       const PolygonInfo *polygon_info)
{
  DrawInfo
    *clone_info;

  double
    mid,
    stroke_width_limited;

  PointInfo
    end,
    resolution,
    start;

  PrimitiveInfo
    primitive_info[6];

  register long
    i;

  SegmentInfo
    bounds;

  long
    coordinates;

  memset(primitive_info,0,sizeof(primitive_info));
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) QueryColorDatabase("#000000ff",&clone_info->fill,&image->exception);
  resolution.x=72.0;
  resolution.y=72.0;
  if (clone_info->density != (char *) NULL)
    {
      int
        count;

      count=GetMagickDimension(clone_info->density,&resolution.x,&resolution.y,NULL,NULL);
      if (count != 2)
        resolution.y=resolution.x;
    }

  /* sanity check for excessively big stroke_width (ticket #515) */
  if  ( (stroke_width_limited = STROKE_WIDTH_LIMIT(image)) > clone_info->stroke_width )
      stroke_width_limited = clone_info->stroke_width;

  mid=(resolution.x/72.0)*ExpandAffine(&clone_info->affine)*
    stroke_width_limited/2.0;
  bounds.x1=0.0;
  bounds.y1=0.0;
  bounds.x2=0.0;
  bounds.y2=0.0;
  if (polygon_info != (PolygonInfo *) NULL)
    {
      bounds=polygon_info->edges[0].bounds;
      for (i=1; i < polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].bounds.x1 < bounds.x1)
          bounds.x1=polygon_info->edges[i].bounds.x1;
        if (polygon_info->edges[i].bounds.y1 < bounds.y1)
          bounds.y1=polygon_info->edges[i].bounds.y1;
        if (polygon_info->edges[i].bounds.x2 > bounds.x2)
          bounds.x2=polygon_info->edges[i].bounds.x2;
        if (polygon_info->edges[i].bounds.y2 > bounds.y2)
          bounds.y2=polygon_info->edges[i].bounds.y2;
      }
      bounds.x1-=mid;
      bounds.x1=bounds.x1 < 0.0 ? 0.0 :
        bounds.x1 >= image->columns ? image->columns-1 : bounds.x1;
      bounds.y1-=mid;
      bounds.y1=bounds.y1 < 0.0 ? 0.0 :
        bounds.y1 >= image->rows ? image->rows-1 : bounds.y1;
      bounds.x2+=mid;
      bounds.x2=bounds.x2 < 0.0 ? 0.0 :
        bounds.x2 >= image->columns ? image->columns-1 : bounds.x2;
      bounds.y2+=mid;
      bounds.y2=bounds.y2 < 0.0 ? 0.0 :
        bounds.y2 >= image->rows ? image->rows-1 : bounds.y2;
      for (i=0; i < polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].direction)
          (void) QueryColorDatabase("red",&clone_info->stroke,
            &image->exception);
        else
          (void) QueryColorDatabase("green",&clone_info->stroke,
            &image->exception);
        start.x=polygon_info->edges[i].bounds.x1-mid;
        start.y=polygon_info->edges[i].bounds.y1-mid;
        end.x=polygon_info->edges[i].bounds.x2+mid;
        end.y=polygon_info->edges[i].bounds.y2+mid;
        primitive_info[0].primitive=RectanglePrimitive;
        TraceRectangle(primitive_info,start,end);
        primitive_info[0].method=ReplaceMethod;
        coordinates=(long) primitive_info[0].coordinates;
        primitive_info[coordinates].primitive=UndefinedPrimitive;
        (void) DrawPrimitive(image,clone_info,primitive_info);
      }
    }
  (void) QueryColorDatabase("blue",&clone_info->stroke,&image->exception);
  start.x=bounds.x1-mid;
  start.y=bounds.y1-mid;
  end.x=bounds.x2+mid;
  end.y=bounds.y2+mid;
  primitive_info[0].primitive=RectanglePrimitive;
  TraceRectangle(primitive_info,start,end);
  primitive_info[0].method=ReplaceMethod;
  coordinates=(long) primitive_info[0].coordinates;
  primitive_info[coordinates].primitive=UndefinedPrimitive;
  (void) DrawPrimitive(image,clone_info,primitive_info);
  DestroyDrawInfo(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C l i p P a t h                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawClipPath() draws the clip path on the image mask.
%
%  The format of the DrawClipPath method is:
%
%      MagickPassFail DrawClipPath(Image *image,const DrawInfo *draw_info,
%        const char *name)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o name: The name of the clip path.
%
%
*/
MagickExport MagickPassFail
DrawClipPath(Image *image,const DrawInfo *draw_info, const char *name)
{
  char
    clip_path[MaxTextExtent];

  const ImageAttribute
    *attribute;

  DrawInfo
    *clone_info;

  MagickPassFail
    status=MagickPass;

  Image
    *image_clip_mask;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (const DrawInfo *) NULL);
  FormatString(clip_path,"[%.1024s]",name);
  attribute=GetImageAttribute(image,clip_path);
  if (attribute == (ImageAttribute *) NULL)
    return(MagickFail);
  image_clip_mask = *ImageGetClipMask(image);
  if (image_clip_mask == (Image *) NULL)
    {
      Image
        *clip_mask;

      clip_mask=CloneImage(image,image->columns,image->rows,MagickTrue,
        &image->exception);
      if (clip_mask == (Image *) NULL)
        return(MagickFail);
      (void) SetImageClipMask(image,clip_mask);
      DestroyImage(clip_mask);
      image_clip_mask = *ImageGetClipMask(image);
    }
  else
    {
      /*
        Re-clone the image attributes, since more may have been added since
        the clip_mask image was created.
      */
      DestroyImageAttributes(image_clip_mask);
      CloneImageAttributes(image_clip_mask,image);
    }
  (void) QueryColorDatabase("none",&image_clip_mask->background_color,
    &image->exception);
  (void) SetImage(image_clip_mask,TransparentOpacity);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "\nbegin clip-path %.1024s",draw_info->extra->clip_path);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) CloneString(&clone_info->primitive,attribute->value);
  (void) QueryColorDatabase("white",&clone_info->fill,&image->exception);

  /*
    According to the SVG spec:

    The raw geometry of each child element exclusive of rendering properties such
    as ‘fill’, ‘stroke’, ‘stroke-width’ within a ‘clipPath’ conceptually defines a
    1-bit mask (with the possible exception of anti-aliasing along the edge of the
    geometry) which represents the silhouette of the graphics associated with that
    element.  Anything outside the outline of the object is masked out.

    To conform with the spec, we make sure that fill color (set above), stroke color,
    stroke width, and group/global opacity are set to appropriate values.
  */
  SetDrawInfoClippingPath(clone_info,MagickTrue);
  if  ( IsDrawInfoSVGCompliant(clone_info) )
    {
      /* changes to fill, etc. will be ignored */
      (void) QueryColorDatabase("none",&clone_info->stroke,&image->exception);  /* SVG default */
      clone_info->stroke_width = 0.0;   /* SVG default */
      clone_info->opacity = OpaqueOpacity;  /* SVG default */
    }

  MagickFreeMemory(clone_info->extra->clip_path);
  status&=DrawImage(image_clip_mask,clone_info);
  status&=NegateImage(image_clip_mask,False);
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"end clip-path");
  return(status);
}

/* code below for DrawCompositeMask() cloned/modifed from DrawClipMask() */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C o m p o s i t e M a s k                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawCompositeMask() draws the composite mask on the image mask.
%
%  The format of the DrawCompositeMask method is:
%
%      MagickPassFail DrawCompositeMask(Image *image,const DrawInfo *draw_info,
%        const char *name)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o name: The name of the composite mask.
%
%
*/
MagickExport MagickPassFail
DrawCompositeMask(Image *image,const DrawInfo *draw_info, const char *name)
{
  char
    composite_path[MaxTextExtent];

  const ImageAttribute
    *attribute;

  DrawInfo
    *clone_info;

  MagickPassFail
    status=MagickPass;

  Image
    *image_composite_mask;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (const DrawInfo *) NULL);
  FormatString(composite_path,"[%.1024s]",name);
  attribute=GetImageAttribute(image,composite_path);
  if (attribute == (ImageAttribute *) NULL)
    return(MagickFail);
  image_composite_mask = *ImageGetCompositeMask(image);
  if (image_composite_mask == (Image *) NULL)
    {
      Image
        *composite_mask;

      composite_mask=CloneImage(image,image->columns,image->rows,MagickTrue,
        &image->exception);
      if (composite_mask == (Image *) NULL)
        return(MagickFail);
      (void) SetImageCompositeMask(image,composite_mask);
      DestroyImage(composite_mask);
      image_composite_mask = *ImageGetCompositeMask(image);
    }
  else
    {
      /*
        Re-clone the image attributes, since more may have been added since
        the composite_mask image was created.
      */
      DestroyImageAttributes(image_composite_mask);
      CloneImageAttributes(image_composite_mask,image);
    }
  (void) QueryColorDatabase("none",&image_composite_mask->background_color,
    &image->exception);
  (void) SetImage(image_composite_mask,TransparentOpacity);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "\nbegin mask %.1024s",draw_info->extra->composite_path);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) CloneString(&clone_info->primitive,attribute->value);
  /* these settings are per the SVG spec */
  (void) QueryColorDatabase("black",&clone_info->fill,&image->exception);
  (void) QueryColorDatabase("none",&clone_info->stroke,&image->exception);
  clone_info->stroke_width = 1.0;
  clone_info->opacity = OpaqueOpacity;
  status&=DrawImage(image_composite_mask,clone_info);
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"end composite-path");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w D a s h P o l y g o n                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawDashPolygon draws a dashed polygon (line, rectangle, ellipse)
%  on the image while respecting the dash offset and dash pattern attributes.
%
%  The format of the DrawDashPolygon method is:
%
%      MagickPassFail DrawDashPolygon(const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info,Image *image)
%
%  A description of each parameter follows:
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%    o image: The image.
%
%
*/
static MagickPassFail
DrawDashPolygon(const DrawInfo *draw_info,const PrimitiveInfo *primitive_info,
                Image *image)
{
  DrawInfo
    *clone_info;

  double
    length,
    maximum_length,
    offset,
    scale,
    total_length;

  MagickPassFail
    status;

  PrimitiveInfo
    *dash_polygon;

  register long
    i;

  register double
    dx,
    dy;

  int
    number_vertices;

  int
    j,
    n;

  assert(draw_info != (const DrawInfo *) NULL);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    begin draw-dash");
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  if (clone_info == (DrawInfo *) NULL)
    return(MagickFail);
  clone_info->miterlimit=0;
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++);
  number_vertices=i;
  dash_polygon=MagickAllocateArray(PrimitiveInfo *,
                                   (size_t) 2*number_vertices+1,
                                   sizeof(PrimitiveInfo));
  if (dash_polygon == (PrimitiveInfo *) NULL)
    {
      DestroyDrawInfo(clone_info);
      return(MagickFail);
    }
  dash_polygon[0]=primitive_info[0];
  scale=ExpandAffine(&draw_info->affine);
  length=scale*draw_info->dash_pattern[0];
  offset=draw_info->dash_offset != 0.0 ? scale*draw_info->dash_offset : 0.0;
  j=1;
  for (n=0; offset > 0.0; j=0)
  {
    if (draw_info->dash_pattern[n] <= 0.0)
      break;
    length=scale*draw_info->dash_pattern[n];
    if (offset > length)
      {
        offset-=length;
        n++;
        length=scale*draw_info->dash_pattern[n];
        continue;
      }
    if (offset < length)
      {
        length-=offset;
        offset=0.0;
        break;
      }
    offset=0.0;
    n++;
  }
  status=MagickPass;
  maximum_length=0.0;
  total_length=0.0;
  for (i=1; (i < number_vertices) && (length >= 0.0); i++)
  {
    dx=primitive_info[i].point.x-primitive_info[i-1].point.x;
    dy=primitive_info[i].point.y-primitive_info[i-1].point.y;
    maximum_length=sqrt(dx*dx+dy*dy+MagickEpsilon);
    if (length == 0.0)
      {
        n++;
        if (draw_info->dash_pattern[n] == 0.0)
          n=0;
        length=scale*draw_info->dash_pattern[n];
      }
    for (total_length=0.0; (length >= 0.0) && (maximum_length >= (length+total_length)); )
    {
      total_length+=length;
      if (n & 0x01)
        {
          dash_polygon[0]=primitive_info[0];
          dash_polygon[0].point.x=primitive_info[i-1].point.x+
            dx*total_length/maximum_length;
          dash_polygon[0].point.y=primitive_info[i-1].point.y+
            dy*total_length/maximum_length;
          j=1;
        }
      else
        {
          if (j+1 > number_vertices)
            break;
          dash_polygon[j]=primitive_info[i-1];
          dash_polygon[j].point.x=primitive_info[i-1].point.x+
            dx*total_length/maximum_length;
          dash_polygon[j].point.y=primitive_info[i-1].point.y+
            dy*total_length/maximum_length;
          dash_polygon[j].coordinates=1;
          j++;
          dash_polygon[0].coordinates=j;
          dash_polygon[j].primitive=UndefinedPrimitive;
          status&=DrawStrokePolygon(image,clone_info,dash_polygon);
        }
      n++;
      if (draw_info->dash_pattern[n] == 0.0)
        n=0;
      length=scale*draw_info->dash_pattern[n];
    }
    length-=(maximum_length-total_length);
    if (n & 0x01)
      continue;
    dash_polygon[j]=primitive_info[i];
    dash_polygon[j].coordinates=1;
    j++;
  }
  if ((total_length < maximum_length) && ((n & 0x01) == 0) && (j > 1))
    {
      dash_polygon[j]=primitive_info[i-1];
      dash_polygon[j].point.x+=MagickEpsilon;
      dash_polygon[j].point.y+=MagickEpsilon;
      dash_polygon[j].coordinates=1;
      j++;
      dash_polygon[0].coordinates=(size_t) j;
      dash_polygon[j].primitive=UndefinedPrimitive;
      status&=DrawStrokePolygon(image,clone_info,dash_polygon);
    }
  MagickFreeMemory(dash_polygon);
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end draw-dash");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use DrawImage() to draw a graphic primitive on your image.  The primitive
%  may be represented as a string or filename.  Precede the filename with an
%  "at" sign (@) and the contents of the file are drawn on the image.  You
%  can affect how text is drawn by setting one or more members of the draw
%  info structure.
%
%  Note that this is a legacy interface. Authors of new code should consider
%  using the Draw* methods defined by magick/draw.h since they are better
%  documented and less error prone.
%
%  The format of the DrawImage method is:
%
%      MagickPassFail DrawImage(Image *image,const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%
*/
static inline MagickBool
IsPoint(const char *point)
{
  char
    *p;

  double
    value;

  /* check for a floating-point (vs. integer) value so .25 will not be rejected */
  value=strtod(point,&p);
  (void) value;
  return(p != point);
}

static const char *recursion_key ="[DrawImageRecursion]";
static long DrawImageGetCurrentRecurseLevel(Image *image)
{
  const ImageAttribute
    *attribute;

  long
    recurse_level=0;

  if ((attribute=GetImageAttribute(image,recursion_key)) != (ImageAttribute *) NULL)
    recurse_level=MagickAtoL(attribute->value);

  return recurse_level;
}
static void DrawImageSetCurrentRecurseLevel(Image *image,const long recurse_level)
{
  char
    recursion_str[MaxTextExtent];

  FormatString(recursion_str,"%ld",recurse_level);
  (void) SetImageAttribute(image,recursion_key,NULL);
  (void) SetImageAttribute(image,recursion_key,recursion_str);
}
static MagickPassFail DrawImageRecurseIn(Image *image)
{
  long
    recurse_level;

  recurse_level=DrawImageGetCurrentRecurseLevel(image);
  recurse_level++;
  DrawImageSetCurrentRecurseLevel(image,recurse_level);

  if ((recurse_level < 0) || (recurse_level > 100))
    {
      char
        recursion_str[MaxTextExtent];
      FormatString(recursion_str,"%ld",recurse_level);
      ThrowException(&image->exception,DrawError,DrawingRecursionDetected,
                     recursion_str);
      return MagickFail;
    }

  return MagickPass;
}
static void DrawImageRecurseOut(Image *image)
{
  long
    recurse_level=0;

  recurse_level=DrawImageGetCurrentRecurseLevel(image);
  recurse_level--;
  DrawImageSetCurrentRecurseLevel(image,recurse_level);
}


/*
  Code from DrawImage() that extracted tokens between push/pop clip-path
  and added them as an attribute has been refactored into new function
  ExtractTokensBetweenPushPop().  Added to support new elements "use" and
  "class".
*/
static
char *	ExtractTokensBetweenPushPop (
  char * q,                 /* address of pointer into primitive string */
  char * token,             /* big enough buffer for extracted string */
  size_t token_max_length,
  char const * pop_string,  /* stop when we see pop pop_string */
  Image *image,
  size_t * pExtractedLength /* if not null, length of extracted string returned */
  )
{/*ExtractTokensBetweenPushPop*/

  char const * p;
  char * pAfterPopString = 0;
  char
    name[MaxTextExtent];
  size_t ExtractedLength = 0;

  /* next token is name associated with push/pop data */
  MagickGetToken(q,&q,token,token_max_length);
  FormatString(name,"[%.1024s]",token);

  /* search for "pop <pop_string>" */
  for (p=q; *q != '\0'; )
  {
    char * qStart = q;
    MagickGetToken(q,&q,token,token_max_length);
    if  ( q == qStart )
      {
        /* infinite loop detection */
        pAfterPopString = q;  /* need this to be valid */
        break;
      }
    if (LocaleCompare(token,"pop") == 0)
      {
        MagickGetToken(q,&pAfterPopString,token,token_max_length);
        if (LocaleCompare(token,pop_string) == 0)
          break;  /* found "pop <pop_string>" */
      }
  }

  /* sanity check on extracted string length */
  if  ( q > (p+4U) )
    {
      ExtractedLength = q - (p+4U);
      (void) strncpy(token,p,ExtractedLength);
    }
  token[ExtractedLength] = '\0';
  (void) SetImageAttribute(image,name,token);
  q = pAfterPopString;  /* skip ID string after "pop" */
  if  ( pExtractedLength )
    *pExtractedLength = ExtractedLength;
	return(q);

}/*ExtractTokensBetweenPushPop*/


/*
  Extract attribute name from input stream, get attribute, and insert it's
  value into the input stream.  Return updated pointer into input stream.
  Added to support new elements "use" and "class".
*/
static
char * InsertAttributeIntoInputStream (
  char * q,                   /* address of pointer into primitive string*/
  char ** pprimitive,         /* ptr to ptr to primitive string buffer */
  size_t * pprimitive_extent,
  char ** ptoken,             /* ptr to ptr to big enough buffer for extracted string */
  size_t * ptoken_max_length,
  Image *image,
  MagickPassFail * pStatus,
  MagickBool UndefAttrIsError
  )
{/*InsertAttributeIntoInputStream*/

  char AttributeName[MaxTextExtent];
  const ImageAttribute *attribute;
  size_t AttributeLength;
  size_t RemainingLength;
  size_t NeededLength;

  /* get attribute name, then get attribute value */
  if (MagickGetToken(q,&q,*ptoken,*ptoken_max_length) < 1)
    {
      *pStatus = MagickFail;
      return(q);
    }
  FormatString(AttributeName,"[%.1024s]",*ptoken);
  attribute=GetImageAttribute(image,AttributeName);
  if (attribute == (ImageAttribute *) NULL)
    {
      /* the client specifies whether or not an undefined attributes is an error */
      if  ( UndefAttrIsError )
        *pStatus = MagickFail;
      return(q);
    }

  /*
    Insert attribute->value into input stream by concatenating it with the remainder
    of the primitive, and updating the required state variables.
  */
  AttributeLength = attribute->length;
  RemainingLength = *pprimitive_extent - (size_t)(q - *pprimitive);
  NeededLength = AttributeLength + RemainingLength;

  if  ( NeededLength <= *pprimitive_extent )
    {/*combined strings fit in existing primitive buffer*/

      q -= AttributeLength;
      memcpy(q,attribute->value,AttributeLength);

    }/*combined strings fit in existing primitive buffer*/
  else
    {/*combined strings need bigger buffer*/

      char * primitiveNew;
      *pprimitive_extent = NeededLength;
      primitiveNew = MagickAllocateMemory(char *,NeededLength+1);
      if (primitiveNew == (char *) NULL)
        {
          *pStatus = MagickFail;
          return(q);
        }
      (void) memcpy(primitiveNew,attribute->value,AttributeLength);
      (void) memcpy(primitiveNew+AttributeLength,q,RemainingLength);
      primitiveNew[NeededLength] = '\0';

      MagickFreeMemory(*pprimitive);
      q = *pprimitive = primitiveNew;
      MagickFreeMemory(*ptoken);
      *ptoken = MagickAllocateMemory(char *,NeededLength+1);
      *ptoken_max_length = NeededLength;

    }/*combined strings need bigger buffer*/

  return(q);

}/*InsertAttributeIntoInputStream*/


static MagickBool
PrimitiveInfoRealloc  (
  PrimitiveInfoMgr * p_PIMgr,
  const size_t       Needed
  )
{/*PrimitiveInfoRealloc*/

  /* grow the PrimitiveInfo array if Needed elements are not available; return true if reallocated */

  PrimitiveInfo * primitive_info;
  size_t NeedAllocCount = p_PIMgr->StoreStartingAt + Needed + ((size_t)100);  /* +100 for headroom */
  if  ( NeedAllocCount <= *p_PIMgr->p_AllocCount )
    return(MagickFalse);

  /*need to realloc*/
  primitive_info = *p_PIMgr->pp_PrimitiveInfo;
  MagickReallocMemory(PrimitiveInfo *,primitive_info,MagickArraySize(NeedAllocCount,sizeof(PrimitiveInfo)));
  if  ( primitive_info == (PrimitiveInfo *) NULL )
      ThrowException3(p_PIMgr->p_Exception,ResourceLimitError,MemoryAllocationFailed,UnableToDrawOnImage);
  *p_PIMgr->pp_PrimitiveInfo = primitive_info;
  *p_PIMgr->p_AllocCount = NeedAllocCount;

  return(MagickTrue);

}/*PrimitiveInfoRealloc*/


MagickExport MagickPassFail
DrawImage(Image *image,const DrawInfo *draw_info)
{
#define RenderImageText "[%s] Render..."

  AffineMatrix
    affine,
    current;

  char
    key[2*MaxTextExtent],
    keyword[MaxTextExtent],
    geometry[MaxTextExtent],
    name[MaxTextExtent],
    pattern[MaxTextExtent],
    *primitive,
    *q,
    *token;

  double
    angle,
    factor,
    points_length;  /* primitive_extent is now a size_t */

  DrawInfo
    **graphic_context;

  long
    j,
    k,
    n;

  PointInfo
    point;

  PixelPacket
    start_color;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register char
    *p;

  register long
    i,
    x;

  SegmentInfo
    bounds;

  size_t
    length,
    token_max_length,
    primitive_extent;

  MagickPassFail
    status;

  size_t
    number_points;

  MagickBool
    FillOpacityPending,
    StrokeOpacityPending;

  double
    FillOpacitySaved,
    StrokeOpacitySaved;

  MagickBool
    TextRotationPerformed;  /* see comments below where TextRotationPerformed=MagickFalse */

  /*
    Use defsPushCount to track when we enter/leave <defs> ... </defs> so we
    know not to render any graphical elements defined within (per the SVG spec).
    This is accomplished by turning off the "render" flag in the graphics
    context if defsPushCount > 0.
  */
  int
    defsPushCount;

  /* These variables are used to track the current text position */
  double
    xTextCurrent,
    yTextCurrent;
  MagickBool
    UseCurrentTextPosition;	/* true=>use (possibly modified) current text position */

  PrimitiveInfoMgr
    PIMgr;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(draw_info->primitive != (char *) NULL);
  /*
    Check for DrawImage recursion
  */
  if (DrawImageRecurseIn(image) == MagickFail)
    return MagickFail;

  if (*draw_info->primitive == '\0')
    return(MagickFail);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"begin draw-image");
  if (*draw_info->primitive != '@')
    primitive=AllocateString(draw_info->primitive);
  else
    primitive=(char *)
      FileToBlob(draw_info->primitive+1,&length,&image->exception);
  if (primitive == (char *) NULL)
    return(MagickFail);
  primitive_extent=strlen(primitive);
  (void) SetImageAttribute(image,"[MVG]",primitive);
  if (getenv("MAGICK_SKIP_RENDERING") != NULL)
    {
      MagickFreeMemory(primitive);
      return MagickPass;
    }
  n=0;
  /*
    Allocate primitive info memory.
  */
  graphic_context=MagickAllocateMemory(DrawInfo **,sizeof(DrawInfo *));
  if (graphic_context == (DrawInfo **) NULL)
    {
      MagickFreeMemory(primitive);
      ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToDrawOnImage)
    }
  number_points=6553;
  primitive_info=
    MagickAllocateArray(PrimitiveInfo *,number_points,sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    {
      MagickFreeMemory(primitive);
      for ( ; n >= 0; n--)
        DestroyDrawInfo(graphic_context[n]);
      MagickFreeMemory(graphic_context);
      ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToDrawOnImage)
    }

  /* set up to manage growing the PrimitiveInfo array */
  PIMgr.pp_PrimitiveInfo = &primitive_info;
  PIMgr.p_AllocCount = &number_points;
  PIMgr.StoreStartingAt = 0;
  PIMgr.p_Exception = &image->exception;

  graphic_context[n]=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  /* next two lines: don't need copy of primitive, just a buffer of the same size */
  token=MagickAllocateMemory(char *,primitive_extent+1);
  token_max_length=primitive_extent;
  (void) QueryColorDatabase("black",&start_color,&image->exception);
  (void) SetImageType(image,TrueColorType);
  status=MagickPass;
  defsPushCount = 0;  /* not inside of <defs> ... </defs> */
  xTextCurrent = yTextCurrent = 0.0;  /* initialize current text position */
  /*
    The purpose of these next four variables is to attempt to handle cases like:

    <rect stroke-opacity="0.5" stroke="green" ... />

    I.e., when the stroke-opacity is set before the stroke color is specified (ditto
    for fill-opacity and fill color).  This creates a problem when the current stroke
    color is "none" (the typical inherited default).  A value of "none" causes the
    stroke color to be set to transparent black, wiping out any current stroke-opacity
    information.  Furthermore, as long as the stroke color is "none", any attempt to
    set the stroke-opacity will be ignored.  I have not tested this, but I believe the
    same condition can be arrived at by setting stroke="black" and stroke-opacity="0".

    The real solution to this problem is to store the stroke="none" state without
    affecting the current stroke-opacity value.  This would require additional data
    structure members and code modifications, so for now we handle this situation
    by remembering when there is a pending stroke-opacity request (same for
    fill-opacity).  When the stroke (fill) color is eventually set, the pending
    opacity value is also set.

    The "pending" flags are reset to false whenever a graphic-context is pushed or
    popped, so this fix may only solve a limited set of cases.  However, these cases
    just happen to be the most common ones.
  */
  FillOpacityPending = MagickFalse;
  StrokeOpacityPending = MagickFalse;
  FillOpacitySaved = 0.0;
  StrokeOpacitySaved = 0.0;

  /*
    When DrawImage() was modified to provide management of the current text position to
    the client, text rotation also had to be modified.  Previously, the client would
    perform text rotation (i.e., next character is to be rotated) by supplying the following
    sequence:

      translate x y (where x, y indicate the current text position)
      rotate angle (where angle is the rotation angle)

    Later, when the actual 'text x y string' is supplied by the client, x and y must both
    be zero since the positioning has already been taken care of by the translate/rotate
    sequence.  The next variable indicates that rotation is being applied so that we can
    use (0,0) instead of the actual current text position.
  */
  TextRotationPerformed = MagickFalse;

  for (q=primitive; *q != '\0'; )
  {
    UseCurrentTextPosition = False;
    /*
      Interpret graphic primitive.
    */
    if (MagickGetToken(q,&q,keyword,MaxTextExtent) < 1)
      break;
    if (*keyword == '\0')
      break;
    if (*keyword == '#')
      {
        /*
          Comment.
        */
        while ((*q != '\n') && (*q != '\0'))
          q++;
        continue;
      }
    p=q-strlen(keyword);
    primitive_type=UndefinedPrimitive;
    current=graphic_context[n]->affine;
    IdentityAffine(&affine);
    switch (*keyword)
    {
      case ';':
        break;
      case 'a':
      case 'A':
      {
        if (LocaleCompare("affine",keyword) == 0)
          {
            MagickBool affine_args_good = MagickFalse;
            do
              {
                if (MagickGetToken(q,&q,token,token_max_length) < 1)
                  break;
                if (MagickAtoFChk(token,&affine.sx) != MagickPass)
                  break;
                if (MagickGetToken(q,&q,token,token_max_length) < 1)
                  break;
                if (*token == ',')
                  if (MagickGetToken(q,&q,token,token_max_length) < 1)
                    break;
                if (MagickAtoFChk(token,&affine.rx) != MagickPass)
                  break;
                if (MagickGetToken(q,&q,token,token_max_length) < 1)
                  break;
                if (*token == ',')
                  if (MagickGetToken(q,&q,token,token_max_length) < 1)
                    break;
                if (MagickAtoFChk(token,&affine.ry) != MagickPass)
                  break;
                if (MagickGetToken(q,&q,token,token_max_length) < 1)
                  break;
                if (*token == ',')
                  if (MagickGetToken(q,&q,token,token_max_length) < 1)
                    break;
                if (MagickAtoFChk(token,&affine.sy) != MagickPass)
                  break;
                if (MagickGetToken(q,&q,token,token_max_length) < 1)
                  break;
                if (*token == ',')
                  if (MagickGetToken(q,&q,token,token_max_length) < 1)
                    break;
                if (MagickAtoFChk(token,&affine.tx) != MagickPass)
                  break;
                if (MagickGetToken(q,&q,token,token_max_length) < 1)
                  break;
                if (*token == ',')
                  if (MagickGetToken(q,&q,token,token_max_length) < 1)
                    break;
                if (MagickAtoFChk(token,&affine.ty) != MagickPass)
                  break;
                affine_args_good=MagickTrue;
              } while (0);
            if (affine_args_good != MagickTrue)
              {
                status=MagickFail;
              }
            break;
          }
        if (LocaleCompare("arc",keyword) == 0)
          {
            primitive_type=ArcPrimitive;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'b':
      case 'B':
      {
        if (LocaleCompare("bezier",keyword) == 0)
          {
            primitive_type=BezierPrimitive;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'c':
      case 'C':
      {
        if (LocaleCompare("class",keyword) == 0)
          {/*class*/
            q = InsertAttributeIntoInputStream(q,&primitive,&primitive_extent,&token,&token_max_length,image,
              &status,MagickFalse/*UndefAttrIsError*/);
            break;
          }/*class*/
        if (LocaleCompare("clip-path",keyword) == 0)
          {
            /*
              Create clip mask.
            */
            if (MagickGetToken(q,&q,token,token_max_length) < 1)
              {
                status=MagickFail;
                break;
              }
            (void) CloneString(&graphic_context[n]->extra->clip_path,token);
            (void) DrawClipPath(image,graphic_context[n],
              graphic_context[n]->extra->clip_path);
            break;
          }
        if (LocaleCompare("clip-rule",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare("evenodd",token) == 0)
              {
                graphic_context[n]->fill_rule=EvenOddRule;
                break;
              }
            if (LocaleCompare("nonzero",token) == 0)
              {
                graphic_context[n]->fill_rule=NonZeroRule;
                break;
              }
            status=MagickFail;
            break;
          }
        if (LocaleCompare("clip-units",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare("userSpace",token) == 0)
              {
                graphic_context[n]->clip_units=UserSpace;
                break;
              }
            if (LocaleCompare("userSpaceOnUse",token) == 0)
              {
                graphic_context[n]->clip_units=UserSpaceOnUse;
                break;
              }
            if (LocaleCompare("objectBoundingBox",token) == 0)
              {
                graphic_context[n]->clip_units=ObjectBoundingBox;
                IdentityAffine(&current);
                affine.sx=draw_info->bounds.x2;
                affine.sy=draw_info->bounds.y2;
                affine.tx=draw_info->bounds.x1;
                affine.ty=draw_info->bounds.y1;
                break;
              }
            status=MagickFail;
            break;
          }
        if (LocaleCompare("circle",keyword) == 0)
          {
            primitive_type=CirclePrimitive;
            break;
          }
        if (LocaleCompare("color",keyword) == 0)
          {
            primitive_type=ColorPrimitive;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'd':
      case 'D':
      {
        if (LocaleCompare("decorate",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare("underline",token) == 0)
              {
                graphic_context[n]->decorate=UnderlineDecoration;
                break;
              }
            if (LocaleCompare("overline",token) == 0)
              {
                graphic_context[n]->decorate=OverlineDecoration;
                break;
              }
            if (LocaleCompare("line-through",token) == 0)
              {
                graphic_context[n]->decorate=LineThroughDecoration;
                break;
              }
            status=MagickFail;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'e':
      case 'E':
      {
        if (LocaleCompare("ellipse",keyword) == 0)
          {
            primitive_type=EllipsePrimitive;
            break;
          }
        if (LocaleCompare("encoding",keyword) == 0)
          {
            if (MagickGetToken(q,&q,token,token_max_length) < 1)
              {
                status=MagickFail;
                break;
              }
            (void) CloneString(&graphic_context[n]->encoding,token);
            break;
          }
        status=MagickFail;
        break;
      }
      case 'f':
      case 'F':
      {
        if (LocaleCompare("fill",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if  ( IsDrawInfoSVGCompliantClippingPath(graphic_context[n]) )
              break;	/* if drawing clip path, ignore changes to fill color */
            FormatString(pattern,"[%.1024s]",token);
            if (GetImageAttribute(image,pattern) != (ImageAttribute *) NULL)
              (void) DrawPatternPath(image,draw_info,token,
                &graphic_context[n]->fill_pattern);
            else
              {/*fill color, not pattern*/

                /* when setting new fill color, try to preserve fill-opacity */
                Quantum FillOpacityOld = graphic_context[n]->fill.opacity;
                (void) QueryColorDatabase(token,&graphic_context[n]->fill,&image->exception);

                if (graphic_context[n]->fill.opacity != TransparentOpacity)
                  {/*new fill color != 'none'*/

                    if  ( FillOpacityOld != TransparentOpacity )
                      graphic_context[n]->fill.opacity = FillOpacityOld;  /* already has group opacity included */
                    else  /* combine fill color's opacity with group opacity per SVG spec */
                      {
                        Quantum FillOpacity = FillOpacityPending ?
                          MaxRGB - (Quantum) (MaxRGBDouble * FillOpacitySaved + 0.5) : graphic_context[n]->fill.opacity;
                        graphic_context[n]->fill.opacity = MaxRGB - (Quantum)((double)(MaxRGB - FillOpacity) * (double)(MaxRGB - graphic_context[n]->opacity) / MaxRGBDouble + 0.5);
                      }
                    FillOpacityPending = MagickFalse;

                  }/*new fill color != 'none'*/

              }/*fill color, not pattern*/
            break;
          }
        if (LocaleCompare("fill-rule",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare("evenodd",token) == 0)
              {
                graphic_context[n]->fill_rule=EvenOddRule;
                break;
              }
            if (LocaleCompare("nonzero",token) == 0)
              {
                graphic_context[n]->fill_rule=NonZeroRule;
                break;
              }
            status=MagickFail;
            break;
          }
        if (LocaleCompare("fill-opacity",keyword) == 0)
          {
            double opacity;
            MagickGetToken(q,&q,token,token_max_length);
            if  ( IsDrawInfoSVGCompliantClippingPath(graphic_context[n]) )
              break;	/* if drawing clip path, ignore changes to fill opacity */
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            if ((MagickAtoFChk(token,&opacity) != MagickPass) ||
                (opacity < 0.0))
              {
                status=MagickFail;
                break;
              }
            opacity *= factor;
            if  ( opacity <= 0.0 )   /* per SVG spec */
              opacity = 0.0;
            else if  ( opacity > 1.0 )
              opacity = 1.0;
            FillOpacitySaved = opacity;
            if (graphic_context[n]->fill.opacity != TransparentOpacity)
              {
                /* combine new fill-opacity with group opacity per SVG spec */
                double opacityGroup = MaxRGB - graphic_context[n]->opacity; /* MaxRGB==opaque */
                graphic_context[n]->fill.opacity = MaxRGB - (Quantum)(opacity * opacityGroup + 0.5);
                FillOpacityPending = MagickFalse;
              }
            else
              FillOpacityPending = MagickTrue;
            break;
          }
        if (LocaleCompare("font",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            (void) CloneString(&graphic_context[n]->font,token);
            if (LocaleCompare("none",token) == 0)
              MagickFreeMemory(graphic_context[n]->font);
            break;
          }
        if (LocaleCompare("font-family",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            (void) CloneString(&graphic_context[n]->family,token);
            break;
          }
        if (LocaleCompare("font-size",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoFChk(token,&graphic_context[n]->pointsize)
                != MagickPass)
              {
                status=MagickFail;
              }
            break;
          }
        if (LocaleCompare("font-stretch",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare(token,"all") == 0)
              graphic_context[n]->stretch=AnyStretch;
            if (LocaleCompare(token,"condensed") == 0)
              graphic_context[n]->stretch=CondensedStretch;
            if (LocaleCompare(token,"expanded") == 0)
              graphic_context[n]->stretch=ExpandedStretch;
            if (LocaleCompare(token,"extra-condensed") == 0)
              graphic_context[n]->stretch=ExtraCondensedStretch;
            if (LocaleCompare(token,"extra-expanded") == 0)
              graphic_context[n]->stretch=ExtraExpandedStretch;
            if (LocaleCompare(token,"normal") == 0)
              graphic_context[n]->stretch=NormalStretch;
            if (LocaleCompare(token,"semi-condensed") == 0)
              graphic_context[n]->stretch=SemiCondensedStretch;
            if (LocaleCompare(token,"semi-expanded") == 0)
              graphic_context[n]->stretch=SemiExpandedStretch;
            if (LocaleCompare(token,"ultra-condensed") == 0)
              graphic_context[n]->stretch=UltraCondensedStretch;
            if (LocaleCompare(token,"ultra-expanded") == 0)
              graphic_context[n]->stretch=UltraExpandedStretch;
            break;
          }
        if (LocaleCompare("font-style",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare(token,"all") == 0)
              graphic_context[n]->style=AnyStyle;
            if (LocaleCompare(token,"italic") == 0)
              graphic_context[n]->style=ItalicStyle;
            if (LocaleCompare(token,"normal") == 0)
              graphic_context[n]->style=NormalStyle;
            if (LocaleCompare(token,"oblique") == 0)
              graphic_context[n]->style=ObliqueStyle;
            break;
          }
        if (LocaleCompare("font-weight",keyword) == 0)
          {
            if (MagickGetToken(q,&q,token,token_max_length) < 1)
              {
                status=MagickFail;
                break;
              }
            if (LocaleCompare(token,"all") == 0)
              graphic_context[n]->weight=0;
            else if (LocaleCompare(token,"bold") == 0)
              graphic_context[n]->weight=700;
            else if (LocaleCompare(token,"bolder") == 0)
              {
                if (graphic_context[n]->weight <= 800)
                  graphic_context[n]->weight+=100;
              }
            else if (LocaleCompare(token,"lighter") == 0)
              {
                if (graphic_context[n]->weight >= 100)
                  graphic_context[n]->weight-=100;
              }
            else if (LocaleCompare(token,"normal") == 0)
              graphic_context[n]->weight=400;
            else if (MagickAtoULChk(token,&graphic_context[n]->weight) != MagickPass)
              status=MagickFail;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'g':
      case 'G':
      {
        if (LocaleCompare("gradient-units",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            break;
          }
        if (LocaleCompare("gravity",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            graphic_context[n]->gravity=StringToGravityType(token);
            if (ForgetGravity != graphic_context[n]->gravity)
              break;
            status=MagickFail;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'i':
      case 'I':
      {
        if (LocaleCompare("image",keyword) == 0)
          {
            primitive_type=ImagePrimitive;
            MagickGetToken(q,&q,token,token_max_length);
            graphic_context[n]->compose=StringToCompositeOperator(token);
            if (UndefinedCompositeOp == graphic_context[n]->compose)
              status=MagickFail;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'l':
      case 'L':
      {
        if (LocaleCompare("line",keyword) == 0)
          {
            primitive_type=LinePrimitive;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'm':
      case 'M':
      {
          if (LocaleCompare("mask",keyword) == 0)   /* added mask */
            {
              /*
                Create mask.
              */
              if (MagickGetToken(q,&q,token,token_max_length) < 1)
                {
                  status=MagickFail;
                  break;
                }
              (void) CloneString(&graphic_context[n]->extra->composite_path,token);
              (void) DrawCompositeMask(image,graphic_context[n],graphic_context[n]->extra->composite_path);
              break;
            }
        if (LocaleCompare("matte",keyword) == 0)
          {
            primitive_type=MattePrimitive;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'o':
      case 'O':
      {
        if (LocaleCompare("offset",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            break;
          }
        if (LocaleCompare("opacity",keyword) == 0)
          {
            double opacity,opacityGroupOld,opacityGroupNew;
            MagickGetToken(q,&q,token,token_max_length);
            if  ( IsDrawInfoSVGCompliantClippingPath(graphic_context[n]) )
              break;	/* if drawing clip path, ignore changes to group/global opacity */
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            if (MagickAtoFChk(token,&opacity) != MagickPass)
              {
                status=MagickFail;
                break;
              }
            opacity *= factor;
            if  ( opacity <= 0.0 )  /* per SVG spec */
              opacity = 0.0;
            else if ( opacity > 1.0 )
              opacity = 1.0;
            opacityGroupOld = MaxRGB - graphic_context[n]->opacity; /* MaxRGB==opaque */
            opacityGroupNew = opacityGroupOld * opacity; /* MaxRGB==opaque */
            graphic_context[n]->opacity = MaxRGB - (Quantum)(opacityGroupNew + /*round*/0.5);
            if (graphic_context[n]->fill.opacity != TransparentOpacity)
            {/*fill color != 'none'*/

              if  ( opacityGroupOld == 0.0 )  /* can't back out old group opacity */
                graphic_context[n]->fill.opacity = graphic_context[n]->opacity; /* reasonable alternative */
              else
              {
                /* back out old group opacity value, include new group opacity value */
                double opacityFill = MaxRGB - graphic_context[n]->fill.opacity; /* MaxRGB==opaque */
                opacityFill = opacityFill * (opacityGroupNew / opacityGroupOld);
                graphic_context[n]->fill.opacity =
                  (opacityFill < MaxRGBDouble) ? MaxRGB - (Quantum)(opacityFill + /*round*/0.5) : 0;
              }

            }/*fill color != 'none'*/

            if (graphic_context[n]->stroke.opacity != TransparentOpacity)
            {/*stroke color != 'none'*/

              if  ( opacityGroupOld == 0.0 ) /* can't back out old group opacity */
                graphic_context[n]->stroke.opacity = graphic_context[n]->opacity; /* reasonable alternative */
              else
              {
                /* back out old group opacity value, include new group opacity value */
                double opacityStroke = MaxRGB - graphic_context[n]->stroke.opacity; /* MaxRGB==opaque */
                opacityStroke = opacityStroke * (opacityGroupNew / opacityGroupOld);
                graphic_context[n]->stroke.opacity =
                  (opacityStroke < MaxRGBDouble) ? MaxRGB - (Quantum)(opacityStroke + /*round*/0.5) : 0;
              }

            }/*stroke color != 'none'*/
            break;
          }
        status=MagickFail;
        break;
      }
      case 'p':
      case 'P':
      {
        if (LocaleCompare("path",keyword) == 0)
          {
            primitive_type=PathPrimitive;
            break;
          }
        if (LocaleCompare("point",keyword) == 0)
          {
            primitive_type=PointPrimitive;
            break;
          }
        if (LocaleCompare("polyline",keyword) == 0)
          {
            primitive_type=PolylinePrimitive;
            break;
          }
        if (LocaleCompare("polygon",keyword) == 0)
          {
            primitive_type=PolygonPrimitive;
            break;
          }
        if (LocaleCompare("pop",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare("class",token) == 0)  /* added "pop class" to support "defs" */
              break;
            if (LocaleCompare("clip-path",token) == 0)
              break;
            if (LocaleCompare("defs",token) == 0)
              {
                /* do not render graphic elements if inside <defs> ... </defs> */
                defsPushCount--;
                graphic_context[n]->render = (defsPushCount > 0) ? 0 : 1;
                break;
              }
            if (LocaleCompare("gradient",token) == 0)
              break;
            if (LocaleCompare("graphic-context",token) == 0)
              {
                if (n <= 0)
                  {
                    ThrowException(&image->exception,DrawError,UnbalancedGraphicContextPushPop,token);
                    status=MagickFail;
                    break;
                  }
                if (graphic_context[n]->extra->clip_path != (char *) NULL)
                  if (LocaleCompare(graphic_context[n]->extra->clip_path,
                      graphic_context[n-1]->extra->clip_path) != 0)
                    (void) SetImageClipMask(image,(Image *) NULL);
                if (graphic_context[n]->extra->composite_path != (char *) NULL)
                  {
                    /* clean up composite mask if different from parent */
                    if (LocaleCompare(graphic_context[n]->extra->composite_path,
                        graphic_context[n-1]->extra->composite_path) != 0)
                      (void) SetImageCompositeMask(image,(Image *) NULL);
                  }
                DestroyDrawInfo(graphic_context[n]);
                n--;
                FillOpacityPending = StrokeOpacityPending = MagickFalse;
                break;
              }
            if (LocaleCompare("id",token) == 0)   /* added "pop id" (to support "defs") */
              break;
            if (LocaleCompare("mask",token) == 0)   /* added mask */
              break;
            if (LocaleCompare("pattern",token) == 0)
              break;
            status=MagickFail;
            break;
          }
        if (LocaleCompare("push",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare("class",token) == 0)  /* added "push class" to support "defs" */
              {
                q = ExtractTokensBetweenPushPop(q,token,token_max_length,"class",image,0);
                break;
              }
            if (LocaleCompare("clip-path",token) == 0)
              {
                /*
                  Code that extracted tokens between push/pop clip-path has been refactored
                  into new function ExtractTokensBetweenPushPop().
                */
                size_t ExtractedLength;
                q = ExtractTokensBetweenPushPop(q,token,token_max_length,"clip-path",image,&ExtractedLength);
                if  ( ExtractedLength == 0 )
                  status=MagickFail;
                break;
              }
            if (LocaleCompare("gradient",token) == 0)
              {
                char
                  key[2*MaxTextExtent],
                  name[MaxTextExtent],
                  type[MaxTextExtent];

                SegmentInfo
                  segment;

                magick_int64_t
                  gradient_width,
                  gradient_height;

                MagickGetToken(q,&q,token,token_max_length);
                (void) strlcpy(name,token,MaxTextExtent);
                MagickGetToken(q,&q,token,token_max_length);
                (void) strlcpy(type,token,MaxTextExtent);
                MagickGetToken(q,&q,token,token_max_length);
                if (MagickAtoFChk(token,&segment.x1) != MagickPass)
                  {
                    status=MagickFail;
                    break;
                  }
                MagickGetToken(q,&q,token,token_max_length);

                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                if (MagickAtoFChk(token,&segment.y1) != MagickPass)
                  {
                    status=MagickFail;
                    break;
                  }
                MagickGetToken(q,&q,token,token_max_length);

                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                if (MagickAtoFChk(token,&segment.x2) != MagickPass)
                  {
                    status=MagickFail;
                    break;
                  }
                MagickGetToken(q,&q,token,token_max_length);

                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                if (MagickAtoFChk(token,&segment.y2) != MagickPass)
                  {
                    status=MagickFail;
                    break;
                  }
                if (LocaleCompare(type,"radial") == 0)
                  {
                    MagickGetToken(q,&q,token,token_max_length);
                    if (*token == ',')
                      MagickGetToken(q,&q,token,token_max_length);
                  }
                for (p=q; *q != '\0'; )
                {
                  MagickGetToken(q,&q,token,token_max_length);
                  if (LocaleCompare(token,"pop") != 0)
                    continue;
                  MagickGetToken(q,(char **) NULL,token,token_max_length);
                  if (LocaleCompare(token,"gradient") != 0)
                    continue;
                  break;
                }
                if ((q == NULL) || (p == NULL) || (q-4 < p))
                  {
                    status=MagickFail;
                    break;
                  }
                (void) strncpy(token,p,q-p-4);
                token[q-p-4]='\0';
                bounds.x1=graphic_context[n]->affine.sx*segment.x1+
                  graphic_context[n]->affine.ry*segment.y1+
                  graphic_context[n]->affine.tx;
                bounds.y1=graphic_context[n]->affine.rx*segment.x1+
                  graphic_context[n]->affine.sy*segment.y1+
                  graphic_context[n]->affine.ty;
                bounds.x2=graphic_context[n]->affine.sx*segment.x2+
                  graphic_context[n]->affine.ry*segment.y2+
                  graphic_context[n]->affine.tx;
                bounds.y2=graphic_context[n]->affine.rx*segment.x2+
                  graphic_context[n]->affine.sy*segment.y2+
                  graphic_context[n]->affine.ty;
                /*
                  Validate gradient image size
                */
                gradient_width=(magick_int64_t) Max(AbsoluteValue(bounds.x2-bounds.x1+1),1);
                gradient_height=(magick_int64_t) Max(AbsoluteValue(bounds.y2-bounds.y1+1),1);
                {
                  char resource_str[MaxTextExtent];
                  const magick_int64_t width_resource_limit = GetMagickResourceLimit(WidthResource);
                  const magick_int64_t hight_resource_limit = GetMagickResourceLimit(HeightResource);
                  const magick_int64_t pixels_resource_limit = GetMagickResourceLimit(PixelsResource);
                  if ((width_resource_limit > 0) && (gradient_width > width_resource_limit))
                    {
                      FormatString(resource_str,"%" MAGICK_INT64_F "d", width_resource_limit);
                      ThrowException(&image->exception,ResourceLimitError,
                                     ImagePixelWidthLimitExceeded,resource_str);
                      status=MagickFail;
                      break;
                    }
                  if ((hight_resource_limit > 0) && (gradient_height > hight_resource_limit))
                    {
                      FormatString(resource_str,"%" MAGICK_INT64_F "d", hight_resource_limit);
                      ThrowException(&image->exception,ResourceLimitError,
                                     ImagePixelHeightLimitExceeded,resource_str);
                      status=MagickFail;
                      break;
                    }
                  if ((pixels_resource_limit > 0) && (gradient_width*gradient_height > pixels_resource_limit))
                    {
                      FormatString(resource_str,"%" MAGICK_INT64_F "d", pixels_resource_limit);
                      ThrowException(&image->exception,ResourceLimitError,
                                     ImagePixelLimitExceeded,resource_str);
                      status=MagickFail;
                      break;
                    }
                }
                /*
                  Apply an arbitrary limit to gradient size requests
                  since gradient images can take a lot of memory.
                  Some tiny SVGs request huge gradients.  This is here
                  to avoid denial of service.
                */
                if (gradient_width*gradient_height > 5000*5000 /*10000*10000*/)
                  {
                    char gradient_size_str[MaxTextExtent];
                    FormatString(gradient_size_str,"%" MAGICK_INT64_F "dx%" MAGICK_INT64_F "d",
                                 gradient_width,gradient_height);
                    ThrowException(&image->exception,DrawError,
                                   UnreasonableGradientSize,gradient_size_str);
                    status=MagickFail;
                    break;
                  }
                FormatString(key,"[%.1024s]",name);
                (void) SetImageAttribute(image,key,token);
                FormatString(key,"[%.1024s-geometry]",name);
                FormatString(geometry,"%gx%g%+g%+g",
                  Max(AbsoluteValue(bounds.x2-bounds.x1+1),1),
                  Max(AbsoluteValue(bounds.y2-bounds.y1+1),1),
                  bounds.x1,bounds.y1);
                (void) SetImageAttribute(image,key,geometry);
                MagickGetToken(q,&q,token,token_max_length);
                break;
              }
            if (LocaleCompare("id",token) == 0)   /* added "push id" (to support "defs") */
              {
                if  ( defsPushCount > 0 )
                  q = ExtractTokensBetweenPushPop(q,token,token_max_length,"id",image,0);
                else	/* extract <identifier> from "push id <identifier>" */
                  MagickGetToken(q,&q,token,token_max_length);
                break;
              }
            if (LocaleCompare("mask",token) == 0)   /* added mask */
              {
                size_t ExtractedLength;
                q = ExtractTokensBetweenPushPop(q,token,token_max_length,"mask",image,&ExtractedLength);
                if  ( ExtractedLength == 0 )
                  status=MagickFail;
                break;
              }
            if (LocaleCompare("pattern",token) == 0)
              {
                double
                  ordinate;
                RectangleInfo
                  bounds;

                MagickGetToken(q,&q,token,token_max_length);
                (void) strlcpy(name,token,MaxTextExtent);
                MagickGetToken(q,&q,token,token_max_length);
                if (MagickAtoFChk(token,&ordinate) == MagickFail)
                  {
                    status=MagickFail;
                    break;
                  }
                bounds.x=(long) ceil(ordinate-0.5);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                if (MagickAtoFChk(token,&ordinate) == MagickFail)
                  {
                    status=MagickFail;
                    break;
                  }
                bounds.y=(long) ceil(ordinate-0.5);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                if ((MagickAtoFChk(token,&ordinate) == MagickFail) ||
                    (ordinate < 0.0))
                  {
                    status=MagickFail;
                    break;
                  }
                bounds.width=(unsigned long) floor(ordinate+0.5);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                if ((MagickAtoFChk(token,&ordinate) == MagickFail) ||
                    (ordinate < 0.0))
                  {
                    status=MagickFail;
                    break;
                  }
                bounds.height=(unsigned long) floor(ordinate+0.5);
                for (p=q; *q != '\0'; )
                {
                  MagickGetToken(q,&q,token,token_max_length);
                  if (LocaleCompare(token,"pop") != 0)
                    continue;
                  MagickGetToken(q,(char **) NULL,token,token_max_length);
                  if (LocaleCompare(token,"pattern") != 0)
                    continue;
                  break;
                }
                if ((q == NULL) || (p == NULL) || (q-4 < p))
                  {
                    status=MagickFail;
                    break;
                  }
                (void) strncpy(token,p,q-p-4);
                token[q-p-4]='\0';
                FormatString(key,"[%.1024s]",name);
                (void) SetImageAttribute(image,key,token);
                FormatString(key,"[%.1024s-geometry]",name);
                FormatString(geometry,"%lux%lu%+ld%+ld",bounds.width,
                  bounds.height,bounds.x,bounds.y);
                (void) SetImageAttribute(image,key,geometry);
                MagickGetToken(q,&q,token,token_max_length);
                break;
              }
            if (LocaleCompare("graphic-context",token) == 0)
              {
                n++;
                MagickReallocMemory(DrawInfo **,graphic_context,
                                    MagickArraySize((n+1),sizeof(DrawInfo *)));
                if (graphic_context == (DrawInfo **) NULL)
                  {
                    ThrowException3(&image->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToDrawOnImage);
                    break;
                  }
                graphic_context[n]=
                  CloneDrawInfo((ImageInfo *) NULL,graphic_context[n-1]);
                FillOpacityPending = StrokeOpacityPending = MagickFalse;
                break;
              }
            if (LocaleCompare("defs",token) == 0)
              {
                /* do not render graphic elements if inside <defs> ... </defs> */
                defsPushCount++;
                graphic_context[n]->render = (defsPushCount > 0) ? 0 : 1;
                break;
              }
            status=MagickFail;
            break;
          }
        status=MagickFail;
        break;
      }
      case 'r':
      case 'R':
      {
        if (LocaleCompare("rectangle",keyword) == 0)
          {
            primitive_type=RectanglePrimitive;
            break;
          }
        if (LocaleCompare("rotate",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoFChk(token,&angle) != MagickPass)
              {
                status=MagickFail;
                break;
              }
            affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
            affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
            affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
            affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("roundRectangle",keyword) == 0)
          {
            primitive_type=RoundRectanglePrimitive;
            break;
          }
        status=MagickFail;
        break;
      }
      case 's':
      case 'S':
      {
        if (LocaleCompare("scale",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoFChk(token,&affine.sx) != MagickPass)
              {
                status=MagickFail;
                break;
              }
            MagickGetToken(q,&q,token,token_max_length);
            if (*token == ',')
              MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoFChk(token,&affine.sy) != MagickPass)
              status=MagickFail;
            break;
          }
        if (LocaleCompare("skewX",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoFChk(token,&angle) != MagickPass)
              {
                status=MagickFail;
                break;
              }
            affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("skewY",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoFChk(token,&angle) != MagickPass)
              {
                status=MagickFail;
                break;
              }
            affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("stop-color",keyword) == 0)
          {
            PixelPacket
              stop_color;

            MagickGetToken(q,&q,token,token_max_length);
            (void) QueryColorDatabase(token,&stop_color,&image->exception);
            (void) GradientImage(image,&start_color,&stop_color);
            start_color=stop_color;
            MagickGetToken(q,&q,token,token_max_length);
            break;
          }
        if (LocaleCompare("stroke",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if  ( IsDrawInfoSVGCompliantClippingPath(graphic_context[n]) )
              break;	/* if drawing clip path, ignore changes to stroke color */
            FormatString(pattern,"[%.1024s]",token);
            if (GetImageAttribute(image,pattern) != (ImageAttribute *) NULL)
              (void) DrawPatternPath(image,draw_info,token,
                &graphic_context[n]->stroke_pattern);
            else
              {/*stroke color, not pattern*/

                /* when setting new stroke color, try to preserve stroke-opacity */
                Quantum StrokeOpacityOld = graphic_context[n]->stroke.opacity;
                (void) QueryColorDatabase(token,&graphic_context[n]->stroke,&image->exception);

                if (graphic_context[n]->stroke.opacity != TransparentOpacity)
                  {/*stroke color != 'none'*/

                    if  ( StrokeOpacityOld != TransparentOpacity )
                      graphic_context[n]->stroke.opacity = StrokeOpacityOld;  /* already has group opacity included */
                    else  /* combine stroke color's opacity with group opacity per SVG spec */
                    {
                      Quantum StrokeOpacity = StrokeOpacityPending ? MaxRGB - (Quantum) (MaxRGBDouble * StrokeOpacitySaved + 0.5) : graphic_context[n]->stroke.opacity;
                      graphic_context[n]->stroke.opacity = MaxRGB - (Quantum)((double)(MaxRGB - StrokeOpacity) * (double)(MaxRGB - graphic_context[n]->opacity) / MaxRGBDouble + 0.5);
                    }
                    StrokeOpacityPending = MagickFalse;

                  }/*stroke color != 'none'*/

              }/*stroke color, not pattern*/
            break;
          }
        if (LocaleCompare("stroke-antialias",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoUIChk(token,&graphic_context[n]->stroke_antialias)
                != MagickPass)
              {
                status=MagickFail;
              }
            break;
          }
        if (LocaleCompare("stroke-dasharray",keyword) == 0)
          {
            MagickFreeMemory(graphic_context[n]->dash_pattern);
            if (IsPoint(q))
              {
                char
                  *p;

                p=q;
                MagickGetToken(p,&p,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(p,&p,token,token_max_length);
                for (x=0; IsPoint(token); x++)
                {
                  MagickGetToken(p,&p,token,token_max_length);
                  if (*token == ',')
                    MagickGetToken(p,&p,token,token_max_length);
                }
                graphic_context[n]->dash_pattern=
                  MagickAllocateArray(double *,(2*x+2),sizeof(double));
                if (graphic_context[n]->dash_pattern == (double *) NULL)
                  {
                    status=MagickFail;
                    ThrowException3(&image->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToDrawOnImage);
                    break;
                  }
                for (j=0; j < x; j++)
                {
                  MagickGetToken(q,&q,token,token_max_length);
                  if (*token == ',')
                    MagickGetToken(q,&q,token,token_max_length);
                  if ((MagickAtoFChk(token,&graphic_context[n]->dash_pattern[j])
                       == MagickFail) ||
                      (graphic_context[n]->dash_pattern[j] < 0.0))
                    status=MagickFail;
                }
                if (status == MagickFail)
                  {
                    MagickFreeMemory(graphic_context[n]->dash_pattern);
                    break;
                  }
                if (x & 0x01)
                  for ( ; j < (2*x); j++)
                    graphic_context[n]->dash_pattern[j]=
                      graphic_context[n]->dash_pattern[j-x];
                graphic_context[n]->dash_pattern[j]=0.0;
                break;
              }
            MagickGetToken(q,&q,token,token_max_length);
            break;
          }
        if (LocaleCompare("stroke-dashoffset",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoFChk(token,&graphic_context[n]->dash_offset)
                == MagickFail)
              status=MagickFail;
            break;
          }
        if (LocaleCompare("stroke-linecap",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare("butt",token) == 0)
              {
                graphic_context[n]->linecap=ButtCap;
                break;
              }
            if (LocaleCompare("round",token) == 0)
              {
                graphic_context[n]->linecap=RoundCap;
                break;
              }
            if (LocaleCompare("square",token) == 0)
              {
                graphic_context[n]->linecap=SquareCap;
                break;
              }
            status=MagickFail;
            break;
          }
        if (LocaleCompare("stroke-linejoin",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare("bevel",token) == 0)
              {
                graphic_context[n]->linejoin=BevelJoin;
                break;
              }
            if (LocaleCompare("miter",token) == 0)
              {
                graphic_context[n]->linejoin=MiterJoin;
                break;
              }
            if (LocaleCompare("round",token) == 0)
              {
                graphic_context[n]->linejoin=RoundJoin;
                break;
              }
            status=MagickFail;
            break;
          }
        if (LocaleCompare("stroke-miterlimit",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if ((MagickAtoULChk(token,&graphic_context[n]->miterlimit)
                 != MagickPass) || (graphic_context[n]->miterlimit < 1))
              status=MagickFail;
            break;
          }
        if (LocaleCompare("stroke-opacity",keyword) == 0)
          {
            double opacity;
            MagickGetToken(q,&q,token,token_max_length);
            if  ( IsDrawInfoSVGCompliantClippingPath(graphic_context[n]) )
              break;	/* if drawing clip path, ignore changes to stroke opacity */
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            if ((MagickAtoFChk(token,&opacity) != MagickPass) ||
                opacity < 0.0)
              {
                status=MagickFail;
                break;
              }
            opacity *= factor;
            if  ( opacity <= 0.0 )  /* per SVG spec */
              opacity = 0.0;
            else if  ( opacity > 1.0 )
              opacity = 1.0;
            StrokeOpacitySaved = opacity;
            if (graphic_context[n]->stroke.opacity != TransparentOpacity)
              {
                /* combine new stroke-opacity with group opacity per SVG spec */
                double opacityGroup = MaxRGB - graphic_context[n]->opacity;
                graphic_context[n]->stroke.opacity = MaxRGB - (Quantum)(opacity * opacityGroup + 0.5);
                StrokeOpacityPending = MagickFalse;
              }
            else
              StrokeOpacityPending = MagickTrue;
            break;
          }
        if (LocaleCompare("stroke-width",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if  ( IsDrawInfoSVGCompliantClippingPath(graphic_context[n]) )
              break;	/* if drawing clip path, ignore changes to stroke width */
            if ((MagickAtoFChk(token,&graphic_context[n]->stroke_width)
                 == MagickFail) ||
                (graphic_context[n]->stroke_width < 0.0))
              status=MagickFail;
            break;
          }
        if (LocaleCompare("svg-compliant",keyword) == 0)
          {
            /* mark the DrawInfo as being drawn SVG compliant or not */
            unsigned int SVGCompliant;
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoUIChk(token,&SVGCompliant) != MagickPass)
              {
                status=MagickFail;
              }
            else
              SetDrawInfoSVGCompliant(graphic_context[n],SVGCompliant?MagickTrue:MagickFalse);
            break;
          }
        status=MagickFail;
        break;
      }
      case 't':
      case 'T':
      {
        if (LocaleCompare("text",keyword) == 0)
          {
            primitive_type=TextPrimitive;
            UseCurrentTextPosition = MagickFalse;   /* use client-supplied text locations */
            break;
          }
        if (LocaleCompare("textc",keyword) == 0)  /* draw text at current text position */
          {
            primitive_type=TextPrimitive;
            UseCurrentTextPosition = MagickTrue;	/* use internally tracked text location*/
            break;
          }
        if (LocaleCompare("textdx",keyword) == 0)  /* update current x position for text */
          {/*textdx*/
            double value;
            MagickGetToken(q,&q,token,token_max_length);
            (void) MagickAtoFChk(token,&value);
            /* value may be specified using "em" or "ex" units */
            if (LocaleNCompare(q,"em",2) == 0)
              {
                value *= graphic_context[n]->pointsize;
                MagickGetToken(q,&q,token,token_max_length);	/* skip over "em" */
              }
            else if (LocaleNCompare(q,"ex",2) == 0)
              {
                value *= 0.5 * graphic_context[n]->pointsize;
                MagickGetToken(q,&q,token,token_max_length);	/* skip over "ex" */
              }
            xTextCurrent += value;
            break;
          }/*textdx*/
        if (LocaleCompare("textdy",keyword) == 0)  /* update current y position for text */
          {/*textdy*/
            double value;
            MagickGetToken(q,&q,token,token_max_length);
            (void) MagickAtoFChk(token,&value);
            /* value may be specified using "em" or "ex" units */
            if (LocaleNCompare(q,"em",2) == 0)
              {
                value *= graphic_context[n]->pointsize;
                MagickGetToken(q,&q,token,token_max_length);	/* skip over "em" */
              }
            else if (LocaleNCompare(q,"ex",2) == 0)
              {
                value *= 0.5 * graphic_context[n]->pointsize;
                MagickGetToken(q,&q,token,token_max_length);	/* skip over "ex" */
              }
            yTextCurrent += value;
            break;
          }/*textdy*/
        /*
          When the current text position was managed in SVGStartElement() in svg.c, and a "rotate"
          was encountered (indicating that the text character was to be rotated), the code would
          emit to the MVG file:

            translate x y (where x, y indicate the current text position)
            rotate angle (where angle indicates the rotation angle)

          Now that the current text position is being managed by DrawImage() in render.c, the code
          in SVGStartElement() cannot issue the "translate" because it can't know the current text
          position.  To handle this, "textr" (text rotation) has been implemented here to perform
          the appropriate translation/rotation sequence.
        */
        if (LocaleCompare("textr",keyword) == 0)  /* text rotation */
          {/*textr*/
            TextRotationPerformed = MagickTrue;
            /* translate x y */
            affine.tx = xTextCurrent;
            affine.ty = yTextCurrent;
            /* rotation angle */
            MagickGetToken(q,&q,token,token_max_length);
            if (MagickAtoFChk(token,&angle) != MagickPass)
              {
                status=MagickFail;
                break;
              }
            affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
            affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
            affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
            affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
            break;
          }/*textr*/
        if (LocaleCompare("textx",keyword) == 0)  /* set current x position for text */
          {
            double value;
            MagickGetToken(q,&q,token,token_max_length);
            (void) MagickAtoFChk(token,&value);
            xTextCurrent = value;
            break;
          }
        if (LocaleCompare("texty",keyword) == 0)  /* set current y position for text */
          {
            double value;
            MagickGetToken(q,&q,token,token_max_length);
            (void) MagickAtoFChk(token,&value);
            yTextCurrent = value;
            break;
          }
        if (LocaleCompare("text-align",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare(token,"left") == 0)
              graphic_context[n]->align=LeftAlign;
            if (LocaleCompare(token,"center") == 0)
              graphic_context[n]->align=CenterAlign;
            if (LocaleCompare(token,"right") == 0)
              graphic_context[n]->align=RightAlign;
            break;
          }
        if (LocaleCompare("text-anchor",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            if (LocaleCompare(token,"start") == 0)
              graphic_context[n]->align=LeftAlign;
            if (LocaleCompare(token,"middle") == 0)
              graphic_context[n]->align=CenterAlign;
            if (LocaleCompare(token,"end") == 0)
              graphic_context[n]->align=RightAlign;
            break;
          }
        if (LocaleCompare("text-antialias",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            graphic_context[n]->text_antialias=MagickAtoI(token);
            break;
          }
        if (LocaleCompare("text-undercolor",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            (void) QueryColorDatabase(token,&graphic_context[n]->undercolor,
              &image->exception);
            break;
          }
        if (LocaleCompare("translate",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            (void) MagickAtoFChk(token,&affine.tx);
            MagickGetToken(q,&q,token,token_max_length);
            if (*token == ',')
              MagickGetToken(q,&q,token,token_max_length);
            (void) MagickAtoFChk(token,&affine.ty);
            break;
          }
        status=MagickFail;
        break;
      }
      case 'u':
      case 'U':
      {
        if (LocaleCompare("use",keyword) == 0)
          {
            q = InsertAttributeIntoInputStream(q,&primitive,&primitive_extent,&token,&token_max_length,image,
              &status,MagickTrue/*UndefAttrIsError*/);
            break;
          }
        status=MagickFail;
        break;
      }
      case 'v':
      case 'V':
      {
        if (LocaleCompare("viewbox",keyword) == 0)
          {
            MagickGetToken(q,&q,token,token_max_length);
            MagickGetToken(q,&q,token,token_max_length);
            if (*token == ',')
              MagickGetToken(q,&q,token,token_max_length);
            MagickGetToken(q,&q,token,token_max_length);
            if (*token == ',')
              MagickGetToken(q,&q,token,token_max_length);
            MagickGetToken(q,&q,token,token_max_length);
            if (*token == ',')
              MagickGetToken(q,&q,token,token_max_length);
            break;
          }
        status=MagickFail;
        break;
      }
      default:
      {
        status=MagickFail;
        break;
      }
    }
    if (status == MagickFail)
      break;
    if ((affine.sx != 1.0) || (affine.rx != 0.0) || (affine.ry != 0.0) ||
        (affine.sy != 1.0) || (affine.tx != 0.0) || (affine.ty != 0.0))
      {
        graphic_context[n]->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
        graphic_context[n]->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
        graphic_context[n]->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
        graphic_context[n]->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
        graphic_context[n]->affine.tx=
          current.sx*affine.tx+current.ry*affine.ty+current.tx;
        graphic_context[n]->affine.ty=
          current.rx*affine.tx+current.sy*affine.ty+current.ty;
      }
    if (primitive_type == UndefinedPrimitive)
      {
        (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  %.*s",
          (int) (q-p),p);
        continue;
      }
    /*
      Parse the primitive attributes.
    */
    PIMgr.StoreStartingAt = i = 0;
    j=0;
    primitive_info[0].point.x=0.0;
    primitive_info[0].point.y=0.0;
    primitive_info[0].coordinates=0;
    primitive_info[0].method=FloodfillMethod;
    PRIMINF_CLEAR_FLAGS(&primitive_info[0]);
    for (x=0; *q != '\0'; x++)
    {
      /*
        Define points.
      */
      if (!IsPoint(q))
        break;
      MagickGetToken(q,&q,token,token_max_length);
      (void) MagickAtoFChk(token,&point.x);
      MagickGetToken(q,&q,token,token_max_length);
      if (*token == ',')
        MagickGetToken(q,&q,token,token_max_length);
      (void) MagickAtoFChk(token,&point.y);
      MagickGetToken(q,(char **) NULL,token,token_max_length);
      if (*token == ',')
        MagickGetToken(q,&q,token,token_max_length);
      assert(i < (long) number_points);
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      PRIMINF_CLEAR_FLAGS(&primitive_info[i]);
      i++;
      PIMgr.StoreStartingAt = i;
      if (i < (long) number_points)
        continue;
      PrimitiveInfoRealloc(&PIMgr,number_points);  /* array is full; double the array size */
    }
    /*
      Special handling when using textc with character rotation; see comments
      above near "textr".
    */
    if  ( (primitive_type == TextPrimitive) && UseCurrentTextPosition && (i == 0) )
      {
        primitive_info[0].primitive=primitive_type;
        PRIMINF_CLEAR_FLAGS(&primitive_info[0]);
        if  ( TextRotationPerformed )
          {
            /* text positioning has already been performed by translate/rotate sequence */
            primitive_info[0].point.x=0;
            primitive_info[0].point.y=0;
            TextRotationPerformed = MagickFalse;
          }
        else
          {
            primitive_info[0].point.x=xTextCurrent;
            primitive_info[0].point.y=yTextCurrent;
          }
        primitive_info[0].coordinates=0;
        primitive_info[0].method=FloodfillMethod;
        i++;
        PIMgr.StoreStartingAt = i;
        x++;
      }
    assert(j < (long) number_points);
    primitive_info[j].primitive=primitive_type;
    primitive_info[j].coordinates=x;
    primitive_info[j].method=FloodfillMethod;
    PRIMINF_CLEAR_FLAGS(&primitive_info[j]);
    primitive_info[j].text=(char *) NULL;
    /*
      Circumscribe primitive within a circle.
    */
    bounds.x1=primitive_info[j].point.x;
    bounds.y1=primitive_info[j].point.y;
    bounds.x2=primitive_info[j].point.x;
    bounds.y2=primitive_info[j].point.y;
    for (k=1; k < (long) primitive_info[j].coordinates; k++)
    {
      assert(j+k < (long) number_points);
      point=primitive_info[j+k].point;
      if (point.x < bounds.x1)
        bounds.x1=point.x;
      if (point.y < bounds.y1)
        bounds.y1=point.y;
      if (point.x > bounds.x2)
        bounds.x2=point.x;
      if (point.y > bounds.y2)
        bounds.y2=point.y;
    }
    /*
      Estimate how many points will be required for the primitive.
    */
    points_length=primitive_info[j].coordinates;
    switch (primitive_type)
    {
      case RectanglePrimitive:
      {
        points_length*=5;
        break;
      }
      case RoundRectanglePrimitive:
      {
        /*
          Round rectangle is rectangle plus elipse
        */
        double
          alpha,
          beta,
          radius;

        alpha=bounds.x2-bounds.x1;
        beta=bounds.y2-bounds.y1;
        radius=hypot((double) alpha,(double) beta);
        points_length*=5;
        points_length+=2*((size_t) ceil((double) MagickPI*radius))+6*BezierQuantum+360;
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates > 107)
          (void) ThrowException(&image->exception,DrawError,
                                TooManyCoordinates,token);
        points_length=primitive_info[j].coordinates*BezierQuantum;
        break;
      }
      case PathPrimitive:
      {
        char
          *s,
          *t;

        MagickGetToken(q,&q,token,token_max_length);
        points_length=1;
        t=token;
        for (s=token; *s != '\0'; s=t)
        {
          double
            value;

          value=strtod(s,&t);
          (void) value;
          if (s == t)
            {
              t++;
              continue;
            }
          points_length++;
        }
        points_length=points_length*BezierQuantum;
        break;
      }
      case CirclePrimitive:
      case ArcPrimitive:
      case EllipsePrimitive:
      {
        double
          alpha,
          beta,
          radius;

        alpha=bounds.x2-bounds.x1;
        beta=bounds.y2-bounds.y1;
        radius=hypot(alpha,beta);
        points_length=2*(ceil(MagickPI*radius))+6*BezierQuantum+360;
        break;
      }
      default:
        break;
    }

    if (((size_t) points_length) < points_length)
      {
        /* points_length too big to be represented as a size_t */
        status=MagickFail;
        ThrowException(&image->exception,DrawError,
                       PrimitiveArithmeticOverflow,keyword);
      }

    if (status == MagickFail)
      break;

    if ((i+points_length) >= number_points)
      {
        double new_number_points = ceil(number_points+points_length+1);
        size_t new_number_points_size_t = (size_t) new_number_points;
        if (new_number_points_size_t != new_number_points)
          {
            /* new_number_points too big to be represented as a size_t */
            status=MagickFail;
            ThrowException3(&image->exception,ResourceLimitError,
                            MemoryAllocationFailed,UnableToDrawOnImage);
            break;
          }

        PIMgr.StoreStartingAt = i;  /* should already be this value; just bein' sure */
        PrimitiveInfoRealloc(&PIMgr,new_number_points_size_t-number_points);
      }

    assert(j < (long) number_points);

    /*
      The TraceXXX funtions that generate a dynamic number of points will automatically
      grow the primitive array as needed.  To make sure that the simpler ones (TracePoint()
      TraceLine(), TraceRectangle(), etc.) have enough space, we make sure there are at
      least 100 elements available.
    */
    PrimitiveInfoRealloc(&PIMgr,100);

    /*
      Trace points
    */
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=MagickFail;
            break;
          }
        TracePoint(primitive_info+j,primitive_info[j].point);
        PIMgr.StoreStartingAt=i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=MagickFail;
            break;
          }
        TraceLine(primitive_info+j,primitive_info[j].point,
                  primitive_info[j+1].point);
        PIMgr.StoreStartingAt=i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case RectanglePrimitive:
      {
        /*
          Rectangle requires 2 primitives.
        */
        if (primitive_info[j].coordinates != 2)
          {
            status=MagickFail;
            break;
          }
        /*
          Negative width is an error
        */
        if ((primitive_info[j+1].point.x - primitive_info[j].point.x) < 0.0)
          {
            status=MagickFail;
            break;
          }
        /*
          Negative height is an error
        */
        if ((primitive_info[j+1].point.y - primitive_info[j].point.y) < 0.0)
          {
            status=MagickFail;
            break;
          }
        TraceRectangle(primitive_info+j,
                       /*start*/primitive_info[j].point,
                       /*end*/primitive_info[j+1].point);
        PIMgr.StoreStartingAt=i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case RoundRectanglePrimitive:
      {
        /*
          Round rectangle requires 3 primitives.
        */
        if (primitive_info[j].coordinates != 3)
          {
            status=MagickFail;
            break;
          }
        /*
          Negative radius values are an error.
        */
        if ((primitive_info[j+2].point.x < 0.0) || (primitive_info[j+2].point.y < 0.0))
          {
            status=MagickFail;
            break;
          }
        /*
          Negative width is an error
        */
        if ((primitive_info[j+1].point.x - primitive_info[j].point.x) < 0.0)
          {
            status=MagickFail;
            break;
          }
        /*
          Negative height is an error
        */
        if ((primitive_info[j+1].point.y - primitive_info[j].point.y) < 0.0)
          {
            status=MagickFail;
            break;
          }
        PIMgr.StoreStartingAt=j;
        TraceRoundRectangle(&PIMgr,
                            /*start*/primitive_info[j].point,
                            /*end*/primitive_info[j+1].point,
                            /*arc*/primitive_info[j+2].point);
         PIMgr.StoreStartingAt=i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        PIMgr.StoreStartingAt=j;
        TraceArc(&PIMgr,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        PIMgr.StoreStartingAt=i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=MagickFail;
            break;
          }
        /*
          Negative radius values are an error.
        */
        if ((primitive_info[j+1].point.x < 0.0) ||
            (primitive_info[j+1].point.y < 0.0))
          {
            status=MagickFail;
            break;
          }
        PIMgr.StoreStartingAt=j;
        TraceEllipse(&PIMgr,
                     /*start*/primitive_info[j].point, /*centerX,centerY*/
                     /*stop*/primitive_info[j+1].point, /*radiusX,radiusY*/
                     /*degrees*/primitive_info[j+2].point); /*arcStart,arcEnd*/
        PIMgr.StoreStartingAt=i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case CirclePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=MagickFail;
            break;
          }
        PIMgr.StoreStartingAt=j;
        TraceCircle(&PIMgr,primitive_info[j].point,
          primitive_info[j+1].point);
        PIMgr.StoreStartingAt=i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case PolylinePrimitive:
      {
        /* the SVG spec does not prohibit polylines containing a single point */
        if (primitive_info[j].coordinates < 1)
          {
            status=MagickFail;
            break;
          }
        break;
      }
      case PolygonPrimitive:
      {
        /*
          Polygon requires at least three points.
        */
        if (primitive_info[j].coordinates < 3)
          {
            status=MagickFail;
            break;
          }
        primitive_info[i]=primitive_info[j];
        primitive_info[i].coordinates=0;
        primitive_info[j].coordinates++;
        PRIMINF_SET_IS_CLOSED_SUBPATH(&primitive_info[j],1);
        i++;
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=MagickFail;
            break;
          }
        PIMgr.StoreStartingAt=j;
        TraceBezier(&PIMgr,primitive_info[j].coordinates);
        PIMgr.StoreStartingAt=i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case PathPrimitive:
      {
        PIMgr.StoreStartingAt=j;
        PIMgr.StoreStartingAt=i=(long) (j+TracePath(image,&PIMgr,token));
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=MagickFail;
            break;
          }
        MagickGetToken(q,&q,token,token_max_length);
        if (LocaleCompare("point",token) == 0)
          primitive_info[j].method=PointMethod;
        if (LocaleCompare("replace",token) == 0)
          primitive_info[j].method=ReplaceMethod;
        if (LocaleCompare("floodfill",token) == 0)
          primitive_info[j].method=FloodfillMethod;
        if (LocaleCompare("filltoborder",token) == 0)
          primitive_info[j].method=FillToBorderMethod;
        if (LocaleCompare("reset",token) == 0)
          primitive_info[j].method=ResetMethod;
        break;
      }
      case TextPrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=MagickFail;
            break;
          }
        if (*token != ',')
          MagickGetToken(q,&q,token,token_max_length);
        primitive_info[j].text=AllocateString(token);
        {/*update current text position for next time*/
          /*
            Clone the DrawInfo, add a blank to the end of
            the text, get the metrics for the concatenated
            string, and use the width to update the text
            current x position.
          */
          DrawInfo * clone_info;
          TypeMetric  metrics;
          clone_info=CloneDrawInfo((ImageInfo *) NULL,graphic_context[n]);
          MagickFreeMemory(clone_info->density);	/* density values already converted, don't do again! */
          clone_info->render = 0;
          clone_info->text=AllocateString(token);
          (void) ConcatenateString(&clone_info->text," ");
          (void) GetTypeMetrics(image,clone_info,&metrics);
          xTextCurrent += metrics.width;
          DestroyDrawInfo(clone_info);
        }/*update current text position for next time*/
        break;
      }
      case ImagePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=MagickFail;
            break;
          }
        MagickGetToken(q,&q,token,token_max_length);
        primitive_info[j].text=AllocateString(token);
        break;
      }
    }

    if (primitive_info == (PrimitiveInfo *) NULL)
      break;
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  %.*s",(int) (q-p),p);
    if (status == MagickFail)
      break;
    assert(i < (long) number_points);
    primitive_info[i].primitive=UndefinedPrimitive;
    if (i == 0)
      continue;
    /*
      Transform points.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      assert((unsigned long) i < number_points);
      point=primitive_info[i].point;
      primitive_info[i].point.x=graphic_context[n]->affine.sx*point.x+
        graphic_context[n]->affine.ry*point.y+graphic_context[n]->affine.tx;
      primitive_info[i].point.y=graphic_context[n]->affine.rx*point.x+
        graphic_context[n]->affine.sy*point.y+graphic_context[n]->affine.ty;
      point=primitive_info[i].point;
      if (point.x < graphic_context[n]->bounds.x1)
        graphic_context[n]->bounds.x1=point.x;
      if (point.y < graphic_context[n]->bounds.y1)
        graphic_context[n]->bounds.y1=point.y;
      if (point.x > graphic_context[n]->bounds.x2)
        graphic_context[n]->bounds.x2=point.x;
      if (point.y > graphic_context[n]->bounds.y2)
        graphic_context[n]->bounds.y2=point.y;
      if (primitive_info[i].primitive == ImagePrimitive)
        break;
    }
    if (graphic_context[n]->render)
      {
        if ((n != 0) && (graphic_context[n]->extra->clip_path != (char *) NULL) &&
            (LocaleCompare(graphic_context[n]->extra->clip_path,
             graphic_context[n-1]->extra->clip_path) != 0))
          if (DrawClipPath(image,graphic_context[n],
                           graphic_context[n]->extra->clip_path) == MagickFail)
            status=MagickFail;
        if ((n != 0) && (graphic_context[n]->extra->composite_path != (char *) NULL) &&
            (LocaleCompare(graphic_context[n]->extra->composite_path,
            graphic_context[n-1]->extra->composite_path) != 0))
          {
            if (DrawCompositeMask(image,graphic_context[n],
                graphic_context[n]->extra->composite_path) == MagickFail)
              status=MagickFail;
          }
        if (DrawPrimitive(image,graphic_context[n],primitive_info)
            == MagickFail)
          status=MagickFail;
      }
    if (primitive_info->text != (char *) NULL)
      MagickFreeMemory(primitive_info->text);
    if (MagickMonitorFormatted(q-primitive,
                               (magick_uint64_t) primitive_extent,
                               &image->exception,
                               RenderImageText,image->filename) == MagickFail)
      status=MagickFail;
    if (status == MagickFail)
      break;
  }
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"end draw-image");
  /*
    Free resources.
  */
  DrawImageRecurseOut(image);
  MagickFreeMemory(token);
  MagickFreeMemory(primitive_info);
  MagickFreeMemory(primitive);
  for ( ; n >= 0; n--)
    DestroyDrawInfo(graphic_context[n]);
  MagickFreeMemory(graphic_context);
  if ((status == MagickFail) &&
      (image->exception.severity < ErrorException))
    ThrowBinaryException(DrawError,NonconformingDrawingPrimitiveDefinition,
                         keyword);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t t e r n P a t h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPatternPath() draws a pattern.
%
%  The format of the DrawPatternPath method is:
%
%      MagickPassFail DrawPatternPath(Image *image,const DrawInfo *draw_info,
%        const char *name,Image **pattern)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o name: The pattern name.
%
%    o image: The image.
%
%
*/
MagickExport MagickPassFail
DrawPatternPath(Image *image,const DrawInfo *draw_info,const char *name,
                Image **pattern)
{
  char
    attribute[MaxTextExtent];

  const ImageAttribute
    *geometry,
    *path;

  DrawInfo
    *clone_info;

  ImageInfo
    *image_info;

  MagickPassFail
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (const DrawInfo *) NULL);
  assert(name != (const char *) NULL);
  FormatString(attribute,"[%.1024s]",name);
  path=GetImageAttribute(image,attribute);
  if (path == (ImageAttribute *) NULL)
    return(MagickFail);
  FormatString(attribute,"[%.1024s-geometry]",name);
  geometry=GetImageAttribute(image,attribute);
  if (geometry == (ImageAttribute *) NULL)
    return(MagickFail);
  if ((*pattern) != (Image *) NULL)
    DestroyImage(*pattern);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image_info->size=AllocateString(geometry->value);
  *pattern=AllocateImage(image_info);
  DestroyImageInfo(image_info);
  (void) QueryColorDatabase("none",&(*pattern)->background_color,
    &image->exception);
  (void) SetImage(*pattern,OpaqueOpacity);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "begin pattern-path %.1024s %.1024s",name,geometry->value);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->fill_pattern=(Image *) NULL;
  clone_info->stroke_pattern=(Image *) NULL;
  (void) CloneString(&clone_info->primitive,path->value);
  status=DrawImage(*pattern,clone_info);
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"end pattern-path");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w P o l y g o n P r i m i t i v e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawPolygonPrimitive draws a polygon on the image.
%
%  The format of the DrawPolygonPrimitive method is:
%
%      unsigned int DrawPolygonPrimitive(Image *image,const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static double
GetPixelOpacity(PolygonInfo * restrict polygon_info,const double mid,
                const unsigned int fill,const FillRule fill_rule,const long x,
                const long y,double * restrict stroke_opacity)
{
  double
    alpha,
    beta,
    distance,
    subpath_opacity;

  int
    winding_number;

  register double
    dx,
    dy;

  register EdgeInfo
    *p;

  register const PointInfo
    *q;

  register long
    i;

  long
    j;

  /*
    Compute fill & stroke opacity for this (x,y) point.
  */
  *stroke_opacity=0.0;
  subpath_opacity=0.0;
  p=polygon_info->edges;
  for (j=0; j < polygon_info->number_edges; j++)
  {
    if (y <= (p->bounds.y1-mid-0.5))
      break;
    if (y > (p->bounds.y2+mid+0.5))
      {
        (void) DestroyEdge(polygon_info,j);
        p++;
        continue;
      }
    if (x <= (p->bounds.x1-mid-0.5))
      {
        p++;
        continue;
      }
    if (x > (p->bounds.x2+mid+0.5))
      {
        p++;
        continue;
      }
    for (i=Max(p->highwater,1); i < p->number_points; i++)
    {
      if (y <= (p->points[i-1].y-mid-0.5))
        break;
      if (y > (p->points[i].y+mid+0.5))
        continue;
      if (p->scanline != y)
        {
          p->scanline=y;
          p->highwater=i;
        }
      /*
        Compute distance between a point and an edge.

        In the comments below, let seg0, seg1 be the end points of the line
        segment (edge), and let pt be (x,y).  Conceptually, consider the segment
        to be a vector from seg0 to seg1, and pt to be a vector from seg0 to pt.
        Let seglen be the length of the segment vector, and ptlen be the length
        of the pt vector.

        The "if" tests below now test "beta <= 0" instead of "beta < 0", and
        "beta >= alpha" instead of "beta > alpha".  This captures the case of
        a zero-length segment and avoids a divide-by-zero when computing 1/alpha.
      */
      q=p->points+i-1;    /* q is seg0, q+1 is seg1 */
      dx=(q+1)->x-q->x,   /* seg1.x - seg0.x */
      dy=(q+1)->y-q->y;   /* seg1.y - seg0.y */
      beta=dx*(x-q->x)+dy*(y-q->y);   /* dot(segvec,ptvec) = seglen * ptlen * costheta */
      if (beta <= 0.0)
        {/*cosine<=0, pt is closest to seg0*/
          dx=x-q->x;
          dy=y-q->y;
          distance=dx*dx+dy*dy;
        }
      else
        {
          alpha=dx*dx+dy*dy;  /*seglen*seglen*/
          if (beta >= alpha)
            {/*pt is closest to seg1*/
              dx=x-(q+1)->x;
              dy=y-(q+1)->y;
              distance=dx*dx+dy*dy;
            }
          else
            {/*pt is closest to a point between seg0 and seg1*/
              alpha=1.0/alpha;  /*don't get here if alpha==0*/
              beta=dx*(y-q->y)-dy*(x-q->x);
              distance=alpha*beta*beta;
            }
        }
      /*
        Compute stroke & subpath opacity.
      */
      beta=0.0;
      if (!p->ghostline)
        {
          alpha=mid+0.5;
          if ((*stroke_opacity < 1.0) &&
              (distance <= ((alpha+0.25)*(alpha+0.25))))
            {
              alpha=mid-0.5;
              if (distance <= ((alpha+0.25)*(alpha+0.25)))
                *stroke_opacity=1.0;
              else
                {
                  beta=1.0;
                  if (distance != 1.0)
                    beta=sqrt(distance);
                  alpha=beta-mid-0.5;
                  if (*stroke_opacity < ((alpha-0.25)*(alpha-0.25)))
                    *stroke_opacity=(alpha-0.25)*(alpha-0.25);
                }
            }
        }
      if (!fill || (distance > 1.0) || (subpath_opacity >= 1.0))
        continue;
      if (distance <= 0.0)
        {
          subpath_opacity=1.0;
          continue;
        }
      if (distance > 1.0)
        continue;
      if (beta == 0.0)
        {
          beta=1.0;
          if (distance != 1.0)
            beta=sqrt(distance);
        }
      alpha=beta-1.0;
      if (subpath_opacity < (alpha*alpha))
        subpath_opacity=alpha*alpha;
    }
    p++;
  }
  /*
    Compute fill opacity.
  */
  if (!fill)
    return(0.0);
  if (subpath_opacity >= 1.0)
    return(1.0);
  /*
    Determine winding number.
  */
  winding_number=0;
  p=polygon_info->edges;
  for (j=0; j < polygon_info->number_edges; j++)
  {
    if (y <= p->bounds.y1)
      break;
    if (y > p->bounds.y2)
      {
        p++;
        continue;
      }
    if (x <= p->bounds.x1)
      {
        p++;
        continue;
      }
    if (x > p->bounds.x2)
      {
        winding_number+=p->direction ? 1 : -1;
        p++;
        continue;
      }
    for (i=Max(p->highwater,1); i < p->number_points; i++)
      if (y <= p->points[i].y)
        break;
    q=p->points+i-1;
    dx=(q+1)->x-q->x;
    dy=(q+1)->y-q->y;
    if ((dx*(y-q->y)) <= (dy*(x-q->x)))
      winding_number+=p->direction ? 1 : -1;
    p++;
  }
  if (fill_rule != NonZeroRule)
    {
      if (AbsoluteValue(winding_number) & 0x01)
        return(1.0);
    }
  else
    if (AbsoluteValue(winding_number) > 0)
      return(1.0);
  return(subpath_opacity);
}

static MagickPassFail
DrawPolygonPrimitive(Image *image,const DrawInfo *draw_info,
                     const PrimitiveInfo *primitive_info)
{
  double
    mid,
    stroke_width_limited;

  SegmentInfo
    bounds;

  ThreadViewDataSet
    * restrict polygon_set;

  MagickPassFail
    status = MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(primitive_info != (PrimitiveInfo *) NULL);
  /*
    Nothing to do.
  */
  if (primitive_info->coordinates <= 1)   /*single point polygons have zero area; don't draw*/
    return(MagickPass);

  {
    /*
      Allocate and initialize thread-specific polygon sets.
    */
    PathInfo
      * restrict path_info;

    unsigned int
      index;

    polygon_set=(ThreadViewDataSet *) NULL;
    path_info=(PathInfo *) NULL;
    if ((path_info=ConvertPrimitiveToPath(draw_info,primitive_info))
        != (PathInfo *) NULL)
      {
        if ((polygon_set=AllocateThreadViewDataSet(DestroyPolygonInfo,image,
                                                   &image->exception))
            != (ThreadViewDataSet *) NULL)
          {
            /*
              Assign polygon for each worker thread.
            */
            for (index=0; index < GetThreadViewDataSetAllocatedViews(polygon_set); index++)
              AssignThreadViewData(polygon_set,index,(void *) ConvertPathToPolygon(path_info));

            /*
              Verify worker thread allocations.
            */
            for (index=0; index < GetThreadViewDataSetAllocatedViews(polygon_set); index++)
              if (AccessThreadViewDataById(polygon_set,index) == (void *) NULL)
                {
                  DestroyThreadViewDataSet(polygon_set);
                  polygon_set=(ThreadViewDataSet *) NULL;
                  break;
                }
          }

        MagickFreeMemory(path_info);
      }
    if (polygon_set == (ThreadViewDataSet *) NULL)
      return MagickFail;
  }

  /*
    Compute bounding box.
  */
  {
    const PolygonInfo
      * restrict polygon_info;

    register long
      i;

    polygon_info=(const PolygonInfo *) AccessThreadViewData(polygon_set);
    bounds=polygon_info->edges[0].bounds;

    if (0) /* DEBUG ??? */
      DrawBoundingRectangles(image,draw_info,polygon_info);

    for (i=1; i < polygon_info->number_edges; i++)
      {
        register const EdgeInfo
          *p;

        p=polygon_info->edges+i;
        if (p->bounds.x1 < bounds.x1)
          bounds.x1=p->bounds.x1;
        if (p->bounds.y1 < bounds.y1)
          bounds.y1=p->bounds.y1;
        if (p->bounds.x2 > bounds.x2)
          bounds.x2=p->bounds.x2;
        if (p->bounds.y2 > bounds.y2)
          bounds.y2=p->bounds.y2;
      }
    /*
      Modified clipping below.  Previous code was (similar for y):

        if (x >= columns) x = columns-1;

      Note that if columns == 50 this results in:

        x==49.999 --> x=49.999
        x==50.000 --> x=49.000

      Pretty sure this was not the intention of the original
      author.  Also, this can result in x_start > x_end, which
      can cause the call to GetImagePixelsEx() to blow up due to
      a column count <= 0 (see code below).

      Changed the code to first check for starting bounds beyond
      right/bottom of image, and ending bounds beyond left/top
      of image.  When this is the case, there is nothing to do,
      since the object lies completely outside the image.  If
      not, clip based on columns-1 and rows-1 instead of columns
      and rows.
    */

    /* sanity check for excessively big stroke_width (ticket #515) */
    if  ( (stroke_width_limited = STROKE_WIDTH_LIMIT(image)) > draw_info->stroke_width )
        stroke_width_limited = draw_info->stroke_width;

    mid=ExpandAffine(&draw_info->affine)*stroke_width_limited/2.0;
    bounds.x1-=(mid+1.0);
    bounds.y1-=(mid+1.0);
    bounds.x2+=(mid+1.0);
    bounds.y2+=(mid+1.0);
    if  ( (bounds.x1 >= image->columns) || (bounds.y1 >= image->rows)
      || (bounds.x2 <= 0.0) || (bounds.y2 <= 0.0) )
      return(MagickPass);   /* object completely outside image */
    bounds.x1=bounds.x1 <= 0.0 ? 0.0 : bounds.x1 >= image->columns-1 ?
      image->columns-1 : bounds.x1;
    bounds.y1=bounds.y1 <= 0.0 ? 0.0 : bounds.y1 >= image->rows-1 ?
      image->rows-1 : bounds.y1;
    bounds.x2=bounds.x2 <= 0.0 ? 0.0 : bounds.x2 >= image->columns-1 ?
      image->columns-1 : bounds.x2;
    bounds.y2=bounds.y2 <= 0.0 ? 0.0 : bounds.y2 >= image->rows-1 ?
      image->rows-1 : bounds.y2;
  }

  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    begin draw-polygon");

  if (primitive_info->coordinates == 1)
    {
      /*
        Draw point.
      */
      long
        x_start,
        x_stop,
        y_start,
        y_stop,
        y;

      PixelPacket
        stroke_color;

      stroke_color=draw_info->stroke;
      x_start=(long) ceil(bounds.x1-0.5);   /* rounds n.5 to n */
      x_stop=(long) floor(bounds.x2+0.5);   /* rounds n.5 to n+1 */
      y_start=(long) ceil(bounds.y1-0.5);
      y_stop=(long) floor(bounds.y2+0.5);
#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(status)
#  else
#    pragma omp parallel for schedule(static) shared(status)
#  endif
#endif
      for (y=y_start; y <= y_stop; y++)
        {
          long
            x;

          PixelPacket
            * restrict q;

          MagickPassFail
            thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_DrawPolygonPrimitive_Status)
#endif
          thread_status=status;
          if (thread_status == MagickFail)
            continue;

          x=x_start;
          q=GetImagePixelsEx(image,x,y,x_stop-x+1,1,&image->exception);
          if (q == (PixelPacket *) NULL)
            thread_status=MagickFail;
          if (thread_status != MagickFail)
            {
              for ( ; x <= x_stop; x++)
                {
                  if ((x == (long) ceil(primitive_info->point.x-0.5)) &&
                      (y == (long) ceil(primitive_info->point.y-0.5)))
                    *q=stroke_color;
                  q++;
                }
              if (!SyncImagePixelsEx(image,&image->exception))
                thread_status=MagickFail;
            }
          if (thread_status == MagickFail)
            {
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_DrawPolygonPrimitive_Status)
#endif
              status=thread_status;
            }
        }
    } /* if (primitive_info->coordinates == 1) */
  else
    {
      /*
        Draw polygon or line.
      */
      long
        x_start,
        x_stop,
        y_start,
        y_stop,
        y;

      const Image
        *fill_pattern=draw_info->fill_pattern,
        *stroke_pattern=draw_info->stroke_pattern;

      unsigned int
        fill;

      fill=(primitive_info->method == FillToBorderMethod) ||
        (primitive_info->method == FloodfillMethod);

      x_start=(long) ceil(bounds.x1-0.5);   /* rounds n.5 to n */
      x_stop=(long) floor(bounds.x2+0.5);   /* rounds n.5 to n+1 */
      y_start=(long) ceil(bounds.y1-0.5);
      y_stop=(long) floor(bounds.y2+0.5);
#if 1
#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(status)
#  else
#    pragma omp parallel for schedule(static) shared(status)
#  endif
#endif
#endif
      for (y=y_start; y <= y_stop; y++)
        {
          PixelPacket
            fill_color,
            stroke_color;

          double
            fill_opacity,
            stroke_opacity;

          long
            x;

          PolygonInfo
            * restrict polygon_info;

          PixelPacket
            * restrict q;

          MagickPassFail
            thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_DrawPolygonPrimitive_Status)
#endif
          thread_status=status;
          if (thread_status == MagickFail)
            continue;

          polygon_info=(PolygonInfo *) AccessThreadViewData(polygon_set);
          fill_color=draw_info->fill;
          stroke_color=draw_info->stroke;
          x=x_start;
          q=GetImagePixelsEx(image,x,y,x_stop-x+1,1,&image->exception);
          if (q == (PixelPacket *) NULL)
            thread_status=MagickFail;

          if (thread_status != MagickFail)
            {
              for ( ; x <= x_stop; x++)
                {
                  /*
                    Fill and/or stroke.  The fill_opacity returned by GetPixelOpacity()
                    handles partial pixel coverage at the edge of a polygon, where
                    0==no coverage and 1==full coverage
                  */
                  fill_opacity=GetPixelOpacity(polygon_info,mid,fill,
                                               draw_info->fill_rule,
                                               x,y,&stroke_opacity);
                  if (!draw_info->stroke_antialias)
                    {
                      /* When stroke antialiasing is disabled, only draw for
                         opacities >= 0.99 in order to ensure that lines are not
                         drawn wider than requested. */
                      if (fill_opacity < 0.99)
                        fill_opacity=0.0;
                      if (stroke_opacity < 0.99)
                        stroke_opacity=0.0;
                    }
                  if ((fill_pattern != (Image *) NULL) &&
                      (fill_pattern->columns != 0) &&
                      (fill_pattern->rows != 0))
                    {
                      (void) AcquireOnePixelByReference
                        (fill_pattern,&fill_color,
                        (long) (x-fill_pattern->tile_info.x) % fill_pattern->columns,
                        (long) (y-fill_pattern->tile_info.y) % fill_pattern->rows,
                        &image->exception);
                      /* apply the group opacity value to the pattern pixel */
                      fill_color.opacity = MaxRGB-((MaxRGB-fill_color.opacity)*(MaxRGB-draw_info->opacity)+(MaxRGB>>1))/MaxRGB;
                    }
                  /* combine fill_opacity with the fill color's opacity */
                  fill_opacity=MaxRGBDouble-fill_opacity*
                    (MaxRGBDouble-(double) fill_color.opacity);
                  /*
                    Notes on call to AlphaCompositePixel():

                      fill_color: the polygon or pattern fill color, not premultiplied
                        by its opacity value
                      fill_opacity: product of the fill color opacity and opacity due
                        to partial pixel coverage (e.g., at the edge of the polygon)
                      q: (input) the background pixel, (output) the composited pixel;
                        neither is premultiplied by its opacity value
                      q->opacity: the background pixel opacity

                    The previous version of this code substituted "OpaqueOpacity"
                    for q->opacity if q->opacity was transparent.  I think this was
                    orignally done to avoid a divide-by-zero in AlphaCompositePixel().
                    However, this substitution results in an incorrect result if the
                    background pixel is completely transparent.  Since the current
                    version of AlphaCompositePixel() has code in it to prevent a
                    divide-by-zero, the code has been fixed to always use q->opacity
                    as the background pixel opacity.
                  */
                  AlphaCompositePixel(q,&fill_color,fill_opacity,q,q->opacity);
                  if ((stroke_pattern != (Image *) NULL) &&
                      (stroke_pattern->columns != 0) &&
                      (stroke_pattern->rows != 0))
                    {
                      (void) AcquireOnePixelByReference
                        (stroke_pattern,&stroke_color,
                        (long) (x-stroke_pattern->tile_info.x) % stroke_pattern->columns,
                        (long) (y-stroke_pattern->tile_info.y) % stroke_pattern->rows,
                        &image->exception);
                      /* apply the group opacity value to the pattern pixel */
                      stroke_color.opacity = MaxRGB-((MaxRGB-stroke_color.opacity)*(MaxRGB-draw_info->opacity)+(MaxRGB>>1))/MaxRGB;
                    }
                  stroke_opacity=MaxRGBDouble-stroke_opacity*
                    (MaxRGBDouble-(double)stroke_color.opacity);
                  /*
                    In the call to AlphaCompositePixel() below, q->opacity is now always
                    used as the background pixel opacity for the same reason as described
                    in the call to AlphaCompositePixel() above.
                  */
                  AlphaCompositePixel(q,&stroke_color,stroke_opacity,q,q->opacity);
                  q++;
                } /* for ( ; x <= x_stop; x++) */
              if (!SyncImagePixelsEx(image,&image->exception))
                thread_status=MagickFail;

              if (thread_status == MagickFail)
                {
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_DrawPolygonPrimitive_Status)
#endif
                  status=thread_status;
                }
            }
        }
    }
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end draw-polygon");
  DestroyThreadViewDataSet(polygon_set);

  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w P r i m i t i v e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawPrimitive draws a primitive (line, rectangle, ellipse) on the
%  image.
%
%  The format of the DrawPrimitive method is:
%
%      void DrawPrimitive(Image *image,const DrawInfo *draw_info,
%        PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static void
LogPrimitiveInfo(const PrimitiveInfo *primitive_info)
{
  char
    *methods[] =
    {
      (char *) "point",
      (char *) "replace",
      (char *) "floodfill",
      (char *) "filltoborder",
      (char *) "reset",
      (char *) "?"
    };

  long
    coordinates,
    y;

  PointInfo
    p={0,0},
    q,
    point;

  register long
    i,
    x;

  x=(long) ceil(primitive_info->point.x-0.5);
  y=(long) ceil(primitive_info->point.y-0.5);
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "PointPrimitive %ld,%ld %s",x,y,methods[primitive_info->method]);
      return;
    }
    case ColorPrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "ColorPrimitive %ld,%ld %s",x,y,methods[primitive_info->method]);
      return;
    }
    case MattePrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "MattePrimitive %ld,%ld %s",x,y,methods[primitive_info->method]);
      return;
    }
    case TextPrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "TextPrimitive %ld,%ld",x,y);
      return;
    }
    case ImagePrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "ImagePrimitive %ld,%ld",x,y);
      return;
    }
    default:
      break;
  }
  coordinates=0;
  q.x=(-1.0);
  q.y=(-1.0);
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    point=primitive_info[i].point;
    if (coordinates <= 0)
      {
        coordinates=(long) primitive_info[i].coordinates;
        (void) LogMagickEvent(RenderEvent,GetMagickModule(),
          "    begin open (%ld)",coordinates);
        p=point;
      }
    point=primitive_info[i].point;
    if ((fabs(q.x-point.x) > MagickEpsilon) ||
        (fabs(q.y-point.y) > MagickEpsilon))
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"      %ld: %g,%g",
        coordinates,point.x,point.y);
    else
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "      %ld: %g,%g (duplicate)",coordinates,point.x,point.y);
    q=point;
    coordinates--;
    if (coordinates > 0)
      continue;
    if ((fabs(p.x-point.x) > MagickEpsilon) ||
        (fabs(p.y-point.y) > MagickEpsilon))
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end last (%ld)",
        coordinates);
    else
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end open (%ld)",
        coordinates);
  }
}

static MagickPassFail
DrawPrimitive(Image *image,const DrawInfo *draw_info,
              const PrimitiveInfo *primitive_info)
{
  long
    y;

  MonitorHandler
    handler;

  register long
    i,
    x;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  begin draw-primitive");
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "    affine: %g,%g,%g,%g,%g,%g",draw_info->affine.sx,draw_info->affine.rx,
    draw_info->affine.ry,draw_info->affine.sy,draw_info->affine.tx,
    draw_info->affine.ty);
  status &= ModifyCache(image,&image->exception);
  if (status == MagickFail)
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  end draw-primitive");
      return status;
    }
  x=(long) ceil(primitive_info->point.x-0.5);
  y=(long) ceil(primitive_info->point.y-0.5);
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    {
      if ((q=GetImagePixels(image,x,y,1,1)) != (PixelPacket *) NULL)
        {
          *q=draw_info->fill;
          status&=SyncImagePixels(image);
        }
      break;
    }
    case ColorPrimitive:
    {
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          if ((q=GetImagePixels(image,x,y,1,1)) != (PixelPacket *) NULL)
            {
              *q=draw_info->fill;
              status&=SyncImagePixels(image);
            }
          break;
        }
        case ReplaceMethod:
        {
          Image
            *pattern;

          PixelPacket
            color,
            target;

          color=draw_info->fill;
          if (AcquireOnePixelByReference(image,&target,x,y,
                                         &image->exception) == MagickFail)
            break;
          pattern=draw_info->fill_pattern;
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              if (!FuzzyColorMatch(q,&target,image->fuzz))
                {
                  q++;
                  continue;
                }
              if (pattern != (Image *) NULL)
                {
                  if (AcquireOnePixelByReference(pattern,&color,
                     (long) (x-pattern->tile_info.x) % pattern->columns,
                     (long) (y-pattern->tile_info.y) % pattern->rows,
                                                 &image->exception) == MagickFail)
                    break;
                  if (!pattern->matte)
                    color.opacity=OpaqueOpacity;
                }
              if (color.opacity != TransparentOpacity)
                AlphaCompositePixel(q,&color,color.opacity,q,q->opacity);
              q++;
            }
            status&=SyncImagePixels(image);
            if (status == MagickFail)
              break;
          }
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          PixelPacket
            border_color,
            target;

          if (AcquireOnePixelByReference(image,&target,x,y,
                                         &image->exception) == MagickFail)
            break;
          if (primitive_info->method == FillToBorderMethod)
            {
              border_color=draw_info->border_color;
              target=border_color;
            }
          status&=ColorFloodfillImage(image,draw_info,target,x,y,
                                      primitive_info->method);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              *q=draw_info->fill;
              q++;
            }
            status&=SyncImagePixels(image);
            if (status == MagickFail)
              break;
          }
          break;
        }
      }
      break;
    }
    case MattePrimitive:
    {
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          q=GetImagePixels(image,x,y,1,1);
          if (q == (PixelPacket *) NULL)
            break;
          q->opacity=TransparentOpacity;
          status&=SyncImagePixels(image);
          break;
        }
        case ReplaceMethod:
        {
          PixelPacket
            target;

          if (AcquireOnePixelByReference(image,&target,x,y,
                                         &image->exception) == MagickFail)
            break;
          status&=TransparentImage(image,target,TransparentOpacity);
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          PixelPacket
            border_color,
            target;

          if (AcquireOnePixelByReference(image,&target,x,y,
                                         &image->exception) == MagickFail)
            break;
          if (primitive_info->method == FillToBorderMethod)
            {
              border_color=draw_info->border_color;
              target=border_color;
            }
          status&=MatteFloodfillImage(image,target,TransparentOpacity,x,y,
                                      primitive_info->method);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
              {
                q->opacity=draw_info->fill.opacity;
                q++;
              }
            status&=SyncImagePixels(image);
            if (status == MagickFail)
              break;
          }
          break;
        }
      }
      break;
    }
    case TextPrimitive:
    {
      char
        geometry[MaxTextExtent];

      DrawInfo
        *clone_info;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
      (void) CloneString(&clone_info->text,primitive_info->text);
      FormatString(geometry,"%+g%+g",primitive_info->point.x,
        primitive_info->point.y);
      (void) CloneString(&clone_info->geometry,geometry);
      /*
        FIXME: AnnotateImage sometimes returns error status here
        without throwing exception and under conditions which should
        be ok when rendering (e.g. off-canvas drawing).
      */
      status&=AnnotateImage(image,clone_info);
      DestroyDrawInfo(clone_info);
      break;
    }
    case ImagePrimitive:
    {
      AffineMatrix
        affine;

      Image
        *composite_image=(Image *) NULL;

      ImageInfo
        *clone_info;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneImageInfo((ImageInfo *) NULL);
      if (LocaleNCompare(primitive_info->text,"data:",5) == 0)
        composite_image=ReadInlineImage(clone_info,primitive_info->text,
          &image->exception);
      else
        {
          /*
            Sanity check URL/path before passing it to ReadImage()

            This is a temporary fix until suitable flags can be passed
            to keep SetImageInfo() from doing potentially dangerous
            magick things.
          */
#define VALID_PREFIX(str,url) (LocaleNCompare(str,url,sizeof(str)-1) == 0)
          if (!VALID_PREFIX("http://", primitive_info->text) &&
              !VALID_PREFIX("https://", primitive_info->text) &&
              !VALID_PREFIX("ftp://", primitive_info->text)  &&
              !(IsAccessibleNoLogging(primitive_info->text))
              )
            {
              ThrowException(&image->exception,FileOpenError,UnableToOpenFile,primitive_info->text);
              status=MagickFail;
            }
          else
            {
              (void) strlcpy(clone_info->filename,primitive_info->text,
                             MaxTextExtent);
              composite_image=ReadImage(clone_info,&image->exception);
            }
        }
      if (image->exception.severity != UndefinedException)
        MagickError2(image->exception.severity,image->exception.reason,
          image->exception.description);
      DestroyImageInfo(clone_info);
      if (composite_image == (Image *) NULL)
        {
          status=MagickFail;
          break;
        }
      if ((primitive_info[1].point.x != composite_image->columns) &&
          (primitive_info[1].point.y != composite_image->rows))
        {
          char
            geometry[MaxTextExtent];

          /*
            Resize image.
          */
          FormatString(geometry,"%gx%g!",primitive_info[1].point.x,
            primitive_info[1].point.y);
          handler=SetMonitorHandler((MonitorHandler) NULL);
          TransformImage(&composite_image,(char *) NULL,geometry);
          (void) SetMonitorHandler(handler);
        }
      if (!composite_image->matte)
        SetImageOpacity(composite_image,OpaqueOpacity);
      if (draw_info->opacity != OpaqueOpacity)
        SetImageOpacity(composite_image,draw_info->opacity);
      affine=draw_info->affine;
      affine.tx=x;
      affine.ty=y;
      status&=DrawAffineImage(image,composite_image,&affine);
      DestroyImage(composite_image);
      break;
    }
    default:
    {
      double
        mid,
        stroke_width_limited,
        scale;

      DrawInfo
        *clone_info;

      if (IsEventLogging())
        LogPrimitiveInfo(primitive_info);
      scale=ExpandAffine(&draw_info->affine);
      if ((draw_info->dash_pattern != (double *) NULL) &&
          (draw_info->dash_pattern[0] != 0.0) &&
          ((scale*draw_info->stroke_width) > MagickEpsilon) &&
          (draw_info->stroke.opacity != TransparentOpacity))
        {
          /*
            Draw dash polygon.
          */
          clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
          clone_info->stroke_width=0.0;
          clone_info->stroke.opacity=TransparentOpacity;
          status&=DrawPolygonPrimitive(image,clone_info,primitive_info);
          DestroyDrawInfo(clone_info);
          status&=DrawDashPolygon(draw_info,primitive_info,image);
          break;
        }

      /* sanity check for excessively big stroke_width (ticket #515) */
      if  ( (stroke_width_limited = STROKE_WIDTH_LIMIT(image)) > draw_info->stroke_width )
          stroke_width_limited = draw_info->stroke_width;

      mid=ExpandAffine(&draw_info->affine)*stroke_width_limited/2.0;
      if ((mid > 1.0) && (draw_info->stroke.opacity != TransparentOpacity))
        {
          unsigned int
            closed_path;

          /*
            Draw strokes while respecting line cap/join attributes.
          */
          closed_path=PRIMINF_GET_IS_CLOSED_SUBPATH(&primitive_info[0]);
          i=(long) primitive_info[0].coordinates;
          if ((((draw_info->linecap == RoundCap) || closed_path) &&
               (draw_info->linejoin == RoundJoin)) ||
               (primitive_info[i].primitive != UndefinedPrimitive))
            {
              status&=DrawPolygonPrimitive(image,draw_info,primitive_info);
              break;
            }
          /* first fill the polygon by cloning and turning off stroking ... */
          clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
          clone_info->stroke_width=0.0;
          clone_info->stroke.opacity=TransparentOpacity;
          status&=DrawPolygonPrimitive(image,clone_info,primitive_info);
          DestroyDrawInfo(clone_info);
          /* ... and then stroke the polygon */
          status&=DrawStrokePolygon(image,draw_info,primitive_info);
          break;
        }
      status&=DrawPolygonPrimitive(image,draw_info,primitive_info);
      break;
    }
  }
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  end draw-primitive");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w S t r o k e P o l y g o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawStrokePolygon draws a stroked polygon (line, rectangle, ellipse)
%  on the image while respecting the line cap and join attributes.
%
%  The format of the DrawStrokePolygon method is:
%
%      unsigned int DrawStrokePolygon(Image *image,const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static void
DrawRoundLinecap(Image *image,const DrawInfo *draw_info,
                 const PrimitiveInfo *primitive_info)
{
  PrimitiveInfo
    linecap[5];

  register long
    i;

  for (i=0; i < 4; i++)
    linecap[i]=(*primitive_info);
  linecap[0].coordinates=4;
  linecap[1].point.x+=10.0*MagickEpsilon;
  linecap[2].point.x+=10.0*MagickEpsilon;
  linecap[2].point.y+=10.0*MagickEpsilon;
  linecap[3].point.y+=10.0*MagickEpsilon;
  linecap[4].primitive=UndefinedPrimitive;
  (void) DrawPolygonPrimitive(image,draw_info,linecap);
}

static MagickPassFail
DrawStrokePolygon(Image *image,const DrawInfo *draw_info,
                  const PrimitiveInfo *primitive_info)
{
  DrawInfo
    *clone_info;

  MagickBool
    closed_path;

  MagickPassFail
    status;

  PrimitiveInfo
    *stroke_polygon;

  register const PrimitiveInfo
    *p,
    *q;

  /*
    Draw stroked polygon.
  */
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "    begin draw-stroke-polygon");
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->fill=draw_info->stroke;
  if (clone_info->fill_pattern != (Image *) NULL)
    {
      DestroyImage(clone_info->fill_pattern);
      clone_info->fill_pattern= (Image *) NULL;
    }
  if (clone_info->stroke_pattern != (Image *) NULL)
    clone_info->fill_pattern=CloneImage(clone_info->stroke_pattern,0,0,
      MagickTrue,&clone_info->stroke_pattern->exception);
  clone_info->stroke.opacity=TransparentOpacity;
  clone_info->stroke_width=0.0;
  clone_info->fill_rule=NonZeroRule;
  status=MagickPass;
  for (p=primitive_info; p->primitive != UndefinedPrimitive; p+=p->coordinates)
  {
    /*
      Per the SVG spec:  A subpath (see Paths) consisting of a
      single moveto shall not be stroked.
    */
    if  ( p->coordinates == 1 )
      continue;
    /*
      *** BUG ALERT! ***

      The sequence below has a bug in it somewhere.  "Thin" polygons that are
      stroked with a stroke-width whose magnitude is close to the size of the
      "thinness" of the polygon are rendered incorrectly.  For example, this
      SVG path renders correctly:

      <path fill="none" stroke="rgb(0,255,0)" stroke-width="3" stroke-linecap="round"
        stroke-linejoin="miter" d="M 10 10 110 10 110 10.5 z" />

      But this one does not:

      <path fill="none" stroke="rgb(0,255,0)" stroke-width="3" stroke-linecap="round"
        stroke-linejoin="miter" d="M 10 10 110 10 110 10.4 z" />

      The only difference is the y coordinate of the last path point (10.5 vs. 10.4).

      The "stroke_polygon" produced by TraceStrokePolygon() for the second path is
      significantly different from that for the first path, so my suspicion is that
      that's where the bug is.  However, it could also be in DrawPolygonPrimitive().
    */
    stroke_polygon=TraceStrokePolygon(image,draw_info,p);
    status&=DrawPolygonPrimitive(image,clone_info,stroke_polygon);
    MagickFreeMemory(stroke_polygon);
    if (status == MagickFail)
      break;
    q=p+p->coordinates-1;
    closed_path=PRIMINF_GET_IS_CLOSED_SUBPATH(p);
    if ((draw_info->linecap == RoundCap) && !closed_path)
      {
        DrawRoundLinecap(image,draw_info,p);
        DrawRoundLinecap(image,draw_info,q);
      }
  }
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "    end draw-stroke-polygon");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t D r a w I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetDrawInfo initializes draw_info to default values.
%
%  The format of the GetDrawInfo method is:
%
%      void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o draw_info: The draw info.
%
%
*/
MagickExport void
GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
{
  ImageInfo
    *clone_info;

  DrawInfoExtra
    *DIExtra;

  /*
    Initialize draw attributes.
  */
  assert(draw_info != (DrawInfo *) NULL);
  (void) memset(draw_info,0,sizeof(DrawInfo));

  /* allocate and initialize struct for extra DrawInfo members */
  DIExtra = MagickAllocateMemory(DrawInfoExtra *,sizeof(DrawInfoExtra));
  if  ( DIExtra == (DrawInfoExtra *) NULL )
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,UnableToAllocateDrawInfo);
  memset(DIExtra,0,sizeof(*DIExtra));
  draw_info->extra = DIExtra;

  clone_info=CloneImageInfo(image_info);
  IdentityAffine(&draw_info->affine);
  draw_info->gravity=NorthWestGravity;
  draw_info->opacity=OpaqueOpacity; /* 0UL */
  draw_info->fill.opacity=OpaqueOpacity; /* 0UL */
  draw_info->stroke.opacity=TransparentOpacity; /* MaxRGB */
  draw_info->stroke_antialias=clone_info->antialias;
  draw_info->stroke_width=1.0;
  draw_info->fill_rule=EvenOddRule;
  draw_info->linecap=ButtCap;
  draw_info->linejoin=MiterJoin;
  draw_info->miterlimit=10;
  draw_info->decorate=NoDecoration;
  if (clone_info->font != (char *) NULL)
    draw_info->font=AllocateString(clone_info->font);
  if (clone_info->density != (char *) NULL)
    draw_info->density=AllocateString(clone_info->density);
  draw_info->text_antialias=clone_info->antialias;
  draw_info->pointsize=clone_info->pointsize;
  draw_info->undercolor.opacity=TransparentOpacity;
  draw_info->border_color=clone_info->border_color;
  draw_info->compose=CopyCompositeOp;
  if (clone_info->server_name != (char *) NULL)
    draw_info->server_name=AllocateString(clone_info->server_name);
  draw_info->render=True;
  draw_info->signature=MagickSignature;
  SetDrawInfoSVGCompliant(draw_info,MagickFalse);
  SetDrawInfoClippingPath(draw_info,MagickFalse);
  DestroyImageInfo(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P e r m u t a t e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Permutate()
%
%  The format of the Permutate method is:
%
%      void Permutate(long n,long k)
%
%  A description of each parameter follows:
%
%    o n:
%
%    o k:
%
%
*/
static inline double
Permutate(const long n,const long k)
{
  double
    r;

  register long
    i;

  r=1.0;
  for (i=k+1; i <= n; i++)
    r*=i;
  for (i=1; i <= (n-k); i++)
    r/=i;
  return(r);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   T r a c e P r i m i t i v e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TracePrimitive is a collection of methods for generating graphic
%  primitives such as arcs, ellipses, paths, etc.
%
%
*/

static void
TraceArc(PrimitiveInfoMgr *p_PIMgr,const PointInfo start,
         const PointInfo end,const PointInfo arc)
{
  PointInfo
    center,
    radius;

  center.x=0.5*(end.x+start.x);
  center.y=0.5*(end.y+start.y);
  radius.x=fabs(center.x-start.x);
  radius.y=fabs(center.y-start.y);
  TraceEllipse(p_PIMgr,center,radius,arc);
}

static void
TraceArcPath(PrimitiveInfoMgr *p_PIMgr,const PointInfo start,
             const PointInfo end,const PointInfo arc,const double angle,
             const unsigned int large_arc,const unsigned int sweep)
{
  double
    alpha,
    beta,
    delta,
    factor,
    gamma,
    theta;

  PointInfo
    center,
    points[3],
    radii;

  register double
    cosine,
    sine;

  register PrimitiveInfo
    *p;

  register long
    i;

  unsigned long
    arc_segments;

  PrimitiveInfo
    **pp_PrimitiveInfo,
    *primitive_info;

  size_t
    InitialStoreStartingAt;

  pp_PrimitiveInfo = p_PIMgr->pp_PrimitiveInfo;
  InitialStoreStartingAt = p_PIMgr->StoreStartingAt;
  primitive_info = *pp_PrimitiveInfo + InitialStoreStartingAt;

  primitive_info->coordinates = 0;	/* in case we return without doing anything */
  /*
    Per the SVG spec: If the endpoints (x1, y1) and (x2, y2) are identical, then
    this is equivalent to omitting the elliptical arc segment entirely.
  */
  if  ( (fabs(start.x-end.x) < MagickEpsilon) && (fabs(start.y-end.y) < MagickEpsilon) )
    {
      /* substitute a lineto command (so zero length arc looks like a zero length segment) */
      TracePoint(primitive_info,end);
      return;
    }
  radii.x=fabs(arc.x);
  radii.y=fabs(arc.y);
  if ((radii.x == 0.0) || (radii.y == 0.0))
    {
      TraceLine(primitive_info,start,end);
      return;
    }
  cosine=cos(DegreesToRadians(fmod(angle,360.0)));
  sine=sin(DegreesToRadians(fmod(angle,360.0)));
  center.x=cosine*(end.x-start.x)/2+sine*(end.y-start.y)/2;
  center.y=cosine*(end.y-start.y)/2-sine*(end.x-start.x)/2;
  delta=(center.x*center.x)/(radii.x*radii.x)+
    (center.y*center.y)/(radii.y*radii.y);
  if (delta > 1.0)
    {
      radii.x*=sqrt(delta);
      radii.y*=sqrt(delta);
    }
  points[0].x=cosine*start.x/radii.x+sine*start.y/radii.x;
  points[0].y=cosine*start.y/radii.y-sine*start.x/radii.y;
  points[1].x=cosine*end.x/radii.x+sine*end.y/radii.x;
  points[1].y=cosine*end.y/radii.y-sine*end.x/radii.y;
  alpha=points[1].x-points[0].x;
  beta=points[1].y-points[0].y;
  factor=1.0/(alpha*alpha+beta*beta)-0.25;
  if (factor <= 0.0)
    factor=0.0;
  else
    {
      factor=sqrt(factor);
      if (sweep == large_arc)
        factor=(-factor);
    }
  center.x=(points[0].x+points[1].x)/2-factor*beta;
  center.y=(points[0].y+points[1].y)/2+factor*alpha;
  alpha=atan2(points[0].y-center.y,points[0].x-center.x);
  theta=atan2(points[1].y-center.y,points[1].x-center.x)-alpha;
  if ((theta < 0.0) && sweep)
    theta+=2.0*MagickPI;
  else
    if ((theta > 0.0) && !sweep)
      theta-=2.0*MagickPI;
  arc_segments=(long) ceil(fabs(theta/(0.5*MagickPI+MagickEpsilon)));
  p=primitive_info;
  for (i=0; i < (long) arc_segments; i++)
  {
    beta=0.5*((alpha+(i+1)*theta/arc_segments)-(alpha+i*theta/arc_segments));
    gamma=(8.0/3.0)*sin(fmod(0.5*beta,DegreesToRadians(360.0)))*
      sin(fmod(0.5*beta,DegreesToRadians(360.0)))/
      sin(fmod(beta,DegreesToRadians(360.0)));
    points[0].x=center.x+
      cos(fmod(alpha+i*theta/arc_segments,DegreesToRadians(360.0)))-gamma*
      sin(fmod(alpha+i*theta/arc_segments,DegreesToRadians(360.0)));
    points[0].y=center.y+
      sin(fmod(alpha+i*theta/arc_segments,DegreesToRadians(360.0)))+gamma*
      cos(fmod(alpha+i*theta/arc_segments,DegreesToRadians(360.0)));
    points[2].x=center.x+
      cos(fmod(alpha+(i+1)*theta/arc_segments,DegreesToRadians(360.0)));
    points[2].y=center.y+
      sin(fmod(alpha+(i+1)*theta/arc_segments,DegreesToRadians(360.0)));
    points[1].x=points[2].x+gamma*
      sin(fmod(alpha+(i+1)*theta/arc_segments,DegreesToRadians(360.0)));
    points[1].y=points[2].y-gamma*
      cos(fmod(alpha+(i+1)*theta/arc_segments,DegreesToRadians(360.0)));
    p->point.x=(p == primitive_info) ? start.x : (p-1)->point.x;
    p->point.y=(p == primitive_info) ? start.y : (p-1)->point.y;
    (p+1)->point.x=cosine*radii.x*points[0].x-sine*radii.y*points[0].y;
    (p+1)->point.y=sine*radii.x*points[0].x+cosine*radii.y*points[0].y;
    (p+2)->point.x=cosine*radii.x*points[1].x-sine*radii.y*points[1].y;
    (p+2)->point.y=sine*radii.x*points[1].x+cosine*radii.y*points[1].y;
    (p+3)->point.x=cosine*radii.x*points[2].x-sine*radii.y*points[2].y;
    (p+3)->point.y=sine*radii.x*points[2].x+cosine*radii.y*points[2].y;
    if (i == (long) (arc_segments-1))
      (p+3)->point=end;
    TraceBezier(p_PIMgr,4);
    p = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base addr might have changed */
    p_PIMgr->StoreStartingAt+=p->coordinates;
    p+=p->coordinates;
  }
  primitive_info = *pp_PrimitiveInfo + InitialStoreStartingAt;
  p_PIMgr->StoreStartingAt = InitialStoreStartingAt;  /* restore original value */
  primitive_info->coordinates=p-primitive_info;
  PRIMINF_CLEAR_FLAGS(primitive_info);
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void
TraceBezier(PrimitiveInfoMgr *p_PIMgr,
            const unsigned long number_coordinates)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    end,
    point,
    *points;

  PrimitiveInfo
    *primitive_info,
    **pp_PrimitiveInfo;

  register PrimitiveInfo
    *p;

  register long
    i,
    j;

  unsigned long
    control_points,
    quantum;

  pp_PrimitiveInfo = p_PIMgr->pp_PrimitiveInfo;
  primitive_info = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;

  /*
    Allocate coeficients.
  */
  quantum=number_coordinates;
  for (i=0; i < (long) number_coordinates; i++)
  {
    for (j=i+1; j < (long) number_coordinates; j++)
    {
      alpha=fabs(primitive_info[j].point.x-primitive_info[i].point.x);
      if (alpha > quantum)
        quantum=(unsigned long) alpha;
      alpha=fabs(primitive_info[j].point.y-primitive_info[i].point.y);
      if (alpha > quantum)
        quantum=(unsigned long) alpha;
    }
  }
  quantum=Min(quantum/number_coordinates,BezierQuantum);
  control_points=quantum*number_coordinates;

  /* make sure we have enough space */
  if  ( PrimitiveInfoRealloc(p_PIMgr,control_points+1) )
    primitive_info = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;

  coefficients=MagickAllocateArray(double *,number_coordinates,sizeof(double));
  points=MagickAllocateArray(PointInfo *,control_points,sizeof(PointInfo));
  if ((coefficients == (double *) NULL) || (points == (PointInfo *) NULL))
    MagickFatalError3(ResourceLimitError,MemoryAllocationFailed,
      UnableToDrawOnImage);
  /*
    Compute bezier points.
  */
  end=primitive_info[number_coordinates-1].point;
  weight=0.0;
  for (i=0; i < (long) number_coordinates; i++)
    coefficients[i]=Permutate((long) number_coordinates-1,i);
  for (i=0; i < (long) control_points; i++)
  {
    p=primitive_info;
    point.x=0;
    point.y=0;
    alpha=pow((double) (1.0-weight),(double) number_coordinates-1);
    for (j=0; j < (long) number_coordinates; j++)
    {
      point.x+=alpha*coefficients[j]*p->point.x;
      point.y+=alpha*coefficients[j]*p->point.y;
      alpha*=weight/(1.0-weight);
      p++;
    }
    points[i]=point;
    weight+=1.0/quantum/number_coordinates;
  }
  /*
    Bezier curves are just short segmented polys.
  */
  p=primitive_info;
  for (i=0; i < (long) control_points; i++)
  {
    TracePoint(p,points[i]);
    p+=p->coordinates;
  }
  TracePoint(p,end);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  PRIMINF_CLEAR_FLAGS(primitive_info);
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  MagickFreeMemory(points);
  MagickFreeMemory(coefficients);
}

static void
TraceCircle(PrimitiveInfoMgr *p_PIMgr,const PointInfo start,
            const PointInfo end)
{
  double
    alpha,
    beta,
    radius;

  PointInfo
    offset,
    degrees;

  alpha=end.x-start.x;
  beta=end.y-start.y;
  radius=sqrt(alpha*alpha+beta*beta);
  offset.x=radius;
  offset.y=radius;
  degrees.x=0.0;
  degrees.y=360.0;
  TraceEllipse(p_PIMgr,start,offset,degrees);
}

static void
TraceEllipse(PrimitiveInfoMgr *p_PIMgr,const PointInfo start,
             const PointInfo stop,const PointInfo degrees)
{
  double
    delta,
    step,
    y;

  PointInfo
    angle,
    point;

  register PrimitiveInfo
    *p;

  PrimitiveInfo
    *primitive_info,
    **pp_PrimitiveInfo;

  size_t
    Needed;

  /*
    Ellipses are just short segmented polys.
  */
  pp_PrimitiveInfo = p_PIMgr->pp_PrimitiveInfo;
  primitive_info = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;
  primitive_info->coordinates = 0;  /* in case we return without doing anything */
  if (stop.x == 0.0 || stop.y == 0.0)  /* actually the x and y radii of the corner ellipse */
    return;
  delta=2.0/Max(stop.x,stop.y);
  step=MagickPI/8.0;
  if (delta < (MagickPI/8.0))
    step=MagickPI/(4*ceil(MagickPI/delta/2));
  angle.x=DegreesToRadians(degrees.x);
  y=degrees.y;
  while (y < degrees.x)
    y+=360.0;
  angle.y=DegreesToRadians(y);

  /* make sure we have enough space */
  Needed = ((size_t)1) + (size_t) ceil((angle.y - angle.x) / step);
  if  ( PrimitiveInfoRealloc(p_PIMgr,Needed) )
    primitive_info = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;

  for (p=primitive_info; angle.x < angle.y; angle.x+=step)
  {
    point.x=cos(fmod(angle.x,DegreesToRadians(360.0)))*stop.x+start.x;
    point.y=sin(fmod(angle.x,DegreesToRadians(360.0)))*stop.y+start.y;
    TracePoint(p,point);
    p+=p->coordinates;
  }
  point.x=cos(fmod(angle.y,DegreesToRadians(360.0)))*stop.x+start.x;
  point.y=sin(fmod(angle.y,DegreesToRadians(360.0)))*stop.y+start.y;
  TracePoint(p,point);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  PRIMINF_CLEAR_FLAGS(primitive_info);
  if  (
    (fabs(primitive_info[0].point.x-primitive_info[primitive_info->coordinates-1].point.x) < MagickEpsilon)
    && (fabs(primitive_info[0].point.y-primitive_info[primitive_info->coordinates-1].point.y) < MagickEpsilon)
    )
    PRIMINF_SET_IS_CLOSED_SUBPATH(primitive_info,1);
  /* NOTE: p pointing just past last traced point! */
  for (p--; p > primitive_info; p--)
    p->primitive=primitive_info->primitive;
}

static void
TraceLine(PrimitiveInfo *primitive_info,const PointInfo start,
          const PointInfo end)
{
  TracePoint(primitive_info,start);
  if ((AbsoluteValue(start.x-end.x) <= MagickEpsilon) &&
      (AbsoluteValue(start.y-end.y) <= MagickEpsilon))
    {
      primitive_info->primitive=PointPrimitive;
      primitive_info->coordinates=1;
      return;
    }
  TracePoint(primitive_info+1,end);
  (primitive_info+1)->primitive=primitive_info->primitive;
  primitive_info->coordinates=2;
  PRIMINF_CLEAR_FLAGS(primitive_info);
}

/*
  Special wrapper macros around strtod(), strtol(), and
  MagickGetToken() to support TracePath() parsing error detection and
  reporting.
*/
#define MagickTracePathAtoF(str,value)                          \
  do {                                                          \
    if (MagickAtoFChk(str,value) != MagickPass)                \
      {                                                         \
        ThrowException(&image->exception,DrawError,FloatValueConversionError,str); \
        return 0;                                               \
      }                                                         \
  } while(0)

#define MagickTracePathAtoI(str,value)                       \
  do {                                                       \
    if (MagickAtoIChk(str,value) != MagickPass)              \
      {                                                      \
        ThrowException(&image->exception,DrawError,IntegerValueConversionError,str); \
        return 0;                                            \
      }                                                      \
  } while(0)

#define MagickTracePathAtoUI(str,value)                       \
  do {                                                       \
    if (MagickAtoUIChk(str,value) != MagickPass)              \
      {                                                      \
        ThrowException(&image->exception,DrawError,IntegerValueConversionError,str); \
        return 0;                                            \
      }                                                      \
  } while(0)

#define MagickGetTracePathToken(p,ep,token,extent)        \
  do {                                                    \
    if (MagickGetToken(p,ep,token,extent) < 1)            \
      {                                                   \
        ThrowException(&image->exception,DrawError,VectorPathTruncated,p); \
        return 0;                        \
      }                                                   \
  } while(0)


static unsigned long
TracePath(Image *image,PrimitiveInfoMgr *p_PIMgr,const char *path)
{
  char
    token[MaxTextExtent];

  char
    *p;

  int
    attribute,
    last_attribute;

  double
    x,
    y;

  PointInfo
    end={0,0},
    points[4]={{0,0},{0,0},{0,0},{0,0}},
    point={0,0},
    start={0,0};

  PrimitiveType
    primitive_type;

  PrimitiveInfo
    *primitive_info,
    **pp_PrimitiveInfo;

  register PrimitiveInfo
    *q;

  register long
    i;

  unsigned long
    number_coordinates,
    z_count;

  size_t
    SubpathOffset;  /* index to start of current subpath */

  pp_PrimitiveInfo = p_PIMgr->pp_PrimitiveInfo;
  SubpathOffset = p_PIMgr->StoreStartingAt;
  primitive_info = *pp_PrimitiveInfo + SubpathOffset;
  attribute=0;
  number_coordinates=0;
  z_count=0;
  primitive_type=primitive_info->primitive;
  q=primitive_info;
  for (p=(char *) path; *p != '\0'; )
  {
    while (isspace((int) *p))
      p++;
    if (*p == '\0')
      break;
    last_attribute=attribute;
    attribute=(*p++);
    switch (attribute)
    {
      /*
        Elliptical arc
      */
      case 'a':
      case 'A':
      {
        MagickBool
          large_arc = MagickFalse,
          sweep = MagickFalse;

        double
          angle = 0.0;

        PointInfo
          arc = {0,0};

        /*
          Compute arc points.
        */
        do
        {
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&arc.x);
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&arc.y);
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&angle);
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoUI(token,&large_arc);
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoUI(token,&sweep);
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&x);
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&y);
          end.x=attribute == 'A' ? x : point.x+x;
          end.y=attribute == 'A' ? y : point.y+y;
          TraceArcPath(p_PIMgr,point,end,arc,angle,large_arc,sweep);
          q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base address might have changed */
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;
          point=end;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p) != MagickFalse);
        break;
      }
      /*
        Cubic Bézier curve
      */
      case 'c':
      case 'C':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 4; i++)
          {
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            if (*token == ',')
              MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            MagickTracePathAtoF(token,&x);
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            if (*token == ',')
              MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            MagickTracePathAtoF(token,&y);
            end.x=attribute == 'C' ? x : point.x+x;
            end.y=attribute == 'C' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i < 4; i++)
            (q+i)->point=points[i];
          TraceBezier(p_PIMgr,4);
          q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base address might have changed */
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;
          point=end;
          /* consume comma if present (as in elliptical arc above) */
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p));
        break;
      }
      case 'H':
      case 'h':
      {
        do
        {
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&x);
          point.x=attribute == 'H' ? x: point.x+x;
          /* make sure we have at least 100 elements available */
          if  ( ((p_PIMgr->StoreStartingAt + 100) > *p_PIMgr->p_AllocCount) && PrimitiveInfoRealloc(p_PIMgr,100) )
              q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base address might have changed */
          TracePoint(q,point);
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;
          /* consume comma if present (as in elliptical arc above) */
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p));
        break;
      }
      /*
        Line to
      */
      case 'l':
      case 'L':
      {
        do
        {
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&x);
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&y);
          point.x=attribute == 'L' ? x : point.x+x;
          point.y=attribute == 'L' ? y : point.y+y;
          /* make sure we have at least 100 elements available */
          if  ( ((p_PIMgr->StoreStartingAt + 100) > *p_PIMgr->p_AllocCount) && PrimitiveInfoRealloc(p_PIMgr,100) )
              q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base address might have changed */
          TracePoint(q,point);
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;
          /* consume comma if present (as in elliptical arc above) */
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p));
        break;
      }
      /*
        Move to
      */
      case 'M':
      case 'm':
      {
        if  ( p_PIMgr->StoreStartingAt != SubpathOffset )
          {
            primitive_info = *pp_PrimitiveInfo + SubpathOffset;
            primitive_info->coordinates=q-primitive_info;
            number_coordinates+=primitive_info->coordinates;
            primitive_info=q;
            SubpathOffset=p_PIMgr->StoreStartingAt;
          }
        i=0;
        do
        {
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&x);
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&y);
          point.x=attribute == 'M' ? x : point.x+x;
          point.y=attribute == 'M' ? y : point.y+y;
          if (i == 0)
            start=point; /*otherwise it's an implied lineto command for both 'M' and 'm'*/
          i++;
          /* make sure we have at least 100 elements available */
          if  ( ((p_PIMgr->StoreStartingAt + 100) > *p_PIMgr->p_AllocCount) && PrimitiveInfoRealloc(p_PIMgr,100) )
              q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base address might have changed */
          TracePoint(q,point);
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;

          /* there was code here that added the point again, but only if 'M' (?) ... deleted */

          /* consume comma if present (as in elliptical arc above) */
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p));
        break;
      }
      /*
        Quadratic Bézier curve
      */
      case 'q':
      case 'Q':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 3; i++)
          {
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            if (*token == ',')
              MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            MagickTracePathAtoF(token,&x);
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            if (*token == ',')
              MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            MagickTracePathAtoF(token,&y);
            if (*p == ',')
              p++;
            end.x=attribute == 'Q' ? x : point.x+x;
            end.y=attribute == 'Q' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          TraceBezier(p_PIMgr,3);
          q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;
          point=end;
          /* consume comma if present (as in elliptical arc above) */
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p));
        break;
      }
      /*
        Cubic Bézier curve
      */
      case 's':
      case 'S':
      {
        /*
          Compute bezier points.
        */
        /*
          Handle multiple pairs of cubic Bezier points when the previous path data
          command was not a cubic Bezier (i.e., not 'c' or 's').
        */
        int previous_path_data_command = last_attribute;  /* the previous path data command upon entry to this 'case' */
        do
        {
          points[0]=points[3];
          points[1].x=2.0*points[3].x-points[2].x;
          points[1].y=2.0*points[3].y-points[2].y;
          for (i=2; i < 4; i++)
          {
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            if (*token == ',')
              MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            MagickTracePathAtoF(token,&x);
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            if (*token == ',')
              MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            MagickTracePathAtoF(token,&y);
            if (*p == ',')
              p++;
            end.x=attribute == 'S' ? x : point.x+x;
            end.y=attribute == 'S' ? y : point.y+y;
            points[i]=end;
          }
          if (strchr("CcSs",previous_path_data_command) == (char *) NULL) /* check the ACTUAL previous command */
            {
              points[0]=point;
              points[1]=point;
            }
          for (i=0; i < 4; i++)
            (q+i)->point=points[i];
          TraceBezier(p_PIMgr,4);
          q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;
          point=end;
          previous_path_data_command = attribute;   /* current path data command becomes previous for next loop iteration */
          /* consume comma if present (as in elliptical arc above) */
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p));
        break;
      }
      /*
        Quadratic Bézier curve
      */
      case 't':
      case 'T':
      {
        /*
          Compute bezier points.
        */
        /*
          Handle multiple pairs of quadratic Bezier points when the previous path data
          command was not a quadratic Bezier (i.e., not 'q' or 't').
        */
        int previous_path_data_command = last_attribute;  /* the previous path data command upon entry to this 'case' */
        do
        {
          points[0]=points[2];
          points[1].x=2.0*points[2].x-points[1].x;
          points[1].y=2.0*points[2].y-points[1].y;
          for (i=2; i < 3; i++)
          {
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            if (*token == ',')
              MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            MagickTracePathAtoF(token,&x);
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            if (*token == ',')
              MagickGetTracePathToken(p,&p,token,MaxTextExtent);
            MagickTracePathAtoF(token,&y);
            end.x=attribute == 'T' ? x : point.x+x;
            end.y=attribute == 'T' ? y : point.y+y;
            points[i]=end;
          }
          if (strchr("QqTt",previous_path_data_command) == (char *) NULL) /* check the ACTUAL previous command */
            {
              points[0]=point;
              points[1]=point;
            }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          TraceBezier(p_PIMgr,3);
          q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;
          point=end;
          previous_path_data_command = attribute;   /* current path data command becomes previous for next loop iteration */
          /* consume comma if present (as in elliptical arc above) */
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p));
        break;
      }
      /*
        Line to
      */
      case 'v':
      case 'V':
      {
        do
        {
          MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          if (*token == ',')
            MagickGetTracePathToken(p,&p,token,MaxTextExtent);
          MagickTracePathAtoF(token,&y);
          point.y=attribute == 'V' ? y : point.y+y;
          /* make sure we have at least 100 elements available */
          if  ( ((p_PIMgr->StoreStartingAt + 100) > *p_PIMgr->p_AllocCount) && PrimitiveInfoRealloc(p_PIMgr,100) )
              q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base address might have changed */
          TracePoint(q,point);
          p_PIMgr->StoreStartingAt += q->coordinates;
          q+=q->coordinates;
          /* consume comma if present (as in elliptical arc above) */
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
          if (*p == ',')
            p++;
        } while (IsPoint(p));
        break;
      }
      /*
        Close path
      */
      case 'z':
      case 'Z':
      {
        point=start;
        /* make sure we have at least 100 elements available */
        if  ( ((p_PIMgr->StoreStartingAt + 100) > *p_PIMgr->p_AllocCount) && PrimitiveInfoRealloc(p_PIMgr,100) )
            q = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base address might have changed */
        TracePoint(q,point);
        p_PIMgr->StoreStartingAt += q->coordinates;
        q+=q->coordinates;
        primitive_info = *pp_PrimitiveInfo + SubpathOffset;
        primitive_info->coordinates=q-primitive_info;
        PRIMINF_SET_IS_CLOSED_SUBPATH(primitive_info,1);
        number_coordinates+=primitive_info->coordinates;
        primitive_info=q;
        SubpathOffset = p_PIMgr->StoreStartingAt;
        z_count++;
        break;
      }
      default:
      {
        if (isalpha((int) attribute))
          (void) fprintf(stderr,"attribute not recognized: %c\n",attribute);
        break;
      }
    }
  }

  primitive_info = *pp_PrimitiveInfo + SubpathOffset;
  primitive_info->coordinates=q-primitive_info;
  number_coordinates+=primitive_info->coordinates;
  for (i=0; i < (long) number_coordinates; i++)
  {
    q--;
    q->primitive=primitive_type;
    if (z_count > 1)
      q->method=FillToBorderMethod;
  }
  q=primitive_info;
  return(number_coordinates);
}

static void
TracePoint(PrimitiveInfo *primitive_info,const PointInfo point)
{
  primitive_info->coordinates=1;
  PRIMINF_CLEAR_FLAGS(primitive_info);
  primitive_info->point=point;
}

static void
TraceRectangle(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  PointInfo
    point;

  register PrimitiveInfo
    *p;

  register long
    i;

  /*
    Per the SVG spec, if the width and/or height are zero, rendering
    the element is disabled.
  */
  if  ( (start.x == end.x) || (start.y == end.y) )
  {
    primitive_info->coordinates = 0;
    return;
  }
  p=primitive_info;
  TracePoint(p,start);
  p+=p->coordinates;
  point.x=start.x;
  point.y=end.y;
  TracePoint(p,point);
  p+=p->coordinates;
  TracePoint(p,end);
  p+=p->coordinates;
  point.x=end.x;
  point.y=start.y;
  TracePoint(p,point);
  p+=p->coordinates;
  TracePoint(p,start);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  PRIMINF_CLEAR_FLAGS(primitive_info);
  PRIMINF_SET_IS_CLOSED_SUBPATH(primitive_info,1);
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void
TraceRoundRectangle(PrimitiveInfoMgr *p_PIMgr,
                    const PointInfo start,const PointInfo end,PointInfo arc)
{
  PointInfo
    degrees,
    offset,
    point;

  register PrimitiveInfo
    *p;

  size_t
    InitialStoreStartingAt;

  PrimitiveInfo
    *primitive_info,
    **pp_PrimitiveInfo;

  pp_PrimitiveInfo = p_PIMgr->pp_PrimitiveInfo;
  InitialStoreStartingAt = p_PIMgr->StoreStartingAt;

  offset.x=AbsoluteValue(end.x-start.x);  /* rect width */
  offset.y=AbsoluteValue(end.y-start.y);  /* rect height */
  /*
    Per the SVG spec, if the width and/or height are zero, rendering
    the element is disabled.
  */
  if  ( (offset.x == 0.0) || (offset.y == 0.0) )
  {
	(*pp_PrimitiveInfo+InitialStoreStartingAt)->coordinates = 0;
    return;
  }

  if (arc.x > (0.5*offset.x))
    arc.x=0.5*offset.x;
  if (arc.y > (0.5*offset.y))
    arc.y=0.5*offset.y;

  point.x=start.x+offset.x-arc.x;
  point.y=start.y+arc.y;
  degrees.x=270.0;
  degrees.y=360.0;
  TraceEllipse(p_PIMgr,point,arc,degrees);
  p = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base addr might have moved */
  p_PIMgr->StoreStartingAt += p->coordinates;

  point.x=start.x+offset.x-arc.x;
  point.y=start.y+offset.y-arc.y;
  degrees.x=0.0;
  degrees.y=90.0;
  TraceEllipse(p_PIMgr,point,arc,degrees);
  p = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base addr might have moved */
  p_PIMgr->StoreStartingAt += p->coordinates;

  point.x=start.x+arc.x;
  point.y=start.y+offset.y-arc.y;
  degrees.x=90.0;
  degrees.y=180.0;
  TraceEllipse(p_PIMgr,point,arc,degrees);
  p = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base addr might have moved */
  p_PIMgr->StoreStartingAt += p->coordinates;

  point.x=start.x+arc.x;
  point.y=start.y+arc.y;
  degrees.x=180.0;
  degrees.y=270.0;
  TraceEllipse(p_PIMgr,point,arc,degrees);
  p = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;  /* base addr might have moved */
  p_PIMgr->StoreStartingAt += p->coordinates;

  /* need only one element, but make sure there is some headroom */
  if  ( (p_PIMgr->StoreStartingAt + 100) > *p_PIMgr->p_AllocCount )
    PrimitiveInfoRealloc(p_PIMgr,100);
  p = *pp_PrimitiveInfo + p_PIMgr->StoreStartingAt;
  TracePoint(p,(*pp_PrimitiveInfo+InitialStoreStartingAt)->point);
  p+=p->coordinates;
  p_PIMgr->StoreStartingAt = InitialStoreStartingAt;  /* restore original value */

  primitive_info = *pp_PrimitiveInfo + InitialStoreStartingAt;
  primitive_info->coordinates=p-primitive_info;
  PRIMINF_CLEAR_FLAGS(primitive_info);
  PRIMINF_SET_IS_CLOSED_SUBPATH(primitive_info,1);
  /* NOTE: p pointing just past last traced point! */
  for (p--; p > primitive_info; p--)
    p->primitive=primitive_info->primitive;
}

static void
TraceSquareLinecap(PrimitiveInfo *primitive_info,
                   const unsigned long number_vertices,const double offset)
{
  double
    distance;

  register double
    dx,
    dy;

  register long
    i;

  long
    j;

  dx=0.0;
  dy=0.0;
  for (i=1; i < (long) number_vertices; i++)
  {
    dx=primitive_info[0].point.x-primitive_info[i].point.x;
    dy=primitive_info[0].point.y-primitive_info[i].point.y;
    if ((fabs(dx) >= MagickEpsilon) || (fabs(dy) >= MagickEpsilon))
      break;
  }
  if (i == (long) number_vertices)
    i=number_vertices-1;
  distance=sqrt(dx*dx+dy*dy+MagickEpsilon);
  primitive_info[0].point.x=primitive_info[i].point.x+
    dx*(distance+offset)/distance;
  primitive_info[0].point.y=primitive_info[i].point.y+
    dy*(distance+offset)/distance;
  for (j=(long) number_vertices-2; j >= 0;  j--)
  {
    dx=primitive_info[number_vertices-1].point.x-primitive_info[j].point.x;
    dy=primitive_info[number_vertices-1].point.y-primitive_info[j].point.y;
    if ((fabs(dx) >= MagickEpsilon) || (fabs(dy) >= MagickEpsilon))
      break;
  }
  distance=sqrt(dx*dx+dy*dy+MagickEpsilon);
  primitive_info[number_vertices-1].point.x=primitive_info[j].point.x+
    dx*(distance+offset)/distance;
  primitive_info[number_vertices-1].point.y=primitive_info[j].point.y+
    dy*(distance+offset)/distance;
}

static PrimitiveInfo *
TraceStrokePolygon(const Image *image,  /* added Image* param so DrawInfo::stroke_width can be clamped */
                   const DrawInfo *draw_info,
                   const PrimitiveInfo *primitive_info)
{
  typedef struct _LineSegment
  {
    double
      p,
      q;
  } LineSegment;

  LineSegment
    dx={0,0},
    dy={0,0},
    inverse_slope={0,0},
    slope={0,0},
    theta={0,0};

  MagickBool
    closed_path;

  double
    delta_theta,
    dot_product,
    mid,
    miterlimit,
    stroke_width_limited;

  PointInfo
    box_p[5],
    box_q[5],
    center,
    offset,
    *path_p,
    *path_q;

  PrimitiveInfo
    *polygon_primitive,
    *stroke_polygon;

  register long
    i;

  unsigned long
    arc_segments,
    max_strokes_p,
    max_strokes_q,
    max_strokes_extra,
    number_vertices;

  unsigned long
    j,
    n,
    p,
    q;

  /*
    Compute initial sizes for paths based on the number of primitive coordinates.  We
    will always allocate max_strokes_extra additional points, so that for each loop
    iteration there will always be enough space, provided we haven't passed
    max_strokes_{p|q} yet.
  */
  number_vertices=primitive_info->coordinates;
  max_strokes_p=max_strokes_q=2*number_vertices;
  max_strokes_extra=6*BezierQuantum+360;  /* will always allocate this much extra */

  /* moved path_p and path_q mem alloc to later since we might not need them */

  polygon_primitive=
    MagickAllocateArray(PrimitiveInfo *,(number_vertices+2),
                        sizeof(PrimitiveInfo));
  if (polygon_primitive == (PrimitiveInfo *) NULL)
    {
      return((PrimitiveInfo *) NULL);
    }
  (void) memcpy(polygon_primitive,primitive_info,number_vertices*
    sizeof(PrimitiveInfo));
  closed_path=PRIMINF_GET_IS_CLOSED_SUBPATH(&primitive_info[0]);
  if ((draw_info->linejoin == RoundJoin) ||
      ((draw_info->linejoin == MiterJoin) && closed_path))
    {
      polygon_primitive[number_vertices]=primitive_info[1];
      number_vertices++;
    }
  polygon_primitive[number_vertices].primitive=UndefinedPrimitive;
  /*
    Compute the slope for the first line segment, p.
  */
  for (n=1; n < number_vertices; n++)
  {
    dx.p=polygon_primitive[n].point.x-polygon_primitive[0].point.x;
    dy.p=polygon_primitive[n].point.y-polygon_primitive[0].point.y;
    if ((fabs(dx.p) >= MagickEpsilon) || (fabs(dy.p) >= MagickEpsilon))
      break;
  }
  if (n == number_vertices)
    {
      /*
        If we get here the subpath consists entirely of "zero length" (within MagickEpsilon)
        segments.  According to the SVG spec:  "Any zero length subpath shall not be stroked
        if the 'stroke-linecap' property has a value of butt but shall be stroked if the
        'stroke-linecap' property has a value of round or square".  Since 'stroke-linecap'
        is only used for open paths, this is only significant if the path is not closed.
      */
      MagickBool DoStroke;
      DoStroke = (!closed_path && (draw_info->linecap == RoundCap));
      if  ( !DoStroke )
        {/*skip stroking*/
          /* create polygon with one element and 0 coords; DrawPolygonPrimitive() will ignore it */
          stroke_polygon = MagickAllocateArray(PrimitiveInfo *,1,sizeof(PrimitiveInfo));
          stroke_polygon[0] = polygon_primitive[0];
          stroke_polygon[0].coordinates = 0;
          MagickFreeMemory(polygon_primitive);
          return(stroke_polygon);
        }/*skip stroking*/
      n=number_vertices-1;
    }

  /* moved path_p and path_q mem allocs to here */

  /*
    Allocate paths.
  */
  path_p=MagickAllocateArray(PointInfo *,max_strokes_p+max_strokes_extra,sizeof(PointInfo));
  if (path_p == (PointInfo *) NULL)
    {
      MagickFreeMemory(polygon_primitive);
      return((PrimitiveInfo *) NULL);
    }
  path_q=MagickAllocateArray(PointInfo *,max_strokes_q+max_strokes_extra,sizeof(PointInfo));
  if (path_q == (PointInfo *) NULL)
    {
      MagickFreeMemory(path_p);
      MagickFreeMemory(polygon_primitive);
      return((PrimitiveInfo *) NULL);
    }

  slope.p=0.0;
  inverse_slope.p=0.0;
  if (fabs(dx.p) < MagickEpsilon)
    {
      if (dx.p >= 0.0)
        slope.p=dy.p < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
      else
        slope.p=dy.p < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
    }
  else
    if (fabs(dy.p) < MagickEpsilon)
      {
        if (dy.p >= 0.0)
          inverse_slope.p=dx.p < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
        else
          inverse_slope.p=dx.p < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
      }
    else
      {
        slope.p=dy.p/dx.p;
        inverse_slope.p=(-1.0/slope.p);
      }

  /* sanity check for excessively big stroke_width (ticket #515) */
  if  ( (stroke_width_limited = STROKE_WIDTH_LIMIT(image)) > draw_info->stroke_width )
      stroke_width_limited = draw_info->stroke_width;

  mid=ExpandAffine(&draw_info->affine)*stroke_width_limited/2.0;
  miterlimit=draw_info->miterlimit*draw_info->miterlimit*mid*mid;
  if ((draw_info->linecap == SquareCap) && !closed_path)
    TraceSquareLinecap(polygon_primitive,number_vertices,mid);
  offset.x=sqrt(mid*mid/(inverse_slope.p*inverse_slope.p+1.0));
  offset.y=offset.x*inverse_slope.p;
  if ((dy.p*offset.x-dx.p*offset.y) > 0.0)
    {
      box_p[0].x=polygon_primitive[0].point.x-offset.x;
      box_p[0].y=polygon_primitive[0].point.y-offset.x*inverse_slope.p;
      box_p[1].x=polygon_primitive[n].point.x-offset.x;
      box_p[1].y=polygon_primitive[n].point.y-offset.x*inverse_slope.p;
      box_q[0].x=polygon_primitive[0].point.x+offset.x;
      box_q[0].y=polygon_primitive[0].point.y+offset.x*inverse_slope.p;
      box_q[1].x=polygon_primitive[n].point.x+offset.x;
      box_q[1].y=polygon_primitive[n].point.y+offset.x*inverse_slope.p;
    }
  else
    {
      box_p[0].x=polygon_primitive[0].point.x+offset.x;
      box_p[0].y=polygon_primitive[0].point.y+offset.y;
      box_p[1].x=polygon_primitive[n].point.x+offset.x;
      box_p[1].y=polygon_primitive[n].point.y+offset.y;
      box_q[0].x=polygon_primitive[0].point.x-offset.x;
      box_q[0].y=polygon_primitive[0].point.y-offset.y;
      box_q[1].x=polygon_primitive[n].point.x-offset.x;
      box_q[1].y=polygon_primitive[n].point.y-offset.y;
    }
  /*
    Create strokes for the line join attribute: bevel, miter, round.
  */
  p=0;
  q=0;
  path_q[p++]=box_q[0];
  path_p[q++]=box_p[0];
  for (i=(long) n+1; i < (long) number_vertices; i++)
  {
    /*
      Compute the slope for this line segment, q.
    */
    dx.q=polygon_primitive[i].point.x-polygon_primitive[n].point.x;
    dy.q=polygon_primitive[i].point.y-polygon_primitive[n].point.y;
    dot_product=dx.q*dx.q+dy.q*dy.q;
    if (dot_product < 0.25)
      continue;
    slope.q=0.0;
    inverse_slope.q=0.0;
    if (fabs(dx.q) < MagickEpsilon)
      {
        if (dx.q >= 0.0)
          slope.q=dy.q < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
        else
          slope.q=dy.q < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
      }
    else
      if (fabs(dy.q) < MagickEpsilon)
        {
          if (dy.q >= 0.0)
            inverse_slope.q=dx.q < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
          else
            inverse_slope.q=dx.q < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
        }
      else
        {
          slope.q=dy.q/dx.q;
          inverse_slope.q=(-1.0/slope.q);
        }
    offset.x=sqrt(mid*mid/(inverse_slope.q*inverse_slope.q+1.0));
    offset.y=offset.x*inverse_slope.q;
    dot_product=dy.q*offset.x-dx.q*offset.y;
    if (dot_product > 0.0)
      {
        box_p[2].x=polygon_primitive[n].point.x-offset.x;
        box_p[2].y=polygon_primitive[n].point.y-offset.y;
        box_p[3].x=polygon_primitive[i].point.x-offset.x;
        box_p[3].y=polygon_primitive[i].point.y-offset.y;
        box_q[2].x=polygon_primitive[n].point.x+offset.x;
        box_q[2].y=polygon_primitive[n].point.y+offset.y;
        box_q[3].x=polygon_primitive[i].point.x+offset.x;
        box_q[3].y=polygon_primitive[i].point.y+offset.y;
      }
    else
      {
        box_p[2].x=polygon_primitive[n].point.x+offset.x;
        box_p[2].y=polygon_primitive[n].point.y+offset.y;
        box_p[3].x=polygon_primitive[i].point.x+offset.x;
        box_p[3].y=polygon_primitive[i].point.y+offset.y;
        box_q[2].x=polygon_primitive[n].point.x-offset.x;
        box_q[2].y=polygon_primitive[n].point.y-offset.y;
        box_q[3].x=polygon_primitive[i].point.x-offset.x;
        box_q[3].y=polygon_primitive[i].point.y-offset.y;
      }
    if (fabs(slope.p-slope.q) <= MagickEpsilon)
      {
        box_p[4]=box_p[1];
        box_q[4]=box_q[1];
      }
    else
      {
        box_p[4].x=(slope.p*box_p[0].x-box_p[0].y-slope.q*box_p[3].x+
          box_p[3].y)/(slope.p-slope.q);
        box_p[4].y=slope.p*(box_p[4].x-box_p[0].x)+box_p[0].y;
        box_q[4].x=(slope.p*box_q[0].x-box_q[0].y-slope.q*box_q[3].x+
          box_q[3].y)/(slope.p-slope.q);
        box_q[4].y=slope.p*(box_q[4].x-box_q[0].x)+box_q[0].y;
      }
    if (p >= max_strokes_p)
      {/*p pointing into extra; time to realloc*/
         max_strokes_p+=max_strokes_extra;
         MagickReallocMemory(PointInfo *,path_p,MagickArraySize(max_strokes_p+max_strokes_extra,sizeof(PointInfo)));
         if (path_p == (PointInfo *) NULL)
      {
             MagickFreeMemory(path_p);
             MagickFreeMemory(path_q);
             MagickFreeMemory(polygon_primitive);
             return((PrimitiveInfo *) NULL);
           }
      }/*p pointing into extra; time to realloc*/
    if (q >= max_strokes_q)
      {/*q pointing into extra; time to realloc*/
         max_strokes_q+=max_strokes_extra;
         MagickReallocMemory(PointInfo *,path_q,MagickArraySize(max_strokes_q+max_strokes_extra,sizeof(PointInfo)));
         if (path_q == (PointInfo *) NULL)
           {
             MagickFreeMemory(path_p);
             MagickFreeMemory(path_q);
             MagickFreeMemory(polygon_primitive);
             return((PrimitiveInfo *) NULL);
           }
      }/*q pointing into extra; time to realloc*/
    dot_product=dx.q*dy.p-dx.p*dy.q;
    if (dot_product <= 0.0)
      switch (draw_info->linejoin)
      {
        case BevelJoin:
        {
          path_q[q++]=box_q[1];
          path_q[q++]=box_q[2];
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_p[p++]=box_p[4];
          else
            {
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          break;
        }
        case MiterJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product > miterlimit)
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
           }
         else
           {
             path_q[q++]=box_q[4];
             path_p[p++]=box_p[4];
           }
          break;
        }
        case RoundJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_p[p++]=box_p[4];
          else
            {
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          center=polygon_primitive[n].point;
          theta.p=atan2(box_q[1].y-center.y,box_q[1].x-center.x);
          theta.q=atan2(box_q[2].y-center.y,box_q[2].x-center.x);
          if (theta.q < theta.p)
            theta.q+=2.0*MagickPI;
          arc_segments=(long) ceil((theta.q-theta.p)/(2.0*sqrt(1.0/mid)));
          /* in case arc_segments is big */
          if  ( (q+arc_segments) >= max_strokes_q )
            {/*q+arc_segments will point into extra; time to realloc*/
              max_strokes_q+=arc_segments+max_strokes_extra;
              MagickReallocMemory(PointInfo *,path_q,MagickArraySize(max_strokes_q+max_strokes_extra,sizeof(PointInfo)));
              if (path_q == (PointInfo *) NULL)
                {
                  MagickFreeMemory(path_p);
                  MagickFreeMemory(path_q);
                  MagickFreeMemory(polygon_primitive);
                  return((PrimitiveInfo *) NULL);
                }
            }/*q+arc_segments will point into extra; time to realloc*/
          path_q[q].x=box_q[1].x;
          path_q[q].y=box_q[1].y;
          q++;
          for (j=1; j < arc_segments; j++)
          {
            delta_theta=j*(theta.q-theta.p)/arc_segments;
            path_q[q].x=center.x+mid*
              cos(fmod(theta.p+delta_theta,DegreesToRadians(360.0)));
            path_q[q].y=center.y+mid*
              sin(fmod(theta.p+delta_theta,DegreesToRadians(360.0)));
            q++;
          }
          path_q[q++]=box_q[2];
          break;
        }
        default:
          break;
      }
    else
      switch (draw_info->linejoin)
      {
        case BevelJoin:
        {
          path_p[p++]=box_p[1];
          path_p[p++]=box_p[2];
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_q[q++]=box_q[4];
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
            }
          break;
        }
        case MiterJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            {
              path_q[q++]=box_q[4];
              path_p[p++]=box_p[4];
            }
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          break;
        }
        case RoundJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_q[q++]=box_q[4];
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
            }
          center=polygon_primitive[n].point;
          theta.p=atan2(box_p[1].y-center.y,box_p[1].x-center.x);
          theta.q=atan2(box_p[2].y-center.y,box_p[2].x-center.x);
          if (theta.p < theta.q)
            theta.p+=2.0*MagickPI;
          arc_segments=(long) ceil((theta.p-theta.q)/(2.0*sqrt(1.0/mid)));
          /* in case arc_segments is big */
          if  ( (p+arc_segments) >= max_strokes_p )
            {/*p+arc_segments will point into extra; time to realloc*/
              max_strokes_p+=arc_segments+max_strokes_extra;
              MagickReallocMemory(PointInfo *,path_p,MagickArraySize(max_strokes_p+max_strokes_extra,sizeof(PointInfo)));
              if (path_p == (PointInfo *) NULL)
                {
                  MagickFreeMemory(path_p);
                  MagickFreeMemory(path_q);
                  MagickFreeMemory(polygon_primitive);
                  return((PrimitiveInfo *) NULL);
                }
            }/*p+arc_segments will point into extra; time to realloc*/
          path_p[p++]=box_p[1];
          for (j=1; j < arc_segments; j++)
          {
            delta_theta=j*(theta.q-theta.p)/arc_segments;
            path_p[p].x=center.x+mid*
              cos(fmod(theta.p+delta_theta,DegreesToRadians(360.0)));
            path_p[p].y=center.y+mid*
              sin(fmod(theta.p+delta_theta,DegreesToRadians(360.0)));
            p++;
          }
          path_p[p++]=box_p[2];
          break;
        }
        default:
          break;
      }
    slope.p=slope.q;
    inverse_slope.p=inverse_slope.q;
    box_p[0]=box_p[2];
    box_p[1]=box_p[3];
    box_q[0]=box_q[2];
    box_q[1]=box_q[3];
    dx.p=dx.q;
    dy.p=dy.q;
    n=i;
  }
  path_p[p++]=box_p[1];
  path_q[q++]=box_q[1];
  /*
    Trace stroked polygon.
  */
  stroke_polygon=
    MagickAllocateArray(PrimitiveInfo *,(p+q+2*closed_path+2),
                        sizeof(PrimitiveInfo));
  if (stroke_polygon != (PrimitiveInfo *) NULL)
    {
      for (i=0; i < (long) p; i++)
      {
        stroke_polygon[i]=polygon_primitive[0];
        stroke_polygon[i].point=path_p[i];
      }
      if (closed_path)
        {
          stroke_polygon[i]=polygon_primitive[0];
          stroke_polygon[i].point=stroke_polygon[0].point;
          i++;
        }
      for ( ; i < (long) (p+q+closed_path); i++)
      {
        stroke_polygon[i]=polygon_primitive[0];
        stroke_polygon[i].point=path_q[p+q+closed_path-(i+1)];
      }
      if (closed_path)
        {
          stroke_polygon[i]=polygon_primitive[0];
          stroke_polygon[i].point=stroke_polygon[p+closed_path].point;
          i++;
        }
      stroke_polygon[i]=polygon_primitive[0];
      stroke_polygon[i].point=stroke_polygon[0].point;
      i++;
      stroke_polygon[i].primitive=UndefinedPrimitive;
      stroke_polygon[0].coordinates=p+q+2*closed_path+1;
    }
  MagickFreeMemory(path_p);
  MagickFreeMemory(path_q);
  MagickFreeMemory(polygon_primitive);
  return(stroke_polygon);
}

#if 0
/* is the DrawInfo drawn as SVG compliant */
static MagickBool
IsDrawInfoClippingPath(const DrawInfo * draw_info)
{
  return(((draw_info->flags&0x2U)==2U)?MagickTrue:MagickFalse);
}
#endif

/* is the DrawInfo drawn as SVG compliant */
static MagickBool
IsDrawInfoSVGCompliant(const DrawInfo * draw_info)
{
  return(((draw_info->flags&0x1U)==1U)?MagickTrue:MagickFalse);
}

/* is the DrawInfo drawn as an SVG compliant clipping path */
static MagickBool
IsDrawInfoSVGCompliantClippingPath(const DrawInfo * draw_info)
{
  return(((draw_info->flags&0x3U)==3U)?MagickTrue:MagickFalse);
}

/* tag the DrawInfo as being a clipping path or not */
static void
SetDrawInfoClippingPath(DrawInfo * draw_info, MagickBool ClippingPath)
{
  draw_info->flags = (draw_info->flags & (~0x2U)) | (ClippingPath?2U:0U);
}

/* tag the DrawInfo as being drawn as SVG compliant or not */
static void
SetDrawInfoSVGCompliant(DrawInfo * draw_info, MagickBool SVGCompliant)
{
  draw_info->flags = (draw_info->flags & (~0x1U)) | (SVGCompliant?1U:0U);
}
