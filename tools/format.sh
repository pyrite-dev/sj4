#!/bin/sh
while [ ! -d .git ]; do
	cd ..
done
clang-format --verbose -i `find src lib include "(" -name "*.c" -or -name "*.h" ")" -and -not -name sj_euc2ucs.h -and -not -name sj_ucs2euc.h  -and -not -name maindic"*".c`
