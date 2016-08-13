setlocal
set CUR_DIR=%CD%
cd /d %MAKE%
lcc -Wl-m -Wl-yt27 -Wl-yo128 -Wl-ya4 -o j.gb e.o asm.o dm.o j.o j2.o m.o m2.o d1.o d2.o b.o b2.o b3.o b4.o b5.o b6.o t.o
cd /d %CUR_DIR%
