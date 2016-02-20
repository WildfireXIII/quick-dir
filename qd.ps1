qdapplication.exe # eventually put subdir in front
$newPath = get-content "C:\dwl\tmp\_FILELAYER_CWD.dat"
del "C:\dwl\tmp\_FILELAYER_CWD.dat"
cd $newPath
