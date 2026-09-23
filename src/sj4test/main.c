#define UTF16
#include <sj4lib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef UTF16
#include <wchar.h>
#include <locale.h>
#endif

#ifdef UTF16
#define CHARSET SJ4UTF16
#define LEN wcslen
#else
#ifdef _WIN32
#define CHARSET SJ4SJIS
#else
#define CHARSET SJ4UTF8
#endif
#define LEN strlen
#endif

int main(int argc, char** argv) {
#ifdef UTF16
	wchar_t c[8 * 1024];
#else
	char c[8 * 1024];
#endif
	Sj4Lib* ctx = sj4_open(CHARSET, "sj4main.dic");
#ifdef UTF16
	unsigned short b;
#else
	char b;
#endif

	if(ctx == NULL) {
		fprintf(stderr, "Failed to create context\n");
		return 1;
	}

#ifdef UTF16
	setlocale(LC_ALL, "");
#endif

	c[0] = 0;

	while(fread(&b, 1, sizeof(b), stdin)) {
		int	 n, i;
		Sj4Kouho kouho;

		if(b == '\n') {
		} else if(b != '\r') {
			int l = LEN(c);

			c[l]	 = b;
			c[l + 1] = 0;

			continue;
		}

		i = 0;
		while((LEN(c) - i) > 0 && (n = sj4_getkan(ctx, c + i, LEN(c + i), &kouho)) != 0) {
			printf(">");
			do {
#ifdef UTF16
				printf(" %ls", kouho.buffer.utf16);
#else
#ifdef _WIN32
				printf(" %s", kouho.buffer.sjis);
#else
				printf(" %s", kouho.buffer.utf8);
#endif
#endif
			} while(sj4_nextkan(ctx) != 0);
			printf("\n");

			i += n;
		}
		c[0] = 0;
	}

	sj4_close(ctx);

	return 0;
}
