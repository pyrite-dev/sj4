#include <sj_kanakan.h>

int main(int argc, char** argv) {
	char	    out[64 * 1024];
	int	    i = 0;
	char	    c[64 * 1024];
	char	    b;
	Sj4Context* ctx = alloccontext("sj4main.dic");

	if(ctx == NULL) {
		fprintf(stderr, "Failed to create context\n");
		return 1;
	}

	c[0] = 0;

	while(fread(&b, 1, 1, stdin) == 1) {
		int n;

		if(b == '\n') {
		} else if(b != '\r') {
			int l = strlen(c);

			c[l]	 = b;
			c[l + 1] = 0;

			continue;
		}

		i = 0;
		while((strlen(c) - i) > 0 && (i += (n = cl2knj(ctx, (u_char*)c + i, strlen(c) - i, (u_char*)out)))) {
			STDYOUT* s   = (STDYOUT*)out;
			char*	 str = malloc(n + 1);

			memcpy(str, c + i - n, n);
			str[i] = 0;

			printf("> %s -> %s\n", str, out + sizeof(STDYOUT));

			free(str);
		}

		c[0] = 0;
	}

	free_context(ctx);

	return 0;
}
