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

/*
 * $SonyRCSfile: mkjiritu.c,v $
 * $SonyRevision: 1.1 $
 * $SonyDate: 1994/06/03 08:02:03 $
 */

#include <string.h>
#include "sj_kcnv.h"

#include "sj_kanakan.h"

static void dic_mu(SJ4_CONTEXT int), dic_cl(SJ4_CONTEXT2);

void mkjiritu(SJ4_CONTEXT int mode) {
	u_char	chkind1;
	u_char	chkind2;
	JREC*	jrec;
	u_char* stb;

	headcode = headlen = 0;

	maxjptr = NULL;

	chkind1 = Chrtbl[*cnvstart];

	if(chkind1 & DICTOP) {
		dic_mu(SJ4_CONTEXT_PASS mode);
	}

	if(mode & DO_CLSTUDY) dic_cl(SJ4_CONTEXT_PASS2);

	srchnum(SJ4_CONTEXT_PASS2);

	if(srchhead(SJ4_CONTEXT_PASS2) && cnvlen != headlen) {
		cnvstart += headlen;
		cnvlen -= headlen;

		chkind2 = Chrtbl[*cnvstart];

		if(chkind2 & DICTOP) {
			dic_mu(SJ4_CONTEXT_PASS mode);
		}

		if(headcode == SETTOU_DAI) {
			srchnum(SJ4_CONTEXT_PASS2);
		}

		cnvstart -= headlen;
		cnvlen += headlen;
	}

	for(jrec = maxjptr; jrec; jrec = jrec->jsort) {
		if((stb = getstb(jrec->hinsi)))
			setubi(SJ4_CONTEXT_PASS jrec, stb);
	}
}

static void
dic_mu(SJ4_CONTEXT int mode) {
	u_char* tagp = NULL;
	DICTL*	dp;

	for(dp = dictlist; dp; dp = dp->next) {
		curdict = dp->dict;
		dicinl	= 1;
		dicsaml = 0;
		prevseg = -1;

		while((tagp = srchdict(SJ4_CONTEXT_PASS tagp))) setjrec(SJ4_CONTEXT_PASS tagp, mode);
	}
}

JREC* argjrec(SJ4_CONTEXT int len, JREC* rec) {
	JREC* ptr;
	JREC* jrec;
	JREC* child;

	jrec = alloc_jrec(SJ4_CONTEXT_PASS2);

	if(!jrec) {
		if(!maxjptr) return NULL;

		ptr  = NULL;
		jrec = maxjptr;
		while((child = jrec->jsort)) {
			ptr  = jrec;
			jrec = child;
		}

		if(len <= (int)jrec->jlen) return NULL;

		if(ptr)
			ptr->jsort = NULL;
		else
			maxjptr = NULL;
	}

	if(rec)
		memcpy((u_char*)jrec, (u_char*)rec, sizeof(JREC));
	else
		memset((u_char*)jrec, 0, sizeof(*jrec));
	jrec->jlen = (u_char)len;

	if(!maxjptr) {
		maxjptr	    = jrec;
		jrec->jsort = NULL;
		return jrec;
	}

	ptr = maxjptr;

	if((int)ptr->jlen < len) {
		jrec->jsort = maxjptr;
		maxjptr	    = jrec;
		return jrec;
	}

	while((child = ptr->jsort)) {
		if((int)child->jlen < len) break;

		ptr = child;
	}

	ptr->jsort  = jrec;
	jrec->jsort = child;
	return jrec;
}

static void
dic_cl(SJ4_CONTEXT2) {
	u_char* p;
	u_short pos;
	int	len;
	int	cmp;

	if(!StudyExist()) return;

	if((pos = CLSTUDYIDX[(short)*cnvstart / CLSTUDYSTEP]) != (u_short)-1) {

		for(p = CLSTUDYDICT + pos; !iseocl(p); p = ClNextTag(p)) {
			cmp = sstrncmp(cnvstart, (u_char*)ClYomiPos(p),
				       len = ClYomiLen(p));

			if(cmp == MATCH) {
				if(!isdpnd(*(cnvstart + len))) {
					setcrec(SJ4_CONTEXT_PASS p);
				}
			}

			else if(cmp == OVER)
				break;
		}
	}
}
