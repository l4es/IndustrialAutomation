@echo off
set ROOT=%~pd0
set OPENSSL_SOURCEDIR=%ROOT%openssl-1.0.??
set OPENSSL_INSTALDIR=%ROOT%openssl
set MAKEFLAGS=

if not "%1" == "" goto help

call :testroot %ROOT%
if errorlevel 1 goto error0

nmake -? >NUL 2>&1
if errorlevel 1 goto error1

ml -? >NUL 2>&1
if errorlevel 1 goto error1

perl -v >NUL 2>&1
if errorlevel 1 goto error2

cd /D %OPENSSL_SOURCEDIR% >NUL 2>&1
if errorlevel 1 goto error3

perl Configure VC-WIN32 no-asm no-ec --prefix=%OPENSSL_INSTALDIR%
if errorlevel 1 goto error

sed -i "/perl/ s/\\\\/\\//g" ms/do_ms.bat 
call ms\do_ms
if errorlevel 1 goto error

sed -i "/PERL/ s/\\\\/\\//g; /PERL/ s/\\$@/\$(@:\\\\=\\/)/;" ms/nt*.mak
nmake -f ms\nt.mak clean

nmake -f ms\nt.mak
if errorlevel 1 goto error

nmake -f ms\nt.mak install
if errorlevel 1 goto error

:ossl_done
cd ..\Stack
if errorlevel 1 goto error

nmake -f win32_msc.mak clean
if errorlevel 1 goto error

nmake -f win32_msc.mak
if errorlevel 1 goto error

cd ..\AnsiCSample
if errorlevel 1 goto error

nmake -f win32_msc.mak clean
if errorlevel 1 goto error

nmake -f win32_msc.mak
if errorlevel 1 goto error

cd ..
goto done

:ossl_build
perl Configure VC-WIN32 no-shared no-asm no-ec --prefix=%OPENSSL_INSTALDIR% --openssldir=%OPENSSL_INSTALDIR%\ssl
if errorlevel 1 goto error

nmake
if errorlevel 1 goto error

nmake install
if errorlevel 1 goto error

copy ..\openssl\lib\libcrypto.lib ..\openssl\lib\libeay32.lib
copy ..\openssl\lib\libssl.lib ..\openssl\lib\ssleay32.lib
goto ossl_done

:testroot
if "%1" == "%ROOT%" exit /B 0
exit /B 1

:error0
echo fatal error: cannot continue.
echo the directory %ROOT% must not contain blanks
goto done

:error1
echo fatal error: cannot continue.
echo this batch has to be called from a
echo 32bit visual studio command shell
goto done

:error2
echo fatal error: cannot continue.
echo perl has to be in the path
goto done

:error3
cd /D %ROOT%openssl-1.1.?? >NUL 2>&1
if not errorlevel 1 goto ossl_build
echo fatal error: cannot continue.
echo openssl sources must be at %OPENSSL_SOURCEDIR%
goto done

:error
echo fatal error: cannot continue.

:help
echo this batch has to be called from a
echo 32bit visual studio command shell
echo the directory %ROOT% must not contain blanks
echo openssl sources must be at %OPENSSL_SOURCEDIR%
echo perl has to be in the path

:done
