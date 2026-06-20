#!/bin/sh

cmp="$1"

if [ "x$cmp" = "x" ]; then
	echo "Usage: $0 [watcom|msvc]"
	exit 1
fi

if [ "$cmp" = "watcom" ]; then
	:
elif [ "$cmp" = "msvc" ]; then
	:
else
	echo "Invalid target"
	exit 1
fi

library() {
	N=$1

	for i in lib/$N/*.c; do
		if [ -f $i.obj ]; then
			continue
		fi
		echo $i

		if [ "$cmp" = "watcom" ]; then
			owcc -Iinclude/sj4common -Iinclude/sj4compat -Iinclude/$N -bnt -c -o $i.obj $i || exit 1
		elif [ "$cmp" = "msvc" ]; then
			cl.exe /nologo /Iinclude/sj4common /Iinclude/sj4compat /Iinclude/$N /c /Fo$i.obj $i || exit 1
		fi
	done

	if [ "$cmp" = "watcom" ]; then
		find lib/$N -name "*.obj" | while read a; do
			echo "+$a"
		done | xargs wlib -q -b -fo -n $N.lib
	elif [ "$cmp" = "msvc" ]; then
		lib.exe /nologo /out:$N.lib lib/$N/*.obj
	fi
}

executable() {
	N=$1

	for i in src/$N/*.c; do
		if [ -f $i.obj ]; then
			continue
		fi
		echo $i
		if [ "$cmp" = "watcom" ]; then
			owcc -DUTF8 -DENGLISH -Iinclude/sj4common -Iinclude/sj4core -bnt -c -o $i.obj $i || exit 1
		elif [ "$cmp" = "msvc" ]; then
			cl.exe /nologo /DUTF8 /DENGLISH /Iinclude/sj4common /Iinclude/sj4core /c /Fo$i.obj $i || exit 1
		fi
	done

	if [ "$cmp" = "watcom" ]; then
		owcc -bnt -o $N.exe src/$N/*.obj $2
	elif [ "$cmp" = "msvc" ]; then
		cl.exe /nologo /Fe$N.exe src/$N/*.obj /link $2
	fi
}

library sj4common
library sj4core
library sj4rkcv

executable sj4mkdic
executable sj4test "sj4common.lib sj4core.lib"
