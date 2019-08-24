CALL D:\tools\Emscripten\emsdk\emsdk_env.bat

REM em++ -v

IF NOT EXIST "build" MKDIR "build"
IF NOT EXIST "build/emscripten" MKDIR "build/emscripten"

REM -x c++ tells the compiler to treat the input files as C++.
em++ -x c++ BibleProgram.project -Icode -s USE_SDL=2 -s USE_WEBGL2=1 -s FULL_ES3=1 -o build/emscripten/index.html -std=c++17 -v