#include "test_gon.h"
#include "test_polygons.h"

int main(int argc, char* argv[]) {
#ifdef VISUAL_UPDATE
    startPolygonsLoop();
#else
    testGon();
#endif
    WAIT_FOR_KEY_ON_WIN();
    return 0;
}