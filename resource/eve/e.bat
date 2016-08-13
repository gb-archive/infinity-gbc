@echo off
afzconv enemies.afz
if ERRORLEVEL 1 goto end
cpp < eform.txt | post > eform.pst
call easm eform.pst eform.dat
:end
