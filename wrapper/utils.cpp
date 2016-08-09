#include <utils/Log.h>
#include "utils.h"
#include <stdlib.h>
#include <time.h> // for performance measurements

namespace android {
    struct timespec start;

void assert_fail(const char *file, int line, const char *func, const char *expr) {
    INFO("assertion failed at file %s, line %d, function %s:",
            file, line, func);
    INFO("%s", expr);
    abort();
}

double now_ms() {
    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);
    return 1000.0 * res.tv_sec + (double)(res.tv_nsec / 1000000);
}

void startTimer() {
    clock_gettime(CLOCK_MONOTONIC, &start);
}

double stopTimer() {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    long secs = (long)end.tv_sec - (long)start.tv_sec - 1;
    long nsecs = 1e9 + end.tv_nsec - start.tv_nsec;
    double result = (double)secs + nsecs/(double)1e9;
    return result;
}

double secs_elapsed(long starts, long startns, timespec end) {
    long secs = (long)end.tv_sec - (long)starts - 1;
    long nsecs = 1e9 + end.tv_nsec - startns;
    double result = (double)secs + nsecs/(double)1e9;
    return result;
}

} //namespace android
