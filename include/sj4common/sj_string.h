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

#ifndef SJ_STRING_H
#define SJ_STRING_H

/* string.c */
int sj4_str_sjistoeuc(unsigned char*, int, unsigned char*, unsigned char*, int*);
int sj4_str_euctosjis(unsigned char*, int, unsigned char*, unsigned char*, int*);
int sj4_sjistoeuclen(unsigned char*, int);

int	       sj4_sjistoeuc(unsigned char* s, int, unsigned char*, int);
int	       sj4_euctosjis(unsigned char* s, int, unsigned char*, int);
void	       sj_euc2sjis(unsigned char*);
void	       sj_jis2sjis(unsigned char*);
void	       sj_sjis2euc(unsigned char*);
void	       sj_sjis2jis(unsigned char*);
unsigned short sj4_jis2sjis(unsigned short);
unsigned short sj4_jis2euc(unsigned short);
unsigned short sj4_sjis2jis(unsigned short);
unsigned short sj4_euc2sjis(unsigned short);
unsigned short sj4_sjis2euc(unsigned short);

#endif /* SJ4INTERN_H */
