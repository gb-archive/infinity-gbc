setlocal

if not exist %MAKE%\NUL mkdir %MAKE%

set CUR_DIR=%CD%
cd /d %SOURCE%

lcc          -c       -o %MAKE%\e.o   e.c
lcc          -c -Wa-g -o %MAKE%\asm.o asmfunc.s

lcc -Wf-bo1  -c       -o %MAKE%\j.o   j.c
lcc -Wf-bo2  -c       -o %MAKE%\j2.o  j2.c

lcc -Wf-bo3  -c       -o %MAKE%\d1.o  d1.c
lcc -Wf-bo4  -c       -o %MAKE%\d2.o  d2.c

lcc -Wf-bo5  -c       -o %MAKE%\m.o   m.c
lcc -Wf-bo6  -c       -o %MAKE%\m2.o  m2.c

lcc -Wf-bo7  -c       -o %MAKE%\b.o   b.c
lcc -Wf-bo8  -c       -o %MAKE%\b2.o  b2.c
lcc -Wf-bo9  -c       -o %MAKE%\b3.o  b3.c
lcc -Wf-bo10 -c       -o %MAKE%\b4.o  b4.c
lcc -Wf-bo11 -c       -o %MAKE%\b5.o  b5.c
lcc -Wf-bo12 -c       -o %MAKE%\b6.o  b6.c

lcc -Wf-bo13 -c -Wa-g -o %MAKE%\dm.o  dfs_main.s
lcc -Wf-bo14 -c -Wa-g -o %MAKE%\t.o   title.s
rem lcc -Wf-bo14 -c -Wa-g -o %MAKE%\s.o   soundext.s

cd /d %CUR_DIR%

call dol.bat

rem copy /Y %MAKE%\j.gb

call dopost.bat

echo 

