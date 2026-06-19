#!/bin/sh

if [ "x$1" = "x" ]; then
	echo "Usage: $0 [watcom|msvc]"
	exit 1
fi

for i in lib/sj4core/*.c; do
	if [ -f $i.obj ]; then
		continue
	fi
	echo $i

	if [ "$1" = "watcom" ]; then
		cl /Iinclude/sj4common /Iinclude/sj4core /c /fo$i.obj $i || exit 1
	else
		owcc -Iinclude/sj4common -Iinclude/sj4core -bnt -c -o $i.obj $i || exit 1
	fi
done

find lib/sj4core -name "*.obj" | while read a; do
	echo "+$a"
done | xargs wlib -q -b -fo -n sj4core.lib

executable() {
	N=$1

	for i in src/$N/*.c; do
		if [ -f $i.obj ]; then
			continue
		fi
		echo $i
		owcc -DUTF8 -DENGLISH -Iinclude/sj4common -Iinclude/sj4core -bnt -c -o $i.obj $i || exit 1
	done

	owcc -bnt -o $N.exe src/$N/*.obj $2
}

executable sj4mkdic
executable sj4test sj4core.lib
