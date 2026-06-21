#include <sj4lib.h>
#include <sj_kanakan.h>
#include <sj_string.h>

#include <stdlib.h>

#ifdef UCS
#include <sj_euc2ucs.h>
#include <sj_ucs2euc.h>
#endif

struct sj4lib {
	Sj4Context* ctx;

	int charset;

	u_char inbuf[SJ4BUFSZ];
	u_char outbuf[SJ4BUFSZ];
	u_char wrkbuf[SJ4BUFSZ];
	u_char wrk2buf[SJ4BUFSZ];

	Sj4Kouho* kouho;
};

#ifdef UCS
static u_int ucs_to_euc(u_int in) {
	if(0 <= in && in <= 0xffff) {
		const T_U2E_BITMAP_INDEX* b = &utf16_to_euc_jp_table[(in >> 8) & 0xff];

		if(b->byType == 2) {
			b = &utf16_to_euc_jp_table[b->dwBitmapIndex + (in & 0xff)];

			if(b->byType == 3) {
				return b->dwEucJpCode;
			}
		}

		return in;
	}

	return 0;
}

static u_int euc_to_ucs(u_int in) {
	const T_E2U_BITMAP_INDEX* b = NULL;

	if(in <= 0xff) {
		b = &euc_jp_to_utf16_table[in];
		if(b->byType != 3) return 0;

		return b->dwUtf16Code;
	} else if(in <= 0xffff) {
		b = &euc_jp_to_utf16_table[(in >> 8) & 0xff];
		if(b->byType != 2) return 0;

		b = &euc_jp_to_utf16_table[b->dwBitmapIndex + ((in >> 0) & 0xff)];
		if(b->byType != 3) return 0;

		return b->dwUtf16Code;
	} else {
		b = &euc_jp_to_utf16_table[(in >> 16) & 0xff];
		if(b->byType != 2) return 0;

		b = &euc_jp_to_utf16_table[b->dwBitmapIndex + ((in >> 8) & 0xff)];
		if(b->byType != 2) return 0;

		b = &euc_jp_to_utf16_table[b->dwBitmapIndex + ((in >> 0) & 0xff)];
		if(b->byType != 3) return 0;

		return b->dwUtf16Code;
	}

	return 0;
}

static int eucjp_write(u_char* out, u_int n) {
	if(n <= 0xff) {
		*out++ = n;

		return 1;
	} else if(n <= 0xffff) {
		*out++ = (n >> 8) & 0xff;
		*out++ = (n >> 0) & 0xff;

		return 2;
	} else {
		*out++ = (n >> 16) & 0xff;
		*out++ = (n >> 8) & 0xff;
		*out++ = (n >> 0) & 0xff;

		return 3;
	}
}
#endif

static int from_sjis(u_char* out, const u_char* in, int len) {
	unsigned char d[] = {0, 0};
	int	      uf  = 0;

	return sj4_str_sjistoeuc(out, SJ4BUFSZ, (u_char*)in, len, d, &uf);
}

#ifdef UCS
static int utf8_later(unsigned char c) {
	return 0x80 <= c && c < 0xc0;
}

#define CAST_I32(x) ((int)(x))

static int from_utf8(u_char* out, const u_char* in, int len) {
	const u_char* o_in  = in;
	u_char*	      o_out = out;

	while((in - o_in) < len) {
		u_char c = *in;
		u_int  n = 0;

		if(c < 0x80) {
			n = c;

			in += 1;
		} else if(0xc2 <= c && c < 0xe0) {
			if(!utf8_later(in[1])) return 0;
			if((in[0] & 0x1e) == 0) return 0;

			n = CAST_I32(in[0] & 0x1f) << 6;
			n |= CAST_I32(in[1] & 0x3f);

			in += 2;
		} else if(0xe0 <= c && c < 0xf0) {
			if(!utf8_later(in[1]) || !utf8_later(in[2])) return 0;
			if((in[0] & 0x0f) == 0 && (in[1] & 0x20) == 0) return 0;

			n = CAST_I32(in[0] & 0x0f) << 12;
			n |= CAST_I32(in[1] & 0x3f) << 6;
			n |= CAST_I32(in[2] & 0x3f);

			in += 3;
		} else if(0xf0 <= c && c < 0xf8) {
			if(!utf8_later(in[1]) || !utf8_later(in[2]) || !utf8_later(in[3])) return 0;
			if((in[0] & 0x07) == 0 && (in[1] & 0x30) == 0) return 0;

			n = CAST_I32(in[0] & 0x07) << 18;
			n |= CAST_I32(in[1] & 0x3f) << 12;
			n |= CAST_I32(in[2] & 0x3f) << 6;
			n |= CAST_I32(in[3] & 0x3f);

			in += 4;
		}

		n = ucs_to_euc(n);

		out += eucjp_write(out, n);
	}

	return out - o_out;
}

int is_high_surr(wchar_t ch) {
	return 0xD800 <= ch && ch < 0xDC00;
}

int is_low_urr(wchar_t ch) {
	return 0xDC00 <= ch && ch < 0xE000;
}

static int from_utf16(u_char* out, const u_char* in, int len) {
	const wchar_t* w_in  = (const wchar_t*)in;
	const wchar_t* o_in  = w_in;
	u_char*	       o_out = out;

	while((w_in - o_in) < len) {
		u_int n = 0;

		if(is_high_surr(w_in[0])) {
			if(is_low_urr(w_in[1])) {
				n = 0x10000 + CAST_I32(w_in[0] - 0xd800) * 0x400 + CAST_I32(w_in[1] - 0xdc00);

				w_in += 2;
			} else if(w_in[1] == 0) {
				n = w_in[0];

				w_in += 2;
			} else {
				return 0;
			}
		} else if(is_low_urr(w_in[0])) {
			if(w_in[1] == 0) {
				n = w_in[0];

				w_in += 2;
			} else {
				return 0;
			}
		} else {
			n = w_in[0];

			w_in += 1;
		}

		n = ucs_to_euc(n);

		out += eucjp_write(out, n);
	}

	return out - o_out;
}

static u_int eucjp_read(const u_char* in) {
	u_char c = *in;
	u_int  n = 0;

	if(c <= 0x7f) {
		n = c;
	} else if(c != 0x8f) {
		n = CAST_I32(in[0]) << 8;
		n |= CAST_I32(in[1]);
	} else {
		n = CAST_I32(in[0]) << 16;
		n |= CAST_I32(in[1]) << 8;
		n |= CAST_I32(in[2]);
	}

	return n;
}

static int eucjp_codesize(u_int n) {
	if(n <= 0xff) {
		return 1;
	} else if(n <= 0xffff) {
		return 2;
	} else {
		return 3;
	}
}
#endif

static int to_sjis(u_char* out, const u_char* in, int len) {
	unsigned char d[] = {0, 0};
	int	      uf  = 0;

	return sj4_str_euctosjis(out, SJ4BUFSZ, (u_char*)in, len, d, &uf);
}

#ifdef UCS
static int to_utf8(u_char* out, const u_char* in, int len) {
	const u_char* o_in  = in;
	u_char*	      o_out = out;

	while((in - o_in) < len) {
		int   i, j = 0;
		u_int n = eucjp_read(in);

		in += eucjp_codesize(n);

		n = euc_to_ucs(n);

		if(n >= 0x10000) {
			*out++ = ((n >> (6 * 3)) & 7) | 0xf0;

			j = 3;
		} else if(n >= 0x800) {
			*out++ = ((n >> (6 * 2)) & 15) | 0xe0;

			j = 2;
		} else if(n >= 0x80) {
			*out++ = ((n >> (6 * 1)) & 31) | 0xc0;

			j = 1;
		} else {
			*out++ = n;
		}

		for(i = 0; i < j; i++) {
			*out++ = ((n >> (6 * (j - i - 1))) & 63) | 0x80;
		}
	}

	return out - o_out;
}

static int to_utf16(u_char* out, const u_char* in, int len) {
	wchar_t*      w_out = (wchar_t*)out;
	const u_char* o_in  = in;
	wchar_t*      o_out = w_out;

	while((in - o_in) < len) {
		int   i, j = 0;
		u_int n = eucjp_read(in);

		in += eucjp_codesize(n);

		n = euc_to_ucs(n);

		if(n < 0x10000) {
			*w_out++ = n;
		} else {
			*w_out++ = (n - 0x10000) / 0x400 + 0xd800;
			*w_out++ = (n - 0x10000) % 0x400 + 0xdc00;
		}
	}

	return w_out - o_out;
}
#endif

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
		len = mode##_sjis(to, from, len); \
	} else if(ctx->charset == SJ4UTF8) { \
		len = mode##_utf8(to, from, len); \
	} else if(ctx->charset == SJ4UTF16) { \
		len = mode##_utf16(to, from, len); \
	}
#else
#define ICONV(to, from, len, mode) \
	if(ctx->charset == SJ4EUCJP) { \
		memcpy(to, from, len); \
	} else if(ctx->charset == SJ4SJIS) { \
		len = mode##_sjis(to, from, len); \
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
