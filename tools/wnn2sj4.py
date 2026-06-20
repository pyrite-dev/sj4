#!/usr/bin/env python3

# THIS IS FOR SYSTEM DICTIONARY GENERATION!!!!!

import sys
import re
from math import *
from collections import defaultdict

HINSHI = {
	"名詞": "名１",
	"固有名詞": "名１",
	"単漢字": "名１",

	"人名": "名前",
	"地名": "地名",
	"人名&地名": "地名",

	"数詞": "数詞",
	"助数詞": "助数",

	"サ行(する)&名詞": "名６",

	"形容動詞": "形動１",
	"形容動詞&名詞": "形動１",
	"形容動詞(たる)": "形動１",

	"形容詞": "形１",

	"副詞": "副１",

	"連体詞": "連体",

	"接続詞,感動詞": "接続",

	"一段": "一段１",
	"一段&名詞": "一段１",

	"カ行五段": "カ五１",
	"ガ行五段": "ガ五１",
	"サ行五段": "サ五１",
	"タ行五段": "タ五１",
	"ナ行五段": "ナ五",
	"バ行五段": "バ五１",
	"マ行五段": "マ五１",
	"ラ行五段": "ラ五１",
	"ワ行五段": "ワ五１",

	"サ行(する)": "サ変",
	"ザ行(ずる)": "ザ変",

	"ラ行(下さい)": "ラ五１",
	"カ行(行く)": "カ五１",

	"接頭語": "接頭１",
	"接頭語(お)": "接頭１",
	"接頭語(各)": "接頭１",

	"接頭助数詞": "接頭４",
	"接頭数詞": "接頭４",
	"接頭地名": "接頭３",

	"接尾語": "接尾１",
	"接尾人名": "接尾１",
	"接尾地名": "接尾４",

	"接尾助数詞": "助数",

	"接尾動詞": "接尾５",
	"形容動詞化接尾語": "接尾６",
	"サ行(する)&名詞化接尾語": "接尾７",
	"形容詞化接尾動詞": "接尾５",

	"一動幹": "一段１"
}

SKIP = {
	"記号",
	"開括弧",
	"閉括弧",

	"来(こ)",
	"来(く)",
	"来(き)",

	"為(せ)",
	"為(し)",
	"為(す)"
}

def p_freq(fields):
	if len(fields) < 4:
		return 0

	return int(fields[-1])

if len(sys.argv) != 3:
	print("usage: wnn2sj4.py input.u output.dic", file=sys.stderr)
	sys.exit(1)

inp = sys.argv[1]
outp = sys.argv[2]

entries = {}

fp = open(inp, "r", encoding="euc_jp")
for lineno, line in enumerate(fp, 1):
	line = line.rstrip()

	if not(line):
		continue

	fields = line.split()

	if len(fields) < 3:
		continue

	y = fields[0]
	k = fields[1]
	h = fields[2]

	if h in SKIP:
		continue

	if re.findall(r'[^あ-んー]', y):
		continue

	sj = HINSHI[h]

	if sj is None:
		sys.stderr.write(f"{lineno}: unknown hinshi: {h}\n")
		sj = "名１"

	freq = p_freq(fields)

	key = (y, k, sj)

	old = entries.get(key)

	if old is None or freq > old:
		entries[key] = freq
fp.close()

grouped = defaultdict(list)

maxf = 0

for (y, k, sj), freq in entries.items():
	if maxf < freq:
		maxf = freq

	grouped[y].append((freq, k, sj, freq))

fp = open(outp, "w", encoding="utf-8")
for y in sorted(grouped.keys()):
	lst = grouped[y]

	lst.sort(key=lambda x: (-x[0], x[1]))

	t = "\t" * (4 - (len(y) // 4))

	for freq, k, sj, freq in lst:
		t2 = "\t" * (4 - (len(k) // 4))
		freq = floor(freq / maxf * 0xffff)
		fp.write(f"{y}{t}{k}{t2}{sj}:[C{freq}]:\n")
fp.close()
