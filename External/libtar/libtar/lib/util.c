/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  All rights reserved.
**
**  util.c - miscellaneous utility code for libtar
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#include <internal.h>

#include <stdio.h>
#include <sys/param.h>
#include <errno.h>

#ifdef STDC_HEADERS
# include <string.h>
#endif


/* hashing function for pathnames */
int
path_hashfunc(char *key, int numbuckets)
{
	char buf[MAXPATHLEN];
	char *p;

	strcpy(buf, key);
	p = basename(buf);

	return (((unsigned int)p[0]) % numbuckets);
}


/* matching function for dev_t's */
int
dev_match(dev_t *dev1, dev_t *dev2)
{
	return !memcmp(dev1, dev2, sizeof(dev_t));
}


/* matching function for ino_t's */
int
ino_match(ino_t *ino1, ino_t *ino2)
{
	return !memcmp(ino1, ino2, sizeof(ino_t));
}


/* hashing function for dev_t's */
int
dev_hash(dev_t *dev)
{
	return *dev % 16;
}


/* hashing function for ino_t's */
int
ino_hash(ino_t *inode)
{
	return *inode % 256;
}


/*
** mkdirhier() - create all directories in a given path
** returns:
**	0			success
**	1			all directories already exist
**	-1 (and sets errno)	error
*/
int
mkdirhier(char *path)
{
	char src[MAXPATHLEN], dst[MAXPATHLEN] = "";
	char *dirp, *nextp = src;
	int retval = 1;

	if (strlcpy(src, path, sizeof(src)) > sizeof(src))
	{
		errno = ENAMETOOLONG;
		return -1;
	}

	if (path[0] == '/')
		strcpy(dst, "/");

	while ((dirp = strsep(&nextp, "/")) != NULL)
	{
		if (*dirp == '\0')
			continue;

		if (dst[0] != '\0')
			strcat(dst, "/");
		strcat(dst, dirp);

		if (mkdir(dst, 0777) == -1)
		{
			if (errno != EEXIST && errno != EACCES)
				return -1;
		}
		else
			retval = 0;
	}

	return retval;
}


/* calculate header checksum */
int
th_crc_calc(TAR *t)
{
	int i, sum = 0;

	for (i = 0; i < T_BLOCKSIZE; i++)
		sum += ((unsigned char *)(&(t->th_buf)))[i];
	for (i = 0; i < 8; i++)
		sum += (' ' - (unsigned char)t->th_buf.chksum[i]);

	return sum;
}


/* string-octal to integer conversion */
int
oct_to_int(char *oct)
{
	int i;

	sscanf(oct, "%o", &i);

	return i;
}


/* integer to string-octal conversion, no NULL */
void
int_to_oct_nonull(int num, char *oct, size_t octlen)
{
	// modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
	//snprintf(oct, octlen, "%*lo", octlen - 1, (unsigned long)num);
	//oct[octlen - 1] = ' ';
	snprintf(oct, octlen, "%0*lo", (int)(octlen - 1), (unsigned long)num);
	oct[octlen - 1] = 0;
}


