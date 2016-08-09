/*
 * Dispatcher.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: Tobias Marktscheffel
 */

#include <pthread.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include "IWrapper.h"
#include "Wrapper.h"
#include "utils.h"
#include "Dispatcher.h"
//#ifdef WITH_DALVIK_BINDER_SERVICE
//#include "DalvikJni.h"
//#endif

#define LOG_TAG "Dispatcher sharedlib"

namespace android {

sp<IWrapper> wrapper;

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
        return wrapper;
    } else {
	ALOGD("Service %s not found!", name);
        return 0;
    }
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.

    return JNI_VERSION_1_6;
}

/*extern "C" void initDalvikJniBinder() {
	pthread_t tid = 0;
	int err = pthread_create(&tid, 0, &binderThreadMain, 0);
	if (err != 0)
		ALOGD("cannot create thread: %s", strerror(err));
	else
		ALOGD("Thread %ld for Dalvik Jni Service created successfully", tid);
	ALOGD("Now Dalvik can continue properly");
}*/

extern "C" bool initDispatcher(bool forceStart) {
    ALOGD("-> Dispatcher init(forceStart=%d)", forceStart);

	// get pid as String
    pid_t pid = getpid();
	assert((int)pid<100000);
    char cpid[6];
    sprintf(cpid, "%d", pid);
	

	//start Dalvik JNI service for callbacks from Wrapper
//#ifdef WITH_DALVIK_BINDER_SERVICE
	//initDalvikJniBinder();
//#endif

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
		ALOGD("<- Dispatcher init(): true");
		return true;
    }
}

extern "C" void setEnv(JNIEnv* env) {
	ALOGD("Dispatcher: setEnv(%p)", env);
}

extern "C" JValTaint* taintCallMethod(JNIEnvMod* pEnv, void* clazz, int argInfo, int argc, const uint32_t* taints,
    	    const uint32_t* argv, const char* shorty, int32_t libHandle, int32_t funcHandle, const char* funcName) {
    ALOGD("taintCallMethod");
    if (wrapper != 0) {
	return wrapper->taintCall(pEnv, clazz, argInfo, argc, taints, argv, shorty, libHandle, funcHandle, funcName);
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

extern "C" void callback() {
	ALOGD("-> callback()");
	if (wrapper != 0) wrapper->callback();
}

} //namespace android

