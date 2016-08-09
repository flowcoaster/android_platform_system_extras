//#ifdef WITH_DALVIK_BINDER_SERVICE

#include <utils/Log.h>
#include "BnDalvikJni.h"
#include <utils/StrongPointer.h> //sp
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/IBinder.h>
#include "utils.h"

namespace android {

class DalvikJni : public BnDalvikJni {
	public:
	virtual jint getVersion(JNIEnvMod* env) {
		ALOGI("DalvikJni::getVersion");
		return 0;
	}
};


}

//#endif
