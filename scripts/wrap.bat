copy "..\bin\lisem_python.cp38-win_amd64.pyd" "lisem_python.dll"
@RD /S /Q "lisem_python"
ECHO "Start wrapping dll...." 
python wrap_dll.py lisem_python.dll
cd lisem_python
ECHO "Make new proxy dll" 
powershell -Command "(gc lisem_python.cpp) -replace '.constprop.', '_constprop_' | Out-File -encoding ASCII lisem_python.cpp"
powershell -Command "(gc lisem_python.def) -replace '.constprop.', '_constprop_' | Out-File -encoding ASCII lisem_python.def"
powershell -Command "(gc lisem_python_asm.asm) -replace '.constprop.', '_constprop_' | Out-File -encoding ASCII lisem_python_asm.asm"
cmake -f CMakeLists.txt
cmake --build .
ECHO "Finished" 
copy "Debug\lisem_python.dll" "Debug\lisem_python.pyd"
cd ../