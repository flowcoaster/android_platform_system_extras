#ifndef binderdemo_utils_h
#define binderdemo_utils_h
//#define WITH_DALVIK_BINDER_SERVICE

#include <time.h>

namespace android {
//utility macros
//#define LOG_TAG "Taintgrind Wrapper"

#define INFO(...) \
    do { \
        printf(__VA_ARGS__); \
        printf("\n"); \
        ALOGD(__VA_ARGS__); \
    } while(0)

#define ASSERT(e) \
    do { \
        if (!(e)) \
            assert_fail(__FILE__, __LINE__, __func__, #e); \
    } while(0)

void assert_fail(const char *file, int line, const char *func, const char *expr);
double now_ms();
double secs_elapsed(long starts, long startns, timespec end);
void startTimer();
double stopTimer();

// Where to print the parcel contents: aout, alog, aerr. alog doesn't seem to work.
//#define PLOG aout

// from dalvik/vm/Common.h

// These match the definitions in the VM specification.
typedef uint8_t             u1;
typedef uint16_t            u2;
typedef uint32_t            u4;
typedef uint64_t            u8;
typedef int8_t              s1;
typedef int16_t             s2;
typedef int32_t             s4;
typedef int64_t             s8;

struct Object;

/*union JValue {
#if defined(HAVE_LITTLE_ENDIAN)
    u1      z;
    s1      b;
    u2      c;
    s2      s;
    s4      i;
    s8      j;
    float   f;
    double  d;
    Object* l;
#endif
#if defined(HAVE_BIG_ENDIAN)
    struct {
        u1    _z[3];
        u1    z;
    };
    struct {
        s1    _b[3];
        s1    b;
    };
    struct {
        u2    _c;
        u2    c;
    };
    struct {
        s2    _s;
        s2    s;
    };
    s4      i;
    s8      j;
    float   f;
    double  d;
    void*   l;
#endif
};*/
// end of copy from dalvik/vm/Common.h

} //namespace android

#endif

