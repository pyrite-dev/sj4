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

void seg_count(SJ4_CONTEXT DICT* dict) {
	u_char*	   p;
	u_char*	   q;
	TypeDicSeg segcnt = 0;

	if(dict->getidx) {
		(*dict->getidx)(SJ4_CONTEXT_PASS dict);

		p = idxbuf;
		q = p + dict->idxlen;
		while(p < q && *p) {
			segcnt++;
			while(*p++);
		}
	}

	dict->segunit = (segcnt == 0) ? 1 : segcnt;
}

void mkidxtbl(SJ4_CONTEXT DICT* dict) {
	u_char*	   p;
	TypeDicSeg seg;

	if(!dict->getidx || !dict->getofs) return;

	seg = 0;

	(*dict->getidx)(SJ4_CONTEXT_PASS dict);
	(*dict->getofs)(SJ4_CONTEXT_PASS dict);

	idxofs[0] = 0;
	for(p = idxbuf; p < idxbuf + dict->idxlen && seg < dict->segunit;) {
		idxofs[seg++] = p - idxbuf;
		while(*p++);
	}
}

void initwork(SJ4_CONTEXT2) {
	jrt1st = jrt2nd = maxjptr = (JREC*)0;
	clt1st = clt2nd = maxclptr = (CLREC*)0;
}

#ifndef SJ4_GLOBAL
Sj4Context* alloccontext(const char* dic) {
	Sj4Context* ctx = calloc(1, sizeof(*ctx));
	DictFile*   dict;
	DICTL*	    node;

	if((ctx->work = calloc(1, sizeof(*ctx->work))) == NULL) {
		free(ctx);

		return NULL;
	}
	if((ctx->stdy = calloc(1, sizeof(*ctx->stdy))) == NULL) {
		free(ctx->work);
		free(ctx);

		return NULL;
	}

	if((node = calloc(1, sizeof(*node))) == NULL) {
		free(ctx->stdy);
		free(ctx->work);
		free(ctx);

		return NULL;
	}

	ctx->work->Jdictlist = node;

	initwork(ctx);

	dict = opendict(ctx, (char*)dic, "");

	if(dict == NULL) {
		free_context(ctx);
		return NULL;
	}

	node->dict = &dict->dict;
	node->next = NULL;

	seldict(ctx, dict->dict.dicid);

	return ctx;
}

void free_context(Sj4Context* ctx) {
	DICTL* node;

	node = ctx->work->Jdictlist;
	while(node != NULL) {
		DICTL* next = node->next;

		if(node->dict != NULL) closedict(ctx, (DictFile*)node->dict);
		free(node);

		node = next;
	}

	freework(ctx);

	if(ctx->jrec_pool != NULL) free(ctx->jrec_pool);
	if(ctx->clrec_pool != NULL) free(ctx->clrec_pool);

	free(ctx->stdy);
	free(ctx->work);

	free(ctx);
}
#endif
