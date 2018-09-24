all: builddir build/j.gb

.PHONY: builddir
builddir:
	mkdir -p build
	mkdir -p source/data

clean:
	rm -f resource/eve/itemdefs.h
	rm -f source/eve/itemdefs.h
	rm -f resource/eve/eform.txt resource/eve/eform.pst resource/eve/eform.dat
	rm -f resource/eve/dialog.dat
	rm -f resource/eve/game.pst resource/eve/demo.edt
	rm -f resource/ext/*.bin
	rm -f resource/*.pag
	rm -f source/data/*.h
	rm -f build/*.o

distclean: clean
	rm -rf source/data
	rm -rf build

source/eve/itemdefs.h: resource/eve/items.ref
	cd resource/eve && ../../bin/itemconv --defs items.ref && cp itemdefs.h ../../source/eve

resource/eve/eform.pst: resource/eve/enemies.afz
	cd resource/eve && ../../bin/afzconv enemies.afz && cpp < eform.txt | ../../bin/post > eform.pst

resource/eve/eform.dat: resource/eve/eform.pst
	yasm -o resource/eve/eform.dat resource/eve/eform.pst

resource/efr00.pag: resource/eve/eform.dat
	cd resource && ../bin/pagify eve/eform.dat efr

resource/eve/dialog.dat: resource/eve/dialog.txt
	cd resource/eve && ../../bin/proctext dialog.txt dialog.dat

resource/txt00.pag: resource/eve/dialog.dat
	cd resource && ../bin/pagify eve/dialog.dat txt

resource/eve/game.pst: resource/eve/game.eve
	cd resource/eve && cpp < game.eve | ../../bin/post > game.pst

resource/eve/demo.edt: resource/eve/game.pst
	yasm -o resource/eve/demo.edt resource/eve/game.pst

resource/eve00.pag: resource/eve/demo.edt
	cd resource && ../bin/pagify eve/demo.edt eve

resource/ext/arrow_w.bin: resource/ext/arrow_w.gbr
	cd resource/ext && ../../bin/gbr2bin arrow_w.gbr arrow_w.bin /nopal 0 15

resource/ext/cure1.bin: resource/ext/cure1.gbr
	cd resource/ext && ../../bin/gbr2bin cure1.gbr cure1.bin /nopal 0 15

resource/ext/fire1.bin: resource/ext/fire1.gbr
	cd resource/ext && ../../bin/gbr2bin fire1.gbr fire1.bin /nopal 0 15

resource/ext/fx1.bin: resource/ext/fx1.gbr
	cd resource/ext && ../../bin/gbr2bin fx1.gbr fx1.bin /nopal 0 11

resource/ext/gemgfx.bin: resource/ext/gems.gbr
	cd resource/ext && ../../bin/gbr2bin gems.gbr gemgfx.bin 0 15

resource/ext/itemgfx.bin: resource/ext/items.gbr
	cd resource/ext && ../../bin/gbr2bin items.gbr itemgfx.bin 0 11

resource/ext/jolt1.bin: resource/ext/jolt1.gbr
	cd resource/ext && ../../bin/gbr2bin jolt1.gbr jolt1.bin /nopal 0 4

resource/ext/menugfx.bin: resource/ext/menugfx.gbr
	cd resource/ext && ../../bin/gbr2bin menugfx.gbr menugfx.bin /nopal 0 95

resource/ext/menuwing.bin: resource/ext/menuwing.gbr
	cd resource/ext && ../../bin/gbr2bin menuwing.gbr menuwing.bin /nopal 0 1

resource/ext/missgfx.bin: resource/ext/miss.gbr
	cd resource/ext && ../../bin/gbr2bin miss.gbr missgfx.bin /nopal 0 3

resource/ext/poof.bin: resource/ext/poof.gbr
	cd resource/ext && ../../bin/gbr2bin poof.gbr poof.bin /nopal 0 3

resource/ext/scratch1.bin: resource/ext/scratch1.gbr
	cd resource/ext && ../../bin/gbr2bin scratch1.gbr scratch1.bin /nopal 0 15

resource/ext/skillgfx.bin: resource/ext/skills.gbr
	cd resource/ext && ../../bin/gbr2bin skills.gbr skillgfx.bin 0 19

resource/ext/slash1.bin: resource/ext/slash.gbr
	cd resource/ext && ../../bin/gbr2bin slash.gbr slash1.bin /nopal 0 3

resource/ext/slurp.bin: resource/ext/slurp.gbr
	cd resource/ext && ../../bin/gbr2bin slurp.gbr slurp.bin /nopal 0 15

resource/ext/spawn1.bin: resource/ext/e_roots2.gbr
	cd resource/ext && ../../bin/gbr2bin e_roots2.gbr spawn1.bin /nopal 3 7

resource/ext/staff1.bin: resource/ext/staff1.gbr
	cd resource/ext && ../../bin/gbr2bin staff1.gbr staff1.bin /nopal 0 0

resource/ext/sword1.bin: resource/ext/sword1.gbr
	cd resource/ext && ../../bin/gbr2bin sword1.gbr sword1.bin /nopal 0 15

resource/ext/tidal1.bin: resource/ext/tidal1.gbr
	cd resource/ext && ../../bin/gbr2bin tidal1.gbr tidal1.bin /nopal 0 3

resource/ext/wake.bin: resource/ext/wake.gbr
	cd resource/ext && ../../bin/gbr2bin wake.gbr wake.bin /nopal 0 15

resource/ext/whirl1.bin: resource/ext/whirl1_adjusted.gbr
	cd resource/ext && ../../bin/gbr2bin whirl1_adjusted.gbr whirl1.bin /nopal 0 11

resource/ext/wingding.bin: resource/ext/wingding.gbr
	cd resource/ext && ../../bin/gbr2bin wingding.gbr wingding.bin /nopal 0 23

source/data/dudepal.h: resource/ext/sprite.pal
	cd resource/ext && ../../bin/gfx2h sprite.pal ../../source/data/dudepal.h dudepal

source/data/dudepal2.h: resource/ext/sprite_gba.pal
	cd resource/ext && ../../bin/gfx2h sprite_gba.pal ../../source/data/dudepal2.h dudepal2

source/data/arrow_w.h: resource/ext/arrow_w.bin
	cd resource/ext && ../../bin/gfx2h arrow_w.bin ../../source/data/arrow_w.h arrow_w

source/data/cure1.h: resource/ext/cure1.bin
	cd resource/ext && ../../bin/gfx2h cure1.bin ../../source/data/cure1.h cure1

source/data/fire1.h: resource/ext/fire1.bin
	cd resource/ext && ../../bin/gfx2h fire1.bin ../../source/data/fire1.h fire1

source/data/fx1.h: resource/ext/fx1.bin
	cd resource/ext && ../../bin/gfx2h fx1.bin ../../source/data/fx1.h fx1

source/data/gemgfx.h: resource/ext/gemgfx.bin
	cd resource/ext && ../../bin/gfx2h gemgfx.bin ../../source/data/gemgfx.h gemgfx

source/data/itemgfx.h: resource/ext/itemgfx.bin
	cd resource/ext && ../../bin/gfx2h itemgfx.bin ../../source/data/itemgfx.h itemgfx

source/data/jolt1.h: resource/ext/jolt1.bin
	cd resource/ext && ../../bin/gfx2h jolt1.bin ../../source/data/jolt1.h jolt1

source/data/menugfx.h: resource/ext/menugfx.bin
	cd resource/ext && ../../bin/gfx2h menugfx.bin ../../source/data/menugfx.h menugfx

source/data/menuwing.h: resource/ext/menuwing.bin
	cd resource/ext && ../../bin/gfx2h menuwing.bin ../../source/data/menuwing.h menuwing

source/data/missgfx.h: resource/ext/missgfx.bin
	cd resource/ext && ../../bin/gfx2h missgfx.bin ../../source/data/missgfx.h missgfx

source/data/poof.h: resource/ext/poof.bin
	cd resource/ext && ../../bin/gfx2h poof.bin ../../source/data/poof.h poofgfx

source/data/scratch1.h: resource/ext/scratch1.bin
	cd resource/ext && ../../bin/gfx2h scratch1.bin ../../source/data/scratch1.h scratch1

source/data/skillgfx.h: resource/ext/skillgfx.bin
	cd resource/ext && ../../bin/gfx2h skillgfx.bin ../../source/data/skillgfx.h skillgfx

source/data/slash1.h: resource/ext/slash1.bin
	cd resource/ext && ../../bin/gfx2h slash1.bin ../../source/data/slash1.h slash1

source/data/slurp.h: resource/ext/slurp.bin
	cd resource/ext && ../../bin/gfx2h slurp.bin ../../source/data/slurp.h slurpgfx

source/data/spawn1.h: resource/ext/spawn1.bin
	cd resource/ext && ../../bin/gfx2h spawn1.bin ../../source/data/spawn1.h spawn1

source/data/staff1.h: resource/ext/staff1.bin
	cd resource/ext && ../../bin/gfx2h staff1.bin ../../source/data/staff1.h staff1gfx

source/data/sword1.h: resource/ext/sword1.bin
	cd resource/ext && ../../bin/gfx2h sword1.bin ../../source/data/sword1.h sword1

source/data/tidal1.h: resource/ext/tidal1.bin
	cd resource/ext && ../../bin/gfx2h tidal1.bin ../../source/data/tidal1.h tidal1

source/data/wake.h: resource/ext/wake.bin
	cd resource/ext && ../../bin/gfx2h wake.bin ../../source/data/wake.h wakegfx

source/data/whirl1.h: resource/ext/whirl1.bin
	cd resource/ext && ../../bin/gfx2h whirl1.bin ../../source/data/whirl1.h whirl1

source/data/wingding.h: resource/ext/wingding.bin
	cd resource/ext && ../../bin/gfx2h wingding.bin ../../source/data/wingding.h wingding

build/e.o: source/e.c
	cd source && lcc -c -o ../build/e.o e.c

build/asm.o: source/asmfunc.s
	cd source && lcc -c -Wa-g -o ../build/asm.o asmfunc.s

build/j.o: source/j.c
	cd source && lcc -Wf-bo1 -c -o ../build/j.o j.c

build/j2.o: source/j2.c
	cd source && lcc -Wf-bo2 -c -o ../build/j2.o j2.c

build/d1.o: source/d1.c source/data/dudepal.h source/data/dudepal2.h source/data/arrow_w.h source/data/cure1.h source/data/fire1.h source/data/fx1.h source/data/gemgfx.h source/data/itemgfx.h source/data/jolt1.h source/data/menugfx.h source/data/menuwing.h source/data/missgfx.h source/data/poof.h source/data/scratch1.h source/data/skillgfx.h source/data/slash1.h source/data/slurp.h source/data/spawn1.h source/data/staff1.h source/data/sword1.h source/data/tidal1.h source/data/wake.h source/data/whirl1.h source/data/wingding.h
	cd source && lcc -Wf-bo3 -c -o ../build/d1.o d1.c

build/d2.o: source/d2.c source/data/slash1.h source/data/slurp.h source/data/spawn1.h source/data/whirl1.h
	cd source && lcc -Wf-bo4 -c -o ../build/d2.o d2.c

build/m.o: source/m.c
	cd source && lcc -Wf-bo5 -c -o ../build/m.o m.c

build/m2.o: source/m2.c
	cd source && lcc -Wf-bo6 -c -o ../build/m2.o m2.c

build/b.o: source/b.c
	cd source && lcc -Wf-bo7 -c -o ../build/b.o b.c

build/b2.o: source/b2.c
	cd source && lcc -Wf-bo8 -c -o ../build/b2.o b2.c

build/b3.o: source/b3.c
	cd source && lcc -Wf-bo9 -c -o ../build/b3.o b3.c

build/b4.o: source/b4.c
	cd source && lcc -Wf-bo10 -c -o ../build/b4.o b4.c

build/b5.o: source/b5.c
	cd source && lcc -Wf-bo11 -c -o ../build/b5.o b5.c

build/b6.o: source/b6.c
	cd source && lcc -Wf-bo12 -c -o ../build/b6.o b6.c

build/dm.o: source/dfs_main.s
	cd source && lcc -Wf-bo13 -c -Wa-g -o ../build/dm.o dfs_main.s

build/t.o: source/title.s
	cd source && lcc -Wf-bo14 -c -Wa-g -o ../build/t.o title.s

build/j.gb: source/eve/itemdefs.h build/e.o build/asm.o build/j.o build/j2.o build/d1.o build/d2.o build/m.o build/m2.o build/b.o build/b2.o build/b3.o build/b4.o build/b5.o build/b6.o build/dm.o build/t.o resource/efr00.pag resource/txt00.pag resource/eve00.pag
	cd build && lcc -Wl-m -Wl-yt27 -Wl-yo128 -Wl-ya4 -o j.gb e.o asm.o j.o j2.o m.o m2.o d1.o d2.o b.o b2.o b3.o b4.o b5.o b6.o dm.o t.o

	cd resource && ../bin/inspage ../build/j.gb zonemap.pag 15
	cd resource && ../bin/inspage ../build/j.gb efr00.pag 16

	cd resource && ../bin/inspage ../build/j.gb eve00.pag 17
	cd resource && ../bin/inspage ../build/j.gb eve01.pag 18
	cd resource && ../bin/inspage ../build/j.gb eve02.pag 19

	cd resource && ../bin/inspage ../build/j.gb txt00.pag 21
	cd resource && ../bin/inspage ../build/j.gb txt01.pag 22
	cd resource && ../bin/inspage ../build/j.gb txt02.pag 23
	cd resource && ../bin/inspage ../build/j.gb txt03.pag 24
	cd resource && ../bin/inspage ../build/j.gb txt04.pag 25
	cd resource && ../bin/inspage ../build/j.gb txt05.pag 26
	cd resource && ../bin/inspage ../build/j.gb txt06.pag 27

	cd resource && ../bin/inspage ../build/j.gb table.pag  28

	cd resource && ../bin/inspage ../build/j.gb spr00.pag  29
	cd resource && ../bin/inspage ../build/j.gb spr01.pag  30
	cd resource && ../bin/inspage ../build/j.gb spr02.pag  31
	cd resource && ../bin/inspage ../build/j.gb spr03.pag  32
	cd resource && ../bin/inspage ../build/j.gb spr04.pag  33
	cd resource && ../bin/inspage ../build/j.gb spr05.pag  34
	cd resource && ../bin/inspage ../build/j.gb spr06.pag  35
	cd resource && ../bin/inspage ../build/j.gb spr07.pag  36
	cd resource && ../bin/inspage ../build/j.gb spr08.pag  37

	cd resource && ../bin/inspage ../build/j.gb stb00.pag  38
	cd resource && ../bin/inspage ../build/j.gb stb01.pag  39
	cd resource && ../bin/inspage ../build/j.gb stb02.pag  40
	cd resource && ../bin/inspage ../build/j.gb stb03.pag  41
	cd resource && ../bin/inspage ../build/j.gb stb04.pag  42
	cd resource && ../bin/inspage ../build/j.gb stb05.pag  43
	cd resource && ../bin/inspage ../build/j.gb stb06.pag  44

	cd resource && ../bin/inspage ../build/j.gb map00.pag  45
	cd resource && ../bin/inspage ../build/j.gb map01.pag  46
	cd resource && ../bin/inspage ../build/j.gb map02.pag  47
	cd resource && ../bin/inspage ../build/j.gb map03.pag  48
	cd resource && ../bin/inspage ../build/j.gb map04.pag  49
	cd resource && ../bin/inspage ../build/j.gb map05.pag  50
	cd resource && ../bin/inspage ../build/j.gb map06.pag  51
	cd resource && ../bin/inspage ../build/j.gb map07.pag  52

	cd resource && ../bin/inspage ../build/j.gb audio1o.bin 63
	cd resource && ../bin/inspage ../build/j.gb audio2o.bin 62

	cd resource && ../bin/inspage ../build/j.gb audio1.bin 64
	cd resource && ../bin/inspage ../build/j.gb audio2.bin 65
	cd resource && ../bin/inspage ../build/j.gb audio3.bin 66
	cd resource && ../bin/inspage ../build/j.gb audio4.bin 67
	cd resource && ../bin/inspage ../build/j.gb audio5.bin 68
	cd resource && ../bin/inspage ../build/j.gb audio6.bin 69
	cd resource && ../bin/inspage ../build/j.gb audio7.bin 70
	cd resource && ../bin/inspage ../build/j.gb audio8.bin 71
	cd resource && ../bin/inspage ../build/j.gb audio9.bin 72
	cd resource && ../bin/inspage ../build/j.gb audio10.bin 73
	cd resource && ../bin/inspage ../build/j.gb audio11.bin 74
	cd resource && ../bin/inspage ../build/j.gb audio12.bin 75
	cd resource && ../bin/inspage ../build/j.gb audio13.bin 76
	cd resource && ../bin/inspage ../build/j.gb audio14.bin 77
	cd resource && ../bin/inspage ../build/j.gb audio15.bin 78

	cd resource && ../bin/itemconv eve/items.ref ../build/j.gb
	cd resource && ../bin/fixgb ../build/j.gb INFINITY
