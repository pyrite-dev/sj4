#include <sj4ime.h>

#include <sj4lib.h>
#include <sj4charset.h>

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct sj4ime {
	Sj4Lib* lib;
	int	charset;

	Sj4Kouho     kouho;
	unsigned int last;

	u_char romabuf[SJ4BUFSZ];
	u_char kanabuf[SJ4BUFSZ];
};

static struct sj4table {
	const char* roma;
	u_char*	    kana;
	const char* roma2;
} kanatbl[] = {
#include "KanaTable"
};

Sj4Ime* sj4_ime(int charset, const char* dict) {
	Sj4Ime* ime;

	if((ime = calloc(1, sizeof(*ime))) == NULL) return NULL;

	if((ime->lib = sj4_open(charset, dict)) == NULL) {
		free(ime);

		return NULL;
	}

	ime->charset = charset;
}

static int w_len(wchar_t* wc) {
	int i;

	for(i = 0; wc[i] != 0; i++);

	return i;
}

static int b_len(int charset, Sj4KouhoBuffer* buffer) {
	if(charset == SJ4UTF16) {
		return w_len(buffer->utf16);
	} else {
		return strlen(buffer->raw);
	}
}

static void concat(int charset, u_char* out, u_char* in) {
	int i;

#ifdef UCS
	if(charset == SJ4UTF16) {
		wchar_t* w_out = (wchar_t*)out;

		i = w_len(w_out);

		w_out[i + sj4_charset_to(charset, (u_char*)(w_out + i), in, strlen(in))] = 0;
	} else
#else
	{
		i = strlen(out);

		out[i + sj4_charset_to(charset, out + i, in, strlen(in))] = 0;
	}
#endif
}

static void comp(Sj4Ime* ime) {
	ime->kanabuf[0] = 0;
	ime->last	= 0;
}

#define ZEROBUF memset(&ime->kouho.buffer, 0, sizeof(ime->kouho.buffer))

void sj4_ime_key(Sj4Ime* ime, int key) {
	if(key == '\n') {
		comp(ime);

		ZEROBUF;
	} else if(key == ' ') {
	reconvert:
		if((ime->last = b_len(ime->charset, &ime->kouho.buffer)) == 0) {
			sj4_getkan(ime->lib, ime->kanabuf, strlen(ime->kanabuf), &ime->kouho);
		} else {
			if((ime->last = sj4_nextkan(ime->lib)) == 0) {
				ZEROBUF;

				goto reconvert;
			}
		}

		printf("%s\n", ime->kouho.buffer.utf8);
	} else if('a' <= tolower(key) && tolower(key) <= 'z') {
		int i;

		if(ime->last != 0) comp(ime);

		key = toupper(key);

		if(ime->romabuf[strlen(ime->romabuf) - 1] == 'N' && !(key == 'A' || key == 'I' || key == 'U' || key == 'E' || key == 'O' || key == 'N')) {
			ime->romabuf[strlen(ime->romabuf) + 1] = 0;
			ime->romabuf[strlen(ime->romabuf)]     = 'N';
		}

		ime->romabuf[strlen(ime->romabuf) + 1] = 0;
		ime->romabuf[strlen(ime->romabuf)]     = key;

	reduce:
		for(i = 0; i < sizeof(kanatbl) / sizeof(kanatbl[0]); i++) {
			int n = strlen(kanatbl[i].roma);

			if(n > strlen(ime->romabuf)) continue;

			if(memcmp(ime->romabuf, kanatbl[i].roma, n) == 0) {
				int j;

				n = strlen(kanatbl[i].roma) - strlen(kanatbl[i].roma2);

				memmove(ime->romabuf - (n < 0 ? n : 0), ime->romabuf + (n > 0 ? n : 0), SJ4BUFSZ - n);
				if(n > 0) {
					for(j = SJ4BUFSZ - n; j < SJ4BUFSZ; j++) ime->romabuf[j] = 0;
				} else {
					for(j = 0; j < -n; j++) ime->romabuf[j] = 0;
				}

				memcpy(ime->romabuf, kanatbl[i].roma2, strlen(kanatbl[i].roma2));

				concat(ime->charset, ime->kanabuf, kanatbl[i].kana);

				goto reduce;
			}
		}
	}
}
