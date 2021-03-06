@ECHO OFF

REM PUT THE COMPILER IN THE PATH IF IT ISN'T ALREADY.
WHERE cl.exe
REM IF %ERRORLEVEL% NEQ 0 CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
WHERE cl.exe

SET TOOL_DIRECTORY_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.25.28610\bin\Hostx64\x64"
SET COMPILER_PATH="%TOOL_DIRECTORY_PATH%\cl.exe"

REM READ THE BUILD MODE COMMAND LINE ARGUMENT.
REM Either "debug" or "release" (no quotes).
REM If not specified, will default to debug.
SET build_mode=%1

REM DEFINE COMPILER OPTIONS.
SET COMMON_COMPILER_OPTIONS=/EHsc /WX /W4 /TP /std:c++latest /wd5054 /wd4459
SET DEBUG_COMPILER_OPTIONS=%COMMON_COMPILER_OPTIONS% /Z7 /Od /MTd
SET RELEASE_COMPILER_OPTIONS=%COMMON_COMPILER_OPTIONS% /O2 /MT

REM DEFINE FILES TO COMPILE/LINK.
SET COMPILATION_FILE="..\BibleProgram.project"
SET MAIN_CODE_DIR="..\code"
SET SDL_DIR=%MAIN_CODE_DIR%"\ThirdParty\SDL"
SET LIBRARIES=SDL2.lib SDL2main.lib opengl32.lib

REM CREATE THE COMMAND LINE OPTIONS FOR THE FILES TO COMPILE/LINK.
SET INCLUDE_DIRS=/I %MAIN_CODE_DIR% /I "%MAIN_CODE_DIR%\ThirdParty\gl3w" /I "%MAIN_CODE_DIR%\ThirdParty\SDL"
SET PROJECT_FILES_DIRS_AND_LIBS=%COMPILATION_FILE% %INCLUDE_DIRS% /link %LIBRARIES% /LIBPATH:%SDL_DIR%

REM MOVE INTO THE BUILD DIRECTORY.
IF NOT EXIST "build" MKDIR "build"
PUSHD "build"

    REM BUILD THE PROGRAM BASED ON THE BUILD MODE.
    IF "%build_mode%"=="release" (
        "%COMPILER_PATH%" %RELEASE_COMPILER_OPTIONS% %PROJECT_FILES_DIRS_AND_LIBS%
    ) ELSE (
        "%COMPILER_PATH%" %DEBUG_COMPILER_OPTIONS% %PROJECT_FILES_DIRS_AND_LIBS%
    )

    IF %ERRORLEVEL% EQU 0 (
        IF NOT EXIST "data" MKDIR "data"
        IF NOT EXIST "data\GratisBible" MKDIR "data\GratisBible"
        COPY /Y "..\data\GratisBible\*.*" "data\GratisBible\*.*"
        IF NOT EXIST "data\SacredTexts" MKDIR "data\SacredTexts"
        COPY /Y "..\data\SacredTexts\*.*" "data\SacredTexts\*.*"
        COPY %SDL_DIR%\SDL2.dll SDL2.dll
        BibleProgram.exe
    )

POPD

ECHO Done

@ECHO ON
