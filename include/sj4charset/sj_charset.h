#ifndef __SJ_CHARSET_H__
#define __SJ_CHARSET_H__

#include <sj_types.h>

#include <wchar.h>

enum SJ4CHARSET {
	SJ4EUCJP = 0,
	SJ4SJIS,
	SJ4UTF8,
	SJ4UTF16
};

int sj4_from_sjis(u_char* out, const u_char* in, int len);
int sj4_from_utf8(u_char* out, const u_char* in, int len);
int sj4_from_utf16(u_char* out, const u_char* in, int len);
int sj4_to_sjis(u_char* out, const u_char* in, int len);
int sj4_to_utf8(u_char* out, const u_char* in, int len);
int sj4_to_utf16(u_char* out, const u_char* in, int len);

int sj4_charset_to(int type, void* output, const void* input, int len);
int sj4_charset_from(int type, void* output, const void* input, int len);

#endif
