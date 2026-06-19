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
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "sj_kcnv.h"
#include "sj_struct.h"

#ifdef SJ3_GLOBAL
#define SJ3_CONTEXT
#define SJ3_CONTEXT2 void
#define SJ3_CONTEXT_PASS
#define SJ3_CONTEXT_PASS2
#else
#define SJ3_CONTEXT Sj3Context *ctx,
#define SJ3_CONTEXT2 Sj3Context* ctx
#define SJ3_CONTEXT_PASS ctx,
#define SJ3_CONTEXT_PASS2 ctx
#endif

/*
 * NOTE:
 *  All of these funtions calling rule are not following
 * ANSI like arguement lists, fun(dest, src, ...).
 * There are fun(src, dest, ...).
 */

/* adddic.c */
u_int adddic(SJ3_CONTEXT u_char*, u_char*, TypeGram);

/* addelcmn.c */
void	   set_size(u_char*, int, int, int);
u_int	   addel_arg(SJ3_CONTEXT u_char*, u_char*, TypeGram, u_char*, int);
int	   cvtknj(SJ3_CONTEXT u_char*, u_char*, u_char*);
int	   srchkana(SJ3_CONTEXT u_char**, int*);
int	   srchgram(u_char*, u_char**, TypeGram);
int	   srchkanji(u_char**, u_char*, int);
TypeIdxOfs count_uidx(SJ3_CONTEXT2);
void	   chg_uidx(SJ3_CONTEXT TypeDicSeg, u_char*, int);

/* alloc.c */
JREC*  alloc_jrec(void);
void   free_jrec(JREC*);
CLREC* alloc_clrec(void);
void   free_clrec(CLREC*);

/* charsize.c */
int codesize(u_char);

/* chrtbl.c */
extern u_char chrtbl[256];

/* cl2knj.c */
int cl2knj(SJ3_CONTEXT u_char*, int, u_char*);
int nextcl(SJ3_CONTEXT u_char*, int);
int prevcl(SJ3_CONTEXT u_char*, int);
int selectnum(SJ3_CONTEXT2);

/* clstudy.c */
int  clstudy(SJ3_CONTEXT u_char*, u_char*, STDYOUT*);
void delclsub(SJ3_CONTEXT u_char*);
void mkclidx(SJ3_CONTEXT2);

/* cmpstr.c */
int cmpstr(u_char*, u_char*);

/* conjunc.c */
extern u_char* conjadr[];

/* connect.c */
extern u_char  connadr[];
extern u_char* rigtadr[];

/* cvtclknj.c */
void cvtclknj(SJ3_CONTEXT2);

/* cvtdict.c */
void cvtdict(SJ3_CONTEXT KHREC*, CLREC*, int);
void cvtminasi(SJ3_CONTEXT int);
void cvtwakachi(SJ3_CONTEXT CLREC*);

/* cvtkanji.c */
void cvtphknj(SJ3_CONTEXT2);
void cvtkouho(SJ3_CONTEXT KHREC*);
void setstyrec(SJ3_CONTEXT KHREC*);

/* deldic.c */
u_int deldic(SJ3_CONTEXT u_char*, u_char*, TypeGram);

/* dict.c */
void	get_askknj(SJ3_CONTEXT2);
int	seldict(SJ3_CONTEXT TypeDicID);
u_char* get_idxptr(SJ3_CONTEXT TypeDicSeg);

/* functbl.c */
extern VFunc   cvtnum_func[];
extern UCPFunc getkan_func[];
extern IFunc   setjrec_func[];
extern IFunc   hiraknj_func[];

/* fuzoku.c */
void	    setclrec(SJ3_CONTEXT JREC*, u_char*, TypeCnct);
RECURS void srchfzk(SJ3_CONTEXT JREC*, u_char*, TypeCnct, int);

/* fzkyomi.c */
extern u_char* fzkadr[];

/* getkanji.c */
u_char* getkan_none(u_char*, u_char*, u_char*, int, int);
u_char* getkan_hira(u_char*, u_char*, u_char*, int, int);
u_char* getkan_kata(u_char*, u_char*, u_char*, int, int);
u_char* getkan_knj(SJ3_CONTEXT u_char*, u_char*, u_char*, int, int);
u_char* getkan_ofs(SJ3_CONTEXT u_char*, u_char*, u_char*, int, int);
u_char* getkan_norm(u_char*, u_char*, u_char*, int, int);
u_char* getkan_ascii(u_char*, u_char*, u_char*, int, int);
int	getkanji(SJ3_CONTEXT u_char*, int, u_char*, u_char*);

/* getrank.c */
void getrank(SJ3_CONTEXT2);

/* hzstrlen.c */
int euc_codesize(u_char c);
int hzstrlen(u_char*, int);

/* init.c */
void seg_count(SJ3_CONTEXT DICT*);
void mkidxtbl(SJ3_CONTEXT DICT*);
void initwork(SJ3_CONTEXT2);
#ifndef SJ3_GLOBAL
Sj3Context* alloccontext(const char*);
void	    free_context(Sj3Context*);
#endif

/* istrcmp.c */
int istrcmp(u_char*, u_char*, int, int);

/* memcpy.c */
void sstrncpy(u_char*, u_char*, int);

/* memory.c */
JREC*  free_jlst(JREC*);
CLREC* free_clst(SJ3_CONTEXT CLREC*, int);
void   free_clall(CLREC*);
void   free_jall(JREC*);
void   freework(SJ3_CONTEXT2);

/* mk2claus.c */
void mk2claus(SJ3_CONTEXT2);

/* mkbunset.c */
void   mkbunsetu(SJ3_CONTEXT2);
CLREC* argclrec(SJ3_CONTEXT int);

/* mkjiritu.c */
void  mkjiritu(SJ3_CONTEXT int);
JREC* argjrec(SJ3_CONTEXT int, JREC*);

/* mkkouho.c */
void	mkkouho(SJ3_CONTEXT2);
u_char* makekan_none(SJ3_CONTEXT u_char*, u_char*, int);
u_char* makekan_1byte(SJ3_CONTEXT u_char*, u_char*, int);
u_char* makekan_knj(SJ3_CONTEXT u_char*, u_char*, int);
u_char* makekan_ofs(SJ3_CONTEXT u_char*, u_char*, int);
u_char* makekan_norm(SJ3_CONTEXT u_char*, u_char*, int);
u_char* makekan_ascii(SJ3_CONTEXT u_char*, u_char*, int);
int	sel_sjmode(JREC*);

/* mknumber.c */
void num_type00(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type01(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type02(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type03(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type04(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type05(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type06(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type07(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type08(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type09(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type10(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type11(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type12(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type13(SJ3_CONTEXT u_char*, u_char*, JREC*);
void num_type14(SJ3_CONTEXT u_char*, u_char*, JREC*);

/* mvmemd.c */
void mvmemd(u_char*, u_char*, int);

/* mvmemi.c */
void mvmemi(u_char*, u_char*, int);

/* peepdic.c */
int getusr(SJ3_CONTEXT u_char*);
int nextusr(SJ3_CONTEXT u_char*);
int prevusr(SJ3_CONTEXT u_char*);

/* ph2knj.c */
int ph2knj(SJ3_CONTEXT u_char*, u_char*, int);

/* ph_khtbl.c */
int ph_khtbl(SJ3_CONTEXT CLREC*);

/* priority.c */
int  priority(SJ3_CONTEXT CLREC*);
void pritiny(SJ3_CONTEXT2);

/* prtytbl.c */
extern u_char taipri[7][45];
extern u_char sttpri[5][31];

/* s2ctbl.c */
extern u_char kigou[];

/* sdepend.c */
DictFile* opendict(SJ3_CONTEXT char*, char*);
int	  closedict(SJ3_CONTEXT DictFile*);
void	  lock_dict(DictFile*, int);
void	  unlock_dict(DictFile*, int);
int	  is_dict_locked(DictFile*);
StdyFile* openstdy(char*, char*);
int	  closestdy(StdyFile*);
int	  putstydic(SJ3_CONTEXT2);
int	  putcldic(SJ3_CONTEXT2);
int	  makedict(char*, int, int, int);
int	  makestdy(char*, int, int, int);
void	  sj_closeall(SJ3_CONTEXT2);
int	  set_dictpass(DictFile*, char*);
int	  set_stdypass(SJ3_CONTEXT char*);
int	  set_dictcmnt(DictFile*, char*);
int	  set_stdycmnt(SJ3_CONTEXT char*);
int	  get_stdysize(SJ3_CONTEXT int*, int*, int*);

/* selclrec.c */
void selclrec(SJ3_CONTEXT2);

/* selsuuji.c */
extern u_short* selsjadrs[];

/* setconj.c */
int setconj(SJ3_CONTEXT TypeGram, JREC*, CREC*);

/* setjrec.c */
int  setj_atrb(SJ3_CONTEXT u_char*);
int  setj_ofs(SJ3_CONTEXT u_char*);
int  setj_knj(SJ3_CONTEXT u_char*);
int  setj_norm1(SJ3_CONTEXT u_char*);
int  setj_norm2(SJ3_CONTEXT u_char*);
int  setj_norm3(SJ3_CONTEXT u_char*); /* unused??? */
void setjrec(SJ3_CONTEXT u_char*, int);
void setnumrec(SJ3_CONTEXT u_char*, JREC*, TypeGram);
void setcrec(SJ3_CONTEXT u_char*);

/* setkouho.c */
void setkouho(SJ3_CONTEXT CLREC*, TypeDicOfs, int);
void ph_setkouho(SJ3_CONTEXT CLREC*, TypeDicOfs, STDYIN*);
int  hiraknj_atrb(SJ3_CONTEXT u_char*, int*);
int  hiraknj_ofs(SJ3_CONTEXT u_char*, int*);
int  hiraknj_knj(SJ3_CONTEXT u_char*, int*);
int  hiraknj_hask(SJ3_CONTEXT u_char*, int*);
int  hiraknj_kask(SJ3_CONTEXT u_char*, int*);
int  hiraknj_norm(SJ3_CONTEXT u_char*, int*);
int  hiraknj_hira(SJ3_CONTEXT u_char*, int*);

/* setubi.c */
u_char* getstb(TypeGram);
void	setubi(SJ3_CONTEXT JREC*, u_char*);

/* sj2code.c */
int sj2cd_chr(u_char*, u_char*);
int sj2cd_str(u_char*, u_char*, int);

/* skiphblk.c */
u_char* skiphblk(u_char*);

/* skipkstr.c */
u_char* skipkstr(u_char*);

/* srchdict.c */
int	yomicmp(SJ3_CONTEXT u_char*, u_char*, u_char*);
u_char* srchdict(SJ3_CONTEXT u_char*);

/* srchhead.c */
int srchhead(SJ3_CONTEXT2);

/* srchidx.c */
TypeDicSeg srchidx(SJ3_CONTEXT TypeDicSeg, int);

/* srchnum.c*/
void srchnum(SJ3_CONTEXT2);
void setwdnum(SJ3_CONTEXT u_char*, int, u_short*);
int  setucnum(SJ3_CONTEXT u_char*, int, u_char*);

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
int	study(SJ3_CONTEXT STDYOUT*);
STDYIN* srchstdy(SJ3_CONTEXT TypeDicSeg, TypeDicOfs, TypeDicID);

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
int terminate(SJ3_CONTEXT TypeCnct, u_char*);

/* termtbl.c */
extern u_char termtbl[];

/* wakachi.c */
void wakachi(SJ3_CONTEXT2);

#endif /* KANAKAN_H */
