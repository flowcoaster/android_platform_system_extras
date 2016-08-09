#include "BpDalvikJni.h"

namespace android {

BpDalvikJni::BpDalvikJni(const sp<IBinder>& impl) : BpInterface<IDalvikJni>(impl) {
	ALOGD("BpDalvikJni::BpDalvikJni");
}

jint BpDalvikJni::getVersion(JNIEnvMod* env) {
	return JNI_VERSION_1_6;
}

}
