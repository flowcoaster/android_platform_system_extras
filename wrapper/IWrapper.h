#ifndef IWrapper_h
#define IWrapper_h

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <utils/Vector.h>
#include <utils/Errors.h> // status_t
#include "nativehelper/jni.h"
#include "utils.h"
#include "JniEnvMod.h"

extern "C" void dvmPlatformInvoke(android::JNIEnvModExt* pEnv, void* clazz, int argInfo,
    int argc, const int* argv, const char* signature, void* func, android::jvalue* pResult);

namespace android {

typedef struct _JValTaint {
	jvalue val;
	int32_t taint;
} JValTaint;

class IWrapper : public IInterface {
    public:
        enum {
            CALL = IBinder::FIRST_CALL_TRANSACTION,
	    ADD_LIB,
	    ADD_FUNC,
	    TAINT_CALL,
		CALLBACK
        };
        // calls the requested function sends the result as reply over binder
	virtual int32_t call(char* library, char* method, Vector<void*> params, Vector<int> sizeofparam, Vector<int> taints) = 0;
	// loads library into wrapper
	virtual int32_t addLib(const char* pathName) = 0;
	// loads library into wrapper
	virtual int32_t addFunc(const char* funcName, int32_t libRef) = 0;
	// calls function with taints, wraps around dvmPlatformInvoke
	virtual JValTaint* taintCall(JNIEnvMod* pEnv, void* clazz, int argInfo, int argc, const uint32_t* taints,
    	    const uint32_t* argv, const char* shorty, int32_t libHandle, int32_t funcHandle, const char* funcName) = 0;
	virtual int callback(int function, int length, void* data, JValTaint* res) = 0;

        DECLARE_META_INTERFACE(Wrapper);  // Expands to 5 lines below:
        //static const android::String16 descriptor;
        //static android::sp<IWrapper> asInterface(const android::sp<android::IBinder>& obj);
        //virtual const android::String16& getInterfaceDescriptor() const;
        //IWrapper();
        //virtual ~IWrapper();

    protected:
	Vector<void*> libtable;
};

} //namespace android

#endif
