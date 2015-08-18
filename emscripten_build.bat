mkdir build
cd build
mkdir Emscripten
cd ..
set EMCC_DEBUG=2
emcc test/main.cpp -O2 -std=c++11 -DWEB -Isrc -Ic:/DEV/gamedev/base/src -Ic:/DEV/libs/mingw/glm -DGLM_FORCE_RADIANS -s -o build/Emscripten/main.html
