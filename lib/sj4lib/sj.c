#include <sj4lib.h>
#include <sj_kanakan.h>
#include <sj_string.h>

#include <sj_charset.h>

#include <stdlib.h>

struct sj4lib {
	Sj4Context* ctx;

	int charset;

	u_char inbuf[SJ4BUFSZ];
	u_char outbuf[SJ4BUFSZ];
	u_char wrkbuf[SJ4BUFSZ];
	u_char wrk2buf[SJ4BUFSZ];

	Sj4Kouho* kouho;
};

Sj4Lib* sj4_open(int charset, const char* dic) {
	Sj4Lib* ctx;

#ifndef UCS
	if(charset == SJ4UTF8 || charset == SJ4UTF16) return NULL;
#endif

	if((ctx = calloc(1, sizeof(*ctx))) == NULL) return NULL;

	if((ctx->ctx = alloccontext(dic)) == NULL) {
		free(ctx);

		return NULL;
	}

	ctx->charset = charset;

	return ctx;
}

#ifdef UCS
#define ICONV(to, from, len, mode) \
	if(ctx->charset == SJ4EUCJP) { \
		memcpy(to, from, len); \
	} else if(ctx->charset == SJ4SJIS) { \
		len = sj4_##mode##_sjis(to, from, len); \
	} else if(ctx->charset == SJ4UTF8) { \
		len = sj4_##mode##_utf8(to, from, len); \
	} else if(ctx->charset == SJ4UTF16) { \
		len = sj4_##mode##_utf16(to, from, len); \
	}
#else
#define ICONV(to, from, len, mode) \
	if(ctx->charset == SJ4EUCJP) { \
		memcpy(to, from, len); \
	} else if(ctx->charset == SJ4SJIS) { \
		len = sj4_##mode##_sjis(to, from, len); \
	}
#endif

#define RUNME(PROC) \
	PROC \
\
	    len2 = strlen(ctx->outbuf + sizeof(STDYOUT)); \
	ICONV(ctx->kouho->buffer.raw, ctx->outbuf + sizeof(STDYOUT), len2, to); \
\
	memcpy(ctx->wrkbuf, ctx->inbuf, len); \
	ICONV(ctx->wrk2buf, ctx->wrkbuf, len, to); \
	if(ctx->charset == SJ4UTF16) { \
		int i; \
		for(i = 0; i < sizeof(wchar_t); i++) ctx->wrk2buf[len * sizeof(wchar_t) + i] = 0; \
	} else { \
		ctx->wrk2buf[len] = 0; \
	} \
\
	if(ctx->charset == SJ4UTF16) { \
		ctx->kouho->buffer.utf16[len2] = 0; \
	} else { \
		ctx->kouho->buffer.raw[len2] = 0; \
	} \
\
	return ctx->charset == SJ4UTF16 ? wcslen((wchar_t*)ctx->wrk2buf) : strlen(ctx->wrk2buf);

#define RUNME2(PROC) \
	ICONV(ctx->inbuf, input, len, from); \
\
	RUNME(PROC)

int sj4_getkan(Sj4Lib* ctx, const void* input, int len, Sj4Kouho* kouho) {
	int len2;

	memset(kouho, 0, sizeof(*kouho));
	ctx->kouho = kouho;

	RUNME2({
		len = cl2knj(ctx->ctx, ctx->inbuf, len, ctx->outbuf);
	});
}

int sj4_nextkan(Sj4Lib* ctx) {
	int len, len2;

	RUNME({
		len = nextcl(ctx->ctx, ctx->outbuf, 0);
	});
}

void sj4_close(Sj4Lib* ctx) {
	free_context(ctx->ctx);
	free(ctx);
}
