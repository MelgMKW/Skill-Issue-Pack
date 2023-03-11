@echo off
SETLOCAL EnableDelayedExpansion

cls

:: Destination (change as necessary)
SET "SOURCE=SIP"
SET "RIIVO=D:\Documents\Dolphin Emulator\Load\SIP"
SET "RELEASE=D:\Modding\Coding\Kamek\SIP\releases"
echo %RIIVO%

:: CPP compilation settings
SET CC="../engine/cw/mwcceppc.exe"
SET CFLAGS=-I- -i "../engine/engine" -i "../engine/source" -i "../engine/source/game" -i code -gcc_extensions on -Cpp_exceptions off -enum int -O4,s -use_lmw_stmw on -fp hard -rostr -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 -rtti off
SET DEFINE=

:: CPP Sources
SET CPPFILES=
for /R code %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

:: Compile CPP
%CC% %CFLAGS% -c -o "build/kamek.o" "D:\Modding\Coding\Kamek\Engine\engine\kamek.cpp"

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    ::echo "Compiling %%H..."
    %CC% %CFLAGS% %DEFINE% -c -o "build/%%~nH.o" "%%H"
    SET "OBJECTS=build/%%~nH.o !OBJECTS!"
)

:: Link
echo Linking...
"../engine/KamekSource/bin/Debug/Kamek" "build/kamek.o" %OBJECTS% -dynamic -externals="../engine/source/symbols.txt" -versions="../engine/source/versions.txt" -output-kamek=build\$KV$.bin
echo Done...
if %ErrorLevel% equ 0 (
    xcopy /Y build\*.bin "%RELEASE%\Binaries"
	del build\*.o
    echo Done...
    cd "D:\Modding\Coding\Kamek\SIP\resources"
    xcopy /Y /S "%RELEASE%\Binaries" "%RIIVO%\Binaries" >NUL
    echo Binaries copied
    
    

    wbmgt encode .\Menu.txt -o  >NUL
    xcopy /Y menu.bmg "SIPUI.d\message" >NUL
    wbmgt encode .\Race.txt -o >NUL
    xcopy /Y race.bmg "SIPUI.d\message" >NUL
    echo BMG created

    wszst create *.d -o >NUL
    xcopy /Y *.szs "%RELEASE%\Assets" >NUL
    xcopy /Y "%RELEASE%\Assets" "%RIIVO%\Assets" >NUL
    echo Misc SZS Copied

    SET /A "j=0"
    FOR %%L in ("SZS/*.szs") DO (
        xcopy /Y /F /Q "SZS\%%L" "!RELEASE!\Tracks\!j!.szs*" > nul
        SET /A j+=1  
    )
    xcopy /Y "%RELEASE%\Tracks" "%RIIVO%\Tracks" > nul
    echo Tracks SZS Copied
)

:end
ENDLOCAL