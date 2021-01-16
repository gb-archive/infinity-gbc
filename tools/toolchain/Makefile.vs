all: ../../bin/afzconv ../../bin/fixgb ../../bin/gfx2h ../../bin/inspage ../../bin/itemconv ../../bin/j2w ../../bin/pagesize ../../bin/pagify ../../bin/post ../../bin/proctext ../../bin/gbr2bin

enemy.o: enemy.c
	cl /c enemy.c

../../bin/afzconv: enemy.o afzconv.c
	cl afzconv.c enemy.obj /Fe../../bin/afzconv.exe

../../bin/fixgb: fixgb.c
	cl fixgb.c /Fe../../bin/fixgb.exe

../../bin/gfx2h: gfx2h.c
	cl gfx2h.c /Fe../../bin/gfx2h.exe

../../bin/inspage: inspage.c
	cl inspage.c /Fe../../bin/inspage.exe

../../bin/itemconv: itemconv.c
	cl itemconv.c /Fe../../bin/itemconv.exe

../../bin/j2w: j2w.c
	cl j2w.c /Fe../../bin/j2w.exe

../../bin/pagesize: pagesize.c
	cl pagesize.c /Fe../../bin/pagesize.exe

../../bin/pagify: pagify.c
	cl pagify.c /Fe../../bin/pagify.exe

../../bin/post: post.c
	cl post.c /Fe../../bin/post.exe

../../bin/proctext: proctext.c
	cl proctext.c /Fe../../bin/proctext.exe

../../bin/gbr2bin: gbr2bin.cpp
	cl gbr2bin.cpp /Fe../../bin/gbr2bin.exe
