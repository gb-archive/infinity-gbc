all: ../../bin/afzconv ../../bin/fixgb ../../bin/gfx2h ../../bin/inspage ../../bin/itemconv ../../bin/j2w ../../bin/pagesize ../../bin/pagify ../../bin/post ../../bin/proctext ../../bin/gbr2bin

enemy.o: enemy.c
	gcc -Wall -o enemy.o -c enemy.c

../../bin/afzconv: enemy.o afzconv.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/afzconv enemy.o afzconv.c

../../bin/fixgb: fixgb.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/fixgb fixgb.c

../../bin/gfx2h: gfx2h.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/gfx2h gfx2h.c

../../bin/inspage: inspage.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/inspage inspage.c

../../bin/itemconv: itemconv.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/itemconv itemconv.c

../../bin/j2w: j2w.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/j2w j2w.c

../../bin/pagesize: pagesize.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/pagesize pagesize.c

../../bin/pagify: pagify.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/pagify pagify.c

../../bin/post: post.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/post post.c

../../bin/proctext: proctext.c
	mkdir -p ../../bin
	gcc -Wall -o ../../bin/proctext proctext.c

../../bin/gbr2bin: gbr2bin.cpp
	mkdir -p ../../bin
	g++ -Wall -o ../../bin/gbr2bin gbr2bin.cpp

clean:
	rm -f enemy.o ../../bin/afzconv ../../bin/fixgb ../../bin/gbr2bin ../../bin/gfx2h ../../bin/inspage ../../bin/itemconv ../../bin/j2w ../../bin/pagesize ../../bin/pagify ../../bin/post ../../bin/proctext
	[ -d ../../bin ] && rmdir ../../bin || /bin/true
