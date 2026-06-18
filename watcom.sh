#!/bin/sh

for i in lib/sj3core/*.c; do
	if [ -f $i.obj ]; then
		continue
	fi
	echo $i
	owcc -Iinclude/sj3common -Iinclude/sj3core -bnt -c -o $i.obj $i || exit 1
done

find lib/sj3core -name "*.obj" | while read a; do
	echo "+$a"
done | xargs wlib -q -b -fo -n sj3core.lib
