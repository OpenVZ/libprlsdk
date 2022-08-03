#ifndef __DIRENT_H__
#define __DIRENT_H__

#include <windows.h>
#include <sys/types.h>

struct dirent {
  ino_t d_ino;
  char  d_name[_MAX_PATH];
};

typedef struct {
  HANDLE h;
  WIN32_FIND_DATAA *fd;
  BOOL has_next;
  struct dirent entry;
} DIR;

DIR *opendir0(const char *name, int *errorp);
#define opendir(n) opendir0((n), NULL)
struct dirent *readdir(DIR *dir);
int closedir(DIR *dir);

#endif
