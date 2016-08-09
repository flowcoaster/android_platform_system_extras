/*
 * Dispatcher.h
 *
 *  Created on: Feb 24, 2014
 *      Author: tobi
 */

#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <stdint.h>
#include "IWrapper.h"
#include <nativehelper/jni.h>
//#define LOG_TAG "Taintgrind Dispatcher"

//copied from nativehelper/jni.h
/*typedef int32_t  jint; 
struct _JNIEnv;
struct _JavaVM;
typedef _JNIEnv JNIEnv;
typedef _JavaVM JavaVM;
#define JNI_VERSION_1_6 0x00010006
#define JNI_OK          (0)   */      /* no error */

namespace android {

sp<IWrapper> getWrapperService(const char* name);
extern "C" JValTaint* taintCallMethod(JNIEnvMod* pEnv, void* clazz, int argInfo, int argc, const uint32_t* taints,
    	    const uint32_t* argv, const char* shorty, int32_t libHandle, int32_t funcHandle, const char* funcName);
extern "C" int32_t addLib(const char* filename);
extern "C" int32_t addFunc(const char* funcName, int32_t libRef);
extern "C" bool initDispatcher(bool forceStart);
extern "C" void callback();
jint JNI_OnLoad(JavaVM* vm, void* reserved);

} //namespace android

#endif /* DISPATCHER_H_ */

