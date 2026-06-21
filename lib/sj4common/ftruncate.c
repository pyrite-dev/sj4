#include "sj4compat.h"

int sj4_ftruncate(FILE* fp, size_t sz) {
#ifdef _WIN32
	return chsize(fileno(fp), sz);
#else
	return ftruncate(fileno(fp), sz);
#endif
}
