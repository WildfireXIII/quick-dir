$BIN_DIR = $env:BIN_DIR
$applicationFolder = "$BIN_DIR/QuickDir"

"$applicationFolder/qdapplication.exe" 
$newPath = get-content "C:\dwl\tmp\_FILELAYER_CWD.dat"
del "C:\dwl\tmp\_FILELAYER_CWD.dat"
cd $newPath
