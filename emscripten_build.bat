mkdir build
cd build
mkdir Emscripten
cd ..
set EMCC_DEBUG=2
emcc src/main.cpp -O2 -std=c++11 -DWEB -Iinclude -Ic:/DEV/gamedev/base/src -Ic:/DEV/libs/mingw/glm -DGLM_FORCE_RADIANS -s -o build/Emscripten/main.html
