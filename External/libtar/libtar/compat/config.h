/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if your system has a working basename */
/* #undef HAVE_BASENAME */

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if the system has the type `dev_t'. */
/* #undef HAVE_DEV_T */

/* Define if your system has a working dirname */
/* #undef HAVE_DIRNAME */

/* Define to 1 if your system has a working POSIX `fnmatch' function. */
/* #undef HAVE_FNMATCH */

/* Define to 1 if you have the <fnmatch.h> header file. */
/* #undef HAVE_FNMATCH_H */

/* Define to 1 if you have the <inttypes.h> header file. */
/* #undef HAVE_INTTYPES_H */

/* Define to 1 if you have the `lchown' function. */
/* #undef HAVE_LCHOWN */

/* Define to 1 if you have the <libgen.h> header file. */
/* #undef HAVE_LIBGEN_H */

/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_LIBZ 1

/* Define to 1 if the system has the type `major_t'. */
/* #undef HAVE_MAJOR_T */

/* Define to 1 if you have the <memory.h> header file. */
/* #undef HAVE_MEMORY_H */

/* Define to 1 if the system has the type `minor_t'. */
/* #undef HAVE_MINOR_T */

/* Define to 1 if the system has the type `nlink_t'. */
/* #undef HAVE_NLINK_T */

/* Define if your system has a working snprintf */
#define HAVE_SNPRINTF 1

/* Define to 1 if the system has the type `socklen_t'. */
/* #undef HAVE_SOCKLEN_T */

/* Define to 1 if you have the <stdint.h> header file. */
/* #undef HAVE_STDINT_H */

/* Define to 1 if you have the <stdlib.h> header file. */
/* #undef HAVE_STDLIB_H */

/* Define if you have the strdup function */
/* #undef HAVE_STRDUP */

/* Define to 1 if you have the `strftime' function. */
/* #undef HAVE_STRFTIME */

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
/* #undef HAVE_STRING_H */

/* Define if you have the strlcpy function */
/* #undef HAVE_STRLCPY */

/* Define if you have the strmode function */
#ifdef _MAC_
#define HAVE_STRMODE 1
#endif //_MAC_
/* #undef HAVE_STRMODE */

/* Define if you have the strsep function */
/* #undef HAVE_STRSEP */
#ifdef _LIN_
#define HAVE_STRSEP 1
#endif //_LIN_
/* Define to 1 if you have the <sys/stat.h> header file. */
/* #undef HAVE_SYS_STAT_H */

/* Define to 1 if you have the <sys/types.h> header file. */
/* #undef HAVE_SYS_TYPES_H */

/* Define to 1 if the system has the type `uint64_t'. */
/* #undef HAVE_UINT64_T */

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef _WIN_
#define HAVE_UNISTD_H 1
#endif //_WIN_

/* Define to 1 if `major', `minor', and `makedev' are declared in <mkdev.h>.
   */
/* #undef MAJOR_IN_MKDEV */

/* Define to 1 if `major', `minor', and `makedev' are declared in
   <sysmacros.h>. */
/* #undef MAJOR_IN_SYSMACROS */

/* Define as 1 if makedev expects three arguments */
/* #undef MAKEDEV_THREE_ARGS */

/* Define if you want to use the basename function */
#define NEED_BASENAME 1

/* Define if you want to use the dirname function */
#define NEED_DIRNAME 1

/* Define if you want to use the fnmatch function */
#define NEED_FNMATCH 1

/* Define if you want to use the glob function */
#define NEED_GLOB 1

/* Define if you want to use the makedev function */
#define NEED_MAKEDEV 1

/* Define if you want to use the snprintf function */
/* #undef NEED_SNPRINTF */


/* Define if you want to use the strdup function */
/* #undef NEED_STRDUP */

/* Define if you want to use the strlcpy function */
#define NEED_STRLCPY 1

/* Define if you want to use the strmode function */
#define NEED_STRMODE 1

/* Define if you want to use the strsep function */
#define NEED_STRSEP 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "libtar"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libtar 1.2.11"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libtar"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.2.11"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
/* #ifndef _ALL_SOURCE */
/* # undef _ALL_SOURCE */
/* #endif */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `unsigned long' if not defined in system header files. */
/* #undef dev_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to `unsigned int' if not defined in system header files. */
/* #undef major_t */

/* Define to `unsigned int' if not defined in system header files. */
/* #undef minor_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to `unsigned short' if not defined in system header files. */
/* #undef nlink_t */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `unsigned long' if not defined in system header files. */
/* #undef socklen_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define to `long long' if not defined in system header files. */
/* #undef uint64_t */
