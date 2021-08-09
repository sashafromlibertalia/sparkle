/* magick/magick_config.h.  Generated from magick_config.h.in by configure.  */
/* magick/magick_config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define if coders and filters are to be built as modules. */
/* #undef BuildMagickModules */

/* Disable OpenMP for algorithms which sometimes run slower */
#define DisableSlowOpenMP 1

/* Enable broken/dangerous file formats support */
/* #undef EnableBrokenCoders */

/* C compiler used for compilation */
#define GM_BUILD_CC "gcc"

/* CFLAGS used for C compilation */
#define GM_BUILD_CFLAGS "-g -O2 -Wall -D_THREAD_SAFE"

/* arguments passed to configure */
#define GM_BUILD_CONFIGURE_ARGS "./configure "

/* CPPFLAGS used for preprocessing */
#define GM_BUILD_CPPFLAGS "-I/usr/local/include/freetype2 -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk/usr/include/libxml2"

/* C++ compiler used for compilation */
#define GM_BUILD_CXX "g++"

/* CXXFLAGS used for C++ compilation */
#define GM_BUILD_CXXFLAGS "-D_THREAD_SAFE"

/* Host identification triplet */
#define GM_BUILD_HOST "x86_64-apple-darwin18.0.0"

/* LDFLAGS used for linking */
#define GM_BUILD_LDFLAGS "-L/usr/local/lib -L/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk/usr/lib"

/* LIBS used for linking */
#define GM_BUILD_LIBS "-lfreetype -lpng16 -lbz2 -lxml2 -lz -lm -lpthread"

/* Define if C++ compiler supports __func__ */
#define HAS_CPP__func__ 1

/* Define if C compiler supports __func__ */
#define HAS_C__func__ 1

/* Define to 1 if you have the `atoll' function. */
#define HAVE_ATOLL 1

/* define if bool is a built-in type */
#define HAVE_BOOL /**/

/* Define to 1 if you have the `clock_getres' function. */
#define HAVE_CLOCK_GETRES 1

/* Define to 1 if you have the `clock_gettime' function. */
#define HAVE_CLOCK_GETTIME 1

/* define if the compiler supports const_cast<> */
#define HAVE_CONST_CAST /**/

/* Define to 1 if you have the `CryptGenRandom' function. */
/* #undef HAVE_CRYPTGENRANDOM */

/* Define to 1 if you have the `ctime_r' function. */
#define HAVE_CTIME_R 1

/* Define to 1 if you have the declaration of `pread', and to 0 if you don't.
   */
#define HAVE_DECL_PREAD 1

/* Define to 1 if you have the declaration of `pwrite', and to 0 if you don't.
   */
#define HAVE_DECL_PWRITE 1

/* Define to 1 if you have the declaration of `strlcpy', and to 0 if you
   don't. */
#define HAVE_DECL_STRLCPY 1

/* Define to 1 if you have the declaration of `vsnprintf', and to 0 if you
   don't. */
#define HAVE_DECL_VSNPRINTF 1

/* define if the compiler supports default template parameters */
#define HAVE_DEFAULT_TEMPLATE_PARAMETERS /**/

/* Have a /dev/urandom device for producing random bytes */
#define HAVE_DEV_URANDOM 1

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* define if the compiler supports exceptions */
#define HAVE_EXCEPTIONS /**/

/* define if the compiler supports the explicit keyword */
#define HAVE_EXPLICIT /**/

/* Define to 1 if you have the `fcntl' function. */
#define HAVE_FCNTL 1

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the `fstatvfs' function. */
#define HAVE_FSTATVFS 1

/* Define to 1 if you have the <ft2build.h> header file. */
#define HAVE_FT2BUILD_H 1

/* Define to 1 if you have the `ftime' function. */
#define HAVE_FTIME 1

/* Define to 1 if you have the `getc_unlocked' function. */
#define HAVE_GETC_UNLOCKED 1

/* Define to 1 if you have the `getexecname' function. */
/* #undef HAVE_GETEXECNAME */

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the `getpid' function. */
#define HAVE_GETPID 1

/* Define to 1 if you have the `getrlimit' function. */
#define HAVE_GETRLIMIT 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the <lcms2.h> header file. */
/* #undef HAVE_LCMS2_H */

/* Define if you have the <lcms2/lcms2.h> header file. */
/* #undef HAVE_LCMS2_LCMS2_H */

/* Define to 1 if you have the `lltostr' function. */
/* #undef HAVE_LLTOSTR */

/* Define to 1 if you have the `localtime_r' function. */
#define HAVE_LOCALTIME_R 1

/* Define to 1 if the type `long double' works and has more range or precision
   than `double'. */
#define HAVE_LONG_DOUBLE_WIDER 1

/* Define to 1 if you have the <machine/param.h> header file. */
#define HAVE_MACHINE_PARAM_H 1

/* Define to 1 if you have the <mach-o/dyld.h> header file. */
#define HAVE_MACH_O_DYLD_H 1

/* Define to 1 if you have the `madvise' function. */
#define HAVE_MADVISE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have a `mmap' system call which handles coherent file
   I/O. */
#define HAVE_MMAP_FILEIO 1

/* define if the compiler supports the mutable keyword */
#define HAVE_MUTABLE /**/

/* define if the compiler implements namespaces */
#define HAVE_NAMESPACES /**/

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* define if the compiler accepts the new for scoping rules */
#define HAVE_NEW_FOR_SCOPING /**/

/* Define to 1 if you have the `pclose' function. */
#define HAVE_PCLOSE 1

/* Define to 1 if you have the `poll' function. */
#define HAVE_POLL 1

/* Define to 1 if you have the `popen' function. */
#define HAVE_POPEN 1

/* Define to 1 if you have the `posix_fadvise' function. */
/* #undef HAVE_POSIX_FADVISE */

/* Define to 1 if you have the `posix_fallocate' function. */
/* #undef HAVE_POSIX_FALLOCATE */

/* Define to 1 if you have the `posix_madvise' function. */
#define HAVE_POSIX_MADVISE 1

/* Define to 1 if you have the `posix_memalign' function. */
#define HAVE_POSIX_MEMALIGN 1

/* Define to 1 if you have the `posix_spawnp' function. */
#define HAVE_POSIX_SPAWNP 1

/* Define to 1 if you have the `pread' function. */
#define HAVE_PREAD 1

/* Define to 1 if you have the <process.h> header file. */
/* #undef HAVE_PROCESS_H */

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Define to 1 if you have the `putc_unlocked' function. */
#define HAVE_PUTC_UNLOCKED 1

/* Define to 1 if you have the `pwrite' function. */
#define HAVE_PWRITE 1

/* Define to 1 if you have the `qsort_r' function. */
#define HAVE_QSORT_R 1

/* Define to 1 if you have the `raise' function. */
#define HAVE_RAISE 1

/* Define to 1 if you have the `rand_r' function. */
#define HAVE_RAND_R 1

/* Define to 1 if you have the `readdir_r' function. */
#define HAVE_READDIR_R 1

/* Define to 1 if you have the `readlink' function. */
#define HAVE_READLINK 1

/* Define to 1 if you have the `realpath' function. */
#define HAVE_REALPATH 1

/* Define to 1 if you have the `seekdir' function. */
#define HAVE_SEEKDIR 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `setrlimit' function. */
#define HAVE_SETRLIMIT 1

/* Define to 1 if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the `sigemptyset' function. */
#define HAVE_SIGEMPTYSET 1

/* Define to 1 if you have the `spawnvp' function. */
/* #undef HAVE_SPAWNVP */

/* define if the compiler supports static_cast<> */
#define HAVE_STATIC_CAST /**/

/* define if the compiler supports ISO C++ standard library */
#define HAVE_STD /**/

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* define if the compiler supports Standard Template Library */
#define HAVE_STL /**/

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the `strerror_r' function. */
#define HAVE_STRERROR_R 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strlcat' function. */
#define HAVE_STRLCAT 1

/* Define to 1 if you have the `strlcpy' function. */
#define HAVE_STRLCPY 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if you have the <sun_prefetch.h> header file. */
/* #undef HAVE_SUN_PREFETCH_H */

/* Define to 1 if you have the `sysconf' function. */
#define HAVE_SYSCONF 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/times.h> header file. */
#define HAVE_SYS_TIMES_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the `telldir' function. */
#define HAVE_TELLDIR 1

/* define if the compiler supports basic templates */
#define HAVE_TEMPLATES /**/

/* Define to 1 if you have the <tiffconf.h> header file. */
/* #undef HAVE_TIFFCONF_H */

/* Define to 1 if you have the `TIFFIsCODECConfigured' function. */
/* #undef HAVE_TIFFISCODECCONFIGURED */

/* Define to 1 if you have the `TIFFMergeFieldInfo' function. */
/* #undef HAVE_TIFFMERGEFIELDINFO */

/* Define to 1 if you have the `TIFFSetErrorHandlerExt' function. */
/* #undef HAVE_TIFFSETERRORHANDLEREXT */

/* Define to 1 if you have the `TIFFSetTagExtender' function. */
/* #undef HAVE_TIFFSETTAGEXTENDER */

/* Define to 1 if you have the `TIFFSetWarningHandlerExt' function. */
/* #undef HAVE_TIFFSETWARNINGHANDLEREXT */

/* Define to 1 if you have the `TIFFSwabArrayOfTriples' function. */
/* #undef HAVE_TIFFSWABARRAYOFTRIPLES */

/* Define to 1 if you have the `times' function. */
#define HAVE_TIMES 1

/* Define to 1 if you have the `ulltostr' function. */
/* #undef HAVE_ULLTOSTR */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `vsprintf' function. */
#define HAVE_VSPRINTF 1

/* Define to 1 if you have the <wincrypt.h> header file. */
/* #undef HAVE_WINCRYPT_H */

/* Define to 1 if you have the `_exit' function. */
#define HAVE__EXIT 1

/* Define to 1 if you have the `_NSGetExecutablePath' function. */
#define HAVE__NSGETEXECUTABLEPATH 1

/* Define to 1 if you have the `_pclose' function. */
/* #undef HAVE__PCLOSE */

/* Define to 1 if you have the `_popen' function. */
/* #undef HAVE__POPEN */

/* Define if you have the bzip2 library */
#define HasBZLIB 1

/* Define if you have Display Postscript */
/* #undef HasDPS */

/* Define if you have FlashPIX library */
/* #undef HasFPX */

/* Define if you have Ghostscript library */
/* #undef HasGS */

/* Define if you have JBIG library */
/* #undef HasJBIG */

/* Define if you have JPEG version 2 "Jasper" library */
/* #undef HasJP2 */

/* Define if you have JPEG library */
/* #undef HasJPEG */

/* Define if you have LCMS (v2.0 or later) library */
/* #undef HasLCMS */

/* Define if using libltdl to support dynamically loadable modules */
/* #undef HasLTDL */

/* Define if you have lzma compression library */
/* #undef HasLZMA */

/* Define if you have PNG library */
#define HasPNG 1

/* X11 server supports shape extension */
/* #undef HasShape */

/* X11 server supports shared memory extension */
/* #undef HasSharedMemory */

/* Define if you have TIFF library */
/* #undef HasTIFF */

/* Define if you have TRIO vsnprintf replacement library */
/* #undef HasTRIO */

/* Define if you have FreeType (TrueType font) library */
#define HasTTF 1

/* Define if you have umem memory allocation library */
/* #undef HasUMEM */

/* Define if you have WEBP library */
/* #undef HasWEBP */

/* Define to use the Windows GDI32 library */
/* #undef HasWINGDI32 */

/* Define if you have wmflite library */
/* #undef HasWMFlite */

/* Define if you have X11 library */
/* #undef HasX11 */

/* Define if you have XML library */
#define HasXML 1

/* Define if you have zlib compression library */
#define HasZLIB 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to prepend to default font search path. */
/* #undef MAGICK_FONT_PATH */

/* Command which returns total physical memory in bytes */
#define MAGICK_PHYSICAL_MEMORY_COMMAND "/usr/sbin/sysctl -n hw.physmem"

/* Target Host CPU */
#define MAGICK_TARGET_CPU x86_64

/* Target Host OS */
#define MAGICK_TARGET_OS darwin18.0.0

/* Target Host Vendor */
#define MAGICK_TARGET_VENDOR apple

/* define if the compiler lacks ios::binary */
/* #undef MISSING_STD_IOS_BINARY */

/* Directory where executables are installed. */
#define MagickBinPath "/usr/local/bin/"

/* Location of coder modules */
#define MagickCoderModulesPath "/usr/local/lib/GraphicsMagick-1.3.30/modules-Q8/coders/"

/* Subdirectory of lib where coder modules are installed */
#define MagickCoderModulesSubdir "GraphicsMagick-1.3.30/modules-Q8/coders"

/* Location of filter modules */
#define MagickFilterModulesPath "/usr/local/lib/GraphicsMagick-1.3.30/modules-Q8/filters/"

/* Subdirectory of lib where filter modules are installed */
#define MagickFilterModulesSubdir "GraphicsMagick-1.3.30/modules-Q8/filters"

/* Directory where architecture-dependent configuration files live. */
#define MagickLibConfigPath "/usr/local/lib/GraphicsMagick-1.3.30/config/"

/* Subdirectory of lib where architecture-dependent configuration files live.
   */
#define MagickLibConfigSubDir "GraphicsMagick-1.3.30/config"

/* Directory where architecture-dependent files live. */
#define MagickLibPath "/usr/local/lib/GraphicsMagick-1.3.30/"

/* Subdirectory of lib where GraphicsMagick architecture dependent files are
   installed */
#define MagickLibSubdir "GraphicsMagick-1.3.30"

/* Directory where architecture-independent configuration files live. */
#define MagickShareConfigPath "/usr/local/share/GraphicsMagick-1.3.30/config/"

/* Subdirectory of lib where architecture-independent configuration files
   live. */
#define MagickShareConfigSubDir "GraphicsMagick-1.3.30/config"

/* Directory where architecture-independent files live. */
#define MagickSharePath "/usr/local/share/GraphicsMagick-1.3.30/"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Prefix Magick library symbols with a common string. */
/* #undef PREFIX_MAGICK_SYMBOLS */

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Number of bits in a pixel Quantum (8/16/32) */
#define QuantumDepth 8

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Setjmp/longjmp are thread safe */
#define SETJMP_IS_THREAD_SAFE 1

/* The size of `off_t', as computed by sizeof. */
#define SIZEOF_OFF_T 8

/* The size of `signed int', as computed by sizeof. */
#define SIZEOF_SIGNED_INT 4

/* The size of `signed long', as computed by sizeof. */
#define SIZEOF_SIGNED_LONG 8

/* The size of `signed long long', as computed by sizeof. */
#define SIZEOF_SIGNED_LONG_LONG 8

/* The size of `signed short', as computed by sizeof. */
#define SIZEOF_SIGNED_SHORT 2

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 8

/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned int*', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INTP 8

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 8

/* The size of `unsigned long long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_LONG 8

/* The size of `unsigned short', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* GraphicsMagick is formally installed under prefix */
#define UseInstalledMagick 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Location of X11 configure files */
#define X11ConfigurePath "X11ConfigurePath"

/* Define to 1 if the X Window System is missing or not being used. */
#define X_DISPLAY_MISSING 1

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to 1 if type `char' is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
/* # undef __CHAR_UNSIGNED__ */
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#define restrict __restrict
/* Work around a bug in Sun C++: it does not support _Restrict or
   __restrict__, even though the corresponding Sun C compiler ends up with
   "#define restrict _Restrict" or "#define restrict __restrict__" in the
   previous line.  Perhaps some future version of Sun C++ will work with
   restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */
