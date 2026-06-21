#include <sj4lib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main(int argc, char** argv) {
	char	c[8 * 1024];
	Sj4Lib* ctx = sj4_open(SJ4UTF8, "sj4main.dic");
	char	b;

	if(ctx == NULL) {
		fprintf(stderr, "Failed to create context\n");
		return 1;
	}

	c[0] = 0;

	setlocale(LC_ALL, "");

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
		while((strlen(c) - i) > 0 && (n = sj4_getkan(ctx, c + i, strlen(c + i), &kouho))) {
			printf("> %s\n", kouho.buffer.sjis);

			i += n;
		}
		c[0] = 0;
	}

	sj4_close(ctx);

	return 0;
}
