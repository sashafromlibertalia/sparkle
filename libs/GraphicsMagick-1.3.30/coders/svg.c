/*
% Copyright (C) 2003 - 2016 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS  V   V   GGGG                              %
%                            SS     V   V  G                                  %
%                             SSS   V   V  G GG                               %
%                               SS   V V   G   G                              %
%                            SSSSS    V     GGG                               %
%                                                                             %
%                                                                             %
%                 Read/Write Scalable Vector Graphics Format.                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                             William Radcliffe                               %
%                                March 2000                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/render.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#if defined(HasXML)
#  if defined(MSWINDOWS)
#    if defined(__MINGW32__)
#      define _MSC_VER
#    else
#      include <win32config.h>
#    endif
#  endif
#  include <libxml/parser.h>
#  include <libxml/xmlmemory.h>
#  include <libxml/parserInternals.h>
#  include <libxml/xmlerror.h>
#endif

/*
  Disable SVG writer by default since it rarely works correctly.
*/
#if !defined(ENABLE_SVG_WRITER)
#  define ENABLE_SVG_WRITER 0
#endif /* if !defined(ENABLE_SVG_WRITER) */

/*
  Avoid shadowing library globals and functions.
*/
#define attribute attribute_magick

#if defined(HasAUTOTRACE)
#include "types.h"
#include "image-header.h"
#include "fit.h"
#include "output.h"
#include "pxl-outline.h"
#include "atquantize.h"
#include "thin-image.h"

char
  *version_string = "AutoTrace version 0.24a";
#endif

/*
  Define declarations.
*/
#define MVGPrintf  (void) fprintf

/*
  Typedef declarations.
*/
typedef struct _BoundingBox
{
  double
    x,
    y,
    width,
    height;
} BoundingBox;

typedef struct _SVGInfo
{
  FILE
    *file;

  ExceptionInfo
    *exception;

  Image
    *image;

  const ImageInfo
    *image_info;

  AffineMatrix
    affine;

  unsigned long
    width,
    height;

  char
    *size,
    *title,
    *comment;

  int
    n;

  double
    *scale,
    pointsize;

  ElementInfo
    element;

  SegmentInfo
    segment;

  BoundingBox
    bounds,
    view_box;

  PointInfo
    radius;

  char
    *stop_color,
    *offset,
    *text,
    *vertices,
    *url;

  /*
    Even though it's unlikely to happen, keep track of nested <defs> and
    elements tagged with an id.
  */
  int
    defsPushCount,      /* for tracking nested <defs> */
    idLevelInsideDefs,  /* when an "id" is seen, remember svg->n (SVG element level) */
    svgPushCount;       /* for tracking nested <svg> elements */

#if defined(HasXML)
  xmlParserCtxtPtr
    parser;

  xmlDocPtr
    document;
#endif
} SVGInfo;

/*
  Forward declarations.
*/
#if ENABLE_SVG_WRITER
static unsigned int
  WriteSVGImage(const ImageInfo *,Image *);
#endif /* if ENABLE_SVG_WRITER */

#if defined(HasXML)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S V G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSVGImage reads a Scalable Vector Gaphics file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSVGImage method is:
%
%      Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSVGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static double
GetUserSpaceCoordinateValue(const SVGInfo *svg_info,
                            int type,
                            const char *string,
                            MagickBool positive)
{
  char
    *p,
    token[MaxTextExtent];

  double
    value;

  assert(string != (const char *) NULL);
  p=(char *) string;
  (void) MagickGetToken(p,&p,token,MaxTextExtent);
  if ((MagickAtoFChk(token,&value) == MagickFail) ||
      (((positive) && value < 0.0)))
    {
      errno=0;
      ThrowException(svg_info->exception,DrawError,InvalidPrimitiveArgument,
                     string);
    }
  if (strchr(token,'%') != (char *) NULL)
    {
      double
        alpha,
        beta;

      if (type > 0)
        return(svg_info->view_box.width*value/100.0);
      if (type < 0)
        return(svg_info->view_box.height*value/100.0);
      alpha=value-svg_info->view_box.width;
      beta=value-svg_info->view_box.height;
      return(sqrt(alpha*alpha+beta*beta)/sqrt(2.0)/100.0);
    }
  (void) MagickGetToken(p,&p,token,MaxTextExtent);
  if (LocaleNCompare(token,"cm",2) == 0)
    return(72.0*svg_info->scale[0]/2.54*value);
  if (LocaleNCompare(token,"em",2) == 0)
    return(svg_info->pointsize*value);
  if (LocaleNCompare(token,"ex",2) == 0)
    return(svg_info->pointsize*value/2.0);
  if (LocaleNCompare(token,"in",2) == 0)
    return(72.0*svg_info->scale[0]*value);
  if (LocaleNCompare(token,"mm",2) == 0)
    return(72.0*svg_info->scale[0]/25.4*value);
  if (LocaleNCompare(token,"pc",2) == 0)
    return(72.0*svg_info->scale[0]/6.0*value);
  if (LocaleNCompare(token,"pt",2) == 0)
    return(svg_info->scale[0]*value);
  if (LocaleNCompare(token,"px",2) == 0)
    return(value);
  return(value);
}

static char **GetStyleTokens(void *context,const char *text,size_t *number_tokens)
{
  char
    **tokens;

  const char
    *p,
    *q;

  size_t
    alloc_tokens,
    i,
    iListFront;

  SVGInfo
    *svg_info;

  MagickBool
    IsFontSize = MagickFalse;

  svg_info=(SVGInfo *) context;
  *number_tokens=0;
  alloc_tokens=0;
  if (text == (const char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.

    style="fill: red; stroke: blue; stroke-width: 3"
  */
  for (p=text; *p != '\0'; p++)
    if (*p == ':')
      alloc_tokens+=2;
  if (alloc_tokens == 0)
    return((char **) NULL);
  tokens=MagickAllocateMemory(char **,(alloc_tokens+2)*sizeof(*tokens));
  if (tokens == (char **) NULL)
    {
      ThrowException3(svg_info->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToConvertStringToTokens);
      return((char **) NULL);
    }
  (void) memset(tokens,0,(alloc_tokens+2)*sizeof(*tokens));
  /*
    Convert string to an ASCII list.
  */
  i=0;
  p=text;
  iListFront = 0;
  for (q=p; *q != '\0'; q++)
    {
      /*
        ':' terminates the style element (e.g., fill:)
        ';' terminates the style element value (e.g., red)
      */
      if ((*q != ':') && (*q != ';') && (*q != '\0'))
        continue;
      tokens[i]=AllocateString(p);
      if (tokens[i] == NULL)
        {
          ThrowException3(svg_info->exception,ResourceLimitError,
                          MemoryAllocationFailed,UnableToConvertStringToTokens);
          break;
        }
      (void) strlcpy(tokens[i],p,q-p+1);
      Strip(tokens[i]);
      /*
        Check for "font-size", which we will move to the first position in
        the list.  This will ensure that any following numerical conversions
        that depend on the font size will use the new value.
      */
      if  ( (i & 1) == 0 )  /*element name*/
        IsFontSize = (LocaleCompare("font-size",tokens[i]) == 0) ? MagickTrue : MagickFalse;
      else if  ( IsFontSize )
        {/*found font-size/value pair*/
          if  ( (i-1) == iListFront )
            iListFront += 2;  /* already at front of list */
          else
            {
              /* move "font-size" and value to top of list */
              char * pToken = tokens[iListFront];
              tokens[iListFront] = tokens[i-1];
              tokens[i-1] = pToken;
              iListFront++;
              pToken = tokens[iListFront];
              tokens[iListFront] = tokens[i];
              tokens[i] = pToken;
              iListFront++;
            }
        }/*found font-size/value pair*/
      i++;
      if (i >= alloc_tokens)
        break;
      p=q+1;
    }
  if (i < alloc_tokens)
    {
      tokens[i]=AllocateString(p);
      if (tokens[i] == NULL)
        {
          ThrowException3(svg_info->exception,ResourceLimitError,
                          MemoryAllocationFailed,UnableToConvertStringToTokens);
        }
      else
        {
          (void) strlcpy(tokens[i],p,q-p+1);
          Strip(tokens[i]);
          i++;
        }
    }
  tokens[i]=(char *) NULL;
  *number_tokens=i;
  return(tokens);
}

static char **GetTransformTokens(void *context,const char *text,
                                 size_t *number_tokens)
{
  char
    **tokens;

  register const char
    *p,
    *q;

  register size_t
    i;

  SVGInfo
    *svg_info;

  size_t
    alloc_tokens;

  svg_info=(SVGInfo *) context;
  *number_tokens=0;
  if (text == (const char *) NULL)
    return((char **) NULL);

  alloc_tokens=8;
  tokens=MagickAllocateMemory(char **,(alloc_tokens+2)*sizeof(*tokens));
  if (tokens == (char **) NULL)
    {
      ThrowException3(svg_info->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToConvertStringToTokens);
      return((char **) NULL);
    }
  /*
    Convert string to an ASCII list.
  */
  i=0;
  p=text;
  for (q=p; *q != '\0'; q++)
    {
      if ((*q != '(') && (*q != ')') && (*q != '\0'))
        continue;
      if (i == alloc_tokens)
        {
          alloc_tokens <<= 1;
          MagickReallocMemory(char **,tokens,(alloc_tokens+2)*sizeof(*tokens));
          if (tokens == (char **) NULL)
            {
              ThrowException3(svg_info->exception,ResourceLimitError,
                              MemoryAllocationFailed,UnableToConvertStringToTokens);
              return((char **) NULL);
            }
        }
      tokens[i]=AllocateString(p);
      (void) strlcpy(tokens[i],p,q-p+1);
      Strip(tokens[i]);
      i++;
      p=q+1;
    }
  tokens[i]=AllocateString(p);
  (void) strlcpy(tokens[i],p,q-p+1);
  Strip(tokens[i++]);
  tokens[i]=(char *) NULL;
  *number_tokens=i;
  return(tokens);
}

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  static int SVGIsStandalone(void *context);

  static int SVGIsStandalone(void *context);

  static int SVGHasInternalSubset(void *context);

  static int SVGHasExternalSubset(void *context);

  static void SVGInternalSubset(void *context,const xmlChar *name,
                                const xmlChar *external_id,
                                const xmlChar *system_id);

  static xmlParserInputPtr SVGResolveEntity(void *context,
                                            const xmlChar *public_id,
                                            const xmlChar *system_id);

  static xmlEntityPtr SVGGetEntity(void *context,const xmlChar *name);

  static xmlEntityPtr SVGGetParameterEntity(void *context,const xmlChar *name);

  static void SVGEntityDeclaration(void *context,const xmlChar *name,int type,
                                   const xmlChar *public_id,
                                   const xmlChar *system_id,xmlChar *content);

  static void SVGAttributeDeclaration(void *context,const xmlChar *element,
                                      const xmlChar *name,int type,int value,
                                      const xmlChar *default_value,
                                      xmlEnumerationPtr tree);

  static void SVGElementDeclaration(void *context,const xmlChar *name,int type,
                                    xmlElementContentPtr content);

  static void SVGNotationDeclaration(void *context,const xmlChar *name,
                                     const xmlChar *public_id,
                                     const xmlChar *system_id);

  static void SVGUnparsedEntityDeclaration(void *context,const xmlChar *name,
                                           const xmlChar *public_id,
                                           const xmlChar *system_id,
                                           const xmlChar *notation);

  static void SVGSetDocumentLocator(void *context,
                                    xmlSAXLocatorPtr location);

  static void SVGStartDocument(void *context);

  static void SVGEndDocument(void *context);

  static void SVGStartElement(void *context,const xmlChar *name,
                              const xmlChar **attributes);

  static void SVGEndElement(void *context,const xmlChar *name);

  static void SVGCharacters(void *context,const xmlChar *c,int length);

  static void SVGReference(void *context,const xmlChar *name);

  static void SVGIgnorableWhitespace(void *context,const xmlChar *c,int length);

  static void SVGProcessingInstructions(void *context,const xmlChar *target,
                                        const xmlChar *data);

  static void SVGComment(void *context,const xmlChar *value);

  static void SVGWarning(void *context,const char *format,...);

  static void SVGError(void *context,const char *format,...);

  static void SVGCDataBlock(void *context,const xmlChar *value,int length);

  static void SVGExternalSubset(void *context,const xmlChar *name,
                                const xmlChar *external_id,
                                const xmlChar *system_id);

  ModuleExport void RegisterSVGImage(void);

  ModuleExport void UnregisterSVGImage(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


static int
SVGIsStandalone(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Is this document tagged standalone?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.SVGIsStandalone()");
  svg_info=(SVGInfo *) context;
  return(svg_info->document->standalone == 1);
}

static int
SVGHasInternalSubset(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an internal subset?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.SVGHasInternalSubset()");
  svg_info=(SVGInfo *) context;
  return(svg_info->document->intSubset != NULL);
}

static int
SVGHasExternalSubset(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an external subset?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.SVGHasExternalSubset()");
  svg_info=(SVGInfo *) context;
  return(svg_info->document->extSubset != NULL);
}

static void
SVGInternalSubset(void *context,const xmlChar *name,
                  const xmlChar *external_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an internal subset?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.internalSubset(%.1024s, %.1024s, %.1024s)",(char *) name,
                        (external_id != (const xmlChar *) NULL ? (char *) external_id : "none"),
                        (system_id != (const xmlChar *) NULL ? (char *) system_id : "none"));
  svg_info=(SVGInfo *) context;
  (void) xmlCreateIntSubset(svg_info->document,name,external_id,system_id);
}

static xmlParserInputPtr
SVGResolveEntity(void *context,
                 const xmlChar *public_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserInputPtr
    stream;

  /*
    Special entity resolver, better left to the parser, it has more
    context than the application layer.  The default behaviour is to
    not resolve the entities, in that case the ENTITY_REF nodes are
    built in the structure (and the parameter values).
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.resolveEntity(%.1024s, %.1024s)",
                        (public_id != (const xmlChar *) NULL ? (char *) public_id : "none"),
                        (system_id != (const xmlChar *) NULL ? (char *) system_id : "none"));
  svg_info=(SVGInfo *) context;
  stream=xmlLoadExternalEntity((const char *) system_id,(const char *)
                               public_id,svg_info->parser);
  return(stream);
}

static xmlEntityPtr
SVGGetEntity(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Get an entity by name.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.SVGGetEntity(%.1024s)",name);
  svg_info=(SVGInfo *) context;
  return(xmlGetDocEntity(svg_info->document,name));
}

static xmlEntityPtr
SVGGetParameterEntity(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Get a parameter entity by name.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.getParameterEntity(%.1024s)",name);
  svg_info=(SVGInfo *) context;
  return(xmlGetParameterEntity(svg_info->document,name));
}

static void
SVGEntityDeclaration(void *context,const xmlChar *name,int type,
                     const xmlChar *public_id,const xmlChar *system_id,xmlChar *content)
{
  SVGInfo
    *svg_info;

  /*
    An entity definition has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.entityDecl(%.1024s, %d, %.1024s, %.1024s, %.1024s)",name,type,
                        public_id != (xmlChar *) NULL ? (char *) public_id : "none",
                        system_id != (xmlChar *) NULL ? (char *) system_id : "none",content);
  svg_info=(SVGInfo *) context;
  if (svg_info->parser->inSubset == 1)
    (void) xmlAddDocEntity(svg_info->document,name,type,public_id,system_id,
                           content);
  else
    if (svg_info->parser->inSubset == 2)
      (void) xmlAddDtdEntity(svg_info->document,name,type,public_id,system_id,
                             content);
}

static void
SVGAttributeDeclaration(void *context,const xmlChar *element,
                        const xmlChar *name,int type,int value,const xmlChar *default_value,
                        xmlEnumerationPtr tree)
{
  SVGInfo
    *svg_info;

  xmlChar
    *fullname,
    *prefix;

  xmlParserCtxtPtr
    parser;

  /*
    An attribute definition has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.attributeDecl(%.1024s, %.1024s, %d, %d, %.1024s, ...)",element,
                        name,type,value,default_value);
  svg_info=(SVGInfo *) context;
  fullname=(xmlChar *) NULL;
  prefix=(xmlChar *) NULL;
  parser=svg_info->parser;
  fullname=(xmlChar *) xmlSplitQName(parser,name,&prefix);
  if (parser->inSubset == 1)
    (void) xmlAddAttributeDecl(&parser->vctxt,svg_info->document->intSubset,
                               element,fullname,prefix,(xmlAttributeType) type,
                               (xmlAttributeDefault) value,default_value,tree);
  else
    if (parser->inSubset == 2)
      (void) xmlAddAttributeDecl(&parser->vctxt,svg_info->document->extSubset,
                                 element,fullname,prefix,(xmlAttributeType) type,
                                 (xmlAttributeDefault) value,default_value,tree);
  if (prefix != (xmlChar *) NULL)
    xmlFree(prefix);
  if (fullname != (xmlChar *) NULL)
    xmlFree(fullname);
}

static void
SVGElementDeclaration(void *context,const xmlChar *name,int type,
                      xmlElementContentPtr content)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    An element definition has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.elementDecl(%.1024s, %d, ...)",name,type);
  svg_info=(SVGInfo *) context;
  parser=svg_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddElementDecl(&parser->vctxt,svg_info->document->intSubset,
                             name,(xmlElementTypeVal) type,content);
  else
    if (parser->inSubset == 2)
      (void) xmlAddElementDecl(&parser->vctxt,svg_info->document->extSubset,
                               name,(xmlElementTypeVal) type,content);
}

static void
SVGNotationDeclaration(void *context,const xmlChar *name,
                       const xmlChar *public_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    What to do when a notation declaration has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.notationDecl(%.1024s, %.1024s, %.1024s)",name,
                        public_id != (const xmlChar *) NULL ? (char *) public_id : "none",
                        system_id != (const xmlChar *) NULL ? (char *) system_id : "none");
  svg_info=(SVGInfo *) context;
  parser=svg_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddNotationDecl(&parser->vctxt,svg_info->document->intSubset,
                              name,public_id,system_id);
  else
    if (parser->inSubset == 2)
      (void) xmlAddNotationDecl(&parser->vctxt,svg_info->document->intSubset,
                                name,public_id,system_id);
}

static void
SVGUnparsedEntityDeclaration(void *context,const xmlChar *name,
                             const xmlChar *public_id,const xmlChar *system_id,
                             const xmlChar *notation)
{
  SVGInfo
    *svg_info;

  /*
    What to do when an unparsed entity declaration is parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.unparsedEntityDecl(%.1024s, %.1024s, %.1024s, %.1024s)",name,
                        public_id != (xmlChar *) NULL ? (char *) public_id : "none",
                        system_id != (xmlChar *) NULL ? (char *) system_id : "none",notation);
  svg_info=(SVGInfo *) context;
  (void) xmlAddDocEntity(svg_info->document,name,
                         XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,public_id,system_id,notation);

}

static void
SVGSetDocumentLocator(void *context,
                      xmlSAXLocatorPtr location)
{
  /*   SVGInfo */
  /*     *svg_info; */

  ARG_NOT_USED(context);
  ARG_NOT_USED(location);
  /*
    Receive the document locator at startup, actually xmlDefaultSAXLocator.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.setDocumentLocator()");
  /*   svg_info=(SVGInfo *) context; */
}

static void
SVGStartDocument(void *context)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when the document start being processed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.startDocument()");
  svg_info=(SVGInfo *) context;
  DestroyExceptionInfo(svg_info->exception);
  GetExceptionInfo(svg_info->exception);
  parser=svg_info->parser;
  svg_info->document=xmlNewDoc(parser->version);
  if (svg_info->document == (xmlDocPtr) NULL)
    return;
  if (parser->encoding == NULL)
    svg_info->document->encoding=(const xmlChar *) NULL;
  else
    svg_info->document->encoding=xmlStrdup(parser->encoding);
  svg_info->document->standalone=parser->standalone;
}

static void
SVGEndDocument(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Called when the document end has been detected.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.endDocument()");
  svg_info=(SVGInfo *) context;
  MagickFreeMemory(svg_info->offset);
  MagickFreeMemory(svg_info->stop_color);
  MagickFreeMemory(svg_info->scale);
  MagickFreeMemory(svg_info->text);
  MagickFreeMemory(svg_info->vertices);
  MagickFreeMemory(svg_info->url);
  if (svg_info->document != (xmlDocPtr) NULL)
    {
      xmlFreeDoc(svg_info->document);
      svg_info->document=(xmlDocPtr) NULL;
    }
}


/*
  Code from SVGStartElement() that processed transform="..." has been refactored
  into new function SVGProcessTransformString().
*/
static void
SVGProcessTransformString  (
        void *context,
        char const *TransformString
        )
{/*SVGProcessTransformString*/

  char
    **tokens;

  AffineMatrix
    affine,
    current,
    transform;

  char
    *p = NULL,
    token[MaxTextExtent];

  SVGInfo
    *svg_info=(SVGInfo *) context;

  size_t
    j,
    number_tokens = 0;

  IdentityAffine(&transform);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  ");
  tokens=GetTransformTokens(context,TransformString,&number_tokens);
  if ((tokens != (char **) NULL) && (number_tokens > 0))
    {/*if ((tokens != (char **) NULL) && (number_tokens > 0))*/

                const char
        *keyword = NULL,
        *value = NULL;

      for (j=0; j < (number_tokens-1); j+=2)
        {/*j token loop*/

          keyword=(char *) tokens[j];   /* matrix, rotate, etc. */
          value=(char *) tokens[j+1];   /* associated numerical values */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    %.1024s: %.1024s",keyword,value);
          current=transform;
          IdentityAffine(&affine);
          switch (*keyword)
            {/*keyword switch*/

            case 'M':
            case 'm':
              {/*Mm*/
                if (LocaleCompare(keyword,"matrix") == 0)
                  {
                    p=(char *) value;
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    affine.sx=MagickAtoF(token);
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    if (*token == ',')
                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    affine.rx=MagickAtoF(token);
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    if (*token == ',')
                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    affine.ry=MagickAtoF(token);
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    if (*token == ',')
                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    affine.sy=MagickAtoF(token);
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    if (*token == ',')
                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    affine.tx=MagickAtoF(token);
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    if (*token == ',')
                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    affine.ty=MagickAtoF(token);
                    break;
                  }
                break;
              }/*Mm*/

            case 'R':
            case 'r':
              {/*Rr*/
                if (LocaleCompare(keyword,"rotate") == 0)
                  {
                    double
                      angle;

                    angle=GetUserSpaceCoordinateValue(svg_info,0,value,MagickFalse);
                    affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                    affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                    affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                    affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                    break;
                  }
                break;
              }/*Rr*/

            case 'S':
            case 's':
              {/*Ss*/
                if (LocaleCompare(keyword,"scale") == 0)
                  {
                    for (p=(char *) value; *p != '\0'; p++)
                      if (isspace((int) (*p)) || (*p == ','))
                        break;
                    affine.sx=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                    affine.sy=affine.sx;
                    if (*p != '\0')
                      affine.sy=
                        GetUserSpaceCoordinateValue(svg_info,-1,p+1,MagickFalse);
                    svg_info->scale[svg_info->n]=ExpandAffine(&affine);
                    break;
                  }
                if (LocaleCompare(keyword,"skewX") == 0)
                  {
                    affine.sx=svg_info->affine.sx;
                    affine.ry=tan(DegreesToRadians(fmod(
                      GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse),
                      360.0)));
                    affine.sy=svg_info->affine.sy;
                    break;
                  }
                if (LocaleCompare(keyword,"skewY") == 0)
                  {
                    affine.sx=svg_info->affine.sx;
                    affine.rx=tan(DegreesToRadians(fmod(
                      GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse),
                      360.0)));
                    affine.sy=svg_info->affine.sy;
                    break;
                  }
                break;
              }/*Ss*/

            case 'T':
            case 't':
              {/*Tt*/
                if (LocaleCompare(keyword,"translate") == 0)
                  {
                    for (p=(char *) value; *p != '\0'; p++)
                      if (isspace((int) (*p)) || (*p == ','))
                        break;
                    affine.tx=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                    affine.ty=affine.tx;
                    if (*p != '\0')
                      affine.ty=
                        GetUserSpaceCoordinateValue(svg_info,-1,p+1,MagickFalse);
                    break;
                  }
                break;
              }/*Tt*/

            default:
              break;

            }/*keyword switch*/

          transform.sx=current.sx*affine.sx+current.ry*affine.rx;
          transform.rx=current.rx*affine.sx+current.sy*affine.rx;
          transform.ry=current.sx*affine.ry+current.ry*affine.sy;
          transform.sy=current.rx*affine.ry+current.sy*affine.sy;
          transform.tx=current.sx*affine.tx+current.ry*affine.ty+
            current.tx;
          transform.ty=current.rx*affine.tx+current.sy*affine.ty+
            current.ty;

        }/*j token loop*/

      MVGPrintf(svg_info->file,"affine %g %g %g %g %g %g\n",
                transform.sx,transform.rx,transform.ry,transform.sy,
                transform.tx,transform.ty);

    }/*if ((tokens != (char **) NULL) && (number_tokens > 0))*/

  /* clean up memory used for tokens */
  if (tokens != (char **) NULL)
    {
      for (j=0; tokens[j] != (char *) NULL; j++)
        MagickFreeMemory(tokens[j]);
      MagickFreeMemory(tokens);
    }

}/*SVGProcessTransformString*/


static void
SVGStartElement(void *context,const xmlChar *name,
                const xmlChar **attributes)
{
  char
    *color = NULL,
    id[MaxTextExtent],
    *p = NULL,
    token[MaxTextExtent],
    *units = NULL;

  const char
    *keyword = NULL,
    *value = NULL;

  size_t
    number_tokens = 0;

  SVGInfo
    *svg_info;

  size_t
    i,
    j;

  char
    svg_element_background_color[MaxTextExtent];  /* to support style="background:color" */

  MagickBool
    IsTSpan = MagickFalse,
    IsTextOrTSpan = MagickFalse;

  /*
    Called when an opening tag has been processed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.startElement(%.1024s",name);
  id[0]='\0';
  token[0]='\0';
  svg_info=(SVGInfo *) context;
  svg_info->n++;
  MagickReallocMemory(double *,svg_info->scale,(svg_info->n+1)*sizeof(double));
  if (svg_info->scale == (double *) NULL)
    {
      ThrowException(svg_info->exception,ResourceLimitError,
                     MemoryAllocationFailed,"unable to convert SVG image");
      return;
    }
  svg_info->scale[svg_info->n]=svg_info->scale[svg_info->n-1];
  color=AllocateString("none");
  units=AllocateString("userSpaceOnUse");
  value=(const char *) NULL;
  svg_element_background_color[0]='\0';
  IsTextOrTSpan = IsTSpan = LocaleCompare((char *) name,"tspan") == 0;  /* need to know this early */
  /*
    According to the SVG spec, for the following SVG elements, if the x or y
    attribute is not specified, the effect is as if a value of "0" were specified.
  */
  if (
         (LocaleCompare((char *) name,"image") == 0)
      || (LocaleCompare((char *) name,"pattern") == 0)
      || (LocaleCompare((char *) name,"rect") == 0)
      || (LocaleCompare((char *) name,"text") == 0)
      || (LocaleCompare((char *) name,"use") == 0)
      )
    {
      svg_info->bounds.x = svg_info->bounds.y = 0;
    }
  /*
    NOTE: SVG spec makes similar statements for cx,cy of circle and ellipse, and
    x1,y1,x2,y2 of line, but these are zeroed out initially, AND at the end of
    SVGEndElement() after they have been used.
  */

  /*
    When "font-size" is (or is contained in) one of the attributes for this SVG
    element, we want it to be processed first so that any numerical conversions
    that depend on the font size will use the new value.  So we will first scan
    the attribute list and move any "font-size", "class" (which may contain a
    "font-size"), or "style" (which may contain a "font-size") attributes to the
    front of the attribute list.

    For now we will ignore the possibility that "font-size" may be specified
    more than once among "font-size", "class", and "style".  However, the
    relative order among these three will be preserved.
  */
  if (attributes != (const xmlChar **) NULL)
  {/*have some attributes*/

    size_t iListFront = 0;
    for  ( i = 0; (attributes[i] != (const xmlChar *) NULL); i += 2 )
      {/*attribute[i]*/

        keyword = (const char *) attributes[i];
        if  (  (LocaleCompare(keyword,"font-size") == 0)
            || (LocaleCompare(keyword,"class") == 0)
            || (LocaleCompare(keyword,"style") == 0)
         )
         {/*(possible) font-size*/

            if  ( i == iListFront )
              iListFront += 2;  /* already at front of list */
            else
              {
                /* move to front of list */
                const xmlChar * pAttr = attributes[iListFront];
                attributes[iListFront] = attributes[i];
                attributes[i] = pAttr;
                iListFront++;
                pAttr = attributes[iListFront];
                attributes[iListFront] = attributes[i+1];
                attributes[i+1] = pAttr;
                iListFront++;
              }

         }/*(possible) font-size*/

      }/*attribute[i]*/

  }/*have some attributes*/

  if (attributes != (const xmlChar **) NULL)
    for (i=0; (svg_info->exception->severity < ErrorException) &&
           (attributes[i] != (const xmlChar *) NULL); i+=2)
      {
        keyword=(const char *) attributes[i];
        value=(const char *) attributes[i+1];
        switch (*keyword)
          {
          case 'C':
          case 'c':
            {
              if (LocaleCompare(keyword,"cx") == 0)
                {
                  svg_info->element.cx=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"cy") == 0)
                {
                  svg_info->element.cy=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              break;
            }
          case 'F':
          case 'f':
            {
              if (LocaleCompare(keyword,"fx") == 0)
                {
                  svg_info->element.major=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"fy") == 0)
                {
                  svg_info->element.minor=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              break;
            }
          case 'H':
          case 'h':
            {
              if (LocaleCompare(keyword,"height") == 0)
                {
                  svg_info->bounds.height=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickTrue);
                  break;
                }
              break;
            }
          case 'I':
          case 'i':
            {
              if (LocaleCompare(keyword,"id") == 0)
                {
                  (void) strlcpy(id,value,MaxTextExtent);
                  /* track elements inside <defs> that have an "id" */
                  if  ( (svg_info->defsPushCount > 0)
                    && (svg_info->idLevelInsideDefs == 0)   /* do not allow nested "id" elements for now */
                    && (LocaleCompare((const char *)name,"clipPath") != 0)  /* handled separately */
                    && (LocaleCompare((const char *)name,"mask") != 0)      /* handled separately */
                    )
                      svg_info->idLevelInsideDefs = svg_info->n;
                  break;
                }
              break;
            }
          case 'R':
          case 'r':
            {
              if (LocaleCompare(keyword,"r") == 0)
                {
                  svg_info->element.angle=
                    GetUserSpaceCoordinateValue(svg_info,0,value,MagickFalse);
                  break;
                }
              break;
            }
          case 'W':
          case 'w':
            {
              if (LocaleCompare(keyword,"width") == 0)
                {
                  svg_info->bounds.width=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickTrue);
                  break;
                }
              break;
            }
          case 'X':
          case 'x':
            {
              if (LocaleCompare(keyword,"x") == 0)
                {
                  /* if processing a tspan, preserve the current bounds.x, which belongs to the
                     previously processed text or tspan; the bounds.x for the current tspan will
                     be set later */
                  if (!IsTSpan)
                    svg_info->bounds.x=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"x1") == 0)
                {
                  svg_info->segment.x1=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"x2") == 0)
                {
                  svg_info->segment.x2=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              break;
            }
          case 'Y':
          case 'y':
            {
              if (LocaleCompare(keyword,"y") == 0)
                {
                  /* if processing a tspan, preserve the current bounds.y, which belongs to the
                     previously processed text or tspan; the bounds.y for the current tspan will
                     be set later */
                  if (!IsTSpan)
                    svg_info->bounds.y=GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"y1") == 0)
                {
                  svg_info->segment.y1=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"y2") == 0)
                {
                  svg_info->segment.y2=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              break;
            }
          default:
            break;
          }
      }
  if (svg_info->exception->severity >= ErrorException)
    goto svg_start_element_error;
  if (strchr((char *) name,':') != (char *) NULL)
    {
      /*
        Skip over namespace.
      */
      for ( ; *name != ':'; name++) ;
      name++;
    }
  switch (*name)
    {
    case 'C':
    case 'c':
      {
        if (LocaleCompare((char *) name,"circle") == 0)
          {
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        if (LocaleCompare((char *) name,"clipPath") == 0)
          {
            MVGPrintf(svg_info->file,"push clip-path '%s'\n",id);
            break;
          }
        break;
      }
    case 'D':
    case 'd':
      {
        if (LocaleCompare((char *) name,"defs") == 0)
          {
            svg_info->defsPushCount++;
            MVGPrintf(svg_info->file,"push defs\n");
            break;
          }
        break;
      }
    case 'E':
    case 'e':
      {
        if (LocaleCompare((char *) name,"ellipse") == 0)
          {
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        break;
      }
    case 'F':
    case 'f':
      {
        /*
          For now we are ignoring "foreignObject".  However, we do a push/pop
          graphic-context so that any settings (e.g., fill) are consumed and
          discarded.  Otherwise they might persist and "leak" into the graphic
          elements that follow.
        */
        if (LocaleCompare((char *) name,"foreignObject") == 0)
          {
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        break;
      }
    case 'G':
    case 'g':
      {
        if (LocaleCompare((char *) name,"g") == 0)
          {
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        break;
      }
    case 'I':
    case 'i':
      {
        if (LocaleCompare((char *) name,"image") == 0)
          {
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        break;
      }
    case 'L':
    case 'l':
      {
        if (LocaleCompare((char *) name,"line") == 0)
          {
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        if (LocaleCompare((char *) name,"linearGradient") == 0)
          {
            MVGPrintf(svg_info->file,"push gradient '%s' linear %g,%g %g,%g\n",id,
                      svg_info->segment.x1,svg_info->segment.y1,svg_info->segment.x2,
                      svg_info->segment.y2);
            break;
          }
        break;
      }
    case 'M':
    case 'm':
      {
        if (LocaleCompare((char *) name,"mask") == 0)   /* added mask */
        {
          MVGPrintf(svg_info->file,"push mask '%s'\n",id);
          break;
        }
      break;
      }
    case 'P':
    case 'p':
      {
        if (LocaleCompare((char *) name,"path") == 0)
          {
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        if (LocaleCompare((char *) name,"pattern") == 0)
          {
            MVGPrintf(svg_info->file,"push pattern '%s' %g,%g %g,%g\n",id,
                      svg_info->bounds.x,svg_info->bounds.y,svg_info->bounds.width,
                      svg_info->bounds.height);
            break;
          }
        if (LocaleCompare((char *) name,"polygon") == 0)
          {
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        if (LocaleCompare((char *) name,"polyline") == 0)
          {
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        break;
      }
    case 'R':
    case 'r':
      {
        if (LocaleCompare((char *) name,"radialGradient") == 0)
          {
            if (svg_info->element.angle < 0.0)
              {
                errno=0;
                ThrowException(svg_info->exception,DrawError,InvalidPrimitiveArgument,
                               value);
                break;
              }
            MVGPrintf(svg_info->file,"push gradient '%s' radial %g,%g %g,%g %g\n",
                      id,svg_info->element.cx,svg_info->element.cy,
                      svg_info->element.major,svg_info->element.minor,
                      svg_info->element.angle);
            break;
          }
        if (LocaleCompare((char *) name,"rect") == 0)
          {
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        break;
      }
    case 'S':
    case 's':
      {
        /* element "style" inside <defs> */
        if (LocaleCompare((char *) name,"style") == 0)
          {
            /*
              This is here more or less as a documentation aid.  The real work is done when
              we encounter </style>.
            */
            break;
          }
        if (LocaleCompare((char *) name,"svg") == 0)
          {
            svg_info->svgPushCount++;
            MVGPrintf(svg_info->file,"push graphic-context\n");
            /*
              Per the SVG spec, initialize the MVG coder with the following
              SVG defaults:
                - svg-compliant: "1" (note: internal to GM, not an SVG element)
                - fill color: "black"
                - fill-opacity value: "1"
                - stroke color: "none"
                - stroke-width value: "1"
                - stroke-opacity value: "1"
                - fill-rule value: "nonzero"
            */
            MVGPrintf(svg_info->file,"svg-compliant 1\n");
            MVGPrintf(svg_info->file,"fill 'black'\n");
            MVGPrintf(svg_info->file,"fill-opacity 1\n");
            MVGPrintf(svg_info->file,"stroke 'none'\n");
            MVGPrintf(svg_info->file,"stroke-width 1\n");
            MVGPrintf(svg_info->file,"stroke-opacity 1\n");
            MVGPrintf(svg_info->file,"fill-rule 'nonzero'\n");
            break;
          }
        break;
      }
    case 'T':
    case 't':
      {
        if (LocaleCompare((char *) name,"text") == 0)
          {
            IsTextOrTSpan = MagickTrue;
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "push id" if warranted */
              MVGPrintf(svg_info->file,"push id '%s'\n",id);
            MVGPrintf(svg_info->file,"push graphic-context\n");
            /* update text current position */
            MVGPrintf(svg_info->file,"textx %g\n",svg_info->bounds.x);
            MVGPrintf(svg_info->file,"texty %g\n",svg_info->bounds.y);
            break;
          }
        if (LocaleCompare((char *) name,"tspan") == 0)
          {
            IsTextOrTSpan = MagickTrue;
            Strip(svg_info->text);
            if (*svg_info->text != '\0')
              {
                char
                  *text;

                text=EscapeString(svg_info->text,'\'');
                MVGPrintf(svg_info->file,"textc '%s'\n",text);
                MagickFreeMemory(text);
                /*
                  The code that used to be here to compute the next text position has been eliminated.
                  The reason is that at this point in the code we may not know the font or font size
                  (they may be hidden in a "class" definition), so we can't really do that computation.
                  This functionality is now handled by DrawImage() in render.c.
                */
                *svg_info->text='\0';
              }
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        break;
      }
    case 'U':
    case 'u':
      {
        if (LocaleCompare((char *) name,"use") == 0)
          {
            /* "use" behaves like "g" */
            MVGPrintf(svg_info->file,"push graphic-context\n");
            break;
          }
        break;
       }
    default:
      break;
    }
  if (attributes != (const xmlChar **) NULL)
    for (i=0; (svg_info->exception->severity < ErrorException) &&
           (attributes[i] != (const xmlChar *) NULL); i+=2)
      {
        keyword=(const char *) attributes[i];
        value=(const char *) attributes[i+1];
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    %.1024s = %.1024s",keyword,value);
        switch (*keyword)
          {
          case 'A':
          case 'a':
            {
              if (LocaleCompare(keyword,"angle") == 0)
                {
                  MVGPrintf(svg_info->file,"angle %g\n",
                            GetUserSpaceCoordinateValue(svg_info,0,value,MagickFalse));
                  break;
                }
              break;
            }
          case 'C':
          case 'c':
            {
              if (LocaleCompare(keyword,"class") == 0)
                {/*"class=classname"*/
                  char * pClassNames = (char * ) value;
                  do
                    {
                      (void) MagickGetToken(pClassNames,&pClassNames,token,MaxTextExtent);
                      if  ( *token == ',' )
                        (void) MagickGetToken(pClassNames,&pClassNames,token,MaxTextExtent);
                      if  ( *token != '\0' )
                        MVGPrintf(svg_info->file,"class '%s'\n",token);
                  }
                  while  ( *token != '\0' );
                  break;
                }/*"class=classname"*/
              if (LocaleCompare(keyword,"clip-path") == 0)
                {
                  MVGPrintf(svg_info->file,"clip-path '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"clip-rule") == 0)
                {
                  MVGPrintf(svg_info->file,"clip-rule '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"clipPathUnits") == 0)
                {
                  (void) CloneString(&units,value);
                  MVGPrintf(svg_info->file,"clip-units '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"color") == 0)
                {
                  (void) CloneString(&color,value);
                  break;
                }
              if (LocaleCompare(keyword,"cx") == 0)
                {
                  svg_info->element.cx=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"cy") == 0)
                {
                  svg_info->element.cy=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              break;
            }
          case 'D':
          case 'd':
            {
              if (LocaleCompare(keyword,"d") == 0)
                {
                  (void) CloneString(&svg_info->vertices,value);
                  break;
                }
              if (LocaleCompare(keyword,"dx") == 0)
                {
                  double dx=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  svg_info->bounds.x+=dx;   /* preserve previous behavior */
                  /* update text current position for text or tspan */
                  if  ( IsTextOrTSpan )
                    {
                      char * pUnit;
                      (void) MagickGetToken(value,&pUnit,token,MaxTextExtent);
                      if  ( *pUnit && ((LocaleNCompare(pUnit,"em",2) == 0) || (LocaleNCompare(pUnit,"ex",2) == 0)) )
                        MVGPrintf(svg_info->file,"textdx %s\n",value);  /* postpone interpretation of "em" or "ex" until we know point size */
                      else
                        MVGPrintf(svg_info->file,"textdx %g\n",dx);
                    }
                  break;
                }
              if (LocaleCompare(keyword,"dy") == 0)
                {
                  double dy=GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  svg_info->bounds.y+=dy;   /* preserve previous behavior */
                  /* update text current position for text or tspan */
                  if  ( IsTextOrTSpan )
                    {
                      char * pUnit;
                      (void) MagickGetToken(value,&pUnit,token,MaxTextExtent);
                      if  ( *pUnit && ((LocaleNCompare(pUnit,"em",2) == 0) || (LocaleNCompare(pUnit,"ex",2) == 0)) )
                        MVGPrintf(svg_info->file,"textdy %s\n",value);  /* postpone interpretation of "em" or "ex" until we know point size */
                      else
                        MVGPrintf(svg_info->file,"textdy %g\n",dy);
                    }
                  break;
                }
              break;
            }
          case 'F':
          case 'f':
            {
              if (LocaleCompare(keyword,"fill") == 0)
                {
                  if (LocaleCompare(value,"currentColor") == 0)
                    {
                      MVGPrintf(svg_info->file,"fill '%s'\n",color);
                      break;
                    }
                  MVGPrintf(svg_info->file,"fill '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"fillcolor") == 0)
                {
                  MVGPrintf(svg_info->file,"fill '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"fill-rule") == 0)
                {
                  MVGPrintf(svg_info->file,"fill-rule '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"fill-opacity") == 0)
                {
                  MVGPrintf(svg_info->file,"fill-opacity '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"font-family") == 0)
                {
                  /*
                    Deal with Adobe Illustrator 10.0 which double-quotes
                    font-family.  Maybe we need a generalized solution for
                    this.
                  */
                  if ((value[0] == '\'') && (value[strlen(value)-1] == '\''))
                    {
                      char nvalue[MaxTextExtent];
                      (void) strlcpy(nvalue,value+1,sizeof(nvalue));
                      nvalue[strlen(nvalue)-1]='\0';
                      MVGPrintf(svg_info->file,"font-family '%s'\n",nvalue);
                    }
                  else
                    {
                      MVGPrintf(svg_info->file,"font-family '%s'\n",value);
                    }
                  break;
                }
              if (LocaleCompare(keyword,"font-stretch") == 0)
                {
                  MVGPrintf(svg_info->file,"font-stretch '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"font-style") == 0)
                {
                  MVGPrintf(svg_info->file,"font-style '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"font-size") == 0)
                {
                  svg_info->pointsize=
                    GetUserSpaceCoordinateValue(svg_info,0,value,MagickTrue);
                  MVGPrintf(svg_info->file,"font-size %g\n",svg_info->pointsize);
                  break;
                }
              if (LocaleCompare(keyword,"font-weight") == 0)
                {
                  MVGPrintf(svg_info->file,"font-weight '%s'\n",value);
                  break;
                }
              break;
            }
          case 'G':
          case 'g':
            {
              if (LocaleCompare(keyword,"gradientTransform") == 0)
                {
                  char
                    **tokens;

                  AffineMatrix
                    affine,
                    current,
                    transform;

                  IdentityAffine(&transform);
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  ");
                  tokens=GetTransformTokens(context,value,&number_tokens);
                  if ((tokens != (char **) NULL) && (number_tokens > 0))
                    {
                      for (j=0; j < (number_tokens-1); j+=2)
                        {
                          keyword=(char *) tokens[j];
                          if (keyword == (char *) NULL)
                            continue;
                          value=(char *) tokens[j+1];
                          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                                "    %.1024s: %.1024s",keyword,value);
                          current=transform;
                          IdentityAffine(&affine);
                          switch (*keyword)
                            {
                            case 'M':
                            case 'm':
                              {
                                if (LocaleCompare(keyword,"matrix") == 0)
                                  {
                                    p=(char *) value;
                                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    affine.sx=MagickAtoF(value);
                                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    if (*token == ',')
                                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    affine.rx=MagickAtoF(token);
                                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    if (*token == ',')
                                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    affine.ry=MagickAtoF(token);
                                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    if (*token == ',')
                                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    affine.sy=MagickAtoF(token);
                                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    if (*token == ',')
                                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    affine.tx=MagickAtoF(token);
                                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    if (*token == ',')
                                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                                    affine.ty=MagickAtoF(token);
                                    break;
                                  }
                                break;
                              }
                            case 'R':
                            case 'r':
                              {
                                if (LocaleCompare(keyword,"rotate") == 0)
                                  {
                                    double
                                      angle;

                                    angle=GetUserSpaceCoordinateValue(svg_info,0,value,MagickFalse);
                                    affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                                    affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                                    affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                                    affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                                    break;
                                  }
                                break;
                              }
                            case 'S':
                            case 's':
                              {
                                if (LocaleCompare(keyword,"scale") == 0)
                                  {
                                    for (p=(char *) value; *p != '\0'; p++)
                                      if (isspace((int) (*p)) || (*p == ','))
                                        break;
                                    affine.sx=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                                    affine.sy=affine.sx;
                                    if (*p != '\0')
                                      affine.sy=
                                        GetUserSpaceCoordinateValue(svg_info,-1,p+1,MagickFalse);
                                    svg_info->scale[svg_info->n]=ExpandAffine(&affine);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"skewX") == 0)
                                  {
                                    affine.sx=svg_info->affine.sx;
                                    affine.ry=tan(DegreesToRadians(fmod(
                                                                        GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse),
                                                                        360.0)));
                                    affine.sy=svg_info->affine.sy;
                                    break;
                                  }
                                if (LocaleCompare(keyword,"skewY") == 0)
                                  {
                                    affine.sx=svg_info->affine.sx;
                                    affine.rx=tan(DegreesToRadians(fmod(
                                                                        GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse),
                                                                        360.0)));
                                    affine.sy=svg_info->affine.sy;
                                    break;
                                  }
                                break;
                              }
                            case 'T':
                            case 't':
                              {
                                if (LocaleCompare(keyword,"translate") == 0)
                                  {
                                    for (p=(char *) value; *p != '\0'; p++)
                                      if (isspace((int) (*p)) || (*p == ','))
                                        break;
                                    affine.tx=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                                    affine.ty=affine.tx;
                                    if (*p != '\0')
                                      affine.ty=
                                        GetUserSpaceCoordinateValue(svg_info,-1,p+1,MagickFalse);
                                    break;
                                  }
                                break;
                              }
                            default:
                              break;
                            } /* end switch */
                          transform.sx=current.sx*affine.sx+current.ry*affine.rx;
                          transform.rx=current.rx*affine.sx+current.sy*affine.rx;
                          transform.ry=current.sx*affine.ry+current.ry*affine.sy;
                          transform.sy=current.rx*affine.ry+current.sy*affine.sy;
                          transform.tx=current.sx*affine.tx+current.ry*affine.ty+
                            current.tx;
                          transform.ty=current.rx*affine.tx+current.sy*affine.ty+
                            current.ty;
                        } /* end for */
                      MVGPrintf(svg_info->file,"affine %g %g %g %g %g %g\n",
                                transform.sx,transform.rx,transform.ry,transform.sy,
                                transform.tx,transform.ty);
                    } /* end if */
                  if (tokens != (char **) NULL)
                    {
                      for (j=0; tokens[j] != (char *) NULL; j++)
                        MagickFreeMemory(tokens[j]);
                      MagickFreeMemory(tokens);
                    }
                  break;
                }
              if (LocaleCompare(keyword,"gradientUnits") == 0)
                {
                  (void) CloneString(&units,value);
                  MVGPrintf(svg_info->file,"gradient-units '%s'\n",value);
                  break;
                }
              break;
            }
          case 'H':
          case 'h':
            {
              if (LocaleCompare(keyword,"height") == 0)
                {
                  svg_info->bounds.height=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickTrue);
                  break;
                }
              if (LocaleCompare(keyword,"href") == 0)
                {
                  /* process "#identifier" as if it were "url(#identifier)" */
                  if  ( value[0] == '#' )
                    {
                      /* reallocate the needed memory once */
                      size_t NewSize = strlen(value) + 6;   /* 6 == url()<null> */
                      MagickReallocMemory(char *,svg_info->url,NewSize);
                      memcpy(svg_info->url,"url(",4);
                      strcpy(svg_info->url+4,value);
                      svg_info->url[NewSize-2] = ')';
                      svg_info->url[NewSize-1] = '\0';
                    }
                  else
                    (void) CloneString(&svg_info->url,value);
                  break;
                }
              break;
            }
          case 'M':
          case 'm':
            {
              if (LocaleCompare(keyword,"major") == 0)
                {
                  svg_info->element.major=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"mask") == 0)   /* added mask */
                {
                  MVGPrintf(svg_info->file,"mask '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"minor") == 0)
                {
                  svg_info->element.minor=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              break;
            }
          case 'O':
          case 'o':
            {
              if (LocaleCompare(keyword,"offset") == 0)
                {
                  (void) CloneString(&svg_info->offset,value);
                  break;
                }
              if (LocaleCompare(keyword,"opacity") == 0)
                {
                  MVGPrintf(svg_info->file,"opacity '%s'\n",value);
                  break;
                }
              break;
            }
          case 'P':
          case 'p':
            {
              if (LocaleCompare(keyword,"path") == 0)
                {
                  (void) CloneString(&svg_info->url,value);
                  break;
                }
              if (LocaleCompare(keyword,"points") == 0)
                {
                  (void) CloneString(&svg_info->vertices,value);
                  break;
                }
              break;
            }
          case 'R':
          case 'r':
            {
              if (LocaleCompare(keyword,"r") == 0)
                {
                  svg_info->element.major=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  svg_info->element.minor=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"rotate") == 0)
                {
                  double
                    angle;

                  angle=GetUserSpaceCoordinateValue(svg_info,0,value,MagickFalse);
                  /*
                    When the current text position was managed in this code, and a "rotate" was encountered
                    (indicating that the text character was to be rotated), the code would emit to the MVG file:

                      translate x y (where x, y indicate the current text position)
                      rotate angle (where angle indicates the rotation angle)

                    Now that the current text position is being managed by DrawImage() in render.c, this code
                    cannot issue the "translate" because it can't know the current text position.  To handle
                    this, "textr" (text rotation) has been implemented in DrawImage() to perform the appropriate
                    translation/rotation sequence.
                  */
                  if ( IsTextOrTSpan )
                    MVGPrintf(svg_info->file,"textr %g\n",angle);  /* pre-translation will be handled in DrawImage() */
                  else
                  {
                    MVGPrintf(svg_info->file,"translate %g,%g\n",svg_info->bounds.x,svg_info->bounds.y);
                    svg_info->bounds.x=0;
                    svg_info->bounds.y=0;
                    MVGPrintf(svg_info->file,"rotate %g\n",angle);
                  }
                  break;
                }
              if (LocaleCompare(keyword,"rx") == 0)
                {
                  if (LocaleCompare((char *) name,"ellipse") == 0)
                    svg_info->element.major=
                      GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  else
                    svg_info->radius.x=
                      GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"ry") == 0)
                {
                  if (LocaleCompare((char *) name,"ellipse") == 0)
                    svg_info->element.minor=
                      GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  else
                    svg_info->radius.y=
                      GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              break;
            }
          case 'S':
          case 's':
            {
              if (LocaleCompare(keyword,"stop-color") == 0)
                {
                  (void) CloneString(&svg_info->stop_color,value);
                  break;
                }
              if (LocaleCompare(keyword,"stroke") == 0)
                {
                  if (LocaleCompare(value,"currentColor") == 0)
                    {
                      MVGPrintf(svg_info->file,"stroke '%s'\n",color);
                      break;
                    }
                  MVGPrintf(svg_info->file,"stroke '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
                {
                  MVGPrintf(svg_info->file,"stroke-antialias %d\n",
                            LocaleCompare(value,"true") == 0);
                  break;
                }
              if (LocaleCompare(keyword,"stroke-dasharray") == 0)
                {
                  MVGPrintf(svg_info->file,"stroke-dasharray %s\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
                {
                  double dashoffset=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  MVGPrintf(svg_info->file,"stroke-dashoffset %g\n",dashoffset);
                  break;
                }
              if (LocaleCompare(keyword,"stroke-linecap") == 0)
                {
                  MVGPrintf(svg_info->file,"stroke-linecap '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"stroke-linejoin") == 0)
                {
                  MVGPrintf(svg_info->file,"stroke-linejoin '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
                {
                  double stroke_miterlimit;
                  if ((MagickAtoFChk(value,&stroke_miterlimit) != MagickPass) ||
                      stroke_miterlimit < 1.0)
                    {
                      errno=0;
                      ThrowException(svg_info->exception,DrawError,InvalidPrimitiveArgument,
                                     value);
                      break;
                    }
                  MVGPrintf(svg_info->file,"stroke-miterlimit '%ld'\n",
                            (long) stroke_miterlimit);
                  break;
                }
              if (LocaleCompare(keyword,"stroke-opacity") == 0)
                {
                  MVGPrintf(svg_info->file,"stroke-opacity '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"stroke-width") == 0)
                {
                  MVGPrintf(svg_info->file,"stroke-width %g\n",
                            GetUserSpaceCoordinateValue(svg_info,1,value,MagickTrue));
                  break;
                }
              if (LocaleCompare(keyword,"style") == 0)
                {
                  char
                    **tokens;

                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  ");
                  tokens=GetStyleTokens(context,value,&number_tokens);
                  if ((tokens != (char **) NULL) && (number_tokens > 0))
                    {
                      for (j=0; j < (number_tokens-1); j+=2)
                        {
                          keyword=(char *) tokens[j];
                          value=(char *) tokens[j+1];
                          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                                "    %.1024s: %.1024s",keyword,value);
                          switch (*keyword)
                            {
                            case 'B':
                            case 'b':
                              {
                                if (LocaleCompare(keyword,"background") == 0)
                                {
                                  /* only do this if background was specified inside <svg ... */
                                  if  ( LocaleCompare((const char *)name,"svg") == 0 )
                                    strlcpy(svg_element_background_color,value,MaxTextExtent);
                                  break;
                                }
                                break;
                              }
                            case 'C':
                            case 'c':
                              {
                                if (LocaleCompare(keyword,"clip-path") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"clip-path '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"clip-rule") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"clip-rule '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"clipPathUnits") == 0)
                                  {
                                    (void) CloneString(&units,value);
                                    MVGPrintf(svg_info->file,"clip-units '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"color") == 0)
                                  {
                                    (void) CloneString(&color,value);
                                    break;
                                  }
                                break;
                              }
                            case 'F':
                            case 'f':
                              {
                                if (LocaleCompare(keyword,"fill") == 0)
                                  {
                                    if (LocaleCompare(value,"currentColor") == 0)
                                      {
                                        MVGPrintf(svg_info->file,"fill '%s'\n",color);
                                        break;
                                      }
                                    MVGPrintf(svg_info->file,"fill '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"fillcolor") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"fill '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"fill-rule") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"fill-rule '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"fill-opacity") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"fill-opacity '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"font-family") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"font-family '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"font-stretch") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"font-stretch '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"font-style") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"font-style '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"font-size") == 0)
                                  {
                                    if (LocaleCompare(value,"medium") == 0)
                                      svg_info->pointsize=12;
                                    else
                                      svg_info->pointsize=
                                        GetUserSpaceCoordinateValue(svg_info,0,value,MagickTrue);
                                    MVGPrintf(svg_info->file,"font-size %g\n",
                                              svg_info->pointsize);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"font-weight") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"font-weight '%s'\n",value);
                                    break;
                                  }
                                break;
                              }
                            case 'O':
                            case 'o':
                              {
                                if (LocaleCompare(keyword,"offset") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"offset %g\n",
                                              GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse));
                                    break;
                                  }
                                if (LocaleCompare(keyword,"opacity") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"opacity '%s'\n",value);
                                    break;
                                  }
                                break;
                              }
                            case 'S':
                            case 's':
                              {
                                if (LocaleCompare(keyword,"stop-color") == 0)
                                  {
                                    (void) CloneString(&svg_info->stop_color,value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke") == 0)
                                  {
                                    if (LocaleCompare(value,"currentColor") == 0)
                                      {
                                        MVGPrintf(svg_info->file,"stroke '%s'\n",color);
                                        break;
                                      }
                                    MVGPrintf(svg_info->file,"stroke '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"stroke-antialias %d\n",
                                              LocaleCompare(value,"true") == 0);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke-dasharray") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"stroke-dasharray %s\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
                                  {
                                    double dashoffset=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                                    MVGPrintf(svg_info->file,"stroke-dashoffset %g\n",dashoffset);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke-linecap") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"stroke-linecap '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke-linejoin") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"stroke-linejoin '%s'\n",
                                              value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
                                  {
                                    double stroke_miterlimit;
                                    if ((MagickAtoFChk(value,&stroke_miterlimit) != MagickPass) ||
                                        stroke_miterlimit < 1.0)
                                      {
                                        errno=0;
                                        ThrowException(svg_info->exception,DrawError,InvalidPrimitiveArgument,
                                                       value);
                                        break;
                                      }
                                    MVGPrintf(svg_info->file,"stroke-miterlimit '%ld'\n",
                                              (long) stroke_miterlimit);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke-opacity") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"stroke-opacity '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"stroke-width") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"stroke-width %g\n",
                                              GetUserSpaceCoordinateValue(svg_info,1,value,MagickTrue));
                                    break;
                                  }
                                break;
                              }
                            case 't':
                            case 'T':
                              {
                                if (LocaleCompare(keyword,"text-align") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"text-align '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"text-anchor") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"text-anchor '%s'\n",value);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"text-decoration") == 0)
                                  {
                                    if (LocaleCompare(value,"underline") == 0)
                                      MVGPrintf(svg_info->file,"decorate underline\n");
                                    if (LocaleCompare(value,"line-through") == 0)
                                      MVGPrintf(svg_info->file,"decorate line-through\n");
                                    if (LocaleCompare(value,"overline") == 0)
                                      MVGPrintf(svg_info->file,"decorate overline\n");
                                    break;
                                  }
                                if (LocaleCompare(keyword,"text-antialiasing") == 0)
                                  {
                                    MVGPrintf(svg_info->file,"text-antialias %d\n",
                                              LocaleCompare(value,"true") == 0);
                                    break;
                                  }
                                if (LocaleCompare(keyword,"transform") == 0)
                                  {
                                    /* implement style="transform: translate(..." */
                                    SVGProcessTransformString(context,value);
                                    break;
                                  }
                                break;
                              }
                            default:
                              break;
                            }
                        }
                    }
                  if (tokens != (char **) NULL)
                    {
                      for (j=0; tokens[j] != (char *) NULL; j++)
                        MagickFreeMemory(tokens[j]);
                      MagickFreeMemory(tokens);
                    }
                  break;
                }
              break;
            }
          case 'T':
          case 't':
            {
              if (LocaleCompare(keyword,"text-align") == 0)
                {
                  MVGPrintf(svg_info->file,"text-align '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"text-anchor") == 0)
                {
                  MVGPrintf(svg_info->file,"text-anchor '%s'\n",value);
                  break;
                }
              if (LocaleCompare(keyword,"text-decoration") == 0)
                {
                  if (LocaleCompare(value,"underline") == 0)
                    MVGPrintf(svg_info->file,"decorate underline\n");
                  if (LocaleCompare(value,"line-through") == 0)
                    MVGPrintf(svg_info->file,"decorate line-through\n");
                  if (LocaleCompare(value,"overline") == 0)
                    MVGPrintf(svg_info->file,"decorate overline\n");
                  break;
                }
              if (LocaleCompare(keyword,"text-antialiasing") == 0)
                {
                  MVGPrintf(svg_info->file,"text-antialias %d\n",
                            LocaleCompare(value,"true") == 0);
                  break;
                }
              if (LocaleCompare(keyword,"transform") == 0)
                {
                  /*
                    The code that was here has been refactored into
                    function SVGProcessTransformString()
                  */
                  SVGProcessTransformString(context,value);
                  break;
                }
              break;
            }
          case 'V':
          case 'v':
            {
              if (LocaleCompare(keyword,"verts") == 0)
                {
                  (void) CloneString(&svg_info->vertices,value);
                  break;
                }
              if (LocaleCompare(keyword,"viewBox") == 0)
                {
                  p=(char *) value;
                  (void) MagickGetToken(p,&p,token,MaxTextExtent);
                  svg_info->view_box.x=MagickAtoF(token);
                  (void) MagickGetToken(p,&p,token,MaxTextExtent);
                  if (*token == ',')
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                  svg_info->view_box.y=MagickAtoF(token);
                  (void) MagickGetToken(p,&p,token,MaxTextExtent);
                  if (*token == ',')
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                  svg_info->view_box.width=MagickAtoF(token);
                  (void) MagickGetToken(p,&p,token,MaxTextExtent);
                  if (*token == ',')
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                  svg_info->view_box.height=MagickAtoF(token);
                  if (svg_info->view_box.width < 0.0 ||
                      svg_info->view_box.height < 0.0)
                    {
                      ThrowException(svg_info->exception,CorruptImageError,
                                     NegativeOrZeroImageSize,
                                     svg_info->image->filename);
                      goto svg_start_element_error;
                    }
                  if (svg_info->bounds.width == 0)
                    svg_info->bounds.width=svg_info->view_box.width;
                  if (svg_info->bounds.height == 0)
                    svg_info->bounds.height=svg_info->view_box.height;
                  break;
                }
              break;
            }
          case 'W':
          case 'w':
            {
              if (LocaleCompare(keyword,"width") == 0)
                {
                  svg_info->bounds.width=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickTrue);
                  break;
                }
              break;
            }
          case 'X':
          case 'x':
            {
              if (LocaleCompare(keyword,"x") == 0)
                {
                  svg_info->bounds.x=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  /* update text current position for tspan (already did this if text) */
                  if  ( IsTSpan )
                    MVGPrintf(svg_info->file,"textx %g\n",svg_info->bounds.x);
                  break;
                }
              if (LocaleCompare(keyword,"xlink:href") == 0)
                {
                  /* process "#identifier" as if it were "url(#identifier)" */
                  if  ( value[0] == '#' )
                    {
                      /* reallocate the needed memory once */
                      size_t NewSize = strlen(value) + 6;   /* 6 == url()<null> */
                      MagickReallocMemory(char *,svg_info->url,NewSize);
                      memcpy(svg_info->url,"url(",4);
                      strcpy(svg_info->url+4,value);
                      svg_info->url[NewSize-2] = ')';
                      svg_info->url[NewSize-1] = '\0';
                    }
                  else
                    (void) CloneString(&svg_info->url,value);
                  break;
                }
              if (LocaleCompare(keyword,"x1") == 0)
                {
                  svg_info->segment.x1=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"x2") == 0)
                {
                  svg_info->segment.x2=
                    GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                  break;
                }
              break;
            }
          case 'Y':
          case 'y':
            {
              if (LocaleCompare(keyword,"y") == 0)
                {
                  svg_info->bounds.y=GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  /* update text current position for tspan (already did this if text) */
                  if  ( IsTSpan )
                    MVGPrintf(svg_info->file,"texty %g\n",svg_info->bounds.y);
                  break;
                }
              if (LocaleCompare(keyword,"y1") == 0)
                {
                  svg_info->segment.y1=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              if (LocaleCompare(keyword,"y2") == 0)
                {
                  svg_info->segment.y2=
                    GetUserSpaceCoordinateValue(svg_info,-1,value,MagickFalse);
                  break;
                }
              break;
            }
          default:
            break;
          }
      }
  if (svg_info->exception->severity >= ErrorException)
    goto svg_start_element_error;
#ifdef BROKEN_SIZE_OPTION
  if (LocaleCompare((char *) name,"svg") == 0)
    {
      if (svg_info->document->encoding != (const xmlChar *) NULL)
        MVGPrintf(svg_info->file,"encoding %.1024s\n",
                  (char *) svg_info->document->encoding);
      if (attributes != (const xmlChar **) NULL)
        {
          double
            sx,
            sy;

          if ((svg_info->view_box.width == 0.0) ||
              (svg_info->view_box.height == 0.0))
            svg_info->view_box=svg_info->bounds;
          svg_info->width=(unsigned long) svg_info->bounds.width;
          svg_info->height=(unsigned long) svg_info->bounds.height;
          MVGPrintf(svg_info->file,"viewbox 0 0 %lu %lu\n",svg_info->width,
                    svg_info->height);
          /*
            Set initial canvas background color to user specified value
          */
          {
            char
              tuple[MaxTextExtent];

            GetColorTuple(&svg_info->image_info->background_color,8,True,True,
                          tuple);

            MVGPrintf(svg_info->file,"push graphic-context\n");
            MVGPrintf(svg_info->file,"fill %s\n", tuple);
            MVGPrintf(svg_info->file,"rectangle 0,0 %g,%g\n",
                      svg_info->view_box.width,svg_info->view_box.height);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
          }
          sx=(double) svg_info->width/svg_info->view_box.width;
          sy=(double) svg_info->height/svg_info->view_box.height;
          MVGPrintf(svg_info->file,"affine %g 0 0 %g %g %g\n",sx,sy,
                    -sx*svg_info->view_box.x,-sy*svg_info->view_box.y);
        }
    }
#else
  if (LocaleCompare((char *) name,"svg") == 0)
    {
      if (svg_info->document->encoding != (const xmlChar *) NULL)
        (void) fprintf(svg_info->file,"encoding %.1024s\n",
                       (char *) svg_info->document->encoding);
      if (attributes != (const xmlChar **) NULL)
        {
          char
            *geometry;

          RectangleInfo
            page;

          double
            sx,
            sy;

          if (svg_info->bounds.width < 0.0 || svg_info->bounds.height < 0.0)
            {
              ThrowException(svg_info->exception,CorruptImageError,
                             NegativeOrZeroImageSize,(char *) NULL);
              goto svg_start_element_error;
            }
          if ((svg_info->view_box.width == 0.0) ||
              (svg_info->view_box.height == 0.0))
            svg_info->view_box=svg_info->bounds;
          if (svg_info->view_box.width < 0.0 || svg_info->view_box.height < 0.0)
            {
              ThrowException(svg_info->exception,CorruptImageError,
                             NegativeOrZeroImageSize,(char *) NULL);
              goto svg_start_element_error;
            }
          SetGeometry(svg_info->image,&page);
          page.width=(unsigned long) svg_info->bounds.width;
          page.height=(unsigned long) svg_info->bounds.height;
          geometry=(char *) NULL;
          /* at one point we use to try to use either page geometry
             or size to set the dimensions of the output page, but
             now we only look for size
          */
#ifdef PARSE_PAGE_FIRST
          if (svg_info->page != (char *) NULL)
            geometry=GetPageGeometry(svg_info->page);
          else
#endif
            if (svg_info->size != (char *) NULL)
              geometry=GetPageGeometry(svg_info->size);
          if (geometry != (char *) NULL)
            {
              /*
                A terminating '>' in a geometry string is interpreted to mean that
                the dimensions of an image should only be changed if its width or
                height exceeds the geometry specification.  For an unapparent and
                undocumented reason, a terminating '>', if present, was being nulled
                out, making this feature unusable for SVG files (now fixed).
              */
              (void) GetMagickGeometry(geometry,&page.x,&page.y,
                                       &page.width,&page.height);
              MagickFreeMemory(geometry);
            }
          /* NOTE: the scale factor returned by ExpandAffine() has already been applied
             to page.width and page.height
          */
          (void) MVGPrintf(svg_info->file,"viewbox 0 0 %g %g\n",
                           svg_info->view_box.width,svg_info->view_box.height);
          sx=(double) page.width/svg_info->view_box.width;
          sy=(double) page.height/svg_info->view_box.height;
          MVGPrintf(svg_info->file,"affine %g 0 0 %g %g %g\n",sx,sy,
                    -sx*svg_info->view_box.x,-sy*svg_info->view_box.y);
          /* only set the output width and height if this is the outermost <svg> */
          if  ( svg_info->svgPushCount == 1 )
            {/*outermost <svg>*/
              svg_info->width=page.width;
              svg_info->height=page.height;
              /* check if background color was specified using <svg ... style="background:color" */
              if  ( svg_element_background_color[0] != '\0' )
                {
                  MVGPrintf(svg_info->file,"push graphic-context\n");
                  MVGPrintf(svg_info->file,"fill %s\n",svg_element_background_color);
                  MVGPrintf(svg_info->file,"rectangle 0,0 %g,%g\n",svg_info->view_box.width,svg_info->view_box.height);
                  MVGPrintf(svg_info->file,"pop graphic-context\n");
                }
            }/*outermost <svg>*/
        }
    }
#endif
  /* Error dispatch point */
 svg_start_element_error:;

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  )");
  MagickFreeMemory(units);
  MagickFreeMemory(color);
}

/* Process the "class" definitions inside <style> ... </style> */
static
void    ProcessStyleClassDefs (
  SVGInfo * svg_info
  )
{/*ProcessStyleClassDefs*/

  /*
    Style defs look like:

      .class1,.class2,.class3{kwd:val;kwd:val;}

    Class name (e.g., ".class1"} can show up multiple times
  */

  /* keyword/value pair for an element */
  typedef struct ElementValue
    {/*ElementValue*/
        struct ElementValue     * pNext;  /* next in list */
        char *                pKeyword;
        char *                pValue;
    }/*ElementValue*/
  ElementValue;

  /* the keyword/value pair list associated with a class */
  typedef struct ClassDef
    {/*ClassDef*/
      struct ClassDef * pNext;            /* next in list */
      struct ClassDef * pActiveNext;      /* next in active list */
      char *            pName;            /* class name */
      ElementValue      ElementValueHead; /* list head for style element/value pairs */
      ElementValue *    pElementValueLast;
    }/*ClassDef*/
  ClassDef;

  ClassDef ClassDefHead;  /* list head for classes */
  ClassDef * pClassDefLast = &ClassDefHead; /* tail ptr for class def list */
  ClassDef ClassDefActiveHead;  /* list head for "active" class defs */
  ClassDef * pClassDefActiveLast = &ClassDefActiveHead; /* tail ptr for "active" class def list */
  ClassDef * pClassDef;
  char * pCopyOfText;
  char * pString;
  char * cp,*cp2;
  int c;

  memset(&ClassDefHead,0,sizeof(ClassDefHead));
  memset(&ClassDefActiveHead,0,sizeof(ClassDefActiveHead));

  /* a macro to allocate an zero out a new struct instance,
      and add it to the end of a linked list */
  #define       ADD_NEW_STRUCT(pNew,pLast,TheTypeDef) \
  pNew = MagickAllocateMemory(TheTypeDef *,sizeof(TheTypeDef)); \
  memset(pNew,0,sizeof(TheTypeDef)); \
  pLast = pLast->pNext = pNew

  /* we will get a modifiable value of the string, and delimit
      substrings by storing null characters */
  pCopyOfText = AcquireString(svg_info->text);
  for  ( pString = pCopyOfText; *pString; )
    {/*pString loop*/
      char * pClassNameList;
      char * pStyleElementList;

      while  ( (c = *pString) && isspace(c) )  pString++;   /* skip white space */
      if  ( !*pString )  break;   /* found end of string; done */
      pClassNameList = pString;

      /* find the end of the comma-separated list of class names */
      while  ( (c = *pString) && (c != '{') )  pString++;
      if  ( !*pString )
        {
          /* malformed input: class name list not followed by '{' */
          MagickFreeMemory(pCopyOfText);
          return;
        }
      *pString++ = '\0';
      pStyleElementList = pString;

      /* extract the class names */
      ClassDefActiveHead.pActiveNext = 0;
      pClassDefActiveLast = &ClassDefActiveHead;  /* initially, no active class defs */
      for  ( cp = pClassNameList; *cp; )
        {/*extract class name loop*/

          while  ( (c = *cp) && (isspace(c) || (c ==',')) )  cp++;  /* skip white space/commas */
          if  ( *cp == '.' )  cp++;     /* .classname, skip leading period */
          if  ( *cp )
          {/*found class name*/

              char * pClassName = cp;
              while  ( (c = *cp) && !(isspace(c) || (c == ',')) )  cp++;  /* find white space/comma/null */
              if  ( *cp )
                *cp++ = '\0';   /* terminate identifier string and increment */
              /* add uniquely to list */
              for  (  pClassDef = ClassDefHead.pNext;
                      pClassDef && (strcmp(pClassName,pClassDef->pName) != 0);
                      pClassDef = pClassDef->pNext );
              if  ( pClassDef == 0 )
                {/*new class name*/
                  ADD_NEW_STRUCT(pClassDef,pClassDefLast,ClassDef);
                  pClassDef->pElementValueLast = &pClassDef->ElementValueHead;
                  pClassDef->pName = pClassName;
                }/*new class name*/
              pClassDefActiveLast = pClassDefActiveLast->pActiveNext = pClassDef;   /* add to active list */

              }/*found class name*/

        }/*extract class name loop*/

      /* find the end of the style elements */
      while  ( (c = *pString) && (c != '}') )  pString++;
      if  ( !*pString )
        {
          /* malformed input: style elements not terminated by '{' */
          MagickFreeMemory(pCopyOfText);
          return;
        }
      *pString++ = '\0';  /* advance past '}' for next loop pass */

      /* get the style elements */
      for  ( cp = pStyleElementList; *cp; )
        {/*extract style elements loop*/

          /* looking for <space><classname><space>: */
          while  ( (c = *cp) && isspace(c) )  cp++;   /* skip white space */
          if  ( *cp )
            {/*found style element*/

              char * pStyleElement = cp;
              while  ( (c = *cp) && (c != ':') )  cp++;   /* find colon/null */
              for  ( cp2 = cp-1; isspace(*cp2); *cp2-- = '\0');   /* trim white space */
              if  ( *cp )
                *cp++ = '\0';   /* terminate style element string and increment */

              /* looking for <space><style-value>; */
              while  ( (c = *cp) && isspace(c) )  cp++;   /* skip white space */
              if  ( *cp )
                {/*found style element value*/

                  char * pStyleValue = cp;
                  while  ( (c = *cp) && (c != ';') )  cp++;   /* find semi-colon/null */
                  for  ( cp2 = cp-1; isspace(*cp2); *cp2-- = '\0');   /* trim white space */
                  if  ( *cp )
                    *cp++ = '\0';   /* terminate style value string and increment */

                  /* add style element/value pair to each active class def */
                  for  ( pClassDef = ClassDefActiveHead.pActiveNext; pClassDef; pClassDef = pClassDef->pActiveNext )
                    {
                      ElementValue * pEV;
                      ADD_NEW_STRUCT(pEV,pClassDef->pElementValueLast,ElementValue);
                      pEV->pKeyword = pStyleElement;
                      pEV->pValue = pStyleValue;
                    }

                }/*found style element value*/

            }/*found style element*/

        }/*extract style elements loop*/

    }/*pString loop*/

  /* emit class definitions */
  for  ( pClassDef = ClassDefHead.pNext; pClassDef; pClassDef = pClassDef->pNext )
    {/*pClassDef loop*/

      ElementValue * pEV;
      MVGPrintf(svg_info->file,"push class '%s'\n",pClassDef->pName);
      /* NOTE: we allow class definitions that are empty */
      for  ( pEV = pClassDef->ElementValueHead.pNext; pEV; pEV = pEV->pNext )
        {/*pEV loop*/

          char * keyword = pEV->pKeyword;
          char * value = pEV->pValue;
          /* switch below was copied/pasted from elsewhere and modified */
          switch (*keyword)
            {/*keyword*/

            case 'C':
            case 'c':
              {/*Cc*/
                if (LocaleCompare(keyword,"clip-path") == 0)
                  {
                    MVGPrintf(svg_info->file,"clip-path '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"clip-rule") == 0)
                  {
                    MVGPrintf(svg_info->file,"clip-rule '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"clipPathUnits") == 0)
                  {
                    MVGPrintf(svg_info->file,"clip-units '%s'\n",value);
                    break;
                  }
                break;
              }/*Cc*/

            case 'F':
            case 'f':
               {/*Ff*/
                if  ( (LocaleCompare(keyword,"fill") == 0) || (LocaleCompare(keyword,"fillcolor") == 0) )
                  {
                    MVGPrintf(svg_info->file,"fill '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"fill-rule") == 0)
                  {
                    MVGPrintf(svg_info->file,"fill-rule '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"fill-opacity") == 0)
                  {
                    MVGPrintf(svg_info->file,"fill-opacity '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"font-family") == 0)
                  {
                    /*
                      Deal with Adobe Illustrator 10.0 which double-quotes
                      font-family.  Maybe we need a generalized solution for
                      this.
                    */
                    size_t n;
                    if  ( (value[0] == '\'') && (value[(n = (strlen(value)-1))] == '\'') )
                      {
                        size_t i;
                        FILE * fp = svg_info->file;
                        MVGPrintf(fp,"font-family '");
                        for(i = 1; i < n; i++)
                          fputc(value[i],fp);
                        MVGPrintf(fp,"'\n");
                      }
                    else
                      MVGPrintf(svg_info->file,"font-family '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"font-stretch") == 0)
                  {
                    MVGPrintf(svg_info->file,"font-stretch '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"font-style") == 0)
                  {
                    MVGPrintf(svg_info->file,"font-style '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"font-size") == 0)
                  {
                    if (LocaleCompare(value,"medium") == 0)
                      svg_info->pointsize = 12;
                    else
                      svg_info->pointsize = GetUserSpaceCoordinateValue(svg_info,0,value,MagickTrue);
                    MVGPrintf(svg_info->file,"font-size %g\n",svg_info->pointsize);
                    break;
                  }
                if (LocaleCompare(keyword,"font-weight") == 0)
                  {
                    MVGPrintf(svg_info->file,"font-weight '%s'\n",value);
                    break;
                  }
                break;
              }/*Ff*/

            case 'O':
            case 'o':
              {/*Oo*/
                if (LocaleCompare(keyword,"offset") == 0)
                  {
                   MVGPrintf(svg_info->file,"offset %g\n",GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse));
                   break;
                  }
                if (LocaleCompare(keyword,"opacity") == 0)
                  {
                    MVGPrintf(svg_info->file,"opacity '%s'\n",value);
                    break;
                  }
                break;
              }/*Oo*/

            case 'S':
            case 's':
              {/*Ss*/
                if (LocaleCompare(keyword,"stop-color") == 0)
                  {
                    (void) CloneString(&svg_info->stop_color,value);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke") == 0)
                  {
                    MVGPrintf(svg_info->file,"stroke '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
                  {
                    MVGPrintf(svg_info->file,"stroke-antialias %d\n",LocaleCompare(value,"true") == 0);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke-dasharray") == 0)
                  {
                    MVGPrintf(svg_info->file,"stroke-dasharray %s\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
                  {
                    double dashoffset=GetUserSpaceCoordinateValue(svg_info,1,value,MagickFalse);
                    MVGPrintf(svg_info->file,"stroke-dashoffset %g\n",dashoffset);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke-linecap") == 0)
                  {
                    MVGPrintf(svg_info->file,"stroke-linecap '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke-linejoin") == 0)
                  {
                    MVGPrintf(svg_info->file,"stroke-linejoin '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
                  {
                    double stroke_miterlimit;
                    if ((MagickAtoFChk(value,&stroke_miterlimit) != MagickPass) || stroke_miterlimit < 1.0)
                      {
                        errno=0;
                        ThrowException(svg_info->exception,DrawError,InvalidPrimitiveArgument,value);
                        break;
                      }
                    MVGPrintf(svg_info->file,"stroke-miterlimit '%ld'\n",(long) stroke_miterlimit);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke-opacity") == 0)
                  {
                    MVGPrintf(svg_info->file,"stroke-opacity '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"stroke-width") == 0)
                  {
                    MVGPrintf(svg_info->file,"stroke-width %g\n",GetUserSpaceCoordinateValue(svg_info,1,value,MagickTrue));
                    break;
                  }
                break;
              }/*Ss*/

            case 'T':
            case 't':
              {/*Tt*/
                if (LocaleCompare(keyword,"text-align") == 0)
                  {
                    MVGPrintf(svg_info->file,"text-align '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"text-anchor") == 0)
                  {
                    MVGPrintf(svg_info->file,"text-anchor '%s'\n",value);
                    break;
                  }
                if (LocaleCompare(keyword,"text-decoration") == 0)
                  {
                    if (LocaleCompare(value,"underline") == 0)
                      MVGPrintf(svg_info->file,"decorate underline\n");
                    if (LocaleCompare(value,"line-through") == 0)
                      MVGPrintf(svg_info->file,"decorate line-through\n");
                    if (LocaleCompare(value,"overline") == 0)
                      MVGPrintf(svg_info->file,"decorate overline\n");
                    break;
                  }
                if (LocaleCompare(keyword,"text-antialiasing") == 0)
                  {
                    MVGPrintf(svg_info->file,"text-antialias %d\n",LocaleCompare(value,"true") == 0);
                    break;
                  }
                if (LocaleCompare(keyword,"transform") == 0)
                  {/*style="transform: ...*/
                    SVGProcessTransformString((void *)svg_info,value);
                    break;
                  }/*style="transform: ...*/
                break;
              }/*Tt*/

            default:
            break;

            }/*keyword*/

        }/*pEV loop*/

      MVGPrintf(svg_info->file,"pop class\n");

    }/*pClassDef loop*/

  /* clean up */
  {
    ClassDef * pClassDef;
    for(pClassDef = ClassDefHead.pNext; pClassDef; )
      {
        ElementValue *pEV;
        ClassDef * pClassDefTemp = pClassDef;
        for(pEV = pClassDef->ElementValueHead.pNext; pEV; )
          {
            ElementValue * pEVTemp = pEV;
            pEV = pEV->pNext;
            MagickFreeMemory(pEVTemp);
          }
        pClassDef = pClassDef->pNext;
        MagickFreeMemory(pClassDefTemp);
      }
  }
  MagickFreeMemory(pCopyOfText);

#undef  ADD_NEW_STRUCT
}/*ProcessStyleClassDefs*/

static void
SVGEndElement(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Called when the end of an element has been detected.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.endElement(%.1024s)",name);
  svg_info=(SVGInfo *) context;
  if (strchr((char *) name,':') != (char *) NULL)
    {
      /*
        Skip over namespace.
      */
      for ( ; *name != ':'; name++) ;
      name++;
    }
  switch (*name)
    {
    case 'C':
    case 'c':
      {
        if (LocaleCompare((char *) name,"circle") == 0)
          {
            MVGPrintf(svg_info->file,"circle %g,%g %g,%g\n",svg_info->element.cx,
                      svg_info->element.cy,svg_info->element.cx,svg_info->element.cy+
                      svg_info->element.minor);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        if (LocaleCompare((char *) name,"clipPath") == 0)
          {
            MVGPrintf(svg_info->file,"pop clip-path\n");
            break;
          }
        break;
      }
    case 'D':
    case 'd':
      {
        if (LocaleCompare((char *) name,"defs") == 0)
          {
            svg_info->defsPushCount--;
            MVGPrintf(svg_info->file,"pop defs\n");
            break;
          }
        if (LocaleCompare((char *) name,"desc") == 0)
          {
            register char
              *p;

            Strip(svg_info->text);
            if (*svg_info->text == '\0')
              break;
            (void) fputc('#',svg_info->file);
            for (p=svg_info->text; *p != '\0'; p++)
              {
                (void) fputc(*p,svg_info->file);
                if (*p == '\n')
                  (void) fputc('#',svg_info->file);
              }
            (void) fputc('\n',svg_info->file);
            *svg_info->text='\0';
            break;
          }
        break;
      }
    case 'E':
    case 'e':
      {
        if (LocaleCompare((char *) name,"ellipse") == 0)
          {
            double
              angle;

            angle=svg_info->element.angle;
            MVGPrintf(svg_info->file,"ellipse %g,%g %g,%g 0,360\n",
                      svg_info->element.cx,svg_info->element.cy,
                      angle == 0.0 ? svg_info->element.major : svg_info->element.minor,
                      angle == 0.0 ? svg_info->element.minor : svg_info->element.major);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        break;
      }
    case 'F':
    case 'f':
      {
        /*
          For now we are ignoring "foreignObject".  However, we do a push/pop
          graphic-context so that any settings (e.g., fill) are consumed and
          discarded.  Otherwise they might persist and "leak" into the graphic
          elements that follow.
        */
        if (LocaleCompare((char *) name,"foreignObject") == 0)
          {
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            break;
          }
        break;
      }
    case 'G':
    case 'g':
      {
        if (LocaleCompare((char *) name,"g") == 0)
          {
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        break;
      }
    case 'I':
    case 'i':
      {
        if (LocaleCompare((char *) name,"image") == 0)
          {
            MVGPrintf(svg_info->file,"image Copy %g,%g %g,%g '%s'\n",
                      svg_info->bounds.x,svg_info->bounds.y,svg_info->bounds.width,
                      svg_info->bounds.height,svg_info->url);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            break;
          }
        break;
      }
    case 'L':
    case 'l':
      {
        if (LocaleCompare((char *) name,"line") == 0)
          {
            MVGPrintf(svg_info->file,"line %g,%g %g,%g\n",svg_info->segment.x1,
                      svg_info->segment.y1,svg_info->segment.x2,svg_info->segment.y2);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        if (LocaleCompare((char *) name,"linearGradient") == 0)
          {
            MVGPrintf(svg_info->file,"pop gradient\n");
            break;
          }
        break;
      }
    case 'M':
    case 'm':
      {
        if (LocaleCompare((char *) name,"mask") == 0)   /* added mask */
          {
            MVGPrintf(svg_info->file,"pop mask\n");
            break;
          }
        break;
      }
    case 'P':
    case 'p':
      {
        if (LocaleCompare((char *) name,"pattern") == 0)
          {
            MVGPrintf(svg_info->file,"pop pattern\n");
            break;
          }
        if (LocaleCompare((char *) name,"path") == 0)
          {
            MVGPrintf(svg_info->file,"path '%s'\n",svg_info->vertices);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        if (LocaleCompare((char *) name,"polygon") == 0)
          {
            MVGPrintf(svg_info->file,"polygon %s\n",svg_info->vertices);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        if (LocaleCompare((char *) name,"polyline") == 0)
          {
            MVGPrintf(svg_info->file,"polyline %s\n",svg_info->vertices);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        break;
      }
    case 'R':
    case 'r':
      {
        if (LocaleCompare((char *) name,"radialGradient") == 0)
          {
            MVGPrintf(svg_info->file,"pop gradient\n");
            break;
          }
        if (LocaleCompare((char *) name,"rect") == 0)
          {
            if ((svg_info->radius.x == 0.0) && (svg_info->radius.y == 0.0))
              {
                MVGPrintf(svg_info->file,"rectangle %g,%g %g,%g\n",
                          svg_info->bounds.x,svg_info->bounds.y,
                          svg_info->bounds.x+svg_info->bounds.width,
                          svg_info->bounds.y+svg_info->bounds.height);
                MVGPrintf(svg_info->file,"pop graphic-context\n");
              if  ( svg_info->idLevelInsideDefs == svg_info->n )        /* emit a "pop id" if warranted */
                {
                  svg_info->idLevelInsideDefs = 0;
                  MVGPrintf(svg_info->file,"pop id\n");
                }
                break;
              }
            if (svg_info->radius.x == 0.0)
              svg_info->radius.x=svg_info->radius.y;
            if (svg_info->radius.y == 0.0)
              svg_info->radius.y=svg_info->radius.x;
            MVGPrintf(svg_info->file,"roundRectangle %g,%g %g,%g %g,%g\n",
                      svg_info->bounds.x,svg_info->bounds.y,svg_info->bounds.x+
                      svg_info->bounds.width,svg_info->bounds.y+svg_info->bounds.height,
                      svg_info->radius.x,svg_info->radius.y);
            svg_info->radius.x=0.0;
            svg_info->radius.y=0.0;
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        break;
      }
    case 'S':
    case 's':
      {
        if (LocaleCompare((char *) name,"stop") == 0)
          {
            MVGPrintf(svg_info->file,"stop-color '%s' %s\n",svg_info->stop_color,
                      svg_info->offset);
            break;
          }
        /* element "style" inside <defs> */
        if (LocaleCompare((char *) name,"style") == 0)
          {
            /* the style definitions are in svg_info->text */
            ProcessStyleClassDefs(svg_info);
            break;
          }
        if (LocaleCompare((char *) name,"svg") == 0)
          {
            svg_info->svgPushCount--;
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            break;
          }
        break;
      }
    case 'T':
    case 't':
      {
        if (LocaleCompare((char *) name,"text") == 0)
          {
            Strip(svg_info->text);
            if (*svg_info->text != '\0')
              {
                char
                  *text;

                text=EscapeString(svg_info->text,'\'');
                MVGPrintf(svg_info->file,"textc '%s'\n",text);  /* write text at current position */
                MagickFreeMemory(text);
                *svg_info->text='\0';
              }
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            if  ( svg_info->idLevelInsideDefs == svg_info->n )  /* emit a "pop id" if warranted */
              {
                svg_info->idLevelInsideDefs = 0;
                MVGPrintf(svg_info->file,"pop id\n");
              }
            break;
          }
        if (LocaleCompare((char *) name,"tspan") == 0)
          {
            Strip(svg_info->text);
            if (*svg_info->text != '\0')
              {
                char
                  *text;

                text=EscapeString(svg_info->text,'\'');
                MVGPrintf(svg_info->file,"textc '%s'\n",text);  /* write text at current position */
                MagickFreeMemory(text);
                /*
                  The code that used to be here to compute the next text position has been eliminated.
                  The reason is that at this point in the code we may not know the font or font size
                  (they may be hidden in a "class" definition), so we can't really do that computation.
                  This functionality is now handled by DrawImage() in render.c.
                */
                *svg_info->text='\0';
              }
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            break;
          }
        if (LocaleCompare((char *) name,"title") == 0)
          {
            Strip(svg_info->text);
            if (*svg_info->text == '\0')
              break;
            (void) CloneString(&svg_info->title,svg_info->text);
            *svg_info->text='\0';
            break;
          }
        break;
      }
    case 'U':
    case 'u':
      {
        if (LocaleCompare((char *) name,"use") == 0)
          {
            /*
              If the "use" had a "transform" attribute it has already been output to the MVG file.

              According to the SVG spec for "use":

              In the generated content, the 'use' will be replaced by 'g', where all attributes
              from the 'use' element except for 'x', 'y', 'width', 'height' and 'xlink:href' are
              transferred to the generated 'g' element. An additional transformation translate(x,y)
              is appended to the end (i.e., right-side) of the 'transform' attribute on the generated
              'g', where x and y represent the values of the 'x' and 'y' attributes on the 'use'
              element. The referenced object and its contents are deep-cloned into the generated tree.
            */
            if  ( (svg_info->bounds.x != 0.0) || (svg_info->bounds.y != 0.0) )
              MVGPrintf(svg_info->file,"translate %g,%g\n",svg_info->bounds.x,svg_info->bounds.y);

            /* NOTE: not implementing "width" and "height" for now */

            MVGPrintf(svg_info->file,"use '%s'\n",svg_info->url);
            MVGPrintf(svg_info->file,"pop graphic-context\n");
            break;
          }
        break;
      }
    default:
      break;
    }
  (void) memset(&svg_info->segment,0,sizeof(svg_info->segment));
  (void) memset(&svg_info->element,0,sizeof(svg_info->element));
  *svg_info->text='\0';
  svg_info->n--;
}

static void
SVGCharacters(void *context,const xmlChar *c,int length)
{
  register char
    *p;

  register size_t
    i;

  SVGInfo
    *svg_info;

  /*
    Receiving some characters from the parser.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.characters(%.1024s,%d)",c,length);
  svg_info=(SVGInfo *) context;
  if (svg_info->text != (char *) NULL)
    {
      MagickReallocMemory(char *,svg_info->text,strlen(svg_info->text)+length+1);
    }
  else
    {
      svg_info->text=MagickAllocateMemory(char *,(size_t) length+1);
      if (svg_info->text != (char *) NULL)
        *svg_info->text='\0';
    }
  if (svg_info->text == (char *) NULL)
    return;
  p=svg_info->text+strlen(svg_info->text);
  for (i=0; i < (size_t) length; i++)
    *p++=c[i];
  *p='\0';
}

static void
SVGReference(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when an entity reference is detected.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.reference(%.1024s)",name);
  svg_info=(SVGInfo *) context;
  parser=svg_info->parser;
  if (*name == '#')
    (void) xmlAddChild(parser->node,xmlNewCharRef(svg_info->document,name));
  else
    (void) xmlAddChild(parser->node,xmlNewReference(svg_info->document,name));
}

static void
SVGIgnorableWhitespace(void *context,const xmlChar *c,int length)
{
  /*   SVGInfo */
  /*     *svg_info; */

  ARG_NOT_USED(context);
  /*
    Receiving some ignorable whitespaces from the parser.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.ignorableWhitespace(%.30s, %d)",c,length);
  /*   svg_info=(SVGInfo *) context; */
}

static void
SVGProcessingInstructions(void *context,const xmlChar *target,
                                      const xmlChar *data)
{
  /*   SVGInfo */
  /*     *svg_info; */

  ARG_NOT_USED(context);
  /*
    A processing instruction has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.processingInstruction(%.1024s, %.1024s)",
                        target,data);
  /*   svg_info=(SVGInfo *) context; */
}

static void
SVGComment(void *context,const xmlChar *value)
{
  SVGInfo
    *svg_info;

  /*
    A comment has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.comment(%.1024s)",value);
  svg_info=(SVGInfo *) context;
  if (svg_info->comment != (char *) NULL)
    (void) ConcatenateString(&svg_info->comment,"\n");
  (void) ConcatenateString(&svg_info->comment,(char *) value);
}

static void
SVGWarning(void *context,const char *format,...)
{
  char
    reason[MaxTextExtent];

  SVGInfo
    *svg_info;

  va_list
    operands;

  /**
     Display and format a warning messages, gives file, line, position and
     extra parameters.
  */
  va_start(operands,format);
  svg_info=(SVGInfo *) context;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.warning: ");
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),format,operands);
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  ThrowException2(svg_info->exception,CoderWarning,reason,(char *) NULL);
  va_end(operands);
}

static void
SVGError(void *context,const char *format,...)
{
  char
    reason[MaxTextExtent];

  SVGInfo
    *svg_info;

  va_list
    operands;

  /*
    Display and format a error formats, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  svg_info=(SVGInfo *) context;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.error: ");
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),format,operands);
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  ThrowException2(svg_info->exception,CoderError,reason,(char *) NULL);
  va_end(operands);
}

static void
SVGCDataBlock(void *context,const xmlChar *value,int length)
{
  SVGInfo
    *svg_info;

  xmlNodePtr
    child;

  xmlParserCtxtPtr
    parser;

  /*
    Called when a pcdata block has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.pcdata(%.1024s, %d)",value,length);
  svg_info=(SVGInfo *) context;
  parser=svg_info->parser;
  child=xmlGetLastChild(parser->node);
  if ((child != (xmlNodePtr) NULL) && (child->type == XML_CDATA_SECTION_NODE))
    {
      (void) xmlTextConcat(child,value,length);
      return;
    }
  (void) xmlAddChild(parser->node,xmlNewCDataBlock(parser->myDoc,value,length));
}

static void
SVGExternalSubset(void *context,const xmlChar *name,
                  const xmlChar *external_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserCtxt
    parser_context;

  xmlParserCtxtPtr
    parser;

  xmlParserInputPtr
    input;

  /*
    Does this document has an external subset?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  SAX.externalSubset(%.1024s, %.1024s, %.1024s)",name,
                        (external_id != (const xmlChar *) NULL ? (char *) external_id : "none"),
                        (system_id != (const xmlChar *) NULL ? (char *) system_id : "none"));
  svg_info=(SVGInfo *) context;
  parser=svg_info->parser;
  if (((external_id == NULL) && (system_id == NULL)) ||
      (!parser->validate || !parser->wellFormed || !svg_info->document))
    return;
  input=SVGResolveEntity(context,external_id,system_id);
  if (input == NULL)
    return;
  (void) xmlNewDtd(svg_info->document,name,external_id,system_id);
  parser_context=(*parser);
  parser->inputTab=(xmlParserInputPtr *) xmlMalloc(5*sizeof(*parser->inputTab));
  if (parser->inputTab == (xmlParserInputPtr *) NULL)
    {
      parser->errNo=XML_ERR_NO_MEMORY;
      parser->input=parser_context.input;
      parser->inputNr=parser_context.inputNr;
      parser->inputMax=parser_context.inputMax;
      parser->inputTab=parser_context.inputTab;
      return;
    }
  parser->inputNr=0;
  parser->inputMax=5;
  parser->input=NULL;
  xmlPushInput(parser,input);
  (void) xmlSwitchEncoding(parser,xmlDetectCharEncoding(parser->input->cur,4));
  if (input->filename == (char *) NULL)
    input->filename=(char *) xmlStrdup(system_id);
  input->line=1;
  input->col=1;
  input->base=parser->input->cur;
  input->cur=parser->input->cur;
  input->free=NULL;
  xmlParseExternalSubset(parser,external_id,system_id);
  while (parser->inputNr > 1)
    (void) xmlPopInput(parser);
  xmlFreeInputStream(parser->input);
  xmlFree(parser->inputTab);
  parser->input=parser_context.input;
  parser->inputNr=parser_context.inputNr;
  parser->inputMax=parser_context.inputMax;
  parser->inputTab=parser_context.inputTab;
}

static Image *
ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  xmlSAXHandler
    SAXModules;

  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    message[MaxTextExtent];

  FILE
    *file;

  Image
    *image;

  size_t
    n;

  SVGInfo
    svg_info;

  unsigned int
    status;

  xmlSAXHandlerPtr
    SAXHandler;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  /*
    If there is a geometry string in image_info->size (e.g., gm convert
    -size "50x50%" in.svg out.png), AllocateImage() sets image->columns
    and image->rows to the width and height values from the size string.
    However, this makes no sense if the size string was something like
    "50x50%" (we'll get columns = rows = 50).  So we set columns and
    rows to 0 here, which is the same as if no size string was supplied
    by the client.  This also results in svg_info.bounds to be set to
    0,0 below (i.e., unknown), so that svg_info.bounds will be set using
    the image size information from either the svg "canvas" width/height
    or from the viewbox.  Later, variable "page" is set from
    svg_info->bounds. Then the geometry string in image_info->size gets
    applied to the (now known) "page" width and height when
    SvgStartElement() calls GetMagickGeometry(), and the intended result
    is obtained.
  */
  image->columns = 0;
  image->rows = 0;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Open draw file.
  */
  file=AcquireTemporaryFileStream(filename,TextFileIOMode);
  if (file == (FILE *) NULL)
    ThrowReaderTemporaryFileException(filename);
  /*
    Parse SVG file.
  */
  (void) memset(&svg_info,0,sizeof(SVGInfo));
  svg_info.file=file;
  svg_info.exception=exception;
  svg_info.image=image;
  svg_info.image_info=image_info;
  svg_info.text=AllocateString("");
  svg_info.scale=MagickAllocateMemory(double *,sizeof(double));
  if ((svg_info.text == (char *) NULL) || (svg_info.scale == (double *) NULL))
    {
      (void) fclose(file);
      (void) LiberateTemporaryFile(filename);
      MagickFreeMemory(svg_info.text);
      MagickFreeMemory(svg_info.scale);
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  IdentityAffine(&svg_info.affine);
  svg_info.affine.sx=
    image->x_resolution == 0.0 ? 1.0 : image->x_resolution/72.0;
  svg_info.affine.sy=
    image->y_resolution == 0.0 ? 1.0 : image->y_resolution/72.0;
  svg_info.scale[0]=ExpandAffine(&svg_info.affine);
  svg_info.bounds.width=image->columns;
  svg_info.bounds.height=image->rows;
  svg_info.defsPushCount = 0;
  svg_info.idLevelInsideDefs = 0;
  svg_info.svgPushCount = 0;
  if (image_info->size != (char *) NULL)
    (void) CloneString(&svg_info.size,image_info->size);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"begin SAX");
  (void) xmlSubstituteEntitiesDefault(1);

  (void) memset(&SAXModules,0,sizeof(SAXModules));
  SAXModules.internalSubset=SVGInternalSubset;
  SAXModules.isStandalone=SVGIsStandalone;
  SAXModules.hasInternalSubset=SVGHasInternalSubset;
  SAXModules.hasExternalSubset=SVGHasExternalSubset;
  SAXModules.resolveEntity=SVGResolveEntity;
  SAXModules.getEntity=SVGGetEntity;
  SAXModules.entityDecl=SVGEntityDeclaration;
  SAXModules.notationDecl=SVGNotationDeclaration;
  SAXModules.attributeDecl=SVGAttributeDeclaration;
  SAXModules.elementDecl=SVGElementDeclaration;
  SAXModules.unparsedEntityDecl=SVGUnparsedEntityDeclaration;
  SAXModules.setDocumentLocator=SVGSetDocumentLocator;
  SAXModules.startDocument=SVGStartDocument;
  SAXModules.endDocument=SVGEndDocument;
  SAXModules.startElement=SVGStartElement;
  SAXModules.endElement=SVGEndElement;
  SAXModules.reference=SVGReference;
  SAXModules.characters=SVGCharacters;
  SAXModules.ignorableWhitespace=SVGIgnorableWhitespace;
  SAXModules.processingInstruction=SVGProcessingInstructions;
  SAXModules.comment=SVGComment;
  SAXModules.warning=SVGWarning;
  SAXModules.error=SVGError;
  SAXModules.fatalError=SVGError;
  SAXModules.getParameterEntity=SVGGetParameterEntity;
  SAXModules.cdataBlock=SVGCDataBlock;
  SAXModules.externalSubset=SVGExternalSubset;

  SAXHandler=(&SAXModules);
  svg_info.parser=xmlCreatePushParserCtxt(SAXHandler,&svg_info,(char *) NULL,0,
                                          image->filename);
  while ((n=ReadBlob(image,MaxTextExtent-1,message)) != 0)
    {
      message[n]='\0';
      status=xmlParseChunk(svg_info.parser,message,(int) n,False);
      if (status != 0)
        break;
    }
  (void) xmlParseChunk(svg_info.parser,message,0,True);
  /*
    Assure that our private context is freed, even if we abort before
    seeing the document end.
  */
  SVGEndDocument(&svg_info);
  xmlFreeParserCtxt(svg_info.parser);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"end SAX");
  xmlCleanupParser();
  (void) fclose(file);
  CloseBlob(image);
  DestroyImage(image);
  image=(Image *) NULL;
  if (!image_info->ping && (exception->severity == UndefinedException))
    {
      ImageInfo
        *clone_info;

      /*
        Draw image.
      */
      clone_info=CloneImageInfo(image_info);
      clone_info->blob=(_BlobInfoPtr_) NULL;
      clone_info->length=0;
      FormatString(geometry,"%ldx%ld",svg_info.width,svg_info.height);
      (void) CloneString(&clone_info->size,geometry);
      FormatString(clone_info->filename,"mvg:%.1024s",filename);
      if (clone_info->density != (char *) NULL)
        MagickFreeMemory(clone_info->density);
      image=ReadImage(clone_info,exception);
      DestroyImageInfo(clone_info);
      if (image != (Image *) NULL)
        (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
    }
  /*
    Free resources.
  */
  MagickFreeMemory(svg_info.size);
  if (svg_info.title != (char *) NULL)
    {
      if (image != (Image *) NULL)
        (void) SetImageAttribute(image,"title",svg_info.title);
      MagickFreeMemory(svg_info.title);
    }
  if (svg_info.comment != (char *) NULL)
    {
      if (image != (Image *) NULL)
        (void) SetImageAttribute(image,"comment",svg_info.comment);
      MagickFreeMemory(svg_info.comment);
    }

  (void) memset(&svg_info,0xbf,sizeof(SVGInfo));
  (void) LiberateTemporaryFile(filename);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S V G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSVGImage adds attributes for the SVG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSVGImage method is:
%
%      RegisterSVGImage(void)
%
*/
ModuleExport void
RegisterSVGImage(void)
{
  static char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(LIBXML_DOTTED_VERSION)
  (void) strlcpy(version,"XML " LIBXML_DOTTED_VERSION,MaxTextExtent);
#endif /* defined(LIBXML_DOTTED_VERSION) */

  entry=SetMagickInfo("SVG");
#if defined(HasXML)
  entry->decoder=(DecoderHandler) ReadSVGImage;
#endif /* defined(HasXML) */
#if ENABLE_SVG_WRITER
  entry->encoder=(EncoderHandler) WriteSVGImage;
#endif /* if ENABLE_SVG_WRITER */
  entry->description="Scalable Vector Graphics";
  if (*version != '\0')
    entry->version=version;
  entry->module="SVG";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("SVGZ");
#if defined(HasXML)
  entry->decoder=(DecoderHandler) ReadSVGImage;
#endif /* defined(HasXML) */
#if ENABLE_SVG_WRITER
  entry->encoder=(EncoderHandler) WriteSVGImage;
#endif /* if ENABLE_SVG_WRITER */
  entry->description="Scalable Vector Graphics (ZIP compressed)";
  if (*version != '\0')
    entry->version=version;
  entry->module="SVG";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S V G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSVGImage removes format registrations made by the
%  SVG module from the list of supported formats.
%
%  The format of the UnregisterSVGImage method is:
%
%      UnregisterSVGImage(void)
%
*/
ModuleExport void
UnregisterSVGImage(void)
{
  (void) UnregisterMagickInfo("SVG");
  (void) UnregisterMagickInfo("SVGZ");
}

#if ENABLE_SVG_WRITER
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e S V G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteSVGImage writes a image in the SVG - XML based W3C standard
%  format.
%
%  The format of the WriteSVGImage method is:
%
%      unsigned int WriteSVGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteSVGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

#if defined(HasAUTOTRACE)
static unsigned int
WriteSVGImage(const ImageInfo *image_info,Image *image)
{
  FILE
    *output_file;

  fitting_opts_type
    fit_info;

  image_header_type
    image_header;

  bitmap_type
    bitmap;

  ImageType
    image_type;

  int
    j;

  pixel_outline_list_type
    pixels;

  PixelPacket
    p;

  PixelPacket
    *pixel;

  QuantizeObj
    *quantize_info;

  spline_list_array_type
    splines;

  output_write
    output_writer;

  register long
    i;

  unsigned long
    number_pixels,
    number_planes,
    point,
    thin;

  thin=False;
  quantize_info=(QuantizeObj *) NULL;
  pixel=(&p);
  fit_info=new_fitting_opts();
  output_writer=output_get_handler("svg");
  if (output_writer == NULL)
    ThrowWriterException(DelegateError,UnableToWriteSVGFormat,image);
  image_type=GetImageType(image);
  number_planes=3;
  if ((image_type == BilevelType) || (image_type == GrayscaleType))
    number_planes=1;
  bitmap.np=number_planes;
  bitmap.dimensions.width=image->columns;
  bitmap.dimensions.height=image->rows;
  number_pixels=image->columns*image->rows;
  bitmap.bitmap=MagickAllocateMemory(unsigned char *,number_planes*number_pixels);
  if (bitmap.bitmap == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  point=0;
  for (j=0; j < image->rows; j++)
    {
      for (i=0; i < image->columns; i++)
        {
          p=AcquireOnePixel(image,i,j,&image->exception);
          bitmap.bitmap[point++]=pixel->red;
          if (number_planes == 3)
            {
              bitmap.bitmap[point++]=pixel->green;
              bitmap.bitmap[point++]=pixel->blue;
            }
        }
    }
  image_header.width=DIMENSIONS_WIDTH(bitmap.dimensions);
  image_header.height=DIMENSIONS_HEIGHT(bitmap.dimensions);
  if ((fit_info.color_count > 0) && (BITMAP_PLANES(bitmap) == 3))
    quantize(bitmap.bitmap,bitmap.bitmap,DIMENSIONS_WIDTH(bitmap.dimensions),
             DIMENSIONS_HEIGHT(bitmap.dimensions),fit_info.color_count,
             fit_info.bgColor,&quantize_info);
  if (thin)
    thin_image(&bitmap);
  pixels=find_outline_pixels (bitmap);
  MagickFreeMemory((bitmap.bitmap));
  splines=fitted_splines(pixels,&fit_info);
  output_file=fopen(image->filename,"w");
  if (output_file == (FILE *) NULL)
    ThrowWriterException(FileOpenError,UnableOpenFile,image);
  output_writer(output_file,image->filename,0,0,image_header.width,
                image_header.height,splines);
  return(True);
}
#else
static void
AffineToTransform(Image *image,AffineMatrix *affine)
{
  char
    transform[MaxTextExtent];

  if ((fabs(affine->tx) < MagickEpsilon) && (fabs(affine->ty) < MagickEpsilon))
    {
      if ((fabs(affine->rx) < MagickEpsilon) &&
          (fabs(affine->ry) < MagickEpsilon))
        {
          if ((fabs(affine->sx-1.0) < MagickEpsilon) &&
              (fabs(affine->sy-1.0) < MagickEpsilon))
            {
              (void) WriteBlobString(image,"\">\n");
              return;
            }
          FormatString(transform,"\" transform=\"scale(%g,%g)\">\n",
                       affine->sx,affine->sy);
          (void) WriteBlobString(image,transform);
          return;
        }
      else
        {
          if ((fabs(affine->sx-affine->sy) < MagickEpsilon) &&
              (fabs(affine->rx+affine->ry) < MagickEpsilon) &&
              (fabs(affine->sx*affine->sx+affine->rx*affine->rx-1.0) <
               2*MagickEpsilon))
            {
              double
                theta;

              theta=(180.0/MagickPI)*atan2(affine->rx,affine->sx);
              FormatString(transform,"\" transform=\"rotate(%g)\">\n",theta);
              (void) WriteBlobString(image,transform);
              return;
            }
        }
    }
  else
    {
      if ((fabs(affine->sx-1.0) < MagickEpsilon) &&
          (fabs(affine->rx) < MagickEpsilon) &&
          (fabs(affine->ry) < MagickEpsilon) &&
          (fabs(affine->sy-1.0) < MagickEpsilon))
        {
          FormatString(transform,"\" transform=\"translate(%g,%g)\">\n",
                       affine->tx,affine->ty);
          (void) WriteBlobString(image,transform);
          return;
        }
    }
  FormatString(transform,"\" transform=\"matrix(%g %g %g %g %g %g)\">\n",
               affine->sx,affine->rx,affine->ry,affine->sy,affine->tx,affine->ty);
  (void) WriteBlobString(image,transform);
}

static inline unsigned int
IsPoint(const char *point)
{
  char
    *p;

  (void) strtol(point,&p,10);
  return(p != point);
}

static unsigned int
WriteSVGImage(const ImageInfo *image_info,Image *image)
{
#define BezierQuantum  200

  AffineMatrix
    affine;

  char
    keyword[MaxTextExtent],
    message[MaxTextExtent],
    name[MaxTextExtent],
    *p,
    *q,
    *token,
    type[MaxTextExtent];

  const ImageAttribute
    *attribute;

  int
    n;

  long
    j;

  PointInfo
    point;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register long
    x;

  register long
    i;

  size_t
    length,
    token_max_length;

  SVGInfo
    svg_info;

  unsigned int
    active,
    status;

  unsigned long
    number_points;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  attribute=GetImageAttribute(image,"[MVG]");
  if ((attribute == (ImageAttribute *) NULL) ||
      (attribute->value == (char *) NULL))
    ThrowWriterException(CoderError,NoImageVectorGraphics,image);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  /*
    Write SVG header.
  */
  (void) WriteBlobString(image,"<?xml version=\"1.0\" standalone=\"no\"?>\n");
  (void) WriteBlobString(image,
                         "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"\n");
  (void) WriteBlobString(image,
                         "  \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");
  (void) FormatString(message,"<svg width=\"%lu\" height=\"%lu\">\n",
                      image->columns,image->rows);
  (void) WriteBlobString(image,message);
  /*
    Allocate primitive info memory.
  */
  number_points=2047;
  primitive_info=MagickAllocateMemory(PrimitiveInfo *,
                                      number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  IdentityAffine(&affine);
  token=AllocateString(attribute->value);
  token_max_length=strlen(token);
  active=False;
  n=0;
  status=True;
  for (q=attribute->value; *q != '\0'; )
    {
      /*
        Interpret graphic primitive.
      */
      MagickGetToken(q,&q,keyword,MaxTextExtent);
      if (*keyword == '\0')
        break;
      if (*keyword == '#')
        {
          /*
            Comment.
          */
          if (active)
            {
              AffineToTransform(image,&affine);
              active=False;
            }
          (void) WriteBlobString(image,"<desc>");
          (void) WriteBlobString(image,keyword+1);
          for ( ; (*q != '\n') && (*q != '\0'); q++)
            switch (*q)
              {
              case '<': (void) WriteBlobString(image,"&lt;"); break;
              case '>': (void) WriteBlobString(image,"&gt;"); break;
              case '&': (void) WriteBlobString(image,"&amp;"); break;
              default: (void) WriteBlobByte(image,*q); break;
              }
          (void) WriteBlobString(image,"</desc>\n");
          continue;
        }
      primitive_type=UndefinedPrimitive;
      switch (*keyword)
        {
        case ';':
          break;
        case 'a':
        case 'A':
          {
            if (LocaleCompare("affine",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                affine.sx=MagickAtoF(token);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                affine.rx=MagickAtoF(token);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                affine.ry=MagickAtoF(token);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                affine.sy=MagickAtoF(token);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                affine.tx=MagickAtoF(token);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                affine.ty=MagickAtoF(token);
                break;
              }
            if (LocaleCompare("angle",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                affine.rx=MagickAtoF(token);
                affine.ry=MagickAtoF(token);
                break;
              }
            if (LocaleCompare("arc",keyword) == 0)
              {
                primitive_type=ArcPrimitive;
                break;
              }
            status=False;
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
            status=False;
            break;
          }
        case 'c':
        case 'C':
          {
            if (LocaleCompare("clip-path",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"clip-path:url(#%.1024s);",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("clip-rule",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"clip-rule:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("clip-units",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"clipPathUnits=%.1024s;",token);
                (void) WriteBlobString(image,message);
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
            status=False;
            break;
          }
        case 'd':
        case 'D':
          {
            if (LocaleCompare("decorate",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"text-decoration:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            status=False;
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
            status=False;
            break;
          }
        case 'f':
        case 'F':
          {
            if (LocaleCompare("fill",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"fill:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("fill-rule",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"fill-rule:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("fill-opacity",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"fill-opacity:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("font-family",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"font-family:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("font-stretch",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"font-stretch:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("font-style",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"font-style:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("font-size",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"font-size:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("font-weight",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"font-weight:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            status=False;
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
            if (LocaleCompare("text-align",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"text-align %.1024s ",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("text-anchor",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"text-anchor %.1024s ",token);
                (void) WriteBlobString(image,message);
                break;
              }
            status=False;
            break;
          }
        case 'i':
        case 'I':
          {
            if (LocaleCompare("image",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                primitive_type=ImagePrimitive;
                break;
              }
            status=False;
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
            status=False;
            break;
          }
        case 'm':
        case 'M':
          {
            if (LocaleCompare("matte",keyword) == 0)
              {
                primitive_type=MattePrimitive;
                break;
              }
            status=False;
            break;
          }
        case 'o':
        case 'O':
          {
            if (LocaleCompare("opacity",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"opacity %.1024s ",token);
                (void) WriteBlobString(image,message);
                break;
              }
            status=False;
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
                if (LocaleCompare("clip-path",token) == 0)
                  {
                    (void) WriteBlobString(image,"</clipPath>\n");
                    break;
                  }
                if (LocaleCompare("defs",token) == 0)
                  {
                    (void) WriteBlobString(image,"</defs>\n");
                    break;
                  }
                if (LocaleCompare("gradient",token) == 0)
                  {
                    FormatString(message,"</%sGradient>\n",type);
                    (void) WriteBlobString(image,message);
                    break;
                  }
                if (LocaleCompare("graphic-context",token) == 0)
                  {
                    n--;
                    if (n < 0)
                      ThrowWriterException(DrawError,
                                           UnbalancedGraphicContextPushPop,image);
                    (void) WriteBlobString(image,"</g>\n");
                  }
                if (LocaleCompare("pattern",token) == 0)
                  {
                    (void) WriteBlobString(image,"</pattern>\n");
                    break;
                  }
                if (LocaleCompare("defs",token) == 0)
                  (void) WriteBlobString(image,"</g>\n");
                break;
              }
            if (LocaleCompare("push",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                if (LocaleCompare("clip-path",token) == 0)
                  {
                    MagickGetToken(q,&q,token,token_max_length);
                    FormatString(message,"<clipPath id=\"%s\">\n",token);
                    (void) WriteBlobString(image,message);
                    break;
                  }
                if (LocaleCompare("defs",token) == 0)
                  {
                    (void) WriteBlobString(image,"<defs>\n");
                    break;
                  }
                if (LocaleCompare("gradient",token) == 0)
                  {
                    MagickGetToken(q,&q,token,token_max_length);
                    (void) strlcpy(name,token,MaxTextExtent);
                    MagickGetToken(q,&q,token,token_max_length);
                    (void) strlcpy(type,token,MaxTextExtent);
                    MagickGetToken(q,&q,token,token_max_length);
                    svg_info.segment.x1=MagickAtoF(token);
                    svg_info.element.cx=MagickAtoF(token);
                    MagickGetToken(q,&q,token,token_max_length);
                    if (*token == ',')
                      MagickGetToken(q,&q,token,token_max_length);
                    svg_info.segment.y1=MagickAtoF(token);
                    svg_info.element.cy=MagickAtoF(token);
                    MagickGetToken(q,&q,token,token_max_length);
                    if (*token == ',')
                      MagickGetToken(q,&q,token,token_max_length);
                    svg_info.segment.x2=MagickAtoF(token);
                    svg_info.element.major=MagickAtoF(token);
                    MagickGetToken(q,&q,token,token_max_length);
                    if (*token == ',')
                      MagickGetToken(q,&q,token,token_max_length);
                    svg_info.segment.y2=MagickAtoF(token);
                    svg_info.element.minor=MagickAtoF(token);
                    FormatString(message,"<%sGradient id=\"%s\" x1=\"%g\" "
                                 "y1=\"%g\" x2=\"%g\" y2=\"%g\">\n",type,name,
                                 svg_info.segment.x1,svg_info.segment.y1,svg_info.segment.x2,
                                 svg_info.segment.y2);
                    if (LocaleCompare(type,"radial") == 0)
                      {
                        MagickGetToken(q,&q,token,token_max_length);
                        if (*token == ',')
                          MagickGetToken(q,&q,token,token_max_length);
                        svg_info.element.angle=MagickAtoF(token);
                        FormatString(message,"<%sGradient id=\"%s\" cx=\"%g\" "
                                     "cy=\"%g\" r=\"%g\" fx=\"%g\" fy=\"%g\">\n",type,name,
                                     svg_info.element.cx,svg_info.element.cy,
                                     svg_info.element.angle,svg_info.element.major,
                                     svg_info.element.minor);
                      }
                    (void) WriteBlobString(image,message);
                    break;
                  }
                if (LocaleCompare("graphic-context",token) == 0)
                  {
                    n++;
                    if (active)
                      {
                        AffineToTransform(image,&affine);
                        active=False;
                      }
                    (void) WriteBlobString(image,"<g style=\"");
                    active=True;
                  }
                if (LocaleCompare("pattern",token) == 0)
                  {
                    MagickGetToken(q,&q,token,token_max_length);
                    (void) strlcpy(name,token,MaxTextExtent);
                    MagickGetToken(q,&q,token,token_max_length);
                    svg_info.bounds.x=MagickAtoF(token);
                    MagickGetToken(q,&q,token,token_max_length);
                    if (*token == ',')
                      MagickGetToken(q,&q,token,token_max_length);
                    svg_info.bounds.y=MagickAtoF(token);
                    MagickGetToken(q,&q,token,token_max_length);
                    if (*token == ',')
                      MagickGetToken(q,&q,token,token_max_length);
                    svg_info.bounds.width=MagickAtoF(token);
                    MagickGetToken(q,&q,token,token_max_length);
                    if (*token == ',')
                      MagickGetToken(q,&q,token,token_max_length);
                    svg_info.bounds.height=MagickAtoF(token);
                    FormatString(message,"<pattern id=\"%s\" x=\"%g\" y=\"%g\" "
                                 "width=\"%g\" height=\"%g\">\n",name,svg_info.bounds.x,
                                 svg_info.bounds.y,svg_info.bounds.width,
                                 svg_info.bounds.height);
                    (void) WriteBlobString(image,message);
                    break;
                  }
                break;
              }
            status=False;
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
            if (LocaleCompare("roundRectangle",keyword) == 0)
              {
                primitive_type=RoundRectanglePrimitive;
                break;
              }
            if (LocaleCompare("rotate",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"rotate(%.1024s) ",token);
                (void) WriteBlobString(image,message);
                break;
              }
            status=False;
            break;
          }
        case 's':
        case 'S':
          {
            if (LocaleCompare("scale",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                affine.sx=MagickAtoF(token);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                affine.sy=MagickAtoF(token);
                break;
              }
            if (LocaleCompare("skewX",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"skewX(%.1024s) ",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("skewY",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"skewY(%.1024s) ",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stop-color",keyword) == 0)
              {
                char
                  color[MaxTextExtent];

                MagickGetToken(q,&q,token,token_max_length);
                (void) strlcpy(color,token,MaxTextExtent);
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,
                             "  <stop offset=\"%s\" stop-color=\"%s\" />\n",token,color);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke-antialias",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke-antialias:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke-dasharray",keyword) == 0)
              {
                if (IsPoint(q))
                  {
                    long
                      k;

                    p=q;
                    (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    for (k=0; IsPoint(token); k++)
                      (void) MagickGetToken(p,&p,token,MaxTextExtent);
                    (void) WriteBlobString(image,"stroke-dasharray:");
                    for (j=0; j < k; j++)
                      {
                        MagickGetToken(q,&q,token,token_max_length);
                        FormatString(message,"%.1024s ",token);
                        (void) WriteBlobString(image,message);
                      }
                    (void) WriteBlobString(image,";");
                    break;
                  }
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke-dasharray:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke-dashoffset",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke-dashoffset:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke-linecap",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke-linecap:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke-linejoin",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke-linejoin:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke-miterlimit",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke-miterlimit:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke-opacity",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke-opacity:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("stroke-width",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"stroke-width:%.1024s;",token);
                (void) WriteBlobString(image,message);
                continue;
              }
            status=False;
            break;
          }
        case 't':
        case 'T':
          {
            if (LocaleCompare("text",keyword) == 0)
              {
                primitive_type=TextPrimitive;
                break;
              }
            if (LocaleCompare("text-antialias",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                FormatString(message,"text-antialias:%.1024s;",token);
                (void) WriteBlobString(image,message);
                break;
              }
            if (LocaleCompare("tspan",keyword) == 0)
              {
                primitive_type=TextPrimitive;
                break;
              }
            if (LocaleCompare("translate",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                affine.tx=MagickAtoF(token);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                affine.ty=MagickAtoF(token);
                break;
              }
            status=False;
            break;
          }
        case 'v':
        case 'V':
          {
            if (LocaleCompare("viewbox",keyword) == 0)
              {
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                MagickGetToken(q,&q,token,token_max_length);
                if (*token == ',')
                  MagickGetToken(q,&q,token,token_max_length);
                MagickGetToken(q,&q,token,token_max_length);
                break;
              }
            status=False;
            break;
          }
        default:
          {
            status=False;
            break;
          }
        }
      if (status == False)
        break;
      if (primitive_type == UndefinedPrimitive)
        continue;
      /*
        Parse the primitive attributes.
      */
      i=0;
      j=0;
      for (x=0; *q != '\0'; x++)
        {
          /*
            Define points.
          */
          if (!IsPoint(q))
            break;
          MagickGetToken(q,&q,token,token_max_length);
          point.x=MagickAtoF(token);
          MagickGetToken(q,&q,token,token_max_length);
          if (*token == ',')
            MagickGetToken(q,&q,token,token_max_length);
          point.y=MagickAtoF(token);
          MagickGetToken(q,(char **) NULL,token,token_max_length);
          if (*token == ',')
            MagickGetToken(q,&q,token,token_max_length);
          primitive_info[i].primitive=primitive_type;
          primitive_info[i].point=point;
          primitive_info[i].coordinates=0;
          primitive_info[i].method=FloodfillMethod;
          i++;
          if (i < (long) (number_points-6*BezierQuantum-360))
            continue;
          number_points+=6*BezierQuantum+360;
          MagickReallocMemory(PrimitiveInfo *,primitive_info,
                              number_points*sizeof(PrimitiveInfo));
          if (primitive_info == (PrimitiveInfo *) NULL)
            {
              ThrowException3(&image->exception,ResourceLimitError,
                              MemoryAllocationFailed,UnableToDrawOnImage);
              break;
            }
        }
      primitive_info[j].primitive=primitive_type;
      primitive_info[j].coordinates=x;
      primitive_info[j].method=FloodfillMethod;
      primitive_info[j].text=(char *) NULL;
      if (active)
        {
          AffineToTransform(image,&affine);
          active=False;
        }
      active=False;
      switch (primitive_type)
        {
        case PointPrimitive:
        default:
          {
            if (primitive_info[j].coordinates != 1)
              {
                status=False;
                break;
              }
            break;
          }
        case LinePrimitive:
          {
            if (primitive_info[j].coordinates != 2)
              {
                status=False;
                break;
              }
            (void) FormatString(message,
                                "  <line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\"/>\n",
                                primitive_info[j].point.x,primitive_info[j].point.y,
                                primitive_info[j+1].point.x,primitive_info[j+1].point.y);
            (void) WriteBlobString(image,message);
            break;
          }
        case RectanglePrimitive:
          {
            if (primitive_info[j].coordinates != 2)
              {
                status=False;
                break;
              }
            (void) FormatString(message,
                                "  <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\"/>\n",
                                primitive_info[j].point.x,primitive_info[j].point.y,
                                primitive_info[j+1].point.x-primitive_info[j].point.x,
                                primitive_info[j+1].point.y-primitive_info[j].point.y);
            (void) WriteBlobString(image,message);
            break;
          }
        case RoundRectanglePrimitive:
          {
            if (primitive_info[j].coordinates != 3)
              {
                status=False;
                break;
              }
            (void) FormatString(message,"  <rect x=\"%g\" y=\"%g\" "
                                "width=\"%g\" height=\"%g\" rx=\"%g\" ry=\"%g\"/>\n",
                                primitive_info[j].point.x,primitive_info[j].point.y,
                                primitive_info[j+1].point.x-primitive_info[j].point.x,
                                primitive_info[j+1].point.y-primitive_info[j].point.y,
                                primitive_info[j+2].point.x,primitive_info[j+2].point.y);
            (void) WriteBlobString(image,message);
            break;
          }
        case ArcPrimitive:
          {
            if (primitive_info[j].coordinates != 3)
              {
                status=False;
                break;
              }
            break;
          }
        case EllipsePrimitive:
          {
            if (primitive_info[j].coordinates != 3)
              {
                status=False;
                break;
              }
            (void) FormatString(message,
                                "  <ellipse cx=\"%g\" cy=\"%g\" rx=\"%g\" ry=\"%g\"/>\n",
                                primitive_info[j].point.x,primitive_info[j].point.y,
                                primitive_info[j+1].point.x,primitive_info[j+1].point.y);
            (void) WriteBlobString(image,message);
            break;
          }
        case CirclePrimitive:
          {
            double
              alpha,
              beta;

            if (primitive_info[j].coordinates != 2)
              {
                status=False;
                break;
              }
            alpha=primitive_info[j+1].point.x-primitive_info[j].point.x;
            beta=primitive_info[j+1].point.y-primitive_info[j].point.y;
            (void) FormatString(message,"  <circle cx=\"%g\" cy=\"%g\" r=\"%g\"/>\n",
                                primitive_info[j].point.x,primitive_info[j].point.y,
                                sqrt(alpha*alpha+beta*beta));
            (void) WriteBlobString(image,message);
            break;
          }
        case PolylinePrimitive:
          {
            if (primitive_info[j].coordinates < 2)
              {
                status=False;
                break;
              }
            (void) strcpy(message,"  <polyline points=\"");
            (void) WriteBlobString(image,message);
            length=strlen(message);
            for ( ; j < i; j++)
              {
                FormatString(message,"%g,%g ",primitive_info[j].point.x,
                             primitive_info[j].point.y);
                length+=strlen(message);
                if (length >= 80)
                  {
                    (void) WriteBlobString(image,"\n    ");
                    length=strlen(message)+5;
                  }
                (void) WriteBlobString(image,message);
              }
            (void) WriteBlobString(image,"\"/>\n");
            break;
          }
        case PolygonPrimitive:
          {
            if (primitive_info[j].coordinates < 3)
              {
                status=False;
                break;
              }
            primitive_info[i]=primitive_info[j];
            primitive_info[i].coordinates=0;
            primitive_info[j].coordinates++;
            i++;
            (void) strcpy(message,"  <polygon points=\"");
            (void) WriteBlobString(image,message);
            length=strlen(message);
            for ( ; j < i; j++)
              {
                FormatString(message,"%g,%g ",primitive_info[j].point.x,
                             primitive_info[j].point.y);
                length+=strlen(message);
                if (length >= 80)
                  {
                    (void) WriteBlobString(image,"\n    ");
                    length=strlen(message)+5;
                  }
                (void) WriteBlobString(image,message);
              }
            (void) WriteBlobString(image,"\"/>\n");
            break;
          }
        case BezierPrimitive:
          {
            if (primitive_info[j].coordinates < 3)
              {
                status=False;
                break;
              }
            break;
          }
        case PathPrimitive:
          {
            int
              number_attributes;

            MagickGetToken(q,&q,token,token_max_length);
            number_attributes=1;
            for (p=token; *p != '\0'; p++)
              if (isalpha((int) *p))
                number_attributes++;
            if (i > (long) (number_points-6*BezierQuantum*number_attributes-1))
              {
                number_points+=6*BezierQuantum*number_attributes;
                MagickReallocMemory(PrimitiveInfo *,primitive_info,
                                    number_points*sizeof(PrimitiveInfo));
                if (primitive_info == (PrimitiveInfo *) NULL)
                  {
                    ThrowException3(&image->exception,ResourceLimitError,
                                    MemoryAllocationFailed,UnableToDrawOnImage);
                    break;
                  }
              }
            (void) WriteBlobString(image,"  <path d=\"");
            (void) WriteBlobString(image,token);
            (void) WriteBlobString(image,"\"/>\n");
            break;
          }
        case ColorPrimitive:
        case MattePrimitive:
          {
            if (primitive_info[j].coordinates != 1)
              {
                status=False;
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
            register char
              *p;

            if (primitive_info[j].coordinates != 1)
              {
                status=False;
                break;
              }
            MagickGetToken(q,&q,token,token_max_length);
            (void) FormatString(message,"  <text x=\"%g\" y=\"%g\">",
                                primitive_info[j].point.x,primitive_info[j].point.y);
            (void) WriteBlobString(image,message);
            for (p=token; *p != '\0'; p++)
              switch (*p)
                {
                case '<': (void) WriteBlobString(image,"&lt;"); break;
                case '>': (void) WriteBlobString(image,"&gt;"); break;
                case '&': (void) WriteBlobString(image,"&amp;"); break;
                default: (void) WriteBlobByte(image,*p); break;
                }
            (void) WriteBlobString(image,"</text>\n");
            break;
          }
        case ImagePrimitive:
          {
            if (primitive_info[j].coordinates != 2)
              {
                status=False;
                break;
              }
            MagickGetToken(q,&q,token,token_max_length);
            (void) FormatString(message,"  <image x=\"%g\" y=\"%g\" "
                                "width=\"%g\" height=\"%g\" xlink:href=\"%.1024s\"/>\n",
                                primitive_info[j].point.x,primitive_info[j].point.y,
                                primitive_info[j+1].point.x,primitive_info[j+1].point.y,token);
            (void) WriteBlobString(image,message);
            break;
          }
        }
      if (primitive_info == (PrimitiveInfo *) NULL)
        break;
      primitive_info[i].primitive=UndefinedPrimitive;
      if (status == False)
        break;
    }
  (void) WriteBlobString(image,"</svg>\n");
  /*
    Free resources.
  */
  MagickFreeMemory(token);
  if (primitive_info != (PrimitiveInfo *) NULL)
    MagickFreeMemory(primitive_info);
  CloseBlob(image);
  return(status);
}
#endif
#endif /* if ENABLE_SVG_WRITER */
