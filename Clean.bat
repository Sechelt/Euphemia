for /r %R in (.tmp) do if exist %R (rmdir /s /q "%R")
for /r %R in (Makefile) do if exist %R (del /s /q "%R")

del /s /q Makefile
del /s /q /a .tmp