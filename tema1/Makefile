build: tema1.exe

tema1.exe: tema1.obj
        link /out:tema1.exe tema1.obj compare.lib

tema1.obj: tema1.c
        cl /MD /nologo /W4 /EHsc /Za /D_CRT_SECURE_NO_DEPRECATE /Fotema1.obj /c tema1.c

clean:
        del *.obj *.exe
