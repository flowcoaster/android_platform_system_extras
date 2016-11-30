#ifndef BpWrapper_h
#define BpWrapper_h

#include <binder/IInterface.h>
#include <IWrapper.h>
#include <utils/CallStack.h>

namespace android {

struct callback_t {
	int function;
	int length;
	int taintlength;
	JValTaint* res;
	void* rawdata;
};

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
	void callGetBooleanArrayRegion();
	void callGetByteArrayRegion();
	void callGetCharArrayRegion();
	void callGetShortArrayRegion();
	void callGetIntArrayRegion();
	void callGetLongArrayRegion();
	void callGetFloatArrayRegion();
	void callGetStringChars();
	void callReleaseStringChars();
	void callReleaseStringCritical();
	void callFromReflectedField();
	void callNewObjectA();
	void callSetStaticBooleanField();
	void callSetStaticByteField();
	void callSetStaticCharField();
	void callSetStaticShortField();
	void callSetStaticIntField();
	void callSetStaticLongField();
	void callSetStaticFloatField();
	void callSetStaticDoubleField();
	void callToReflectedMethod();
	void callToReflectedField();
	void callFromReflectedMethod();
	void callGetPrimitiveArrayCritical();
	void callReleasePrimitiveArrayCritical();
	void callReleaseStringUTFChars();
	void callGetStringCritical();
	void callDeleteWeakGlobalRef();
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
	void callSetBooleanArrayRegion();
	void callSetByteArrayRegion();
	void callSetCharArrayRegion();
	void callSetIntArrayRegion();
	void callSetLongArrayRegion();
	void callSetDoubleArrayRegion();
	void callGetBooleanArrayElements();
	void callGetByteArrayElements();
	void callGetCharArrayElements();
	void callGetIntArrayElements();
	void callGetLongArrayElements();
	void callGetFloatArrayElements();
	void callGetDoubleArrayElements();
	void callGetShortArrayElements();
	void callGetDoubleArrayRegion();
	void callReleaseByteArrayElements();
	void callReleaseBooleanArrayElements();
	void callReleaseCharArrayElements();
	void callReleaseShortArrayElements();
	void callReleaseIntArrayElements();
	void callReleaseLongArrayElements();
	void callReleaseFloatArrayElements();
	void callReleaseDoubleArrayElements();
	void callNewWeakGlobalRef();
	void callNewBooleanArray();
	void callNewByteArray();
	void callNewCharArray();
	void callNewShortArray();
	void callNewIntArray();
	void callNewLongArray();
	void callNewFloatArray();
	void callNewDoubleArray();
	void callGetObjectRefType();
	void callGetStringLength();
	void callGetStringRegion();
	void callGetStringUTFRegion();
	void callNewObjectArray();
	void callEnsureLocalCapacity();
	void callUnregisterNatives();
	void callPushLocalFrame();
	void callPopLocalFrame();
	void callGetStaticFieldID();
	void callGetStaticBooleanField();
	void callGetStaticByteField();
	void callGetStaticCharField();
	void callGetStaticShortField();
	void callGetStaticIntField();
	void callGetStaticLongField();
	void callGetStaticFloatField();
	void callGetStaticDoubleField();
	void callGetStaticObjectField();
	void callGetStringUTFLength();
	void callRegisterNatives();
	void callSetStaticObjectField();
	void callNewString();
	void callGetArrayLength();
	void callGetObjectArrayElement();
	void callSetObjectArrayElement();
	void callMonitorEnter();
	void callMonitorExit();
	void callNewDirectByteBuffer();
	void callGetDirectBufferAddress();
	void callGetDirectBufferCapacity();
	void handleJNIRequest(JValTaint* res, Parcel* reply);
	int doCallbackTransaction(int function, int length, void* rawdata,
                              int taintlength, JValTaint* res, Parcel* reply, u4 threadId);
	void* replydata;
	void* replytaint;
	void* callbackdata;
	int size, taintsize, replylength;
    CallStack *cs;
	callback_t* cbdata;
	bool serviceAlive;

    public:
    BpWrapper(const sp<IBinder>& impl);

	virtual int32_t addLib(const char* pathName);
	virtual int32_t addFunc(const char* funcName, int libRef);
	virtual JValTaint* taintCall(JNIEnvMod* pEnv, int clazz, int argInfo, int argc, const uint32_t* taints,
                                 const uint32_t* argv, const char* shorty, int32_t libHandle, int32_t funcHandle, const char* funcName, u4 threadId);
	virtual int32_t changeFunc(int32_t oldHandle, int32_t newHandle);

	void setJniEnv(JNIEnvMod* jniEnv);
	virtual void setServiceState(bool online);
};

}

#endif

