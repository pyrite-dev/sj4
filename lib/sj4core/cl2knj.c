/*
 * Copyright (c) 1991-1994  Sony Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL SONY CORPORATION BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Sony Corporation
 * shall not be used in advertising or otherwise to promote the sale, use
 * or other dealings in this Software without prior written authorization
 * from Sony Corporation.
 *
 */

#include "sj_kcnv.h"

#include "sj_kanakan.h"

static CLREC *nextrecblk(SJ4_CONTEXT2), *prevrecblk(SJ4_CONTEXT2);

static void initkbuf(SJ4_CONTEXT u_char*);

int cl2knj(SJ4_CONTEXT u_char* yomi, int len, u_char* kouho) {
	u_char* ptr1;
	u_char* ptr2;
	int	i;

	khcount = nkhcount = 0;

	initkbuf(SJ4_CONTEXT_PASS kouho);

	if(len > MAXCLINPUTLEN * 2) len = MAXCLINPUTLEN * 2;

	ptr1 = yomi;
	ptr2 = hyomi;
	for(i = 0; i < len; i++) {
		if(isknj1(*ptr1)) {
			if(++i >= len) break;
		}
		ptr1 += sj2cd_chr(ptr1, ptr2++);
	}
	*ptr2 = 0;

	sj4_strlcpy((char*)orgyomi, (char*)yomi, (int)(ptr1 - yomi) + 1);

	if(!hyomi[0]) return 0;

	freework(SJ4_CONTEXT_PASS2);

	inputyomi = orgyomi;
	cnvstart = ystart = hyomi;
	cnvlen		  = sstrlen(hyomi);

	mkjiritu(SJ4_CONTEXT_PASS 0);

	mkbunsetu(SJ4_CONTEXT_PASS2);

	if(!maxclptr) wakachi(SJ4_CONTEXT_PASS2);

	jrt1st = maxjptr;
	clt1st = maxclptr;

	mkkouho(SJ4_CONTEXT_PASS2);

	getrank(SJ4_CONTEXT_PASS2);

	selectid = 1;

	cvtclknj(SJ4_CONTEXT_PASS2);

	return hzstrlen(inputyomi, (int)clt1st->cllen);
}

int nextcl(SJ4_CONTEXT u_char* kouho, int mode) {
	CLREC* clptr;

	initkbuf(SJ4_CONTEXT_PASS kouho);

	if(!khcount) return 0;

	if(!mode && (selectid < khcount))
		selectid++;

	else if((mode < 2) && (clptr = nextrecblk(SJ4_CONTEXT_PASS2))) {
		selectid = 1;
		clt1st	 = clptr;
		mkkouho(SJ4_CONTEXT_PASS2);
		getrank(SJ4_CONTEXT_PASS2);
	} else {
		cvtclknj(SJ4_CONTEXT_PASS2);
		return 0;
	}

	cvtclknj(SJ4_CONTEXT_PASS2);

	return hzstrlen(inputyomi, (int)clt1st->cllen);
}

static CLREC*
nextrecblk(SJ4_CONTEXT2) {
	CLREC* clptr;
	int    keeplen;

	keeplen = clt1st->cllen;

	clptr = clt1st->clsort;

	while(clptr && (int)clptr->cllen == keeplen)
		clptr = clptr->clsort;

	return clptr;
}

int prevcl(SJ4_CONTEXT u_char* kouho, int mode) {
	CLREC* clptr;

	initkbuf(SJ4_CONTEXT_PASS kouho);

	if(!khcount) return 0;

	if(!mode && (selectid > 1))
		selectid--;

	else if((mode < 2) && (clptr = prevrecblk(SJ4_CONTEXT_PASS2))) {
		clt1st = clptr;
		mkkouho(SJ4_CONTEXT_PASS2);
		getrank(SJ4_CONTEXT_PASS2);
		selectid = khcount;
	} else {
		cvtclknj(SJ4_CONTEXT_PASS2);
		return 0;
	}

	cvtclknj(SJ4_CONTEXT_PASS2);

	return hzstrlen(inputyomi, (int)clt1st->cllen);
}

static CLREC*
prevrecblk(SJ4_CONTEXT2) {
	CLREC* clptr;
	CLREC* keepptr;
	int    keeplen;
	int    cllen;

	clptr = maxclptr;

	if((cllen = clt1st->cllen) == clptr->cllen)
		return NULL;

	while((int)clptr->cllen > cllen) {
		keeplen = clptr->cllen;

		keepptr = clptr;

		while((int)clptr->cllen == keeplen)
			clptr = clptr->clsort;
	}

	return keepptr;
}

static void
initkbuf(SJ4_CONTEXT u_char* kouho) {
	kanjitmp = kouho;
	memset(kanjitmp, 0, sizeof(STDYOUT) + 1);
}

int selectnum(SJ4_CONTEXT2) {
	return selectid;
}
