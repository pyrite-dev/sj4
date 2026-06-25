#ifndef __SJ_CHARSET_H__
#define __SJ_CHARSET_H__

#include <sj_types.h>

#include <wchar.h>

int sj4_from_sjis(u_char* out, const u_char* in, int len);
int sj4_from_utf8(u_char* out, const u_char* in, int len);
int sj4_from_utf16(u_char* out, const u_char* in, int len);
int sj4_to_sjis(u_char* out, const u_char* in, int len);
int sj4_to_utf8(u_char* out, const u_char* in, int len);
int sj4_to_utf16(u_char* out, const u_char* in, int len);

#endif
