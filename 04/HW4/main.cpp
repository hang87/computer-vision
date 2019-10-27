#include "ostu.h"
#include "segment.h"
#include <string>
using namespace std;


int main() {
    string filename = "./input/H-Image.bmp";

    // OSTU *ostu = new OSTU(filename);
    SEGMENT segment(filename);
    segment.run();

    return 0;
}