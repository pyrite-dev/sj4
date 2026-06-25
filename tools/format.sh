#!/bin/sh
while [ ! -d .git ]; do
	cd ..
done
clang-format --verbose -i `find src lib include "(" -name "*.c" -or -name "*.h" ")" -and -not -name euc2ucs.c -and -not -name ucs2euc.c  -and -not -name maindic"*".c`
