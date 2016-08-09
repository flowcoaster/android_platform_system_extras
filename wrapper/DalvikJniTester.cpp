#include <pthread.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/IBinder.h>
#include "Wrapper.h"
#include "IWrapper.h"
#include "utils.h"
#include "DalvikJni.h"
#include "DalvikJniTester.h"
#include <utils/Log.h>
#include "JniEnvMod.h"
#include "IDalvikJni.h"
#include "BpDalvikJni.h"
//#include "utils.h"

#define LOG_TAG "Dalvik Jni Tester"

namespace android {

sp<IDalvikJni> dvmJni;
//sp<IWrapper> dvmJni;

sp<IDalvikJni> getDalvikJniService(const char* name) {
//sp<IWrapper> getDalvikJniService(const char* name) {
    sp<IServiceManager> sm = 0;
    sm = defaultServiceManager();
    ASSERT(sm != 0);
    sp<IBinder> binder;
	ALOGD("Now looking for Dalvik Jni Service");
	while (binder == 0) binder = sm->getService(String16(name));
    if (binder != 0) {
	ALOGD("Found service: %s", name);
        sp<IDalvikJni> dalvikJni = interface_cast<IDalvikJni>(binder);
        //sp<IWrapper> dalvikJni = interface_cast<IWrapper>(binder);
        ASSERT(dalvikJni != 0);
        return dalvikJni;
    } else {
	ALOGD("Service %s not found!", name);
        return 0;
    }
}

int main(int argc, char** argv) {
	//dvmJni = getDalvikJniService("DalvikJniTest");
	dvmJni = getDalvikJniService("DalvikJni");
	ALOGD("now calling JNI Version function via Binder");
	JNIEnvMod* x = 0;
	jint version = dvmJni->getVersion(x);
	ALOGD("version=%d", version);
	return 0;
}

} //namespace

int main(int argc, char** argv) {
	android::main(argc, argv);
}
