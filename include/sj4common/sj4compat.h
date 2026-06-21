#ifndef __SJ4COMPAT_H__
#define __SJ4COMPAT_H__

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

int    sj4_snprintf(char*, size_t, const char*, ...);
size_t sj4_strlcpy(char*, const char*, size_t);

#endif
