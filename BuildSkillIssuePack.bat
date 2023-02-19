@echo off
SETLOCAL EnableDelayedExpansion

cls

:: Destination (change as necessary)
SET "SOURCE=SkillIssuePack"
SET "RIIVO=D:\Documents\Dolphin Emulator\Load\SIP"
SET "RELEASE=D:\Modding\Coding\Kamek\releases\SIP\SIP"
echo %RIIVO%

:: CPP compilation settings
SET CC="cw/mwcceppc.exe"
SET CFLAGS=-I- -i engine -i source -i source/game -i projects/%SOURCE% -gcc_extensions on -Cpp_exceptions off -enum int -O4,s -use_lmw_stmw on -fp hard -rostr -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 -rtti off
SET DEFINE=

:: CPP Sources
SET CPPFILES=
for /R projects/%SOURCE% %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

:: Compile CPP
%CC% %CFLAGS% -c -o "projects/%SOURCE%/build/kamek.o" "D:\Modding\Coding\Kamek\engine\kamek.cpp"

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    ::echo "Compiling %%H..."
    %CC% %CFLAGS% %DEFINE% -c -o "projects/%SOURCE%/build/%%~nH.o" "%%H"
    SET "OBJECTS=projects/%SOURCE%/build/%%~nH.o !OBJECTS!"
)

:: Link
echo Linking...
"KamekSource/bin/Debug/Kamek" "projects/%SOURCE%/build/kamek.o" %OBJECTS% -dynamic -externals=symbols.txt -versions=versions.txt -output-kamek=projects\%SOURCE%\build\$KV$.bin >NUL


if %ErrorLevel% equ 0 (
    xcopy /Y projects\%SOURCE%\build\*.bin "%RELEASE%\Binaries" >NUL
	del projects\%SOURCE%\build\*.o >NUL

    cd "D:\Modding\Coding\Kamek\projects\SkillIssuePack\Resources\"
    xcopy /Y /S "%RELEASE%\Binaries" "%RIIVO%\Binaries" >NUL
    echo Binaries copied
)

:end
ENDLOCAL