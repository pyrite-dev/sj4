/*
 * Copyright (c) 2004 Iwata <iwata@quasiquote.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SJ4RKCV_H
#define SJ4RKCV_H

#include "rk.h"

/* rk_conv.c */
#define RKSIZE 256
#define RKINCZ 32
typedef struct rkbufrec {
	RkTablW16*	 rkbuf;
	RkTablW16*	 rkend;
	struct rkbufrec* rkbufp;
} RkbufRec;

#define STRSIZE 1024
#define STRINCZ 256
typedef struct strbufrec {
	wchar16_t*	  strbuf;
	wchar16_t*	  strend;
	struct strbufrec* strbufp;
} StrbufRec;

#define YMISIZE 512
#define YMIINCZ 64
typedef struct ymibufrec {
	wchar16_t*	  ymibuf;
	wchar16_t*	  ymiend;
	struct ymibufrec* ymibufp;
} YmibufRec;
extern RkTabl*	  sj4_rcode[MAXCODE];
extern RkTablW16* sj4_rcode_w16[MAXCODE];
extern RkTablW16* rktblp;
extern wchar16_t  rline[MAXLLEN];
extern wchar16_t  lstr[MAXLLEN];

void	   sj4_rkcode(int);
int	   sj4_rkinit2(char*, int);
int	   rcode_sjis_init();
int	   rcode_euc_init();
int	   sj4_rkinit(char*);
int	   sj4_rkinit_euc(char*);
int	   sj4_rkinit_mb(char*);
int	   sj4_rkinit_sub(char* rkfile, int (*)());
char*	   rkgetyomi(char*, wchar16_t*, int*);
char*	   getkey(char*, wchar16_t*, int*);
void	   cltable();
int	   chk_rstr(wchar16_t*, wchar16_t*, int, int);
RkTablW16* rkalloc();
int	   stradd(wchar16_t**, wchar16_t*, int);
int	   kstradd(wchar16_t**, wchar16_t*, int);
RkTablW16* mktable(wchar16_t*, int);
void	   sj4_rkebell(int);
void	   sj4_rkclear();
void	   sj4_rkreset();
void	   sj4_rkconvc(wchar16_t, u_int*);
int	   sj4_rkconv2(wchar16_t*, u_int*, int);
int	   sj4_rkconv(u_char*, u_char*);
int	   sj4_rkconv_euc(u_char*, u_char*);
int	   sj4_rkconv_mb(u_char*, u_char*);
int	   sj4_rkconv_w16(wchar16_t*, wchar16_t*);
int	   rkmatch(wchar16_t*, wchar16_t*, int);

/* sj4_rkcv.h */
#ifdef ADDHK
void setl_hktozh();
#endif
void mkkigou();
int  sj_addten(u_short, u_short);
int  sj_han2zen(u_short);
int  sj4_hantozen_euc(u_char*, u_char*);
int  sj4_hantozen_mb(u_char*, u_char*);
int  sj4_hantozen_w16(wchar16_t*, wchar16_t*);
int  sj_hantozen(wchar16_t*, wchar16_t*, int);
int  sj_zen2han(u_short);
int  sj4_zentohan(u_char*, u_char*);
int  sj4_zentohan_euc(u_char*, u_char*);
int  sj4_zentohan_mb(u_char*, u_char*);
int  sj4_zentohan_w16(wchar16_t*, wchar16_t*);
int  sj_zentohan(wchar16_t*, wchar16_t*, int);
int  sj_tokata(wchar16_t);
int  sj_tohira(wchar16_t);
int  sj_htok(wchar16_t*, wchar16_t*);
int  sj_ktoh(wchar16_t*, wchar16_t*);

/* wc16_str.c */
int	     sj4_iswcntrl16(wchar16_t);
int	     sj4_iswupper16(wchar16_t);
int	     sj4_iswdigit16(wchar16_t);
int	     sj4_iswxdigit16(wchar16_t);
int	     sj4_wslen16(wchar16_t*);
int	     sj4_wscmp16(wchar16_t*, wchar16_t*);
int	     sj4_wsncmp16(wchar16_t*, wchar16_t*, int);
wchar16_t*   sj4_wscpy16(wchar16_t*, wchar16_t*);
wchar16_t*   sj4_wsncpy16(wchar16_t*, wchar16_t*, int);
wchar16_t*   sj4_wscat16(wchar16_t*, wchar16_t*);
wchar16_t    sj4_euc2wc16(unsigned int);
wchar16_t    sj4_sjis2wc16(unsigned int);
unsigned int sj4_wc2euc16(wchar16_t);
unsigned int sj4_wc2sjis16(wchar16_t);
int	     sj4_wcs2eucs16(unsigned char*, wchar16_t*, int);
int	     sj4_wcs2sjiss16(unsigned char*, wchar16_t*, int);
int	     sj4_eucs2wcs16(wchar16_t*, unsigned char*, int);
int	     sj4_sjiss2wcs16(wchar16_t*, unsigned char*, int);
int	     sj4_mbstowcs16(wchar16_t*, unsigned char*, int);
int	     sj4_wcstombs16(unsigned char*, wchar16_t*, int);

/* XXX */
extern int current_locale;

#endif
