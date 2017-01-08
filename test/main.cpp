
#include "base.h"
#include "maths.h"
using namespace grynca;
#include "test_polygons.h"

int main(int argc, char* argv[]) {
#ifdef VISUAL_UPDATE
    startPolygonsLoop();
#else
    // TODO:
#endif
    WAIT_FOR_KEY_ON_WIN();

    return 0;
}