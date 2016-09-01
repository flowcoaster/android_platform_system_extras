#ifndef BpWrapper_h
#define BpWrapper_h

#include <binder/IInterface.h>
#include <IWrapper.h>

namespace android {

// Client
class BpWrapper : public BpInterface<IWrapper> {
    private:
	JNIEnvMod* jniEnv;
	void callGetStringUTFChars();
	void callNewStringUTF();
	void callGetVersion();
	void callFindClass();
	void callGetSuperclass();
	void callIsAssignableFrom();
	void callAllocObject();
	void callThrow();
	void callThrowNew();
	void callExceptionOccurred();
	void callExceptionDescribe();
	void callExceptionClear();
	void callExceptionCheck();
	void callGetMethodID();
	void callGetFieldID();
	void callGetStaticMethodID();
	void callGetObjectField();
	void callGetBooleanField();
	void callGetByteField();
	void callGetCharField();
	void callGetShortField();
	void callGetIntField();
	void callGetLongField();
	void callGetFloatField();
	void callGetDoubleField();
	void callSetObjectField();
	void callSetBooleanField();
	void callSetByteField();
	void callSetCharField();
	void callSetShortField();
	void callSetIntField();
	void callSetLongField();
	void callSetFloatField();
	void callSetDoubleField();
	void callGetObjectClass();
	void callIsInstanceOf();
	void callNewLocalRef();
	void callNewGlobalRef();
	void callDeleteGlobalRef();
	void callIsSameObject();
	void callCallBooleanMethod();
	void callCallBooleanMethodV();
	void callCallBooleanMethodA();
	void callCallIntMethod();
	void callCallIntMethodV();
	void callCallIntMethodA();
	void callCallFloatMethodA();
	void callCallObjectMethod();
	void callCallVoidMethod();
	void callCallNonvirtualObjectMethodA();
	void callCallStaticShortMethodA();
	void callCallStaticVoidMethodA();
	void callDeleteLocalRef();
	void callSetShortArrayRegion();
	void callGetByteArrayRegion();
	void callGetStringChars();
	void callReleaseStringChars();
	void callReleaseStringCritical();
	void callFromReflectedField();
	void callNewObjectA();
	void callSetStaticLongField();
	void callToReflectedMethod();
	void callToReflectedField();
	void callFromReflectedMethod();
	void callGetPrimitiveArrayCritical();
	void callReleasePrimitiveArrayCritical();
	void callReleaseStringUTFChars();
	void callGetStringCritical();
	void callDeleteWeakGlobalRef();
	void callGetByteArrayElements();
	void callCallByteMethodA();
	void callCallCharMethodA();
	void callCallLongMethodA();
	void callCallDoubleMethodA();
	void callCallNonvirtualBooleanMethodA();
	void callCallNonvirtualByteMethodA();
	void callCallNonvirtualCharMethodA();
	void callCallNonvirtualShortMethodA();
	void callCallNonvirtualIntMethodA();
	void callCallNonvirtualLongMethodA();
	void callCallNonvirtualFloatMethodA();
	void callCallNonvirtualDoubleMethodA();
	void callCallNonvirtualVoidMethodA();
	void callCallShortMethodA();
	void callCallStaticCharMethodA();
	void callCallStaticByteMethodA();
	void callCallStaticBooleanMethodA();
	void callCallStaticObjectMethodA();
	void callCallStaticIntMethodA();
	void callCallStaticLongMethodA();
	void callCallStaticFloatMethodA();
	void callCallStaticDoubleMethodA();
	void callSetFloatArrayRegion();
	void callSetByteArrayRegion();
	void callSetCharArrayRegion();
	void callSetIntArrayRegion();
	int handleJNIRequest(JValTaint* res, Parcel* reply);
	void* replydata;
	void* replytaint;
	void* callbackdata;
	int size, taintsize, replylength;

    public:
        BpWrapper(const sp<IBinder>& impl);

	virtual int32_t call(char* library, char* method, Vector<void*> params, Vector<int> sizeofparam, Vector<int> taints);
	virtual int32_t addLib(const char* pathName);
	virtual int32_t addFunc(const char* funcName, int libRef);
	virtual JValTaint* taintCall(JNIEnvMod* pEnv, void* clazz, int argInfo, int argc, const uint32_t* taints,
    	    const uint32_t* argv, const char* shorty, int32_t libHandle, int32_t funcHandle, const char* funcName);
	virtual int callback(int function, int length, void* data, int taintlength, JValTaint* res);

	void setJniEnv(JNIEnvMod* jniEnv);
};

}

#endif

