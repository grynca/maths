#define PROFILE_IN_RELEASE

#include "base.h"
using namespace grynca;
#include "test_polygons.h"

int main(int argc, char* argv[]) {
    srand(time(NULL));
    SDLTestBenchSton::create(1024, 768, true);
    SDLTestBench& testbench = SDLTestBenchSton::get();

    TestPolygonsFixture f;
    std::string name = "Test Polygons";
    testbench.addTest(name, &f);


    testbench.runTest(0);
    return 0;
}