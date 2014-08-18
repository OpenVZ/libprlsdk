/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  All rights reserved.
**
**  decode.c - libtar code to decode tar header blocks
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#include <internal.h>

#include <stdio.h>
#include <sys/param.h>
#include <pwd.h>
#include <grp.h>

#ifdef STDC_HEADERS
# include <string.h>
#endif


/* determine full path name */
char *
th_get_pathname(TAR *t)
{
	char filename[MAXPATHLEN];

	if (t->th_buf.gnu_longname)
		return t->th_buf.gnu_longname;

	if (t->th_buf.prefix[0] != '\0')
	{
		snprintf(filename, sizeof(filename), "%.155s/%.100s",
			 t->th_buf.prefix, t->th_buf.name);
		return strdup(filename);
	}

	snprintf(filename, sizeof(filename), "%.100s", t->th_buf.name);
	return strdup(filename);
}


uid_t
th_get_uid(TAR *t)
{
	int uid;
#ifndef _WIN32 // modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
	struct passwd *pw;

	pw = getpwnam(t->th_buf.uname);
	if (pw != NULL)
		return pw->pw_uid;
#endif

	/* if the password entry doesn't exist */
	sscanf(t->th_buf.uid, "%o", &uid);
	return uid;
}


gid_t
th_get_gid(TAR *t)
{
	int gid;
#ifndef _WIN32 // modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
	struct group *gr;

	gr = getgrnam(t->th_buf.gname);
	if (gr != NULL)
		return gr->gr_gid;
#endif

	/* if the group entry doesn't exist */
	sscanf(t->th_buf.gid, "%o", &gid);
	return gid;
}


mode_t
th_get_mode(TAR *t)
{
	mode_t mode;

	mode = (mode_t)oct_to_int(t->th_buf.mode);
	if (! (mode & S_IFMT))
	{
		switch (t->th_buf.typeflag)
		{
#ifndef _WIN32 // modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
		case SYMTYPE:
			mode |= S_IFLNK;
			break;
#endif
		case CHRTYPE:
			mode |= S_IFCHR;
			break;
#ifndef _WIN32 // modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
		case BLKTYPE:
			mode |= S_IFBLK;
			break;
#endif
		case DIRTYPE:
			mode |= S_IFDIR;
			break;
#ifndef _WIN32 // modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
		case FIFOTYPE:
			mode |= S_IFIFO;
			break;
#endif
		case AREGTYPE:
			if (t->th_buf.name[strlen(t->th_buf.name) - 1] == '/')
			{
				mode |= S_IFDIR;
				break;
			}
			/* FALLTHROUGH */
		case LNKTYPE:
		case REGTYPE:
		default:
			mode |= S_IFREG;
		}
	}

	return mode;
}


