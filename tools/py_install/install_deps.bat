
SET PATH=%PATH:Python=NONE %

python.exe get-pip.py

for /f "delims=" %%a in ('dir /s /b *._pth') do set "TEMP_VAR=%%a"

echo Lib/site-packages>> %TEMP_VAR%

python -m pip install ruamel.yaml
python -m pip install pybind11

cmd