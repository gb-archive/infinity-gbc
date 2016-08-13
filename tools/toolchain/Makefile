all: ../../bin/afzconv ../../bin/fixgb ../../bin/gfx2h ../../bin/inspage ../../bin/itemconv ../../bin/j2w ../../bin/pagesize ../../bin/pagify ../../bin/post ../../bin/proctext ../../bin/gbr2bin

enemy.o: enemy.c
	gcc -o enemy.o -c enemy.c

../../bin/afzconv: enemy.o afzconv.c
	gcc -o ../../bin/afzconv enemy.o afzconv.c

../../bin/fixgb: fixgb.c
	gcc -o ../../bin/fixgb fixgb.c

../../bin/gfx2h: gfx2h.c
	gcc -o ../../bin/gfx2h gfx2h.c

../../bin/inspage: inspage.c
	gcc -o ../../bin/inspage inspage.c

../../bin/itemconv: itemconv.c
	gcc -o ../../bin/itemconv itemconv.c

../../bin/j2w: j2w.c
	gcc -o ../../bin/j2w j2w.c

../../bin/pagesize: pagesize.c
	gcc -o ../../bin/pagesize pagesize.c

../../bin/pagify: pagify.c
	gcc -o ../../bin/pagify pagify.c

../../bin/post: post.c
	gcc -o ../../bin/post post.c

../../bin/proctext: proctext.c
	gcc -o ../../bin/proctext proctext.c

../../bin/gbr2bin: gbr2bin.cpp
	g++ -o ../../bin/gbr2bin gbr2bin.cpp
