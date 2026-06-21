#include "sj4compat.h"

int sj4_unlink(const char* path) {
	return unlink(path);
}
