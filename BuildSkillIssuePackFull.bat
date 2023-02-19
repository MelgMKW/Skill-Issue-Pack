@echo off
SETLOCAL EnableDelayedExpansion

cls

:: Destination (change as necessary)
SET "SOURCE=SkillIssuePack"
SET "RIIVO=D:\Documents\Dolphin Emulator\Load\SIP"
SET "RELEASE=D:\Modding\Coding\Kamek\releases\SIP\SIP"
echo %RIIVO%

:: CPP compilation settings -d LECODE=1
SET CC="cw/mwcceppc.exe"
SET CFLAGS=-I- -i engine -i source -i source/game -i projects/%SOURCE% -Cpp_exceptions off -enum int -O4,s -use_lmw_stmw on -fp hard -rostr -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 -rtti off
SET DEFINE=-d BUFFBADKARTS=1 -d UMT=1 -d INFODISPLAY=1 -d BOOTMENUS=1 -d LECODE=1 -d CCMODIF=1 -d FILELOADING=1 -d ENHANCEDREPLAY=1

:: Engine

:: CPP Sources
SET CPPFILES=
for /R projects/%SOURCE% %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

:: Compile CPP
%CC% %CFLAGS% -c -o "projects/%SOURCE%/build/kamek.o" "D:\Modding\Coding\Kamek\engine\kamek.cpp"

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    :: echo "Compiling %%H.cpp..."
    %CC% %CFLAGS% %DEFINE% -c -o "projects/%SOURCE%/build/%%~nH.o" "%%H"
    SET "OBJECTS=projects/%SOURCE%/build/%%~nH.o !OBJECTS!"
)

:: Link
echo Linking...
"KamekSource/bin/Debug/Kamek" "projects/%SOURCE%/build/kamek.o" %OBJECTS% -dynamic -externals=symbols.txt -versions=versions.txt -output-kamek=projects\%SOURCE%\build\$KV$.bin >NUL
echo Done...
if %ErrorLevel% equ 0 (
    xcopy /Y projects\%SOURCE%\build\*.bin "%RELEASE%\Binaries"
	del projects\%SOURCE%\build\*.o
    echo Done...
    cd "D:\Modding\Coding\Kamek\projects\SkillIssuePack\Resources\"
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