#include <stdlib.h>
#include <time.h>

int randomInt(int min, int max) {
    srand((unsigned)time(0));
    return rand() % (max - min + 1) + min;
}