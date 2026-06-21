#ifndef __SJ4LIB_H__
#define __SJ4LIB_H__

#include <wchar.h>

/* increase this as needed... */
#define SJ4BUFSZ (16 * 1024)

typedef union sj4kouhobuffer {
	char	sjis[SJ4BUFSZ];
	char	eucjp[SJ4BUFSZ];
	char	utf8[SJ4BUFSZ];
	wchar_t utf16[SJ4BUFSZ / sizeof(wchar_t)];

	unsigned char raw[SJ4BUFSZ];
	wchar_t	      wraw[SJ4BUFSZ / sizeof(wchar_t)];
} Sj4KouhoBuffer;

typedef struct sj4kouho {
	Sj4KouhoBuffer buffer;
} Sj4Kouho;

typedef struct sj4lib Sj4Lib;

enum SJ4CHARSET {
	SJ4EUCJP = 0,
	SJ4SJIS,
	SJ4UTF8,
	SJ4UTF16
};

Sj4Lib* sj4_open(int, const char*);
int	sj4_getkan(Sj4Lib*, const void*, int len, Sj4Kouho*);
void	sj4_close(Sj4Lib*);

#endif
