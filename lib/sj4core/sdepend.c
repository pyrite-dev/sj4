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
#include "Dict.h"
#include "sj4err.h"

#include "sj_kanakan.h"

#if defined(__FD_SET) && !defined(FD_SET)
#define FD_SET __FD_SET
#define FD_CLR __FD_CLR
#endif

#ifndef L_SET
#define L_SET SEEK_SET
#endif

#ifndef L_CUR
#define L_CUR SEEK_CUR
#endif

#ifndef L_END
#define L_END SEEK_END
#endif

#ifdef SJ4_GLOBAL
DictFile* dictlink = NULL;
StdyFile* stdylink = NULL;
#else
#define dictlink ctx->dictlink
#define stdylink ctx->stdylink
#endif

static long
get4byte(u_char* p) {
	long i;

	i = *p;
	i = (i << 8) + *++p;
	i = (i << 8) + *++p;
	return ((i << 8) + *++p);
}
static void
Put4byte(u_char* p, long n) {
	p += 3;
	*p-- = n;
	n >>= 8;
	*p-- = n;
	n >>= 8;
	*p-- = n;
	n >>= 8;
	*p = n;
}
#define put4byte(p, n) Put4byte((p), (long)(n))

static int
fgetfile(VFile* vfp, FILE* fp, long pos, long len, u_char* p) {
	if(vfp != NULL && vfp->buffer != NULL) {
		if(pos >= vfp->size) {
			serv_errno = SJ4_FileSeekError;
			return ERROR;
		}
		if((pos + len) > vfp->size) {
			serv_errno = SJ4_FileSeekError;
			return ERROR;
		}

		memcpy(p, vfp->buffer + pos, len);

		return SJ4_NormalEnd;
	}

	if(fseek(fp, pos, 0) == ERROR) {
		serv_errno = SJ4_FileSeekError;
		return ERROR;
	}
	if(fread(p, len, 1, fp) != 1) {
		serv_errno = SJ4_FileReadError;
		return ERROR;
	}
	return SJ4_NormalEnd;
}
static int
fputfile(FILE* fp, long pos, int len, u_char* p) {
	if(fseek(fp, pos, 0) == ERROR) {
		serv_errno = SJ4_FileSeekError;
		return ERROR;
	}
	if(fwrite(p, len, 1, fp) != 1) {
		serv_errno = SJ4_FileWriteError;
		return ERROR;
	}
	return SJ4_NormalEnd;
}

static int
check_passwd(u_char* buf, char* passwd) {
	buf += PASSWDPOS;
	return (*buf && strncmp(passwd, (char*)buf, PASSWDLEN)) ? FALSE : TRUE;
}
static void
set_passwd(u_char* buf, char* passwd) {
	strncpy((char*)buf + PASSWDPOS, passwd, PASSWDLEN);
}

static int
check_dictfile(u_char* buf) {
	return (DICTVERSION != get4byte(buf + VERSIONPOS)) ? FALSE : TRUE;
}

static DictFile*
search_same_dict(SJ4_CONTEXT ino_t ino) {
	DictFile* p;

	for(p = dictlink; p != NULL; p = p->link)
		if(p->dict.dicid == ino) return p;

	return NULL;
}

static int
getofs(SJ4_CONTEXT DictFile* dp) {
	idxofs = dp->ofsptr;
	return 0;
}
static int
getidx(SJ4_CONTEXT DictFile* dp) {
	idxbuf = dp->buffer + dp->idxstrt;
	return 0;
}
static int
getdic(SJ4_CONTEXT DictFile* dp, TypeDicSeg seg) {
	if(seg >= dp->dict.segunit) return -1;
	dicbuf = dp->buffer + dp->segstrt + dp->dict.seglen * seg;
	return 0;
}
static int
putidx(SJ4_CONTEXT DictFile* dp, short dummy) {
	return fputfile(dp->fp, dp->idxstrt, dp->dict.idxlen, idxbuf);
}
static int
putdic(SJ4_CONTEXT DictFile* dp, TypeDicSeg seg) {
	u_char* p;
	long	i, j;

	if(seg >= dp->dict.maxunit) return -1;
	i = dp->segstrt + dp->dict.seglen * (seg + 1);
	if(i > dp->bufsiz) {
		if(!(p = (u_char*)malloc(i))) return -1;
		memcpy(p, dp->buffer, dp->bufsiz);

		if(idxbuf - dp->buffer == dp->idxstrt)
			idxbuf = p + dp->idxstrt;
		if((dicbuf >= dp->buffer) &&
		   ((j = dicbuf - dp->buffer) < dp->bufsiz))
			dicbuf = p + j;

		free((char*)(dp->buffer));
		dp->buffer = p;
		dp->bufsiz = i;
	}

	i = dp->segstrt + dp->dict.seglen * seg;
	if((p = dp->buffer + i) != dicbuf) memcpy(p, dicbuf, dp->dict.seglen);

	return fputfile(dp->fp, i, dp->dict.seglen, p);
}
static int
rszdic(SJ4_CONTEXT DictFile* dp, TypeDicSeg seg) {
	long	i;
	u_char* p;

	i = dp->segstrt + dp->dict.seglen * seg;
	if(dp->bufsiz != i) {
		if(!(p = (u_char*)malloc(i))) return -1;
		memcpy(p, dp->buffer, ((i > dp->bufsiz) ? dp->bufsiz : i));

		if(idxbuf - dp->buffer == dp->idxstrt)
			idxbuf = p + dp->idxstrt;
		if((dicbuf >= dp->buffer) &&
		   ((i = dicbuf - dp->buffer) < dp->bufsiz))
			dicbuf = p + i;

		free((char*)(dp->buffer));
		dp->buffer = p;
		dp->bufsiz = i;
		if(sj4_ftruncate(dp->fp, i) == -1) return -1;
	}
	put4byte(dp->buffer + DICTSEGNUM, seg);
	return fputfile(dp->fp, 0, HEADERLENGTH, dp->buffer);
}

DictFile*
opendict(SJ4_CONTEXT char* name, char* passwd) {
	FILE*	    fp = NULL;
	struct stat sbuf;
	DictFile*   dfp;
	u_char	    tmp[HEADERLENGTH];
	int	    i;
	u_char*	    dp;
	VFile	    vf;

	memset(&vf, 0, sizeof(vf));

#if defined(EMBED) || defined(EMBED_PLUS)
	if(strcmp(name, "sj4main.dic") == 0) {
		extern u_char sj4main_dic[];
		extern u_int  sj4main_dic_len;

		vf.buffer    = sj4main_dic;
		vf.size	     = sj4main_dic_len;
		sbuf.st_size = vf.size;
		sbuf.st_ino  = -1;
	} else
#endif
	    if(stat(name, &sbuf) == ERROR) {
		if(errno == ENOENT)
			serv_errno = SJ4_FileNotExist;
		else
			serv_errno = SJ4_CannotAccessFile;
		return NULL;
	}

	if((dfp = search_same_dict(SJ4_CONTEXT_PASS sbuf.st_ino)) != NULL) {
		dfp->refcnt++;
		return dfp;
	}

	if(vf.buffer != NULL) {
	} else if((fp = fopen(name, "r+b")) == NULL) {
		if((fp = fopen(name, "rb")) == NULL) {
			serv_errno = SJ4_CannotOpenFile;
			return NULL;
		}
		i = FALSE;
	} else
		i = TRUE;

	if(fgetfile(&vf, fp, 0L, sizeof(tmp), tmp) == ERROR) goto error1;

	if(!check_dictfile(tmp)) {
		serv_errno = SJ4_IllegalDictFile;
		goto error1;
	}

	if(!check_passwd(tmp, passwd)) {
		serv_errno = SJ4_IncorrectPasswd;
		goto error1;
	}

	if((dp = (u_char*)malloc((long)sbuf.st_size)) == NULL) {
		serv_errno = SJ4_NotEnoughMemory;
		goto error1;
	}
	if((dfp = (DictFile*)malloc(sizeof(*dfp))) == NULL) {
		serv_errno = SJ4_NotEnoughMemory;
		goto error2;
	}
	memset(dfp, '\0', sizeof(*dfp));

	if(fgetfile(&vf, fp, 0L, (long)sbuf.st_size, dp) == ERROR) goto error3;

	dfp->dict.dicid	  = sbuf.st_ino;
	dfp->dict.idxlen  = get4byte(dp + DICTIDXLEN);
	dfp->dict.seglen  = get4byte(dp + DICTSEGLEN);
	dfp->dict.segunit = get4byte(dp + DICTSEGNUM);
	dfp->dict.maxunit = i ? get4byte(dp + DICTSEGMAX) : 0;
	dfp->dict.getofs  = (IFuncDict)getofs;
	dfp->dict.getidx  = (IFuncDict)getidx;
	dfp->dict.getdic  = (IFunc)getdic;
	dfp->dict.putidx  = (IFunc)putidx;
	dfp->dict.putdic  = (IFunc)putdic;
	dfp->dict.rszdic  = (IFunc)rszdic;
	dfp->refcnt	  = 1;
	dfp->fp		  = fp;
	dfp->vf		  = vf;
	dfp->buffer	  = dp;
	dfp->bufsiz	  = sbuf.st_size;
	dfp->idxstrt	  = get4byte(dp + DICTIDXPOS);
	dfp->segstrt	  = get4byte(dp + DICTSEGPOS);

	i = dfp->dict.segunit;
	if(i < dfp->dict.maxunit) i = dfp->dict.maxunit;
	i *= sizeof(TypeIdxOfs);
	if((dfp->ofsptr = (TypeIdxOfs*)malloc(i)) == NULL) {
		serv_errno = SJ4_NotEnoughMemory;
		goto error3;
	}
	mkidxtbl(SJ4_CONTEXT_PASS & (dfp->dict));

	dfp->link = dictlink;
	dictlink  = dfp;

	return dfp;

error3:
	free((char*)dfp);
error2:
	free((char*)dp);
error1:
	if(fp != NULL) fclose(fp);

	return NULL;
}

int closedict(SJ4_CONTEXT DictFile* dfp) {
	DictFile* df;

	if(--dfp->refcnt > 0) return 0;

	if(dictlink == dfp)
		dictlink = dfp->link;
	else {
		for(df = dictlink; df; df = df->link)
			if(df->link == dfp) {
				df->link = dfp->link;
				break;
			}
	}

	if(dfp->fp) fclose(dfp->fp);

	free((char*)(dfp->ofsptr));
	free((char*)(dfp->buffer));
	free((char*)dfp);

	return 0;
}

static int
check_stdyfile(u_char* buf) {
	return (STDYVERSION != get4byte(buf + VERSIONPOS)) ? FALSE : TRUE;
}

static StdyFile*
search_same_stdy(SJ4_CONTEXT ino_t ino) {
	StdyFile* p;

	for(p = stdylink; p != NULL; p = p->link)
		if(p->inode == ino) return p;

	return NULL;
}

StdyFile*
openstdy(SJ4_CONTEXT char* name, char* passwd) {
	FILE*	    fp;
	struct stat sbuf;
	StdyFile*   sfp;
	u_char*	    hd;
	STDYIN*	    sp;
	u_short*    cip;
	u_char*	    clp;
	long	    stdycnt, stdypos, stdylen, stdymax;
	long	    clidxpos, clidxlen;
	long	    clstdypos, clstdylen, clstdystep;
	long	    len;

	if(stat(name, &sbuf) == ERROR) {
		if(errno == ENOENT)
			serv_errno = SJ4_FileNotExist;
		else
			serv_errno = SJ4_CannotAccessFile;
		return NULL;
	}

	if((sfp = search_same_stdy(SJ4_CONTEXT_PASS sbuf.st_ino)) != NULL) {
		sfp->refcnt++;
		return sfp;
	}

	if((hd = (u_char*)malloc(HEADERLENGTH + COMMENTLENGTH)) == NULL) {
		serv_errno = SJ4_NotEnoughMemory;
		return NULL;
	}

	if((fp = fopen(name, "r+b")) == NULL) {
		serv_errno = SJ4_CannotOpenFile;
		goto error0;
	}

	if(fgetfile(NULL, fp, 0L, HEADERLENGTH + COMMENTLENGTH, hd) == ERROR)
		goto error1;

	if(!check_stdyfile(hd)) {
		serv_errno = SJ4_IllegalStdyFile;
		goto error1;
	}

	if(!check_passwd(hd, passwd)) {
		serv_errno = SJ4_IncorrectPasswd;
		goto error1;
	}

	stdycnt	   = get4byte(hd + STDYNORMCNT);
	stdypos	   = get4byte(hd + STDYNORMPOS);
	stdylen	   = get4byte(hd + STDYNORMLEN);
	stdymax	   = get4byte(hd + STDYNORMNUM);
	clidxpos   = get4byte(hd + STDYCLIDXPOS);
	clidxlen   = get4byte(hd + STDYCLIDXLEN);
	clstdypos  = get4byte(hd + STDYCLSEGPOS);
	clstdylen  = get4byte(hd + STDYCLSEGLEN);
	clstdystep = get4byte(hd + STDYCLIDXSTEP);
	len	   = sizeof(STDYIN) * stdymax;

	if((sfp = (StdyFile*)malloc(sizeof(*sfp))) == NULL) {
		serv_errno = SJ4_NotEnoughMemory;
		goto error1;
	}
	memset(sfp, '\0', sizeof(*sfp));
	if((sp = (STDYIN*)malloc(len)) == NULL) {
		serv_errno = SJ4_NotEnoughMemory;
		goto error2;
	}
	if((cip = (u_short*)malloc(clidxlen)) == NULL) {
		serv_errno = SJ4_NotEnoughMemory;
		goto error3;
	}
	if((clp = (u_char*)malloc(clstdylen)) == NULL) {
		serv_errno = SJ4_NotEnoughMemory;
		goto error4;
	}

	if(fgetfile(NULL, fp, clidxpos, clidxlen, (u_char*)cip) == ERROR) goto error5;	 /* XXX */
	if(fgetfile(NULL, fp, clstdypos, clstdylen, (u_char*)clp) == ERROR) goto error5; /* XXX */
	if(fgetfile(NULL, fp, stdypos, len, (u_char*)sp) == ERROR) stdycnt = 0;		 /* XXX */

	sfp->stdy.stdycnt    = stdycnt;
	sfp->stdy.stdymax    = stdymax;
	sfp->stdy.stdydic    = sp;
	sfp->stdy.clstdystep = clstdystep;
	sfp->stdy.clstdyidx  = cip;
	sfp->stdy.clstdylen  = clstdylen;
	sfp->stdy.clstdydic  = clp;
	sfp->refcnt	     = 1;
	sfp->inode	     = sbuf.st_ino;
	sfp->fp		     = fp;
	sfp->header	     = hd;

	sfp->link = stdylink;
	stdylink  = sfp;

	return sfp;

error5:
	free((char*)clp);
error4:
	free((char*)cip);
error3:
	free((char*)sp);
error2:
	free((char*)sfp);
error1:
	fclose(fp);
error0:
	free((char*)hd);

	return NULL;
}

int closestdy(SJ4_CONTEXT StdyFile* sfp) {
	StdyFile* sf;

	if(--sfp->refcnt > 0) return 0;

	if(stdylink == sfp)
		stdylink = sfp->link;
	else {
		for(sf = stdylink; sf; sf = sf->link)
			if(sf->link == sfp) {
				sf->link = sfp->link;
				break;
			}
	}

	if(sfp->fp) fclose(sfp->fp);

	free((char*)sfp->stdy.stdydic);
	free((char*)sfp->stdy.clstdyidx);
	free((char*)sfp->stdy.clstdydic);
	free((char*)sfp->header);
	free((char*)sfp);

	return 0;
}

int putstydic(SJ4_CONTEXT2) {
	FILE*	  fp;
	u_char*	  hd;
	StdyFile* sf;
	long	  len;

	sf = (StdyFile*)stdy_base;
	fp = sf->fp;
	hd = sf->header;

	put4byte(hd + STDYNORMCNT, sf->stdy.stdycnt);

	len = sizeof(STDYIN) * sf->stdy.stdymax;
	put4byte(hd + STDYNORMLEN, len);

	if(fputfile(fp, 0, HEADERLENGTH + COMMENTLENGTH, hd)) return ERROR;

	return fputfile(fp, get4byte(hd + STDYNORMPOS), len, (u_char*)sf->stdy.stdydic);
}

int putcldic(SJ4_CONTEXT2) {
	FILE*	  fp;
	u_char*	  hd;
	StdyFile* sf;

	sf = (StdyFile*)stdy_base;
	fp = sf->fp;
	hd = sf->header;

	if(fputfile(fp, get4byte(hd + STDYCLIDXPOS),
		    get4byte(hd + STDYCLIDXLEN), (u_char*)sf->stdy.clstdyidx)) /* XXX */
		return ERROR;
	return fputfile(fp, get4byte(hd + STDYCLSEGPOS),
			get4byte(hd + STDYCLSEGLEN), sf->stdy.clstdydic);
}

int makedict(char* path, int idxlen, int seglen, int segnum) {
	FILE*  fp;
	u_char tmp[HEADERLENGTH + COMMENTLENGTH];
	long   pos = HEADERLENGTH + COMMENTLENGTH;
	int    ret = ERROR;
	int    i;

	if(idxlen < MINIDXLEN || seglen < MINSEGLEN || segnum < MINSEGNUM) {
		serv_errno = SJ4_IllegalParameter;
		return ret;
	}

	if(!(fp = fopen(path, "wb"))) {
		serv_errno = SJ4_CannotCreateFile;
		return ret;
	}
	memset(tmp, '\0', sizeof(tmp));
	put4byte(tmp + VERSIONPOS, DICTVERSION);

	put4byte(tmp + DICTIDXPOS, pos);
	put4byte(tmp + DICTIDXLEN, idxlen);
	if(fseek(fp, pos, 0) == ERROR) {
		serv_errno = SJ4_FileSeekError;
		goto error;
	}
	for(i = 0; i < idxlen; i++)
		if(putc(0, fp)) {
			serv_errno = SJ4_FileWriteError;
			goto error;
		}
	pos += idxlen;

	put4byte(tmp + DICTSEGPOS, pos);
	put4byte(tmp + DICTSEGLEN, seglen);
	put4byte(tmp + DICTSEGNUM, 1);
	put4byte(tmp + DICTSEGMAX, segnum);
	if(fseek(fp, pos, 0) == ERROR) {
		serv_errno = SJ4_FileSeekError;
		goto error;
	}
	if(putc(0, fp)) {
		serv_errno = SJ4_FileWriteError;
		goto error;
	}
	for(i = 1; i < seglen; i++)
		if(putc(0xff, fp) != 0xff) {
			serv_errno = SJ4_FileWriteError;
			goto error;
		}

	if(fputfile(fp, 0L, sizeof(tmp), tmp) == SJ4_NormalEnd)
		ret = SJ4_NormalEnd;
error:
	fclose(fp);
	if(ret != SJ4_NormalEnd) remove(path);
	return ret;
}

int makestdy(char* path, int stynum, int clstep, int cllen) {
	FILE*  fp;
	u_char tmp[HEADERLENGTH + COMMENTLENGTH];
	long   pos = HEADERLENGTH + COMMENTLENGTH;
	int    ret = ERROR;
	int    i, j;

	if(stynum == 0) stynum = MINSTYNUM;
	if(clstep == 0) clstep = MINCLSTEP;
	if(cllen == 0) cllen = MINCLLEN;

	if(stynum < MINSTYNUM || clstep < MINCLSTEP || cllen < MINCLLEN) {
		serv_errno = SJ4_IllegalParameter;
		return ret;
	}
	if(stynum > MAXSTYNUM || clstep > MAXCLSTEP || cllen > MAXCLLEN) {
		serv_errno = SJ4_IllegalParameter;
		return ret;
	}

	if(!(fp = fopen(path, "wb"))) {
		serv_errno = SJ4_CannotCreateFile;
		return ret;
	}
	memset(tmp, '\0', sizeof(tmp));
	put4byte(tmp + VERSIONPOS, STDYVERSION);

	j = 256 * sizeof(u_short);
	put4byte(tmp + STDYCLIDXPOS, pos);
	put4byte(tmp + STDYCLIDXLEN, j);
	put4byte(tmp + STDYCLIDXSTEP, clstep);
	if(fseek(fp, pos, 0) == ERROR) {
		serv_errno = SJ4_FileSeekError;
		goto error;
	}
	for(i = 0; i < j; i++)
		if(putc(0, fp)) {
			serv_errno = SJ4_FileWriteError;
			goto error;
		}
	pos += j;

	put4byte(tmp + STDYCLSEGPOS, pos);
	put4byte(tmp + STDYCLSEGLEN, cllen);
	if(fseek(fp, pos, 0) == ERROR) {
		serv_errno = SJ4_FileSeekError;
		goto error;
	}
	for(i = 0; i < cllen; i++)
		if(putc(0, fp)) {
			serv_errno = SJ4_FileWriteError;
			goto error;
		}
	pos += cllen;

	j = sizeof(STDYIN) * stynum;
	put4byte(tmp + STDYNORMPOS, pos);
	put4byte(tmp + STDYNORMLEN, j);
	put4byte(tmp + STDYNORMNUM, stynum);
	put4byte(tmp + STDYNORMCNT, 0);
	if(fseek(fp, pos, 0) == ERROR) {
		serv_errno = SJ4_FileSeekError;
		goto error;
	}
	for(i = 0; i < j; i++) {
		if(putc(0, fp)) {
			serv_errno = SJ4_FileWriteError;
			goto error;
		}
	}
	pos += j;

	if(fputfile(fp, 0L, sizeof(tmp), tmp) == SJ4_NormalEnd)
		ret = SJ4_NormalEnd;
error:
	fclose(fp);
	if(ret != SJ4_NormalEnd) remove(path);
	return ret;
}

void sj_closeall(SJ4_CONTEXT2) {
	while(dictlink) closedict(SJ4_CONTEXT_PASS dictlink);
	while(stdylink) closestdy(SJ4_CONTEXT_PASS stdylink);
}

int set_dictpass(DictFile* dp, char* pass) {
	set_passwd(dp->buffer, pass);
	return fputfile(dp->fp, 0, HEADERLENGTH + COMMENTLENGTH, dp->buffer);
}

int set_stdypass(SJ4_CONTEXT char* pass) {
	StdyFile* sp;

	sp = (StdyFile*)stdy_base;
	set_passwd(sp->header, pass);
	return fputfile(sp->fp, 0, HEADERLENGTH + COMMENTLENGTH, sp->header);
}

static void
set_comment(u_char* buf, char* comment) {
	sj4_strlcpy((char*)buf + HEADERLENGTH, comment, COMMENTLENGTH);
}

int set_dictcmnt(DictFile* dp, char* cmnt) {
	set_comment(dp->buffer, cmnt);
	return fputfile(dp->fp, 0, HEADERLENGTH + COMMENTLENGTH, dp->buffer);
}

int set_stdycmnt(SJ4_CONTEXT char* cmnt) {
	StdyFile* sp;

	sp = (StdyFile*)stdy_base;
	set_comment(sp->header, cmnt);
	return fputfile(sp->fp, 0, HEADERLENGTH + COMMENTLENGTH, sp->header);
}

int get_stdysize(SJ4_CONTEXT int* stynum, int* clstep, int* cllen) {
	*stynum = STUDYMAX;
	*clstep = CLSTUDYSTEP;
	*cllen	= CLSTUDYLEN;
	return 1;
}
