#include <sj4lib.h>
#include <sj_kanakan.h>

#include <stdlib.h>

#include <sj_euc2ucs.h>
#include <sj_ucs2euc.h>

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

static int from_sjis(u_char* out, const u_char* in, int len) {
}

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

		if(n <= 0xff) {
			*out++ = n;
		} else if(n <= 0xffff) {
			*out++ = (n >> 8) & 0xff;
			*out++ = (n >> 0) & 0xff;
		} else {
			*out++ = (n >> 16) & 0xff;
			*out++ = (n >> 8) & 0xff;
			*out++ = (n >> 0) & 0xff;
		}
	}

	return out - o_out;
}

static int from_utf16(u_char* out, const u_char* in, int len) {
}

static int to_sjis(u_char* out, const u_char* in, int len) {
}

static int to_utf8(u_char* out, const u_char* in, int len) {
	const u_char* o_in  = in;
	u_char*	      o_out = out;

	while((in - o_in) < len) {
		u_char c = *in;
		u_int  n = 0;

		if(c <= 0x7f) {
			n = c;

			in += 1;
		} else if(c != 0x8f) {
			n = CAST_I32(in[0]) << 8;
			n |= CAST_I32(in[1]);

			in += 2;
		} else {
			n = CAST_I32(in[0]) << 16;
			n |= CAST_I32(in[1]) << 8;
			n |= CAST_I32(in[2]);

			in += 3;
		}
	}

	return out - o_out;
}

static int to_utf16(u_char* out, const u_char* in, int len) {
}

struct sj4lib {
	Sj4Context* ctx;

	int charset;

	u_char inbuf[SJ4BUFSZ];
	u_char outbuf[SJ4BUFSZ];
};

Sj4Lib* sj4_open(int charset, const char* dic) {
	Sj4Lib* ctx;

	if((ctx = calloc(1, sizeof(*ctx))) == NULL) return NULL;

	if((ctx->ctx = alloccontext(dic)) == NULL) {
		free(ctx);

		return NULL;
	}

	ctx->charset = charset;

	return ctx;
}

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

int sj4_getkan(Sj4Lib* ctx, const void* input, int len, Sj4Kouho* kouho) {
	memset(kouho, 0, sizeof(*kouho));

	ICONV(ctx->inbuf, input, len, from);

	len = cl2knj(ctx->ctx, ctx->inbuf, len, ctx->outbuf);

	ICONV(kouho->buffer.raw, ctx->outbuf, len, to);

	return len;
}

void sj4_close(Sj4Lib* ctx) {
	free(ctx);
}
