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
 * $SonyRCSfile: deldic.c,v $
 * $SonyRevision: 1.1 $
 * $SonyDate: 1994/06/03 08:01:42 $
 */

#include <string.h>
#include "sj_kcnv.h"

#include "sj_kanakan.h"

static int  del_douon(SJ3_CONTEXT TypeDicSeg, u_char*, TypeDicOfs);
static int  del_segment(SJ3_CONTEXT TypeDicSeg);
static void del_uidx(SJ3_CONTEXT TypeDicSeg);
static void del_stdy(SJ3_CONTEXT TypeDicSeg, TypeDicOfs, int);

u_int deldic(SJ3_CONTEXT u_char* yomi, u_char* kanji, TypeGram hinsi) {
	u_char	   yptr[MAXWDYOMILEN + 1];
	u_char	   kptr[MAXWDKANJILEN + 1];
	u_short	   err;
	int	   knjlen;
	TypeDicSeg useg;
	TypeDicOfs ofs;
	u_char *   p1, *p2, *p3;
	int	   dblknum;
	int	   hblknum;
	int	   kblknum;
	int	   samlen;
	u_char *   stp, *edp;
	int	   size, len;

	if((err = addel_arg(SJ3_CONTEXT_PASS yomi, kanji, hinsi, yptr, sizeof(yptr))))
		return err;

	if(!curdict->maxunit) return AD_NOTUSERDICT;

	knjlen = cvtknj(SJ3_CONTEXT_PASS yomi, kanji, kptr);

	cnvstart = yptr;
	cnvlen	 = sstrlen(yptr);

	useg = srchidx(SJ3_CONTEXT_PASS(TypeDicSeg) DICSEGBASE, cnvlen);
	(*curdict->getdic)(SJ3_CONTEXT_PASS curdict, useg);

	if(!(dblknum = srchkana(SJ3_CONTEXT_PASS & p1, &samlen))) return AD_NOMIDASI;

	if(!(hblknum = srchgram(p1, &p2, hinsi))) return AD_NOHINSI;

	p3	= p2;
	kblknum = srchkanji(&p3, kptr, knjlen);
	if(*p3 == HINSIBLKTERM) return AD_NOKANJI;
	ofs = p3 - dicbuf;

	edp = skipkstr(p3);

	if(kblknum == 1) {
		if(hblknum == 1) {
			if(dblknum == 1) {
				return del_segment(SJ3_CONTEXT_PASS useg);
			} else {
				return del_douon(SJ3_CONTEXT_PASS useg, p1, ofs);
			}
		} else {
			stp = p2;
			edp++;
		}
	} else {
		stp = p3;
	}

	size = edp - stp;

	len = dicbuf + (curdict->seglen) - edp;
	mvmemi(edp, stp, len);

	edp += len;
	memset(edp, DICSEGTERM, dicbuf + curdict->seglen - edp);

	set_size(p1, (int)(getsize(p1) - size),
		 (int)getplen(p1), (int)getnlen(p1));

	(*curdict->putdic)(SJ3_CONTEXT_PASS curdict, useg);

	del_stdy(SJ3_CONTEXT_PASS useg, ofs, size);

	return AD_DONE;
}

static int
del_douon(SJ3_CONTEXT TypeDicSeg seg, u_char* ptr, TypeDicOfs ofs) {
	u_char *nxt, *p1, *p2;
	int	size;
	int	nlen, plen, len;

	nxt = getntag(ptr);

	if(segend(nxt)) {
		size = nxt - ptr;

		memset(ptr, DICSEGTERM, dicbuf + curdict->seglen - nxt);
	} else {
		size = getsize(nxt);
		nlen = getnlen(nxt);
		plen = getplen(nxt);

		if(ptr == segtop()) {
			if(nlen + plen > (unsigned int)count_uidx(SJ3_CONTEXT_PASS2) + sstrlen(get_idxptr(SJ3_CONTEXT_PASS seg)))
				return AD_OVFLWINDEX;
		}

		len = ((len = getplen(ptr)) < plen) ? plen - len : 0;

		set_size(ptr, size + len, plen - len, nlen + len);

		p1 = nxt + DOUONBLKSIZENUMBER;
		p2 = ptr + DOUONBLKSIZENUMBER + len;

		mvmemi(p1, p2, dicbuf + curdict->seglen - p1);

		size = p1 - p2;
		memset(dicbuf + curdict->seglen - size, DICSEGTERM, size);

		if(ptr == segtop()) {
			chg_uidx(SJ3_CONTEXT_PASS seg, ptr + DOUONBLKSIZENUMBER, nlen + len);
		}
	}

	(*curdict->putdic)(SJ3_CONTEXT_PASS curdict, seg);

	del_stdy(SJ3_CONTEXT_PASS seg, ofs, size);

	return AD_DONE;
}

static int
del_segment(SJ3_CONTEXT TypeDicSeg seg) {
	int	   i;
	STDYIN*	   styp;
	TypeDicID  dicid;
	TypeStyNum stdynum;
	TypeDicSeg sg;

	if(curdict->segunit > 1) {
		for(sg = seg + 1; sg < curdict->segunit; sg++) {
			(*curdict->getdic)(SJ3_CONTEXT_PASS curdict, sg);
			(*curdict->putdic)(SJ3_CONTEXT_PASS curdict, sg - 1);
		}
		curdict->segunit--;
	} else {
		memset(dicbuf, DICSEGTERM, (int)curdict->seglen);
		dicbuf[0] = 0;
		(*curdict->putdic)(SJ3_CONTEXT_PASS curdict, DICSEGBASE);
	}

	(*curdict->rszdic)(SJ3_CONTEXT_PASS curdict, curdict->segunit);

	del_uidx(SJ3_CONTEXT_PASS seg);

	if(StudyExist()) {
		dicid = curdict->dicid;
		for(i = 0, styp = STUDYDICT; i < STUDYCOUNT;) {
			if(styp->dicid != dicid)
				;
			else if(styp->seg > seg)
				styp->seg -= 1;
			else if(styp->seg < seg)
				;
			else {
				stdynum = styp->styno;
				STUDYCOUNT--;
				mvmemi((u_char*)(styp + 1), (u_char*)styp,
				       sizeof(*styp) * (STUDYCOUNT - i));
				continue;
			}

			i++;
			styp++;
		}
		for(i = 0, styp = STUDYDICT; i < STUDYCOUNT; i++, styp++)
			if(styp->styno > stdynum) styp->styno -= 1;

		putstydic(SJ3_CONTEXT_PASS2);
	}

	return AD_DONE;
}

static void
del_uidx(SJ3_CONTEXT TypeDicSeg seg) {
	u_char *p, *q;
	int	len;

	p = get_idxptr(SJ3_CONTEXT_PASS seg);
	for(q = p; *q++;);

	mvmemi(q, p, idxbuf + curdict->idxlen - q);

	len = q - p;
	memset(idxbuf + curdict->idxlen - len, 0, len);

	(*curdict->putidx)(SJ3_CONTEXT_PASS curdict, 0);

	mkidxtbl(SJ3_CONTEXT_PASS curdict);
}

static void
del_stdy(SJ3_CONTEXT TypeDicSeg seg, TypeDicOfs ofs, int size) {
	int	   i;
	TypeStyNum stdynum;
	STDYIN*	   stdy;
	TypeDicID  dicid;

	if(StudyExist()) {

		stdynum = 0;
		dicid	= curdict->dicid;
		for(i = 0, stdy = STUDYDICT; i < STUDYCOUNT;) {
			if(stdy->dicid != dicid)
				;
			else if(stdy->seg != seg)
				;
			else if(stdy->offset > ofs)
				stdy->offset -= size;

			else if(stdy->offset == ofs) {
				stdynum = stdy->styno;
				STUDYCOUNT--;
				mvmemi((u_char*)(stdy + 1), (u_char*)stdy,
				       sizeof(*stdy) * (STUDYCOUNT - i));
				continue;
			}

			i++;
			stdy++;
		}
		for(i = 0, stdy = STUDYDICT; i < STUDYCOUNT; i++, stdy++)
			if(stdy->styno > stdynum) (stdy->styno)--;

		putstydic(SJ3_CONTEXT_PASS2);
	}
}
