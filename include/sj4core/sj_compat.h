#ifndef __SJ4_COMPAT_H__
#define __SJ4_COMPAT_H__

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(_MSC_VER)

#if defined(_MSC_VER) && _MSC_VER < 1900
static int __inline sj4_snprintf(char* buffer, size_t count, const char* fmt, ...){
	int st;
	va_list va;

	va_start(va, fmt);
	st = vsprintf(buffer, fmt, va);
	va_end(va);

	return st;
}
#else
#define sj4_snprintf snprintf
#endif

#define sj4_strlcpy(x, y, z) strcpy((x), (y))
#else
#define sj4_snprintf snprintf
#define sj4_strlcpy strlcpy
#endif

#endif
