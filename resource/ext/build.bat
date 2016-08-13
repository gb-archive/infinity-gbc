@echo off

del game.j2w
j2w game.j2w infinity.in
j2w game.j2w
copy *.pag ..
