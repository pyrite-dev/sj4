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

int yomicmp(SJ4_CONTEXT u_char* ptr1, u_char* ptr2, u_char* saml) {
	int i, j;
	int same;

	same = *saml;

	i = getplen(ptr2);
	j = getnlen(ptr2);

	if(j == 0) {
		ptr2 = get_idxptr(SJ4_CONTEXT_PASS prevseg);

		for(i = same; i; i--)
			if(*ptr1++ != *ptr2++) return OVER;

		j = sstrlen(ptr2);
	}

	else if(i > same)
		return CONT;

	else {
		if(i < same) same = i;
		ptr1 += same;
		ptr2 += DOUONBLKSIZENUMBER;
	}

	while(j-- > 0) {
		if(*ptr1 > *ptr2) {
			*saml = same;
			return CONT;
		} else if(*ptr1++ < *ptr2++) {
			return OVER;
		}
		same++;
	}

	*saml = same;
	return MATCH;
}

static __inline int yomicmp_seg(SJ4_CONTEXT u_char* ptr1, u_char* ptr2, u_char* saml, const u_char* segidx, int segidxlen) {
	int i, j;
	int same;

	same = *saml;

	i = getplen(ptr2);
	j = getnlen(ptr2);

	if(j == 0) {
		ptr2 = (u_char*)segidx;

		for(i = same; i; i--)
			if(*ptr1++ != *ptr2++) return OVER;

		j = segidxlen;
	}

	else if(i > same)
		return CONT;

	else {
		if(i < same) same = i;
		ptr1 += same;
		ptr2 += DOUONBLKSIZENUMBER;
	}

	while(j-- > 0) {
		if(*ptr1 > *ptr2) {
			*saml = same;
			return CONT;
		} else if(*ptr1++ < *ptr2++) {
			return OVER;
		}
		same++;
	}

	*saml = same;
	return MATCH;
}

static __inline TypeDicSeg
srchidx_loaded(SJ4_CONTEXT TypeDicSeg low, int len) {
	TypeDicSeg high;
	TypeDicSeg mid;
	int	   cmp;
	u_char*	   target;

	if((high = curdict->segunit - 1) < 1) return 0;

	if(low == (TypeDicSeg)-1) low = 0;

	while(low <= high) {

		mid = (low + high) >> 1;

		target = idxbuf + idxofs[mid];

		cmp = istrcmp(cnvstart, target, len, sstrlen(target));

		if(cmp == OVER) {
			high = mid - 1;
		}

		else if(cmp != MATCH) {
			low = mid + 1;
		}

		else {
			return mid;
		}
	}

	return (cmp == OVER && mid != DICSEGBASE) ? mid - 1 : mid;
}

u_char*
srchdict(SJ4_CONTEXT u_char* tagp) {
	TypeDicSeg segno;
	int	   cmp;
	int	   maxlen;
	u_char*	   segidx    = NULL;
	int	   segidxlen = 0;

	if(prevseg != (TypeDicSeg)-1) {
		segidx	  = idxbuf + idxofs[prevseg];
		segidxlen = sstrlen(segidx);
	}

	maxlen = (cnvlen > MAXWDYOMILEN) ? MAXWDYOMILEN : cnvlen;

	while((int)dicinl <= maxlen) {

		segno = srchidx_loaded(SJ4_CONTEXT_PASS prevseg, (int)dicinl);

		(*curdict->getdic)(SJ4_CONTEXT_PASS curdict, segno);

		if(prevseg != segno) {
			(*curdict->getdic)(SJ4_CONTEXT_PASS curdict, segno);

			prevseg	  = segno;
			segidx	  = idxbuf + idxofs[segno];
			segidxlen = sstrlen(segidx);

			tagp = dicbuf;
		}

		else {

			tagp = getntag(tagp);

			if(segend(tagp)) {

				prevseg = ++segno;

				if(segno >= curdict->segunit) return NULL;

				(*curdict->getdic)(SJ4_CONTEXT_PASS curdict, segno);
				segidx	  = idxbuf + idxofs[segno];
				segidxlen = sstrlen(segidx);

				tagp = dicbuf;
			}
		}

		if(tagp == dicbuf) {
			tagp += *tagp + 1;
		}

		while(!segend(tagp)) {

			cmp = yomicmp_seg(SJ4_CONTEXT_PASS cnvstart, tagp, &dicsaml, segidx, segidxlen);

			if((int)dicsaml > maxlen) return NULL;

			if(cmp == OVER)
				return NULL;

			else if(cmp == MATCH) {

				if(isdpnd(*(cnvstart + dicsaml))) {
					break;
				}

				else {
					dicinl = dicsaml + 1;
					if(askbase != dicbuf) get_askknj(SJ4_CONTEXT_PASS2);
					return tagp;
				}
			}

			else if(cmp == CONT && getnlen(tagp) != 0 && getplen(tagp) > dicsaml && curdict->skipdic != NULL) {
				u_char* next = (*curdict->skipdic)(SJ4_CONTEXT_PASS tagp, segno);

				if(next != NULL) {
					tagp = next;
					continue;
				}
			}
			tagp = getntag(tagp);
		}
		dicinl = dicsaml + 1;
	}

	return NULL;
}
