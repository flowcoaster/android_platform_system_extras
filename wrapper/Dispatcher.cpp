/*
 * Dispatcher.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: flowcoaster2
 */

#include <pthread.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include "IWrapper.h"
#include "Wrapper.h"
#include "utils.h"
#include "Dispatcher.h"
#include "WrapperDeathObserver.h"
//#ifdef WITH_DALVIK_BINDER_SERVICE
//#include "DalvikJni.h"
//#endif

#define LOG_TAG "Dispatcher sharedlib"

namespace android {

sp<IWrapper> wrapper;
sp<WrapperDeathObserver> deathObserver;


/*void* binderThreadMain(void* unused) {
	ALOGD("This is the new thread for the Binder service");
    pid_t pid = getpid();
	pid_t tid = gettid();
	assert((int)pid<100000);
    char cpid[5];
    sprintf(cpid, "%d", pid);
	ALOGD("cpid=%s, tid=%d", cpid, tid);
	const char* name = "DalvikJni";
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
}*/

sp<IWrapper> getWrapperService(const char* name, bool forceStart);

void registerDeathObserver(sp<IWrapper> wrapper) {
	deathObserver = new WrapperDeathObserver();
	deathObserver->setWrapper(wrapper);
	wrapper->asBinder()->linkToDeath(deathObserver);
}

// Helper function to get a hold of the Wrapper service.
sp<IWrapper> getWrapperService(const char* name, bool forceStart) {
    sp<IServiceManager> sm = 0;
    sm = defaultServiceManager();
    ASSERT(sm != 0);
    sp<IBinder> binder = 0;
    binder = sm->getService(String16(name));
    while (binder == 0 && forceStart)
	binder = sm->getService(String16(name));
    if (binder != 0) {
		ALOGD("Found service: %s", name);
        sp<IWrapper> wrapper = interface_cast<IWrapper>(binder);
        ASSERT(wrapper != 0);
		registerDeathObserver(wrapper);
		wrapper->setWrapperPointer(wrapper);
		wrapper->setServiceState(true);
        return wrapper;
    } else {
		ALOGD("Service %s not found!", name);
        return 0;
    }
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	ALOGD("JNI On load");
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.

    return JNI_VERSION_1_6;
}

extern "C" bool initDispatcher(bool forceStart) {
    ALOGD("-> Dispatcher init(forceStart=%d)", forceStart);

	// get pid as String
    pid_t pid = getpid();
	assert((int)pid<100000);
    char cpid[6];
    sprintf(cpid, "%d", pid);
	
	// get Wrapper service
	ALOGD("now trying to get Wrapper Service");
    //sp<IWrapper> dummy = getWrapperService("SurfaceFlinger");
    const char* cwrapper = "TaintgrindWrapper";
    char* result = (char*) malloc(1+strlen(cwrapper)+strlen(cpid));
    strcpy(result, cwrapper);
    strcat(result, cpid);
    wrapper = getWrapperService(result, forceStart);
    if (wrapper == 0) {
		ALOGD("<- Dispatcher init(): false");
		return false;
    } else {
		wrapper->setServiceName(result);
		ALOGD("<- Dispatcher init(): true");
		return true;
    }
}

extern "C" void setEnv(JNIEnv* env) {
	ALOGD("Dispatcher: setEnv(%p)", env);
}

extern "C" JValTaint* taintCallMethod(JNIEnvMod* pEnv, int clazz, int argInfo, int argc, const uint32_t* taints,
                                      const uint32_t* argv, const char* shorty, int32_t libHandle, int32_t funcHandle, const char* funcName, u4 threadId) {
    ALOGD("taintCallMethod");
    if (wrapper != 0) {
      return wrapper->taintCall(pEnv, clazz, argInfo, argc, taints, argv, shorty, libHandle, funcHandle, funcName, threadId);
    }
    return 0;
}

extern "C" int32_t addLib(const char* filename) {
    ALOGD("-> addLib(%s)", filename);
    if (wrapper!=0) {
        return wrapper->addLib(filename);
    } else {
	ALOGW("Wrapper service not initialized! Now waiting and trying indefinitely...");
	if (initDispatcher(true)) return wrapper->addLib(filename);
	return 0;
    }
}

extern "C" int32_t addFunc(const char* funcName, int32_t libRef) {
    ALOGD("-> addFunc(funcName=%s, libRef=%d)", funcName, libRef);
    if (wrapper != 0) {
		return wrapper->addFunc(funcName, libRef);
    } else return 0;
}

extern "C" int32_t changeFunc(int32_t oldHandle, int32_t newHandle) {
    ALOGD("-> changeFung(oldHandle=%08x, newHandle=%d)", oldHandle, newHandle);
    if (wrapper != 0) {
		return wrapper->changeFunc(oldHandle, newHandle);
    } else return 0;
}

} //namespace android

