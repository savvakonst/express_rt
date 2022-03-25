set path=%path%;"C:\Program Files\CMake\bin"



rmdir /S /Q x64
rmdir /S /Q x86

mkdir x64
::mkdir x86
cd x64
 
cmake -DCMAKE_PREFIX_PATH="C:\Qt\5.12.8\msvc2017_64"  -G "Visual Studio 16 2019"  ..

::cd ..\x86
:: cmake -DCMAKE_PREFIX_PATH="C:\Qt\5.12.8\msvc2017" -G "Visual Studio 16 2019" -A Win32 ..
:: cd ..

cmd