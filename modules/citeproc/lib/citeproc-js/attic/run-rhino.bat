@echo off
setlocal

echo Started test-run
set starttime=%time%

cd %~dp0

set RHINO=%~dp0rhino\js-1.7R1.jar
set DOJO=%~dp0dojo\dojo\dojo.js
set DOH=%~dp0dojo\util\doh\

set TARGET=%~dp0tests\javascript\runner_rhino.js

java -client -jar %RHINO% %TARGET% dojoUrl=%DOJO%  testModule=""

echo Finished test-run
echo Start Time = %starttime%
echo   End Time = %time%
