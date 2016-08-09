#include <utils/StrongPointer.h> //sp
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/IBinder.h>
#include "Wrapper.h"
#include "utils.h"

namespace android {

struct Callback jniCallback;

int androidmain(int argc, char **argv) {
    assert(argc >= 2); // need two args: id for service name and time to calculate startup time
    long times = strtol(argv[2], NULL, 10);
    long timens = strtol(argv[3], NULL, 10);
    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);
    //long delay = res.tv_nsec - time;
    double starttime = secs_elapsed(times, timens, res);
    ALOGD("Taintgrind Wrapper[%s] service starting after %f s! (from %ld to %ld)",
	argv[1], starttime, times, res.tv_sec);
	//initialize callback struct
	jniCallback.code = 0;
	jniCallback.data = 0;
	jniCallback.size = 0;
    String16 name = String16("TaintgrindWrapper");
    name.append(String16(argv[1]));
    defaultServiceManager()->addService(name, new Wrapper(strtol(argv[1], NULL, 10), getuid()));
    ProcessState::self()->startThreadPool();
    ALOGD("Wrapper service %s is now ready", (char*)name.string());
    IPCThreadState::self()->joinThreadPool();
    //ALOGD("Wrapper service thread joined");
    return 0;
}

} //namespace android

int main(int argc, char **argv) {
    ALOGD("Wrapper started ... ");
    return android::androidmain(argc, argv);
}

