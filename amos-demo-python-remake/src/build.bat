del build\*.* /s /f /q
rmdir build\*.* /s /f /q
rmdir build\ /s /q

mkdir build
c:\Python34\Python.exe setup.py build

cd build
rename exe.win-amd64-3.4 AMOS-Demo(Win64)
cd "AMOS-Demo(Win64)"

del openvr_api.dll
del openvr_plugin.dll

cd..
"C:\Program Files\7-Zip\7z.exe" a AMOS-Demo(Win64).zip
pause