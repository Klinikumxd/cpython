@rem Fetches (and builds if necessary) external dependencies

@rem Assume we start inside the Python source directory
call "Tools\buildbot\external-common.bat"
call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64

if not exist tcltk64\bin\tcl86tg.dll (
    cd tcl-8.6.1.0\win
    nmake -f makefile.vc OPTS=symbols MACHINE=AMD64 INSTALLDIR=..\..\tcltk64 clean core shell dlls
    nmake -f makefile.vc OPTS=symbols MACHINE=AMD64 INSTALLDIR=..\..\tcltk64 install-binaries install-libraries
    cd ..\..
)

if not exist tcltk64\bin\tk86tg.dll (
    cd tk-8.6.1.0\win    
    nmake -f makefile.vc OPTS=symbols MACHINE=AMD64 INSTALLDIR=..\..\tcltk64 TCLDIR=..\..\tcl-8.6.1.0 clean
    nmake -f makefile.vc OPTS=symbols MACHINE=AMD64 INSTALLDIR=..\..\tcltk64 TCLDIR=..\..\tcl-8.6.1.0 all
    nmake -f makefile.vc OPTS=symbols MACHINE=AMD64 INSTALLDIR=..\..\tcltk64 TCLDIR=..\..\tcl-8.6.1.0 install-binaries install-libraries
    cd ..\..
)

