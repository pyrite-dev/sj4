#include <sj4ime.h>
#include <stdio.h>

#ifdef _WIN32
#define CHARSET SJ4SJIS
#else
#define CHARSET SJ4UTF8
#endif

int main() {
	Sj4Ime* ime = sj4_ime(CHARSET, "sj4main.dic");
	char	b;

	if(ime == NULL) {
		fprintf(stderr, "Failed to create context\n");
		return 1;
	}

	while(fread(&b, 1, sizeof(b), stdin)) {
		sj4_ime_key(ime, b);
	}
}
