$BIN_DIR = $env:BIN_DIR
$applicationFolder = "$BIN_DIR/QuickDir" # USE FOR PUBLISH
#$applicationFolder = "C:\dwl\tmp\bin" # DELETE FOR PUBLISH

. "$applicationFolder/qdapplication.exe"
$newPath = get-content "C:\dwl\tmp\_FILELAYER_CWD.dat"
del "C:\dwl\tmp\_FILELAYER_CWD.dat"
cd $newPath
