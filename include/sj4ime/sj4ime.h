#ifndef __SJ4IME_H__
#define __SJ4IME_H__

#include <sj4charset.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sj4ime Sj4Ime;

typedef void (*Sj4ImePacket)(Sj4Ime* ime, int type, void* data);

enum SJ4IMEPACKET {
	Sj4ImeBeginKanList = 0,
	Sj4ImeKanEntry,
	Sj4ImeEndKanList
};

Sj4Ime* sj4_ime(int charset, const char* dict, Sj4ImePacket packet);
void	sj4_ime_key(Sj4Ime* ime, int key);
int	sj4_ime_length(Sj4Ime* ime);
void*	sj4_ime_kouho(Sj4Ime* ime);
void*	sj4_ime_kanabuf(Sj4Ime* ime);
void*	sj4_ime_convbuf(Sj4Ime* ime);

#ifdef __cplusplus
}
#endif

#endif
