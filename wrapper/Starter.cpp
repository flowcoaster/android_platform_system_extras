#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <utils/Log.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include "DalvikJni.h"

namespace android {

void* binderThreadMain(void* unused) {
	ALOGD("This is the new thread for the Binder Test service");
    pid_t pid = getpid();
	pid_t tid = gettid();
	assert((int)pid<100000);
    char cpid[5];
    sprintf(cpid, "%d", pid);
	ALOGD("cpid=%s, tid=%d", cpid, tid);
	const char* name = "DalvikJniTest";
	char* resultd = (char*) malloc(strlen(name)+strlen(cpid));
	strcpy (resultd, name);
	strcat(resultd, cpid);
	ALOGD("Dispatcher: Starting Service %s", name);
	defaultServiceManager()->addService(String16(name), new DalvikJni());
	ALOGD("started service, now starting thread pool");
	ProcessState::self()->startThreadPool();
	ALOGD("started thread pool, now joining thread pool");
	IPCThreadState::self()->joinThreadPool();
	ALOGD("Thread finished. Exiting...");
	return 0;
}

int launcher() {
	pthread_t tid = 0;
	int err = pthread_create(&tid, 0, &binderThreadMain, 0);
	if (err != 0)
		ALOGD("cannot create thread: %s", strerror(err));
	else
		ALOGD("Thread %ld created successfully", tid);
	ALOGD("Now launcher can idle...zzzz...");
	sleep(1000);
	return 0;
}

} //namespace

int main(int argc, char **argv) {
    ALOGD("DVMJNI test service started with %d... ", argc);
	if (argc>1) {
		void* dispatcherHandle = dlopen("/system/lib/dispatcher.so", RTLD_LAZY);
    	typedef bool (*init_tt)(bool);
		init_tt initDispatcher = (init_tt) dlsym(dispatcherHandle, "initDispatcher");
    	typedef void (*initdjni_tt)();
		initdjni_tt initDalvikJniBinder = (initdjni_tt) dlsym(dispatcherHandle, "initDalvikJniBinder");
		initDalvikJniBinder();
		sleep(1000);
	} else
    return android::launcher();
}

