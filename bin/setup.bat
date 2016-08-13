:: set up affinix development environment
::
:: set up j:\project\project_name as the build root
@echo off

if %PROJECT%.==infinity. goto :EOF
set PROJECT=infinity

set PROJECT_ROOT=%~dp0\..

set BUILDER=j:\sdk\gbz80-gb\2-1-5\bin
set BIN=j:\bin
set MAKE=j:\build
set RELEASE=j:\release
set SOURCE=j:\source
set RESOURCE=j:\resource
set PATH=%PATH%;%BIN%;%BUILDER%

if not exist %BIN% subst j: %PROJECT_ROOT%
cd /d j:\
