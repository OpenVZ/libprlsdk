#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "dirent.h"

#define S_ISDIR(m) (((m) & (_S_IFMT)) == (_S_IFDIR))

DIR *opendir0(const char *name, int *errorp) {
  char *path;
  HANDLE h;
  WIN32_FIND_DATAA *fd;
  DIR *dir;
  int namlen;
  struct stat st;

  if (name == NULL) {
    return NULL;
  }

  if ((namlen = (int)strlen(name)) == -1) {
    return NULL;
  }

  if (stat(name, &st) == -1) {
    return NULL;
  }

  if (!S_ISDIR(st.st_mode)) {
    if (errorp) {
      *errorp = ENOTDIR;
    }

    return NULL;
  }

  if (name[namlen -1] == '\\' || name[namlen -1] == '/') {
    path = _alloca(namlen + 2);
    strcpy_s(path, namlen + 2, name);
    path[namlen] = '*';
    path[namlen + 1] = '\0';
  } else {
    path = _alloca(namlen + 3);
    strcpy_s(path, namlen + 3, name);

    path[namlen] = (strchr(name, '/') != NULL) ? '/' : '\\';
    path[namlen + 1] = '*';
    path[namlen + 2] = '\0';
  }

  if ((fd = malloc(sizeof(WIN32_FIND_DATA))) == NULL) {
    return NULL;
  }

  if ((h = FindFirstFileA(path, fd)) == INVALID_HANDLE_VALUE) {
    free(fd);
    return NULL;
  }

  if ((dir = malloc(sizeof(DIR))) == NULL) {
    FindClose(h);
    free(fd);
    return NULL;
  }

  dir->h = h;
  dir->fd = fd;
  dir->has_next = TRUE;

  return dir;
}

struct dirent *readdir(DIR *dir) {
  char *cFileName;
  char *d_name;

  if (dir == NULL) {
    return NULL;
  }

  if (dir->fd == NULL) {
    return NULL;
  }

  if (!dir->has_next) {
    return NULL;
  }

  cFileName = dir->fd->cFileName;
  d_name = dir->entry.d_name;
  strcpy_s(d_name, _MAX_PATH, cFileName);
  dir->has_next = FindNextFileA(dir->h, dir->fd);

  return &dir->entry;
}

int closedir(DIR *dir) {
  if (dir == NULL) {
    return -1;
  }

  if (dir->h && dir->h != INVALID_HANDLE_VALUE) {
    FindClose(dir->h);
  }

  if (dir->fd) {
    free(dir->fd);
  }

  free(dir);

  return 0;
}
