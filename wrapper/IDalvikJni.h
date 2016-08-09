#ifndef IDALVIKJNI_H
#define IDALVIKJNI_H

//#ifdef WITH_DALVIK_BINDER_SERVICE
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <utils/Errors.h> // status_t
#include "utils.h"
#include "JniEnvMod.h"

#define JNI_VERSION_1_6 0x00010006

namespace android {

class IDalvikJni : public IInterface {
	public:
	enum {
		GET_VERSION = IBinder::FIRST_CALL_TRANSACTION
	};

	virtual jint getVersion(JNIEnvMod* env) = 0;

	DECLARE_META_INTERFACE(DalvikJni);
};

}

//#endif //ifdef WITH_DALVIK_BINDER_SERVICE

#endif //ifndef IDALVIKJNI_H
