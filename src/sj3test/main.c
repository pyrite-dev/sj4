#include <sj_kanakan.h>

int main(int argc, char** argv) {
	DictFile* dict;
	DICTL*	  node;
	char	  out[64 * 1024];
	int	  i = 0;
	char	  c[64 * 1024];
	char	  b;

	work_base = calloc(1, sizeof(Global));
	node	  = calloc(1, sizeof(DICTL));

	dictlist = node;

	initwork();

	dict = opendict("dic.bin", "");

	node->dict = &dict->dict;
	node->next = NULL;

	seldict(dict->dict.dicid);

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
		while((strlen(c) - i) > 0 && (i += cl2knj(c + i, strlen(c) - i, out))) {
			STDYOUT* s = (STDYOUT*)out;

			printf("> %s\n", out + sizeof(STDYOUT));
		}

		c[0] = 0;
	}
}
