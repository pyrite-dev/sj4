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

KATAKANA = "アイウエオカキクケコタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン"
HIRAGANA = "あいうえおかきくけこたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをん"

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

for lineno, line in enumerate(fp, 1):
	line = line.rstrip()

	if not(line):
		continue

	words = list(filter(None, re.split(r'[: \t]+', line)))
	for i in words[2:]:
		if not(words[0] in dic):
			dic[words[0]] = {
				"types": [],
				"write": words[1].translate(hftrans)
			}
		if not(i in dic[words[0]]["types"]):
			dic[words[0]]["types"].append(i)

fp = open(inp, "r")
for lineno, line in enumerate(fp, 1):
	line = line.rstrip()

	if not(line):
		continue

	words = line.split(',')
	types = words[4:10]
	read = words[11].translate(khtrans)
	write = words[0].translate(fhtrans)
	if not(read in dic):
		next

def reptab(s):
	n = 4 - int(len(s) / 4)

	return "	" * n

fp = open(outp, "w")
s = sorted(list(dic.keys()))
for i in s:
	n = 0
	print(i + reptab(i) + dic[i]["write"] + reptab(dic[i]["write"]), end="", file=fp)
	for j in dic[i]["types"]:
		if n == 4:
			print("", file=fp)
			print(i + reptab(i) + dic[i]["write"] + reptab(dic[i]["write"]), end="", file=fp)
			n = 0
		print(j + ":", end="", file=fp)
		n = n + 1
	print("", file=fp)
