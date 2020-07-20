CALL D:\tools\Emscripten\emsdk\emsdk_env.bat

REM em++ -v

IF NOT EXIST "build" MKDIR "build"
IF NOT EXIST "build/emscripten" MKDIR "build/emscripten"

REM -x c++ tells the compiler to treat the input files as C++.
em++ -x c++ BibleProgram.project -Icode -s USE_SDL=2 -s USE_WEBGL2=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -o build/emscripten/index.html --shell-file code/emscripten_shell.html -std=c++17 --preload-file data/SacredTexts/kjvdat.txt --preload-file data/GratisBible/bbe.xml --preload-file data/GratisBible/web.xml --preload-file data/GratisBible/ylt.xml -v
