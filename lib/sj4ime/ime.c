#include <sj4ime.h>

#include <stdlib.h>

struct sj4ime {
	Sj4Lib* lib;
};

Sj4Ime* sj4_ime(int charset, const char* dict) {
	Sj4Ime* ime;

	if((ime = calloc(1, sizeof(*ime))) == NULL) return NULL;

	if((ime->lib = sj4_open(charset, dict)) == NULL) {
		free(ime);

		return NULL;
	}
}
