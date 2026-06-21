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

#ifndef KANAKAN_H
#define KANAKAN_H

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <errno.h>
#include <sys/stat.h>

#include "sj_kcnv.h"
#include "sj_struct.h"

#ifdef SJ4_GLOBAL
#define SJ4_CONTEXT
#define SJ4_CONTEXT2 void
#define SJ4_CONTEXT_PASS
#define SJ4_CONTEXT_PASS2
#else
#define SJ4_CONTEXT Sj4Context *ctx,
#define SJ4_CONTEXT2 Sj4Context* ctx
#define SJ4_CONTEXT_PASS ctx,
#define SJ4_CONTEXT_PASS2 ctx
#endif

/*
 * NOTE:
 *  All of these funtions calling rule are not following
 * ANSI like arguement lists, fun(dest, src, ...).
 * There are fun(src, dest, ...).
 */

/* adddic.c */
u_int adddic(SJ4_CONTEXT u_char*, u_char*, TypeGram);

/* addelcmn.c */
void	   set_size(u_char*, int, int, int);
u_int	   addel_arg(SJ4_CONTEXT u_char*, u_char*, TypeGram, u_char*, int);
int	   cvtknj(SJ4_CONTEXT u_char*, u_char*, u_char*);
int	   srchkana(SJ4_CONTEXT u_char**, int*);
int	   srchgram(u_char*, u_char**, TypeGram);
int	   srchkanji(u_char**, u_char*, int);
TypeIdxOfs count_uidx(SJ4_CONTEXT2);
void	   chg_uidx(SJ4_CONTEXT TypeDicSeg, u_char*, int);

/* alloc.c */
JREC*  alloc_jrec(SJ4_CONTEXT2);
void   free_jrec(SJ4_CONTEXT JREC*);
CLREC* alloc_clrec(SJ4_CONTEXT2);
void   free_clrec(SJ4_CONTEXT CLREC*);

/* charsize.c */
int codesize(u_char);

/* chrtbl.c */
extern u_char chrtbl[256];

/* cl2knj.c */
int cl2knj(SJ4_CONTEXT u_char*, int, u_char*);
int nextcl(SJ4_CONTEXT u_char*, int);
int prevcl(SJ4_CONTEXT u_char*, int);
int selectnum(SJ4_CONTEXT2);

/* clstudy.c */
int  clstudy(SJ4_CONTEXT u_char*, u_char*, STDYOUT*);
void delclsub(SJ4_CONTEXT u_char*);
void mkclidx(SJ4_CONTEXT2);

/* cmpstr.c */
int cmpstr(u_char*, u_char*);

/* conjunc.c */
extern u_char* conjadr[];

/* connect.c */
extern u_char  connadr[];
extern u_char* rigtadr[];

/* cvtclknj.c */
void cvtclknj(SJ4_CONTEXT2);

/* cvtdict.c */
void cvtdict(SJ4_CONTEXT KHREC*, CLREC*, int);
void cvtminasi(SJ4_CONTEXT int);
void cvtwakachi(SJ4_CONTEXT CLREC*);

/* cvtkanji.c */
void cvtphknj(SJ4_CONTEXT2);
void cvtkouho(SJ4_CONTEXT KHREC*);
void setstyrec(SJ4_CONTEXT KHREC*);

/* deldic.c */
u_int deldic(SJ4_CONTEXT u_char*, u_char*, TypeGram);

/* dict.c */
void	get_askknj(SJ4_CONTEXT2);
int	seldict(SJ4_CONTEXT TypeDicID);
u_char* get_idxptr(SJ4_CONTEXT TypeDicSeg);

/* functbl.c */
extern VFunc cvtnum_func[];

/* fuzoku.c */
void	    setclrec(SJ4_CONTEXT JREC*, u_char*, TypeCnct);
RECURS void srchfzk(SJ4_CONTEXT JREC*, u_char*, TypeCnct, int);

/* fzkyomi.c */
extern u_char* fzkadr[];

/* getkanji.c */
u_char* getkan_none(u_char*, u_char*, u_char*, int, int);
u_char* getkan_hira(u_char*, u_char*, u_char*, int, int);
u_char* getkan_kata(u_char*, u_char*, u_char*, int, int);
u_char* getkan_knj(SJ4_CONTEXT u_char*, u_char*, u_char*, int, int);
u_char* getkan_ofs(SJ4_CONTEXT u_char*, u_char*, u_char*, int, int);
u_char* getkan_norm(u_char*, u_char*, u_char*, int, int);
u_char* getkan_ascii(u_char*, u_char*, u_char*, int, int);
int	getkanji(SJ4_CONTEXT u_char*, int, u_char*, u_char*);

/* getrank.c */
void getrank(SJ4_CONTEXT2);

/* hzstrlen.c */
int euc_codesize(u_char c);
int hzstrlen(u_char*, int);

/* init.c */
void seg_count(SJ4_CONTEXT DICT*);
void mkidxtbl(SJ4_CONTEXT DICT*);
void initwork(SJ4_CONTEXT2);
#ifndef SJ4_GLOBAL
Sj4Context* alloccontext(const char*);
void	    free_context(Sj4Context*);
#endif

/* istrcmp.c */
int istrcmp(u_char*, u_char*, int, int);

/* memcpy.c */
void sstrncpy(u_char*, u_char*, int);

/* memory.c */
JREC*  free_jlst(SJ4_CONTEXT JREC*);
CLREC* free_clst(SJ4_CONTEXT CLREC*, int);
void   free_clall(SJ4_CONTEXT CLREC*);
void   free_jall(SJ4_CONTEXT JREC*);
void   freework(SJ4_CONTEXT2);

/* mk2claus.c */
void mk2claus(SJ4_CONTEXT2);

/* mkbunset.c */
void   mkbunsetu(SJ4_CONTEXT2);
CLREC* argclrec(SJ4_CONTEXT int);

/* mkjiritu.c */
void  mkjiritu(SJ4_CONTEXT int);
JREC* argjrec(SJ4_CONTEXT int, JREC*);

/* mkkouho.c */
void	mkkouho(SJ4_CONTEXT2);
u_char* makekan_none(SJ4_CONTEXT u_char*, u_char*, int);
u_char* makekan_1byte(SJ4_CONTEXT u_char*, u_char*, int);
u_char* makekan_knj(SJ4_CONTEXT u_char*, u_char*, int);
u_char* makekan_ofs(SJ4_CONTEXT u_char*, u_char*, int);
u_char* makekan_norm(SJ4_CONTEXT u_char*, u_char*, int);
u_char* makekan_ascii(SJ4_CONTEXT u_char*, u_char*, int);
int	sel_sjmode(JREC*);

/* mknumber.c */
void num_type00(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type01(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type02(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type03(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type04(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type05(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type06(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type07(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type08(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type09(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type10(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type11(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type12(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type13(SJ4_CONTEXT u_char*, u_char*, JREC*);
void num_type14(SJ4_CONTEXT u_char*, u_char*, JREC*);

/* mvmemd.c */
void mvmemd(u_char*, u_char*, int);

/* mvmemi.c */
void mvmemi(u_char*, u_char*, int);

/* peepdic.c */
int getusr(SJ4_CONTEXT u_char*);
int nextusr(SJ4_CONTEXT u_char*);
int prevusr(SJ4_CONTEXT u_char*);

/* ph2knj.c */
int ph2knj(SJ4_CONTEXT u_char*, u_char*, int);

/* ph_khtbl.c */
int ph_khtbl(SJ4_CONTEXT CLREC*);

/* priority.c */
int  priority(SJ4_CONTEXT CLREC*);
void pritiny(SJ4_CONTEXT2);

/* prtytbl.c */
extern u_char taipri[7][45];
extern u_char sttpri[5][31];

/* s2ctbl.c */
extern u_char kigou[];

/* sdepend.c */
DictFile* opendict(SJ4_CONTEXT char*, char*);
int	  closedict(SJ4_CONTEXT DictFile*);
StdyFile* openstdy(SJ4_CONTEXT char*, char*);
int	  closestdy(SJ4_CONTEXT StdyFile*);
int	  putstydic(SJ4_CONTEXT2);
int	  putcldic(SJ4_CONTEXT2);
int	  makedict(char*, int, int, int);
int	  makestdy(char*, int, int, int);
void	  sj_closeall(SJ4_CONTEXT2);
int	  set_dictpass(DictFile*, char*);
int	  set_stdypass(SJ4_CONTEXT char*);
int	  set_dictcmnt(DictFile*, char*);
int	  set_stdycmnt(SJ4_CONTEXT char*);
int	  get_stdysize(SJ4_CONTEXT int*, int*, int*);

/* selclrec.c */
void selclrec(SJ4_CONTEXT2);

/* selsuuji.c */
extern u_short* selsjadrs[];

/* setconj.c */
int setconj(SJ4_CONTEXT TypeGram, JREC*, CREC*);

/* setjrec.c */
int  setj_atrb(SJ4_CONTEXT u_char*);
int  setj_ofs(SJ4_CONTEXT u_char*);
int  setj_knj(SJ4_CONTEXT u_char*);
int  setj_norm1(SJ4_CONTEXT u_char*);
int  setj_norm2(SJ4_CONTEXT u_char*);
int  setj_norm3(SJ4_CONTEXT u_char*); /* unused??? */
void setjrec(SJ4_CONTEXT u_char*, int);
void setnumrec(SJ4_CONTEXT u_char*, JREC*, TypeGram);
void setcrec(SJ4_CONTEXT u_char*);

/* setkouho.c */
void setkouho(SJ4_CONTEXT CLREC*, TypeDicOfs, int);
void ph_setkouho(SJ4_CONTEXT CLREC*, TypeDicOfs, STDYIN*);
int  hiraknj_atrb(SJ4_CONTEXT u_char*, int*);
int  hiraknj_ofs(SJ4_CONTEXT u_char*, int*);
int  hiraknj_knj(SJ4_CONTEXT u_char*, int*);
int  hiraknj_hask(SJ4_CONTEXT u_char*, int*);
int  hiraknj_kask(SJ4_CONTEXT u_char*, int*);
int  hiraknj_norm(SJ4_CONTEXT u_char*, int*);
int  hiraknj_hira(SJ4_CONTEXT u_char*, int*);

/* setubi.c */
u_char* getstb(TypeGram);
void	setubi(SJ4_CONTEXT JREC*, u_char*);

/* sj2code.c */
int sj2cd_chr(u_char*, u_char*);
int sj2cd_str(u_char*, u_char*, int);

/* skiphblk.c */
u_char* skiphblk(u_char*);

/* skipkstr.c */
u_char* skipkstr(u_char*);

/* srchdict.c */
int	yomicmp(SJ4_CONTEXT u_char*, u_char*, u_char*);
u_char* srchdict(SJ4_CONTEXT u_char*);

/* srchhead.c */
int srchhead(SJ4_CONTEXT2);

/* srchidx.c */
TypeDicSeg srchidx(SJ4_CONTEXT TypeDicSeg, int);

/* srchnum.c*/
void srchnum(SJ4_CONTEXT2);
void setwdnum(SJ4_CONTEXT u_char*, int, u_short*);
int  setucnum(SJ4_CONTEXT u_char*, int, u_char*);

/* sstrcmp.c */
int sstrcmp(u_char*, u_char*);

/* sstrlen.c */
int sstrlen(u_char*);

/* sstrncmp.c */
int sstrncmp(u_char*, u_char*, int);

/* stbtbl.c */
extern u_char* stbadr[];

/* stttbl.c */
extern u_char* settou_ptr[];
extern u_char  scncttbl[2][24];

/* study.c */
int	study(SJ4_CONTEXT STDYOUT*);
STDYIN* srchstdy(SJ4_CONTEXT TypeDicSeg, TypeDicOfs, TypeDicID);

/* suujitbl.c */
extern u_char suuji_tbl[];
extern u_char kurai1_tbl[];
extern u_char kurai2_tbl[];
extern u_char num1tbl[];
extern u_char num2tbl[];
extern u_char num3tbl[];
extern u_char num4tbl[];
extern u_char num5tbl[];
extern u_char num6tbl[];

/* terminat.c */
int terminate(SJ4_CONTEXT TypeCnct, u_char*);

/* termtbl.c */
extern u_char termtbl[];

/* wakachi.c */
void wakachi(SJ4_CONTEXT2);

#endif /* KANAKAN_H */
