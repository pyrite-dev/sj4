#!/bin/sh

cmp="$1"

if [ "x$cmp" = "x" ]; then
	echo "Usage: $0 [watcom|msvc]"
	exit 1
fi

INCS="sj4common sj4core sj4rkcv sj4lib"
DEFS="UTF8 UCS ENGLISH"

CFLAGS=""

if [ "$cmp" = "watcom" ]; then
	for def in $DEFS; do
		CFLAGS="$CFLAGS -D$def"
	done
	for inc in $INCS; do
		CFLAGS="$CFLAGS -Iinclude/$inc"
	done
elif [ "$cmp" = "msvc" ]; then
	for def in $DEFS; do
		CFLAGS="$CFLAGS /D$def"
	done
	for inc in $INCS; do
		CFLAGS="$CFLAGS /Iinclude/$inc"
	done
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
			owcc -bnt $CFLAGS -c -o $i.obj $i || exit 1
		elif [ "$cmp" = "msvc" ]; then
			cl.exe /nologo $CFLAGS /c /Fo$i.obj $i || exit 1
		fi
	done

	if [ -f $N.lib ]; then
		return
	fi

	echo $N.lib

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
			owcc -bnt $CFLAGS -c -o $i.obj $i || exit 1
		elif [ "$cmp" = "msvc" ]; then
			cl.exe /nologo $CFLAGS /c /Fo$i.obj $i || exit 1
		fi
	done

	if [ -f $N.exe ]; then
		return
	fi

	echo "$N.exe"

	if [ "$cmp" = "watcom" ]; then
		owcc -bnt -o $N.exe src/$N/*.obj $2
	elif [ "$cmp" = "msvc" ]; then
		cl.exe /nologo /Fe$N.exe src/$N/*.obj /link $2
	fi
}

library sj4common
library sj4core
library sj4rkcv
library sj4lib

executable sj4mkdic sj4common.lib
executable sj4test "sj4common.lib sj4core.lib sj4lib.lib"
