

rmdir /S /Q x64
rmdir /S /Q x86

mkdir x64
mkdir x86
cd x64


cmake  -G "Visual Studio 16 2019"  ..

cd ..\x86



cmake  -G "Visual Studio 16 2019" -A Win32 ..

cd ..