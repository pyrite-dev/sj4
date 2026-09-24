#!/usr/bin/env python3

# THIS IS FOR SYSTEM DICTIONARY GENERATION!!!!!

import re
import sys

FULLWIDTH_DIGITS = "０１２３４５６７８９"
FULLWIDTH_ALPHABET = "ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ"
FULLWIDTH_PUNCTUATION = "！＂＃＄％＆＇（）＊＋，－．／：；＜＝＞？＠［＼］＾＿｀｛｜｝～　"
FULLWIDTH_ALPHANUMERIC = FULLWIDTH_DIGITS + FULLWIDTH_ALPHABET
FULLWIDTH_ALL = FULLWIDTH_ALPHANUMERIC + FULLWIDTH_PUNCTUATION

HALFWIDTH_DIGITS = "0123456789"
HALFWIDTH_ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
HALFWIDTH_PUNCTUATION = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ "
HALFWIDTH_ALPHANUMERIC = HALFWIDTH_DIGITS + HALFWIDTH_ALPHABET
HALFWIDTH_ALL = HALFWIDTH_ALPHANUMERIC + HALFWIDTH_PUNCTUATION

KATAKANA = "アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヰヲヱンッャュョァィゥェォガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポ"
HIRAGANA = "あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわゐをゑんっゃゅょぁぃぅぇぉがぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽ"

if len(sys.argv) != 3:
	print("usage: mecab2sj4.py input.csv output.dic", file=sys.stderr)
	sys.exit(1)

inp = sys.argv[1]
outp = sys.argv[2]

try:
	fp = open("dict/visual.plus.dic", "r")
except:
	fp = open("../dict/visual.plus.dic", "r")

dic = {}
fhtrans = str.maketrans(FULLWIDTH_ALL, HALFWIDTH_ALL)
hftrans = str.maketrans(HALFWIDTH_ALL, FULLWIDTH_ALL)
khtrans = str.maketrans(KATAKANA, HIRAGANA)

def maybe_new(read, write):
	if not(read in dic):
		dic[read] = {
			"types": [],
			"costs": [],
			"write": write.translate(hftrans)
		}

def append_type(read, type, cost=0):
	if not(type in dic[read]["types"]):
		dic[read]["types"].append(type)
		dic[read]["costs"].append(cost)

for lineno, line in enumerate(fp, 1):
	line = line.rstrip()

	if not(line):
		continue

	words = list(filter(None, re.split(r'[: \t]+', line)))
	for i in words[2:]:
		maybe_new(words[0], words[1])
		append_type(words[0], i)

fp = open(inp, "r")
for lineno, line in enumerate(fp, 1):
	line = line.rstrip()

	if not(line):
		continue

	words = line.split(',')
	types = words[4:10]
	read = words[11].translate(khtrans).translate(khtrans).replace(":", "")
	write = words[0].translate(fhtrans)
	cost = int(int(words[3]) / 250)
	if set(["名詞", "固有名詞", "人名", "姓"]) <= set(types):
		maybe_new(read, write)
		append_type(read, "苗字", cost)
	elif set(["名詞", "固有名詞", "人名", "名"]) <= set(types):
		maybe_new(read, write)
		append_type(read, "名前", cost)
	elif set(["名詞", "固有名詞", "地域", "一般"]) <= set(types):
		maybe_new(read, write)
		append_type(read, "地名", cost)
	elif set(["名詞", "固有名詞", "組織"]) <= set(types):
		maybe_new(read, write)
		append_type(read, "企業", cost)

def reptab(s):
	n = 6 - int(len(s) / 4)

	if n <= 0:
		return " "
	return "	" * n

fp = open(outp, "w")
s = sorted(list(dic.keys()))
for i in s:
	n = 0
	incr = 0
	print(i + reptab(i) + dic[i]["write"] + reptab(dic[i]["write"]), end="", file=fp)
	for j in dic[i]["types"]:
		if n == 1:
			print("", file=fp)
			print(i + reptab(i) + dic[i]["write"] + reptab(dic[i]["write"]), end="", file=fp)
			n = 0
		print(j, end="", file=fp)
		print(":[C" + str(dic[i]["costs"][incr]) + "]", end="", file=fp)
		print(":", end="", file=fp)
		n = n + 1
		incr = incr + 1
	print("", file=fp)
