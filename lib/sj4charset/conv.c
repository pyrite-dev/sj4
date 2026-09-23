#include "sj4charset.h"

#include "sj_euc2ucs.h"
#include "sj_ucs2euc.h"
#include "sj_string.h"

#include <string.h>
#include <stdio.h>

#ifdef UCS
static u_int ucs_to_euc(u_int in) {
	if(in == 0x3094) in = 0x30f4;

	if(in <= 0xffff) {
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

int sj4_from_sjis(u_char* out, const u_char* in, int len) {
	unsigned char d[] = {0, 0};
	int	      uf  = 0;

	return sj4_str_sjistoeuc(out, 0xffffff, (u_char*)in, len, d, &uf);
}

#ifdef UCS
static int utf8_later(unsigned char c) {
	return 0x80 <= c && c < 0xc0;
}

#define CAST_I32(x) ((int)(x))

int sj4_utf8_read(const unsigned char* in, int* output) {
	u_char c = in[0];
	u_int  n = 0;

	if(c < 0x80) {
		*output = c;

		return 1;
	} else if(0xc2 <= c && c < 0xe0) {
		if(!utf8_later(in[1])) return 0;
		if((in[0] & 0x1e) == 0) return 0;

		n = CAST_I32(in[0] & 0x1f) << 6;
		n |= CAST_I32(in[1] & 0x3f);

		*output = n;

		return 2;
	} else if(0xe0 <= c && c < 0xf0) {
		if(!utf8_later(in[1]) || !utf8_later(in[2])) return 0;
		if((in[0] & 0x0f) == 0 && (in[1] & 0x20) == 0) return 0;

		n = CAST_I32(in[0] & 0x0f) << 12;
		n |= CAST_I32(in[1] & 0x3f) << 6;
		n |= CAST_I32(in[2] & 0x3f);

		*output = n;

		return 3;
	} else if(0xf0 <= c && c < 0xf8) {
		if(!utf8_later(in[1]) || !utf8_later(in[2]) || !utf8_later(in[3])) return 0;
		if((in[0] & 0x07) == 0 && (in[1] & 0x30) == 0) return 0;

		n = CAST_I32(in[0] & 0x07) << 18;
		n |= CAST_I32(in[1] & 0x3f) << 12;
		n |= CAST_I32(in[2] & 0x3f) << 6;
		n |= CAST_I32(in[3] & 0x3f);

		*output = n;

		return 4;
	}

	return 0;
}

int sj4_from_utf8(u_char* out, const u_char* in, int len) {
	const u_char* o_in  = in;
	u_char*	      o_out = out;

	while((in - o_in) < len) {
		int n;

		in += sj4_utf8_read(in, &n);

		n = ucs_to_euc(n);

		out += eucjp_write(out, n);
	}

	return out - o_out;
}

static int is_high_surr(wchar_t ch) {
	return 0xD800 <= ch && ch < 0xDC00;
}

static int is_low_urr(wchar_t ch) {
	return 0xDC00 <= ch && ch < 0xE000;
}

int sj4_utf16_read(const unsigned char* in, int* output) {
	const wchar_t* w_in = (const wchar_t*)in;

	if(is_high_surr(w_in[0])) {
		if(is_low_urr(w_in[1])) {
			*output = 0x10000 + CAST_I32(w_in[0] - 0xd800) * 0x400 + CAST_I32(w_in[1] - 0xdc00);

			return 2;
		} else if(w_in[1] == 0) {
			*output = w_in[0];

			return 2;
		} else {
			return 0;
		}
	} else if(is_low_urr(w_in[0])) {
		if(w_in[1] == 0) {
			*output = w_in[0];

			return 2;
		} else {
			return 0;
		}
	} else {
		*output = w_in[0];

		return 1;
	}
}

int sj4_from_utf16(u_char* out, const u_char* in, int len) {
	const wchar_t* w_in  = (const wchar_t*)in;
	const wchar_t* o_in  = w_in;
	u_char*	       o_out = out;

	while((w_in - o_in) < len) {
		int n = 0;

		w_in += sj4_utf16_read((const unsigned char*)w_in, &n);

		n = ucs_to_euc(n);

		out += eucjp_write(out, n);
	}

	return out - o_out;
}
#endif

int sj4_eucjp_read(const unsigned char* in, int* output) {
	u_char c = *in;
	u_int  n = 0;

	if(c <= 0x7f) {
		*output = c;

		return 1;
	} else if(c != 0x8f) {
		n = CAST_I32(in[0]) << 8;
		n |= CAST_I32(in[1]);

		*output = n;

		return 2;
	} else {
		n = CAST_I32(in[0]) << 16;
		n |= CAST_I32(in[1]) << 8;
		n |= CAST_I32(in[2]);

		*output = n;

		return 3;
	}
}

int sj4_sjis_read(const unsigned char* in, int* output) {
	u_char c;
	u_int  n = 0;

	c = in[0];

	if(c <= 0x7f || (0xa1 <= c && c <= 0xdf)) {
		*output = c;

		return 1;
	} else if((0x81 <= c && c <= 0x9f) || (0xe0 <= c && c <= 0xfc)) {
		n = CAST_I32(in[0]) << 8;
		n |= in[1];

		*output = n;

		return 2;
	}

	return 0;
}

int sj4_to_sjis(u_char* out, const u_char* in, int len) {
	unsigned char d[] = {0, 0};
	int	      uf  = 0;

	return sj4_str_euctosjis(out, 0xffffff, (u_char*)in, len, d, &uf);
}

#ifdef UCS
int sj4_to_utf8(u_char* out, const u_char* in, int len) {
	const u_char* o_in  = in;
	u_char*	      o_out = out;

	while((in - o_in) < len) {
		int i, j = 0;
		int n;

		in += sj4_eucjp_read(in, &n);

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

int sj4_to_utf16(u_char* out, const u_char* in, int len) {
	wchar_t*      w_out = (wchar_t*)out;
	const u_char* o_in  = in;
	wchar_t*      o_out = w_out;

	while((in - o_in) < len) {
		int i, j = 0;
		int n;

		in += sj4_eucjp_read(in, &n);

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

#ifdef UCS
#define CONV(type, to, from, len, mode) \
	if(type == SJ4EUCJP) { \
		memcpy(to, from, len); \
	} else if(type == SJ4SJIS) { \
		len = sj4_##mode##_sjis(to, from, len); \
	} else if(type == SJ4UTF8) { \
		len = sj4_##mode##_utf8(to, from, len); \
	} else if(type == SJ4UTF16) { \
		len = sj4_##mode##_utf16(to, from, len); \
	}
#else
#define CONV(type, to, from, len, mode) \
	if(type == SJ4EUCJP) { \
		memcpy(to, from, len); \
	} else if(type == SJ4SJIS) { \
		len = sj4_##mode##_sjis(to, from, len); \
	}
#endif

int sj4_charset_to(int type, void* output, const void* input, int len) {
	CONV(type, output, input, len, to);

	return len;
}

int sj4_charset_from(int type, void* output, const void* input, int len) {
	CONV(type, output, input, len, from);

	return len;
}

int sj4_charset_read(int type, int* output, const void* input) {
	if(type == SJ4EUCJP) {
		return sj4_eucjp_read(input, output);
	} else if(type == SJ4SJIS) {
		return sj4_sjis_read(input, output);
	}
#ifdef UCS
	else if(type == SJ4UTF8) {
		return sj4_utf8_read(input, output);
	} else if(type == SJ4UTF16) {
		return sj4_utf16_read(input, output);
	}
#endif
}
