/*
**  Copyright 2002-2003 University of Illinois Board of Trustees
**  Copyright 2002-2003 Mark D. Roth
**  All rights reserved.
**
**  internal.h - internal header file for libtar
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#include <config.h>
#include <compat.h>

#include <libtar.h>

// modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
#ifdef _WIN32

#include <direct.h>
#include <io.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)

#define S_ISREG(m)  (((m) & (_S_IFMT)) == (_S_IFREG))
#define S_ISDIR(m)  (((m) & (_S_IFMT)) == (_S_IFDIR))
#define S_ISCHR(m)  (((m) & (_S_IFMT)) == (_S_IFCHR))
#define S_ISBLK(m)  (0)
#define S_ISFIFO(m) (((m) & (_S_IFMT)) == (_S_IFIFO))
#define S_ISLNK(m)  (0)
#define S_ISSOCK(m) (0)

#define chown(p, o, g)         (0) 
#define geteuid()              (0)
#define lstat(p, b)            stat((p), (b))
#define makedev(maj, min)      (0)
#define mkdir(d, m)            _mkdir(d)
#define mkfifo(p, m)           (0)
#define mknod(p, m, d)         (0)
#define snprintf(s, n, f, ...) _snprintf((s), (n), (f), __VA_ARGS__)

#ifndef chmod
#define chmod					_chmod
#endif

#ifndef open
#define open					_open
#endif

#ifndef close
#define close					_close
#endif

#ifndef write
#define write					_write
#endif

#ifndef read
#define read					_read
#endif

#ifndef strdup
#define strdup					_strdup
#endif

#endif
