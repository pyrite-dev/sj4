#ifndef __SJ4COMPAT_H__
#define __SJ4COMPAT_H__

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

int    sj4_snprintf(char*, size_t, const char*, ...);
size_t sj4_strlcpy(char*, const char*, size_t);
int    sj4_ftruncate(FILE*, size_t);

#endif
