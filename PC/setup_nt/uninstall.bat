cd "%1"
bin\python.exe uninstall.py
del lib\win\*.pyc
del lib\win\*.pyd
del lib\win\*.py
rd lib\win
rd lib
del bin\*.exe
del bin\*.dll
rd bin
del uninstall.*
