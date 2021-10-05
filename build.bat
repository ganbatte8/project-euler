@echo off

set CommonCompilerFlags= -nologo -MTd -Gm- -GR- -EHa- -FC -Z7 -O2 -WX -W4 -wd4706 -wd4505 -wd4201 -wd4100 -wd4189 -wd4456 -wd4996 -DDEBUG=0


cl %CommonCompilerFlags% euler.c
