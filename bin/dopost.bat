@echo off

setlocal
set CUR_DIR=%CD%
cd /d %RESOURCE%

inspage %MAKE%\j.gb zonemap.pag 15
inspage %MAKE%\j.gb efr00.pag 16

inspage %MAKE%\j.gb eve00.pag 17
inspage %MAKE%\j.gb eve01.pag 18
inspage %MAKE%\j.gb eve02.pag 19

inspage %MAKE%\j.gb txt00.pag 21
inspage %MAKE%\j.gb txt01.pag 22
inspage %MAKE%\j.gb txt02.pag 23
inspage %MAKE%\j.gb txt03.pag 24
inspage %MAKE%\j.gb txt04.pag 25
inspage %MAKE%\j.gb txt05.pag 26
inspage %MAKE%\j.gb txt06.pag 27

inspage %MAKE%\j.gb table.pag  28

inspage %MAKE%\j.gb spr00.pag  29
inspage %MAKE%\j.gb spr01.pag  30
inspage %MAKE%\j.gb spr02.pag  31
inspage %MAKE%\j.gb spr03.pag  32
inspage %MAKE%\j.gb spr04.pag  33
inspage %MAKE%\j.gb spr05.pag  34
inspage %MAKE%\j.gb spr06.pag  35
inspage %MAKE%\j.gb spr07.pag  36
inspage %MAKE%\j.gb spr08.pag  37

inspage %MAKE%\j.gb stb00.pag  38
inspage %MAKE%\j.gb stb01.pag  39
inspage %MAKE%\j.gb stb02.pag  40
inspage %MAKE%\j.gb stb03.pag  41
inspage %MAKE%\j.gb stb04.pag  42
inspage %MAKE%\j.gb stb05.pag  43
inspage %MAKE%\j.gb stb06.pag  44

inspage %MAKE%\j.gb map00.pag  45
inspage %MAKE%\j.gb map01.pag  46
inspage %MAKE%\j.gb map02.pag  47
inspage %MAKE%\j.gb map03.pag  48
inspage %MAKE%\j.gb map04.pag  49
inspage %MAKE%\j.gb map05.pag  50
inspage %MAKE%\j.gb map06.pag  51
inspage %MAKE%\j.gb map07.pag  52


inspage %MAKE%\j.gb audio1o.bin 63
inspage %MAKE%\j.gb audio2o.bin 62

inspage %MAKE%\j.gb audio1.bin 64
inspage %MAKE%\j.gb audio2.bin 65
inspage %MAKE%\j.gb audio3.bin 66
inspage %MAKE%\j.gb audio4.bin 67
inspage %MAKE%\j.gb audio5.bin 68
inspage %MAKE%\j.gb audio6.bin 69
inspage %MAKE%\j.gb audio7.bin 70
inspage %MAKE%\j.gb audio8.bin 71
inspage %MAKE%\j.gb audio9.bin 72
inspage %MAKE%\j.gb audio10.bin 73
inspage %MAKE%\j.gb audio11.bin 74
inspage %MAKE%\j.gb audio12.bin 75
inspage %MAKE%\j.gb audio13.bin 76
inspage %MAKE%\j.gb audio14.bin 77
inspage %MAKE%\j.gb audio15.bin 78

itemconv eve\items.ref %MAKE%\j.gb
fixgb %MAKE%\j.gb INFINITY

cd /d %CUR_DIR%
