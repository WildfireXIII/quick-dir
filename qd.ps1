./qdapplication.exe
$newPath = get-content "_FILELAYER_CWD.dat"
del _FILELAYER_CWD.dat
cd $newPath
