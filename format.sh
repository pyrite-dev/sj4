#!/bin/sh
clang-format --verbose -i `find src lib include -name "*.c" -or -name "*.h"`
