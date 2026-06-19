#include <sj_kanakan.h>

int main(int argc, char** argv) {
	char	    out[64 * 1024];
	int	    i = 0;
	char	    c[64 * 1024];
	char	    b;
	Sj4Context* ctx = alloccontext("dic.bin");

	if(ctx == NULL){
		fprintf(stderr, "Failed to create context\n");
		return 1;
	}

	c[0] = 0;

	while(fread(&b, 1, 1, stdin) == 1) {
		if(b == '\n') {
		} else if(b != '\r') {
			int l = strlen(c);

			c[l]	 = b;
			c[l + 1] = 0;

			continue;
		}

		i = 0;
		while((strlen(c) - i) > 0 && (i += cl2knj(ctx, (u_char*)c + i, strlen(c) - i, (u_char*)out))) {
			STDYOUT* s = (STDYOUT*)out;

			printf("> %s\n", out + sizeof(STDYOUT));
		}

		c[0] = 0;
	}

	free_context(ctx);

	return 0;
}
