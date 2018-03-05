#include "base.h"
using namespace grynca;
#include "test_polygons.h"
#include "test_overlaps.h"

int main(int argc, char* argv[]) {
    srand(time(NULL));

    SDLTestBenchSton::create(1024, 768, true);
    SDLTestBench& testbench = SDLTestBenchSton::get();

    TestOverlapsFixture tof;
    testbench.addTest("Test Overlaps", &tof);
    TestPolygonsFixture tpf;
    testbench.addTest("Test Polygons", &tpf);


    testbench.runTest(0);
    return 0;
}