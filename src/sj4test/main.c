#include <sj4lib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define CHARSET SJ4SJIS
#else
#define CHARSET SJ4UTF8
#endif

int main(int argc, char** argv) {
	char	c[8 * 1024];
	Sj4Lib* ctx = sj4_open(CHARSET, "sj4main.dic");
	char	b;

	if(ctx == NULL) {
		fprintf(stderr, "Failed to create context\n");
		return 1;
	}

	c[0] = 0;

	while(fread(&b, 1, sizeof(b), stdin)) {
		int	 n, i;
		Sj4Kouho kouho;

		if(b == '\n') {
		} else if(b != '\r') {
			int l = strlen(c);

			c[l]	 = b;
			c[l + 1] = 0;

			continue;
		}

		i = 0;
		while((strlen(c) - i) > 0 && (n = sj4_getkan(ctx, c + i, strlen(c + i), &kouho)) != 0) {
			printf(">");
			do {
				printf(" %s", kouho.buffer.utf8);
			} while(sj4_nextkan(ctx) != 0);
			printf("\n");

			i += n;
		}
		c[0] = 0;
	}

	sj4_close(ctx);

	return 0;
}
