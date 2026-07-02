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


#define SJ4SERV_VERSION_NO 2

#define SJ4_CONNECT 1
#define SJ4_DISCONNECT 2

#define SJ4_OPENDICT 11
#define SJ4_CLOSEDICT 12

#define SJ4_OPENSTDY 21
#define SJ4_CLOSESTDY 22
#define SJ4_STDYSIZE 23

#define SJ4_LOCK 31
#define SJ4_UNLOCK 32

#define SJ4_PH2KNJ 41

#define SJ4_CL2KNJ 51
#define SJ4_NEXTCL 52
#define SJ4_PREVCL 53
#define SJ4_CL2KNJ_ALL 54
#define SJ4_CL2KNJ_CNT 55

#define SJ4_STUDY 61
#define SJ4_CLSTUDY 62

#define SJ4_ADDDICT 71
#define SJ4_DELDICT 72

#define SJ4_MAKEDICT 81
#define SJ4_MAKESTDY 82
#define SJ4_MAKEDIR 83
#define SJ4_ACCESS 84

#define SJ4_GETDICT 91
#define SJ4_NEXTDICT 92
#define SJ4_PREVDICT 93

#define SJ4_WHO 100
#define SJ4_QUIT 101
#define SJ4_KILL 102
#define SJ4_VERSION 103
#define SJ4_DICTPASS 104
#define SJ4_DICTCMNT 105
#define SJ4_STDYPASS 106
#define SJ4_STDYCMNT 107

#define SJ4_SETCVER 110
#define SJ4_PH2KNJ_EUC 111

#define SJ4_CL2KNJ_EUC 112
#define SJ4_NEXTCL_EUC 113
#define SJ4_PREVCL_EUC 114
#define SJ4_CL2KNJ_ALL_EUC 115
#define SJ4_CL2KNJ_CNT_EUC 116

#define SJ4_CLSTUDY_EUC 117

#define SJ4_ADDDICT_EUC 118
#define SJ4_DELDICT_EUC 119

#define SJ4_GETDICT_EUC 120
#define SJ4_NEXTDICT_EUC 121
#define SJ4_PREVDICT_EUC 122
