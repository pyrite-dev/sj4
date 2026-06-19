#!/bin/sh
while [ ! -d .git ]; do
	cd ..
done
clang-format --verbose -i `find src lib include "(" -name "*.c" -or -name "*.h" ")" -and -not -name ucstable.h`
