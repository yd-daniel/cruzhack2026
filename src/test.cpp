#include "RollingAverage.h"
#include <iostream>

int main()
{
    float backing[5] = {0,0,0,0,0};
    RollingAverage ra;

    std::cout << "expected: init with size=5\n";
    RollingAverage_init(&ra, backing, 5);

    std::cout << "expected: average is 0 (empty)\n";
    std::cout << "got: " << RollingAverage_getAverage(&ra) << "\n\n";

    std::cout << "expected: addValue(10)\n";
    RollingAverage_addValue(&ra, 10.0f);

    std::cout << "expected: average is 10\n";
    std::cout << "got: " << RollingAverage_getAverage(&ra) << "\n\n";

    std::cout << "expected: addValue(20)\n";
    RollingAverage_addValue(&ra, 20.0f);

    std::cout << "expected: average is 15\n";
    std::cout << "got: " << RollingAverage_getAverage(&ra) << "\n\n";

    std::cout << "expected: addValue(30)\n";
    RollingAverage_addValue(&ra, 30.0f);

    std::cout << "expected: average is 20\n";
    std::cout << "got: " << RollingAverage_getAverage(&ra) << "\n\n";

    float outA[5] = {-1,-1,-1,-1,-1};
    std::cout << "expected: getBuffer -> [10, 20, 30, (unused), (unused)]\n";
    RollingAverage_getBuffer(&ra, outA);
    std::cout << "got: [" << outA[0] << ", " << outA[1] << ", " << outA[2] << ", "
              << outA[3] << ", " << outA[4] << "]\n\n";

    std::cout << "expected: addValue(40)\n";
    RollingAverage_addValue(&ra, 40.0f);

    std::cout << "expected: addValue(50)\n";
    RollingAverage_addValue(&ra, 50.0f);

    std::cout << "expected: average is 30\n";
    std::cout << "got: " << RollingAverage_getAverage(&ra) << "\n\n";

    float outB[5] = {-1,-1,-1,-1,-1};
    std::cout << "expected: getBuffer -> [10, 20, 30, 40, 50]\n";
    RollingAverage_getBuffer(&ra, outB);
    std::cout << "got: [" << outB[0] << ", " << outB[1] << ", " << outB[2] << ", "
              << outB[3] << ", " << outB[4] << "]\n\n";

    std::cout << "expected: addValue(60) (drops 10)\n";
    RollingAverage_addValue(&ra, 60.0f);

    std::cout << "expected: average is 40 (20+30+40+50+60)/5\n";
    std::cout << "got: " << RollingAverage_getAverage(&ra) << "\n\n";

    float outC[5] = {-1,-1,-1,-1,-1};
    std::cout << "expected: getBuffer -> [20, 30, 40, 50, 60]\n";
    RollingAverage_getBuffer(&ra, outC);
    std::cout << "got: [" << outC[0] << ", " << outC[1] << ", " << outC[2] << ", "
              << outC[3] << ", " << outC[4] << "]\n\n";

    std::cout << "expected: addValue(61)\n";
    RollingAverage_addValue(&ra, 61.0f);
    std::cout << "expected: addValue(62)\n";
    RollingAverage_addValue(&ra, 62.0f);
    std::cout << "expected: addValue(63)\n";
    RollingAverage_addValue(&ra, 63.0f);
    std::cout << "expected: addValue(64)\n";
    RollingAverage_addValue(&ra, 64.0f);
    std::cout << "expected: addValue(65)\n";
    RollingAverage_addValue(&ra, 65.0f);
    std::cout << "expected: addValue(66)\n";
    RollingAverage_addValue(&ra, 66.0f);
    std::cout << "expected: addValue(67)\n";
    RollingAverage_addValue(&ra, 67.0f);

    std::cout << "expected: average is 65 (63+64+65+66+67)/5\n";
    std::cout << "got: " << RollingAverage_getAverage(&ra) << "\n\n";

    float outD[5] = {-1,-1,-1,-1,-1};
    std::cout << "expected: getBuffer -> [63, 64, 65, 66, 67]\n";
    RollingAverage_getBuffer(&ra, outD);
    std::cout << "got: [" << outD[0] << ", " << outD[1] << ", " << outD[2] << ", "
              << outD[3] << ", " << outD[4] << "]\n\n";

//    ret
//::contentReference[oaicite:0]{index=0}
}