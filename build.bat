@echo off

pushd bin
call cl "..\*.cpp" /I C:\dwl\lib /EHsc /Feqdapplication /w
copy "..\qd.ps1" ".\qd.ps1"
popd

echo -------------------- PROGRAM RUN --------------------
::qdtesting.exe
powershell -NoProfile -File "./psbuildpart.ps1" 
