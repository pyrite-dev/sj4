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


#define SJ4_InternalError -1

#define SJ4_NormalEnd 0

#define SJ4_ServerDown 1
#define SJ4_OpenSocket 2
#define SJ4_ConnectSocket 3
#define SJ4_GetHostByName 4
#define SJ4_NotOpened 5
#define SJ4_NotEnoughMemory 6
#define SJ4_IllegalCommand 7

#define SJ4_DifferentVersion 11
#define SJ4_NoHostName 12
#define SJ4_NoUserName 13
#define SJ4_NotAllowedUser 14
#define SJ4_AlreadyConnected 15
#define SJ4_NotConnected 16

#define SJ4_TooLongParameter 21
#define SJ4_IllegalParameter 22

#define SJ4_BadDictID 31
#define SJ4_IllegalDictFile 32
#define SJ4_IllegalStdyFile 33
#define SJ4_IncorrectPasswd 34
#define SJ4_FileNotExist 35
#define SJ4_CannotAccessFile 36
#define SJ4_CannotOpenFile 37
#define SJ4_CannotCreateFile 38
#define SJ4_FileReadError 39
#define SJ4_FileWriteError 40
#define SJ4_FileSeekError 41

#define SJ4_StdyAlreadyOpened 51
#define SJ4_StdyFileNotOpened 52
#define SJ4_TooSmallStdyArea 53

#define SJ4_LockedByOther 61
#define SJ4_NotLocked 62

#define SJ4_NoSuchDict 71
#define SJ4_ReadOnlyDict 72
#define SJ4_DictLocked 73
#define SJ4_BadYomiString 74
#define SJ4_BadKanjiString 75
#define SJ4_BadHinsiCode 76

#define SJ4_AddDictFailed 81
#define SJ4_AlreadyExistWord 82
#define SJ4_NoMoreDouonWord 83
#define SJ4_NoMoreUserDict 84
#define SJ4_NoMoreIndexBlock 85

#define SJ4_DelDictFailed 91
#define SJ4_NoSuchWord 92

#define SJ4_DirAlreadyExist 101
#define SJ4_CannotCreateDir 102

#define SJ4_NoMoreDictData 111

#define SJ4_UserConnected 121

#define SJ4_TooLongPasswd 131
#define SJ4_TooLongComment 132

#define SJ4_CannotCodeConvert 133
