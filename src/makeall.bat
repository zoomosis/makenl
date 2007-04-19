wmake -f makefile.watcom OS=WIN
copy makenl.exe d:\makenl\binaries\windows
wmake -f makefile.watcom OS=WIN clean
wmake -f makefile.watcom OS=DOS
copy makenl.exe d:\makenl\binaries\dos-16
wmake -f makefile.watcom OS=DOS clean
wmake -f makefile.watcom OS=D32
copy makenl.exe d:\makenl\binaries\dos-32
wmake -f makefile.watcom OS=D32 clean
wmake -f makefile.watcom OS=O16
copy makenl.exe d:\makenl\binaries\OS2-16
wmake -f makefile.watcom OS=O16 clean
wmake -f makefile.watcom OS=OS2
copy makenl.exe d:\makenl\binaries\OS2-32
wmake -f makefile.watcom OS=OS2 clean
