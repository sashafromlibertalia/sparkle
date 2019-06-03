/*
% Copyright (C) 2003-2018 GraphicsMagick Group
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
%                  SSSSS  TTTTT   AAA   TTTTT  IIIII   CCCC                   %
%                  SS       T    A   A    T      I    C                       %
%                   SSS     T    AAAAA    T      I    C                       %
%                     SS    T    A   A    T      I    C                       %
%                  SSSSS    T    A   A    T    IIIII   CCCC                   %
%                                                                             %
%                                                                             %
%                        GraphicsMagick Static Methods                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2000                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/log.h"
#include "magick/module.h"
#include "magick/static.h"
#include "magick/utility.h"

#if !defined(SupportMagickModules)

#include "magick/module_aliases.h"

typedef struct _StaticModule
{
  const char* name;
  void (*register_fn)(void);
  void (*unregister_fn)(void);
  MagickBool loaded;
  const unsigned int name_length;

} StaticModule;

/*
  This list must be ordered by 'name' in an ascending order based on
  strcmp().
*/
static StaticModule
StaticModules[] =
{
#define STATICM(name,register_fn,unregister_fn) {name,register_fn,unregister_fn,MagickFalse,sizeof(name)-1}
  STATICM("ART",RegisterARTImage,UnregisterARTImage),
  STATICM("AVS",RegisterAVSImage,UnregisterAVSImage),
  STATICM("BMP",RegisterBMPImage,UnregisterBMPImage),
  STATICM("CALS",RegisterCALSImage,UnregisterCALSImage),
  STATICM("CAPTION",RegisterCAPTIONImage,UnregisterCAPTIONImage),
  STATICM("CINEON",RegisterCINEONImage,UnregisterCINEONImage),
#if defined(HasWINGDI32)
  STATICM("CLIPBOARD",RegisterCLIPBOARDImage,UnregisterCLIPBOARDImage),
#endif
  STATICM("CMYK",RegisterCMYKImage,UnregisterCMYKImage),
  STATICM("CUT",RegisterCUTImage,UnregisterCUTImage),
  STATICM("DCM",RegisterDCMImage,UnregisterDCMImage),
  STATICM("DCRAW",RegisterDCRAWImage,UnregisterDCRAWImage),
  STATICM("DIB",RegisterDIBImage,UnregisterDIBImage),
#if defined(HasDPS)
  STATICM("DPS",RegisterDPSImage,UnregisterDPSImage),
#endif
  STATICM("DPX",RegisterDPXImage,UnregisterDPXImage),
#if defined(HasWINGDI32)
  STATICM("EMF",RegisterEMFImage,UnregisterEMFImage),
#endif
#if defined(HasTIFF)
  STATICM("EPT",RegisterEPTImage,UnregisterEPTImage),
#endif
  STATICM("FAX",RegisterFAXImage,UnregisterFAXImage),
  STATICM("FITS",RegisterFITSImage,UnregisterFITSImage),
#if defined(HasFPX)
  STATICM("FPX",RegisterFPXImage,UnregisterFPXImage),
#endif
  STATICM("GIF",RegisterGIFImage,UnregisterGIFImage),
  STATICM("GRAY",RegisterGRAYImage,UnregisterGRAYImage),
  STATICM("GRADIENT",RegisterGRADIENTImage,UnregisterGRADIENTImage),
  STATICM("HISTOGRAM",RegisterHISTOGRAMImage,UnregisterHISTOGRAMImage),
  STATICM("HRZ",RegisterHRZImage,UnregisterHRZImage),
  STATICM("HTML",RegisterHTMLImage,UnregisterHTMLImage),
  STATICM("ICON",RegisterICONImage,UnregisterICONImage),
  STATICM("IDENTITY",RegisterIDENTITYImage,UnregisterIDENTITYImage),
  STATICM("INFO",RegisterINFOImage,UnregisterINFOImage),
#if defined(HasJBIG)
  STATICM("JBIG",RegisterJBIGImage,UnregisterJBIGImage),
#endif
#if defined(HasJPEG)
  STATICM("JNX",RegisterJNXImage,UnregisterJNXImage),
#endif
#if defined(HasJPEG)
  STATICM("JPEG",RegisterJPEGImage,UnregisterJPEGImage),
#endif
#if defined(HasJP2)
  STATICM("JP2",RegisterJP2Image,UnregisterJP2Image),
#endif
  STATICM("LABEL",RegisterLABELImage,UnregisterLABELImage),
  STATICM("LOCALE",RegisterLOCALEImage,UnregisterLOCALEImage),
  STATICM("LOGO",RegisterLOGOImage,UnregisterLOGOImage),
  STATICM("MAC",RegisterMACImage,UnregisterMACImage),
  STATICM("MAP",RegisterMAPImage,UnregisterMAPImage),
  STATICM("MAT",RegisterMATImage,UnregisterMATImage),
  STATICM("MATTE",RegisterMATTEImage,UnregisterMATTEImage),
  STATICM("META",RegisterMETAImage,UnregisterMETAImage),
  STATICM("MIFF",RegisterMIFFImage,UnregisterMIFFImage),
  STATICM("MONO",RegisterMONOImage,UnregisterMONOImage),
  STATICM("MPC",RegisterMPCImage,UnregisterMPCImage),
  STATICM("MPEG",RegisterMPEGImage,UnregisterMPEGImage),
  STATICM("MPR",RegisterMPRImage,UnregisterMPRImage),
  STATICM("MSL",RegisterMSLImage,UnregisterMSLImage),
  STATICM("MTV",RegisterMTVImage,UnregisterMTVImage),
  STATICM("MVG",RegisterMVGImage,UnregisterMVGImage),
  STATICM("NULL",RegisterNULLImage,UnregisterNULLImage),
  STATICM("OTB",RegisterOTBImage,UnregisterOTBImage),
  STATICM("PALM",RegisterPALMImage,UnregisterPALMImage),
  STATICM("PCD",RegisterPCDImage,UnregisterPCDImage),
  STATICM("PCL",RegisterPCLImage,UnregisterPCLImage),
  STATICM("PCX",RegisterPCXImage,UnregisterPCXImage),
  STATICM("PDB",RegisterPDBImage,UnregisterPDBImage),
  STATICM("PDF",RegisterPDFImage,UnregisterPDFImage),
  STATICM("PICT",RegisterPICTImage,UnregisterPICTImage),
  STATICM("PIX",RegisterPIXImage,UnregisterPIXImage),
  STATICM("PLASMA",RegisterPLASMAImage,UnregisterPLASMAImage),
#if defined(HasPNG)
  STATICM("PNG",RegisterPNGImage,UnregisterPNGImage),
#endif
  STATICM("PNM",RegisterPNMImage,UnregisterPNMImage),
  STATICM("PREVIEW",RegisterPREVIEWImage,UnregisterPREVIEWImage),
  STATICM("PS",RegisterPSImage,UnregisterPSImage),
  STATICM("PS2",RegisterPS2Image,UnregisterPS2Image),
  STATICM("PS3",RegisterPS3Image,UnregisterPS3Image),
#if defined(EnableBrokenCoders) && EnableBrokenCoders
  STATICM("PSD",RegisterPSDImage,UnregisterPSDImage),
#endif /* defined(EnableBrokenCoders) && EnableBrokenCoders */
  STATICM("PWP",RegisterPWPImage,UnregisterPWPImage),
  STATICM("RGB",RegisterRGBImage,UnregisterRGBImage),
  STATICM("RLA",RegisterRLAImage,UnregisterRLAImage),
  STATICM("RLE",RegisterRLEImage,UnregisterRLEImage),
  STATICM("SCT",RegisterSCTImage,UnregisterSCTImage),
  STATICM("SFW",RegisterSFWImage,UnregisterSFWImage),
  STATICM("SGI",RegisterSGIImage,UnregisterSGIImage),
  STATICM("STEGANO",RegisterSTEGANOImage,UnregisterSTEGANOImage),
  STATICM("SUN",RegisterSUNImage,UnregisterSUNImage),
  STATICM("SVG",RegisterSVGImage,UnregisterSVGImage),
  STATICM("TGA",RegisterTGAImage,UnregisterTGAImage),
#if defined(HasTIFF)
  STATICM("TIFF",RegisterTIFFImage,UnregisterTIFFImage),
#endif
  STATICM("TILE",RegisterTILEImage,UnregisterTILEImage),
  STATICM("TIM",RegisterTIMImage,UnregisterTIMImage),
  STATICM("TOPOL",RegisterTOPOLImage,UnregisterTOPOLImage),
  STATICM("TTF",RegisterTTFImage,UnregisterTTFImage),
  STATICM("TXT",RegisterTXTImage,UnregisterTXTImage),
  STATICM("UIL",RegisterUILImage,UnregisterUILImage),
  STATICM("URL",RegisterURLImage,UnregisterURLImage),
  STATICM("UYVY",RegisterUYVYImage,UnregisterUYVYImage),
  STATICM("VICAR",RegisterVICARImage,UnregisterVICARImage),
  STATICM("VID",RegisterVIDImage,UnregisterVIDImage),
  STATICM("VIFF",RegisterVIFFImage,UnregisterVIFFImage),
  STATICM("WBMP",RegisterWBMPImage,UnregisterWBMPImage),
#if defined(HasWEBP)
  STATICM("WEBP",RegisterWEBPImage,UnregisterWEBPImage),
#endif
  STATICM("WMF",RegisterWMFImage,UnregisterWMFImage),
  STATICM("WPG",RegisterWPGImage,UnregisterWPGImage),
#if defined(HasX11)
  STATICM("X",RegisterXImage,UnregisterXImage),
#endif /* defined(HasX11) */
  STATICM("XBM",RegisterXBMImage,UnregisterXBMImage),
  STATICM("XC",RegisterXCImage,UnregisterXCImage),
  STATICM("XCF",RegisterXCFImage,UnregisterXCFImage),
  STATICM("XPM",RegisterXPMImage,UnregisterXPMImage),
#if defined(_VISUALC_)
  STATICM("XTRN",RegisterXTRNImage,UnregisterXTRNImage),
#endif /* defined(_VISUALC_) */
#if defined(HasX11)
  STATICM("XWD",RegisterXWDImage,UnregisterXWDImage),
#endif /* defined(HasX11) */
  STATICM("YUV",RegisterYUVImage,UnregisterYUVImage)
};


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y M a g i c k M o d u l e s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickResources() destroys the resource environment.
%
%  The format of the DestroyMagickResources() method is:
%
%      DestroyMagickResources(void)
%
%
*/
MagickExport void
DestroyMagickModules(void)
{
  unsigned int index;

  for (index=0; index < sizeof(StaticModules)/sizeof(StaticModules[0]);index++)
    {
      if (StaticModules[index].loaded == MagickTrue)
        {
          (StaticModules[index].unregister_fn)();
          StaticModules[index].loaded = MagickFalse;
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e M a g i c k M o d u l e s                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeMagickModules() initializes the module loader.
%
%  The format of the InitializeMagickModules() method is:
%
%      InitializeMagickModules(void)
%
%
*/
MagickExport void
InitializeMagickModules(void)
{
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n M o d u l e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  OpenModule() loads a module, and invokes its registration method.  It
%  returns MagickPass on success, and MagickFail if there is an error.
%
%  The format of the OpenModule method is:
%
%      MagickPassFail OpenModule(const char *module,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method OpenModule returns MagickPass if the specified module is
%      loaded, otherwise MagickFail.
%
%    o module: a character string that indicates the module to load.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail
OpenModule(const char *module,ExceptionInfo *exception)
{
  char
    module_name[MaxTextExtent];

  size_t
    name_length;

  unsigned int
    index;

  MagickPassFail
    status = MagickFail;

  (void) exception;
  /*
    Assign module name from alias.
  */
  assert(module != (const char *) NULL);
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                        "Magick \"%s\"", module);
  name_length=strlcpy(module_name,module,MaxTextExtent);
  for (index=0; index < ArraySize(ModuleAliases);index++)
    {
      if (ModuleAliases[index].magick[0] > module[0])
        break;
      if ((ModuleAliases[index].magick[0] == module[0]) &&
          (ModuleAliases[index].magick_len == name_length) &&
          (memcmp(ModuleAliases[index].magick,module,name_length) == 0))
        {
          name_length=strlcpy(module_name,ModuleAliases[index].name,MaxTextExtent);
          break;
        }
    }

  /*
    Find module in list and load if not already loaded
  */
  for (index=0; index < ArraySize(StaticModules);index++)
    {
      if (StaticModules[index].name[0] > module_name[0])
        break;
      if ((StaticModules[index].name[0] == module_name[0]) &&
          (StaticModules[index].name_length == name_length) &&
          (memcmp(StaticModules[index].name,module_name,name_length) == 0))
        {
          if (StaticModules[index].loaded == MagickFalse)
            {
              (StaticModules[index].register_fn)();
              StaticModules[index].loaded = MagickTrue;
              (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                                    "Loaded static module \"%s\"", module_name);
            }
          status=MagickPass;
          break;
        }
    }
  if (index == ArraySize(StaticModules))
    status=MagickFail;

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n M o d u l e s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenModules loads all available modules.
%
%  The format of the OpenModules method is:
%
%      MagickPassFail OpenModules(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method OpenModules returns True if the modules are loaded,
%      otherwise False.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport MagickPassFail
OpenModules(ExceptionInfo *exception)
{
  unsigned int index;
  (void) exception;

  for (index=0; index < ArraySize(StaticModules);index++)
    {
      if (StaticModules[index].loaded == MagickFalse)
        {
          (StaticModules[index].register_fn)();
          StaticModules[index].loaded = MagickTrue;
        }
    }

  return MagickPass;
}
#endif /* !defined(SupportMagickModules) */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x e c u t e S t a t i c M o d u l e P r o c e s s                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ExecuteStaticModuleProcess() is just a template method. This version is
%  only used for static and moby builds.  See the version in module.c for
%  module-only builds. This function takes the place of ExecuteModuleProcess
%  for builds which do not support loading modules.
%
%  The format of the ExecuteStaticModuleProcess method is:
%
%      unsigned int ExecuteStaticModuleProcess(const char *tag,
%        Image **image,const int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o tag: The module tag.
%
%    o image: The image.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
*/
#if defined(SupportMagickModules)
MagickExport unsigned int ExecuteStaticModuleProcess(const char *tag,
  Image **image,const int argc,char **argv)
#else
MagickExport unsigned int ExecuteModuleProcess(const char *tag,
  Image **image,const int argc,char **argv)
#endif /* defined(SupportMagickModules) */
{
  unsigned int
    status = False;

#if !defined(SupportMagickModules)
  unsigned int
    (*method)(Image **,const int,char **) = 0;

  if (LocaleCompare("analyze",tag) == 0)
    method=AnalyzeImage;

  if (method)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" filter module",tag);

      status=(*method)(image,argc,argv);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Returned from \"%.1024s\" filter module",tag);
    }
#else
  ARG_NOT_USED(tag);
  ARG_NOT_USED(image);
  ARG_NOT_USED(argc);
  ARG_NOT_USED(argv);
#endif /* !defined(SupportMagickModules) */
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S t a t i c M o d u l e s                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
RegisterStaticModules() statically registers all the available module
%  handlers.
%
%  The format of the RegisterStaticModules method is:
%
%      RegisterStaticModules(void)
%
%
*/
MagickExport void RegisterStaticModules(void)
{
#if !defined(SupportMagickModules)
  unsigned int index;

  for (index=0; index < ArraySize(StaticModules); index++)
    {
      if (StaticModules[index].loaded == MagickFalse)
        {
          (StaticModules[index].register_fn)();
          StaticModules[index].loaded = MagickTrue;
        }
    }
#endif /* !defined(SupportMagickModules) */
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S t a t i c M o d u l e s                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnregisterStaticModules() statically unregisters all the available module
%  handlers. This allows allocated resources to be freed.
%
%  The format of the UnRegisterStaticModules method is:
%
%      UnRegisterStaticModules(void)
%
%
*/
MagickExport void UnregisterStaticModules(void)
{
#if !defined(SupportMagickModules)
  unsigned int index;

  for (index=0; index < ArraySize(StaticModules);index++)
    {
      if (StaticModules[index].loaded == MagickTrue)
        {
          (StaticModules[index].unregister_fn)();
          StaticModules[index].loaded = MagickFalse;
        }
    }
#endif /* !defined(SupportMagickModules) */
}
