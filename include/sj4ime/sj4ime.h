#ifndef __SJ4IME_H__
#define __SJ4IME_H__

#include <sj4charset.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sj4ime Sj4Ime;

Sj4Ime* sj4_ime(int charset, const char* dict);
void	sj4_ime_key(Sj4Ime* ime, int key);

#ifdef __cplusplus
}
#endif

#endif
