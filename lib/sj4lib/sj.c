#include <sj4lib.h>
#include <sj_kanakan.h>
#include <sj_string.h>

#include <sj4charset.h>

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

#define RUNME(PROC) \
	PROC \
\
	    len2 = sj4_charset_to(ctx->charset, ctx->kouho->buffer.raw, ctx->outbuf + sizeof(STDYOUT), strlen(ctx->outbuf + sizeof(STDYOUT))); \
\
	memcpy(ctx->wrkbuf, ctx->inbuf, len); \
	len = sj4_charset_to(ctx->charset, ctx->wrk2buf, ctx->wrkbuf, len); \
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

int sj4_getkan(Sj4Lib* ctx, const void* input, int len, Sj4Kouho* kouho) {
	int len2;

	memset(kouho, 0, sizeof(*kouho));
	ctx->kouho = kouho;

	len = sj4_charset_from(ctx->charset, ctx->inbuf, input, len);

	RUNME({
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
