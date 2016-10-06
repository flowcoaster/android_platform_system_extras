//#ifdef WITH_DALVIK_BINDER_SERVICE

#include <JniEnvMod.h>
#include <stddef.h> // for NULL
#include <utils/Log.h>
#include "ExecutionManager.h"

#define LOG_TAG "JNI Remote"

#ifndef FLOWCOASTER_DEBUG
	#define ALOGD(...) void();
#endif

namespace android {

static jint AttachCurrentThread(JavaVM* vm, JNIEnvMod** p_env, void* thr_args) {
    ALOGD("AttachCurrentThread()");
	return 0;
}

static jint AttachCurrentThreadAsDaemon(JavaVM* vm, JNIEnvMod** p_env, void* thr_args) {
	ALOGD("AttachCurrentThreadAsDaemon()");
	return 0;
}

static jint DetachCurrentThread(JavaVM* vm) {
	ALOGD("DetachCurrentThread()");
	return 0;
}

static jint GetEnv(JavaVM* vm, void** env, jint version) {
	ALOGD("GetEnv()");
	return 0;
}

static jint DestroyJavaVM(JavaVM* vm) {
	ALOGD("DestroyJavaVM()");
	return 0;
}

static const struct JNIInvokeInterface gInvokeInterface = {
    NULL,
    NULL,
    NULL,

    DestroyJavaVM,
    AttachCurrentThread,
    DetachCurrentThread,

    GetEnv,

    AttachCurrentThreadAsDaemon,
};

//blueprint
//ALOGD method call
//get env pointer
//calculate size of data
//create void pointer for parcel and copy data into it
//in execManager, set function code, length, and data pointer
//call function
//get result out of param_data (needs pointer casting magic to get useful data type out of void pointer)
//free void pointer
//return

//code 3
static jint GetVersion(JNIEnvMod* env) {
	ALOGD("jniEnvMod->GetVersion(env=%08x)", (int)env);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 3;
	em->jniCall.taintsize = 0;
	em->jniCall.length = 0;
	em->reqJniCall();
	jint result = *(jint*)(em->jniCall.param_data);
	return result;
}

static jclass DefineClass(JNIEnvMod* env, const char *name, jobject loader,
    const jbyte* buf, jsize bufLen) {
	ALOGE("DefineClass(env=%08x, name=%s, loader=%08x, buf=%08x, bufLen=%d) is not supported.",
		(int)env, name, (int)loader, (int)buf, bufLen);
	return NULL;
}

//code 4
static jclass FindClass(JNIEnvMod* env, const char* name) {
	ALOGD("jniEnvMod->FindClass(env=%08x, name=%s at %08x)", (int)env, name, (int)name);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	if (name == 0) return NULL;
	int size = strlen(name)+1;
    em->jniCall.function = 4;
    em->jniCall.length = size;
    em->jniCall.taintsize = 0;
    em->jniCall.param_data = name;
    em->reqJniCall();
    jclass result = *(jclass*)(em->jniCall.param_data);
    ALOGD("returning %08x", (int)result);
    return result;
}

//code 64
static jmethodID FromReflectedMethod(JNIEnvMod* env, jobject jmethod) {
	ALOGD("jniEnvMod->FromReflectedMethod(env=%08x, jmethod=%08x)", (int)env, (int)jmethod);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 64;
    em->jniCall.length = sizeof(jmethod);
    em->jniCall.taintsize = 0;
    em->jniCall.param_data = &jmethod;
    em->reqJniCall();
	return *((jmethodID*)(em->jniCall.param_data));
}

//code 58
static jfieldID FromReflectedField(JNIEnvMod* env, jobject jfield) {
	ALOGD("jniEnvMod->FromReflectedField(env=%08x, jfield=%08x)", (int)env, (int)jfield);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 58;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jfield);
    em->jniCall.param_data = &jfield;
    em->reqJniCall();
	return *((jfieldID*)(em->jniCall.param_data));
}

//code 61
static jobject ToReflectedMethod(JNIEnvMod* env, jclass jcls, jmethodID methodID, jboolean isStatic) {
	ALOGD("jniEnvMod->ToReflectedMethod(env=%08x, jclass=%08x, methodID=%08x)",
		(int)env, (int)jcls, (int)methodID);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jcls) + sizeof(methodID);
	int* data = (int*)malloc(size);
	data[0] = (int)jcls;
	data[1] = (int)methodID;
    em->jniCall.function = 61;
    em->jniCall.taintsize = 0;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *((jobject*)(em->jniCall.param_data));
	free(data);
	return result;
}

//code 63
static jobject ToReflectedField(JNIEnvMod* env, jclass jcls, jfieldID fieldID, jboolean isStatic) {
	ALOGD("jniEnvMod->ToReflectedField(env=%08x, jclass=%08x, fieldID=%08x)",
		(int)env, (int)jcls, (int)fieldID);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jcls) + sizeof(fieldID);
	int* data = (int*)malloc(size);
	data[0] = (int)jcls;
	data[1] = (int)fieldID;
    em->jniCall.function = 63;
    em->jniCall.taintsize = 0;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 5
static jclass GetSuperclass(JNIEnvMod* env, jclass jclazz) {
	ALOGD("jniEnvMod->GetSuperclass(env=%08x, jclazz=%08x)", (int)env, (int)jclazz);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 5;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jclazz);
    em->jniCall.param_data = &jclazz;
    em->reqJniCall();
	jclass resultClass = *(jclass*)em->jniCall.param_data;
    ALOGD("returning %08x", (int)resultClass);
    return resultClass;
}

//code6
static jboolean IsAssignableFrom(JNIEnvMod* env, jclass jclazz1, jclass jclazz2) {
	ALOGD("jniEnvMod->IsAssignableFrom(env=%08x, jclazz1=%08x, jclazz2=%08x)",
		(int)env, (int)jclazz1, (int)jclazz2);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = 2*sizeof(jclazz1);
	jclass* data = (jclass*)malloc(size);
	data[0] = jclazz1;
	data[1] = jclazz2;
	em->jniCall.function = 6;
	em->jniCall.taintsize = 0;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jboolean result = *(jboolean*)em->jniCall.param_data;
	free(data);
	ALOGD("returning %08x", (int)result);
	return result;
}

//code8
static jint Throw(JNIEnvMod* env, jthrowable jt) {
	ALOGD("jniEnvMod->Throw(env=%08x, jthrowable=%08x)", (int)env, (int)jt);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 8;
	em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jt);
    em->jniCall.param_data = &jt;
    em->reqJniCall();
	jint result = *(jint*)em->jniCall.param_data;
	ALOGD("returning %08x", (int)result);
	return result;
}

//code 9
static jint ThrowNew(JNIEnvMod* env, jclass jclazz, const char* message) {
	ALOGD("jniEnvMod->ThrowNew(env=%08x, jclazz=%08x, message='%s')", (int)env, (int)jclazz, message);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jclazz)+sizeof(char)*(strlen(message)+1);
	jclass* data = (jclass*)malloc(size);
	data[0] = jclazz;
	memcpy(&data[1], message, sizeof(char)*(strlen(message)+1));
    em->jniCall.function = 9;
    em->jniCall.taintsize = 0;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jint result = *(jint*)em->jniCall.param_data;
	free(data);
	ALOGD("returning %08x", (int)result);
	return result;
}

//code 10
static jthrowable ExceptionOccurred(JNIEnvMod* env) {
	ALOGD("jniEnvMod->ExceptionOccurred(env=%08x)", (int)env);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 10;
    em->jniCall.taintsize = 0;
    em->jniCall.length = 0;
    em->jniCall.param_data = 0;
    em->reqJniCall();
	jthrowable result = *(jthrowable*)em->jniCall.param_data;
	ALOGD("returning %08x", (int)result);
	return result;
}

//code 11
static void ExceptionDescribe(JNIEnvMod* env) {
	ALOGD("jniEnvMod->ExceptionDescribe(env=%08x)", (int)env);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 11;
    em->jniCall.taintsize = 0;
    em->jniCall.length = 0;
    em->jniCall.param_data = 0;
    em->reqJniCall();
	ALOGD("returning after ExceptionDescribe");
}	

//code 12
static void ExceptionClear(JNIEnvMod* env) {
	ALOGD("jniEnvMod->ExceptionDescribe(env=%08x)", (int)env);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 12;
    em->jniCall.taintsize = 0;
    em->jniCall.length = 0;
    em->jniCall.param_data = 0;
    em->reqJniCall();
	ALOGD("returning after ExceptionClear");
}

static void FatalError(JNIEnvMod* env, const char* msg) {
    ALOGE("FatalError(env=%08x, msg=%s) is not supported.", (int)env, msg);
}

//code 125
static jint PushLocalFrame(JNIEnvMod* env, jint capacity) {
	ALOGD("jniEnvMod->PushLocalFrame(env=%08x, capacity=%08x)", (int)env, capacity);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 125;
	em->jniCall.taintsize = 0;
	em->jniCall.length = sizeof(capacity);
	em->jniCall.param_data = &capacity;
	em->reqJniCall();
	jint result = *(jint*)em->jniCall.param_data;
	return result;
}

//code 126
static jobject PopLocalFrame(JNIEnvMod* env, jobject jresult) {
	ALOGD("jniEnvMod->PopLocalFrame(env=%08x, jresult=%08x)", (int)env, (int)jresult);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 126;
	em->jniCall.taintsize = 0;
	em->jniCall.length = sizeof(jresult);
	em->jniCall.param_data = &jresult;
	em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	return result;
}

//code 21
static jobject NewGlobalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->NewGlobalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 21;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jobj);
    em->jniCall.param_data = &jobj;
    em->reqJniCall();
	return *(jobject*)em->jniCall.param_data;
}

//code 22
static void DeleteGlobalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->DeleteGlobalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 22;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jobj);
    em->jniCall.param_data = &jobj;
    em->reqJniCall();
}

//code 32
static void DeleteLocalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->DeleteLocalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 32;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jobj);
    em->jniCall.param_data = &jobj;
    em->reqJniCall();
}

//code 23
static jboolean IsSameObject(JNIEnvMod* env, jobject jobj1, jobject jobj2) {
	ALOGD("jniEnvMod->IsSameObject(env=%08x, jobj1=%08x, jobj2=%08x)", (int)env, (int)jobj1, (int)jobj2);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = 2*sizeof(jobj1);
	jobject* data = (jobject*)malloc(size);
	data[0] = jobj1;
	data[1] = jobj2;
    em->jniCall.function = 23;
    em->jniCall.taintsize = 0;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jboolean result = *(jboolean*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 20
static jobject NewLocalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->NewLocalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 20;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jobj);
    em->jniCall.param_data = &jobj;
    em->reqJniCall();
	return *(jobject*)em->jniCall.param_data;
}

//code 123
static jint EnsureLocalCapacity(JNIEnvMod* env, jint capacity) {
	ALOGD("jniEnvMod->NewLocalRef(env=%08x, capacity=%d)", (int)env, capacity);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 123;
    em->jniCall.taintsize = 0;
	em->jniCall.length = sizeof(capacity);
	em->jniCall.param_data = &capacity;
	em->reqJniCall();
	return *(jint*)em->jniCall.param_data;
}

//code 7
static jobject AllocObject(JNIEnvMod* env, jclass jclazz) {
	ALOGD("jniEnvMod->AllocObject(env=%08x, jclazz=%08x)", (int)env, (int)jclazz);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 7;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jclazz);
    em->jniCall.param_data = &jclazz;
    em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	ALOGD("returning %08x", (int)result);
	return result;
}

#define CALL_COPY() \
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager; \
	int paramSize = 8*em->getNumParams(methodID); \
	ALOGD("paramSize=%d", paramSize); \
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize; \
	int taintsize = paramSize+sizeof(jobject)+sizeof(methodID); \
	int* data = (int*)malloc(size+taintsize); \
	data[0] = (int)jobj; \
	data[1] = (int)methodID; \
	void* d2 = (void*)&data[2]; \
	const char* sig = em->getSignatureForMethod(methodID); \

#define NEW_COPY() \
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager; \
	int paramSize = 8*em->getNumParams(methodID); \
	ALOGD("paramSize=%d", paramSize); \
	int size = sizeof(jclass) + sizeof(jmethodID) + paramSize; \
	int taintsize = paramSize+sizeof(jobject)+sizeof(methodID); \
	int* data = (int*)malloc(size); \
	void* d2 = (void*)&data[2]; \
	data[0] = (int)jclazz; \
	data[1] = (int)methodID; \
	const char* sig = em->getSignatureForMethod(methodID);

#define CALL_COPY_NONVIRTUAL() \
	JNIEnvModExt* ext = (JNIEnvModExt*)env; \
	int paramSize = 8*ext->execManager->getNumParams(methodID); \
	ALOGD("paramSize=%d", paramSize); \
	int size = sizeof(jobject) + sizeof(jclass) + sizeof(jmethodID) + sizeof(paramSize) + paramSize; \
	void* data = malloc(size); \
	memcpy(data, &jobj, sizeof(jobj)); \
	void* d2 = data + sizeof(jobj); \
	memcpy(d2, &jc, sizeof(jc)); \
	d2 += sizeof(jc); \
	memcpy(d2, &methodID, sizeof(methodID)); \
	d2 += sizeof(methodID); \
	memcpy(d2, &paramSize, sizeof(paramSize)); \
	d2 += sizeof(paramSize); \
	const char* sig = ext->execManager->getSignatureForMethod(methodID);

#define CALL_COPY_STATIC() \
	JNIEnvModExt* ext = (JNIEnvModExt*)env; \
	int paramSize = 8*ext->execManager->getNumParams(methodID); \
	ALOGD("paramSize=%d", paramSize); \
	int size = sizeof(jclass) + sizeof(jmethodID) + paramSize; \
	void* data = malloc(size); \
	memcpy(data, &jc, sizeof(jc)); \
	void* d2 = data + sizeof(jc); \
	memcpy(d2, &methodID, sizeof(methodID)); \
	d2 += sizeof(methodID); \
	memcpy(d2, &paramSize, sizeof(paramSize)); \
	d2 += sizeof(paramSize); \
	const char* sig = ext->execManager->getSignatureForMethod(methodID);

#define PARAMS_FROM_ARGS() \
	int i = 0; \
	jvalue v; \
	while (sig[i] != ')') { \
		switch (sig[i]) { \
			case '(': break; \
			case 'L': { \
				v.l = va_arg(args, jobject); \
				memcpy(d2, &v, sizeof(v)); \
				ALOGD("copying jobject %08x", (int)v.l); \
				d2 += sizeof(v); \
				break; } \
			case 'Z': { \
				v.z = va_arg(args, jboolean); \
				memcpy(d2, &v, sizeof(v)); \
				ALOGD("copying jboolean %08x", (int)v.z); \
				d2 += sizeof(v); \
				break; } \
			case 'B': \
			case 'C': \
			case 'S': \
			case 'I': { \
				v.i = va_arg(args, jint); \
				memcpy(d2, &v, sizeof(v)); \
				ALOGD("copying jint %d", v.i); \
				d2 += sizeof(v); \
				break; } \
			case 'D': \
			case 'J': { \
				v.j = va_arg(args, jlong); \
				memcpy(d2, &v, sizeof(v)); \
				ALOGD("copying jdouble/jlong %f/%lld", v.d, v.j); \
				d2 += sizeof(v); \
				break; } \
			default: \
				ALOGE("Unexpected character in signature string: <%c>", sig[i]); \
				break; \
		} \
		i++; \
	} 

//code 59
static jobject NewObject(JNIEnvMod* env, jclass jclazz, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->NewObject(env=%08x, jclazz=%08x, method=%08x)", (int)env, (int)jclazz, (int)methodID);
	NEW_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 59;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 59
static jobject NewObjectV(JNIEnvMod* env, jclass jclazz, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->NewObject(env=%08x, jclazz=%08x, method=%08x, va_list)", (int)env, (int)jclazz, (int)methodID);
	NEW_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 59;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 59
static jobject NewObjectA(JNIEnvMod* env, jclass jclazz, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->NewObjectA(env=%08x, jclazz=%08x, method=%08x, args=%08x)",
		(int)env, (int)jclazz, (int)methodID, (int)args);
	NEW_COPY();
	memcpy(&data[2], args, paramSize);
    em->jniCall.function = 59;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 18
static jclass GetObjectClass(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->GetObjectClass(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 18;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jobj);
    em->jniCall.param_data = &jobj;
    em->reqJniCall();
	return *(jclass*)em->jniCall.param_data;
}

//code 19
static jboolean IsInstanceOf(JNIEnvMod* env, jobject jobj, jclass jclazz) {
	ALOGD("jniEnvMod->IsInstanceOf(env=%08x, jobj=%08x, jclazz=%08x)", (int)env, (int)jobj, (int)jclazz);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jobj) + sizeof(jclazz);
	int* data = (int*)malloc(size);
	data[0] = (int)jobj;
	data[1] = (int)jclazz;
    em->jniCall.function = 19;
    em->jniCall.taintsize = 0;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jboolean result = *(jboolean*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 14
static jmethodID GetMethodID(JNIEnvMod* env, jclass jclazz, const char* name, const char* sig) {
	ALOGD("jniEnvMod->GetMethodID(env=%08x, jclazz=%08x, name=%s, sig=%s)", (int)env, (int)jclazz, name, sig);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jclazz) + sizeof(char)*(strlen(name)+1) + sizeof(char)*(strlen(sig)+1);
	int* data = (int*)malloc(size);
	data[0] = (int)jclazz;
	void* d2 = (void*)&(data[1]);
	memcpy(d2, name, sizeof(char)*(strlen(name)+1));
	d2 += sizeof(char)*(strlen(name)+1);
	memcpy(d2, sig, sizeof(char)*(strlen(sig)+1));
    em->jniCall.function = 14;
    em->jniCall.taintsize = 0;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jmethodID result = *(jmethodID*)em->jniCall.param_data;
	if (result != 0) em->addSignatureForMethod(result, sig);
	free(data);
	return result;
}

//code 33
static jobject CallObjectMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 33;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 33
static jobject CallObjectMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 33;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 33
static jobject CallObjectMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 33;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 24
static jboolean CallBooleanMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 24;
    em->jniCall.length = size;
    em->jniCall.taintsize = taintsize;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jboolean result = *(jboolean*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 25
static jboolean CallBooleanMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallBooleanMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 25;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jboolean result = *(jboolean*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 26
static jboolean CallBooleanMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 26;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jboolean result = *(jboolean*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 71
static jbyte CallByteMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallByteMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args)",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 71;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jbyte result = *(jbyte*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 71
static jbyte CallByteMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallByteMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 71;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jbyte result = *(jbyte*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 71
static jbyte CallByteMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallByteMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 71;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jbyte result = *(jbyte*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 72
static jchar CallCharMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallCharMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args)",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 72;
	em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jchar result = *(jchar*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 72
static jchar CallCharMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallCharMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 72;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jchar result = *(jchar*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 72
static jchar CallCharMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallCharMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 72;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jchar result = *(jchar*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 84
static jshort CallShortMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallShortMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 84;
	em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jshort result = *(jshort*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 84
static jshort CallShortMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallShortMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 84;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jshort result = *(jshort*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 84
static jshort CallShortMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallShortMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 84;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jshort result = *(jshort*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 27
static jint CallIntMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallIntMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 27;
	em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jint result = *(jint*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 28
static jint CallIntMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallIntMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 28;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jint result = *(jint*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 29
static jint CallIntMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallIntMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 29;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jint result = *(jint*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 73
static jlong CallLongMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallLongMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 73;
	em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jlong result = *(jlong*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 73
static jlong CallLongMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallLongMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 73;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jlong result = *(jlong*)(em->jniCall.param_data);
	free(data);
	return result;
}

//code 73
static jlong CallLongMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallLongMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 73;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jlong result = *(jlong*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 62
static jfloat CallFloatMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallFloatMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 62;
	em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jfloat result = *(jfloat*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 62
static jfloat CallFloatMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallFloatMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 62;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jfloat result = *(jfloat*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 62
static jfloat CallFloatMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallFloatMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 62;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jfloat result = *(jfloat*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 74
static jdouble CallDoubleMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallDoubleMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 74;
	em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jdouble result = *(jdouble*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 74
static jdouble CallDoubleMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallDoubleMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 74;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jdouble result = *(jdouble*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 74
static jdouble CallDoubleMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallDoubleMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 74;
    em->jniCall.taintsize = taintsize;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jdouble result = *(jdouble*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 34
static void CallVoidMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallIntMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    em->jniCall.function = 34;
    em->jniCall.length = size;
    em->jniCall.taintsize = taintsize;
    em->jniCall.param_data = data;
    em->reqJniCall();
	free(data);
}

//code 34
static void CallVoidMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallVoidMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    em->jniCall.function = 34;
    em->jniCall.length = size;
    em->jniCall.taintsize = taintsize;
    em->jniCall.param_data = data;
    em->reqJniCall();
	free(data);
}

//code 34
static void CallVoidMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallVoidMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    em->jniCall.function = 34;
    em->jniCall.length = size;
    em->jniCall.taintsize = taintsize;
    em->jniCall.param_data = data;
    em->reqJniCall();
	free(data);
}

//code 31
static jobject CallNonvirtualObjectMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualObjectMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 31;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 31
static jobject CallNonvirtualObjectMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualObjectMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 31;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 31
static jobject CallNonvirtualObjectMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualObjectMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 31;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 75
static jboolean CallNonvirtualBooleanMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualBooleanMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 75;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 75
static jboolean CallNonvirtualBooleanMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualBooleanMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 75;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 75
static jboolean CallNonvirtualBooleanMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualBooleanMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 75;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 76
static jbyte CallNonvirtualByteMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualByteMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 76;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 76
static jbyte CallNonvirtualByteMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualByteMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 76;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 76
static jbyte CallNonvirtualByteMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualByteMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 76;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 77
static jchar CallNonvirtualCharMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualCharMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 77;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 77
static jchar CallNonvirtualCharMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualCharMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 77;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 77
static jchar CallNonvirtualCharMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualCharMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 77;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 78
static jshort CallNonvirtualShortMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualShortMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 78;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 78
static jshort CallNonvirtualShortMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualShortMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 78;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 78
static jshort CallNonvirtualShortMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualShortMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 78;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 79
static jint CallNonvirtualIntMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualIntMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 79;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 79
static jint CallNonvirtualIntMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualIntMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 79;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 79
static jint CallNonvirtualIntMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualIntMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 79;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 80
static jlong CallNonvirtualLongMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualLongMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 80;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 80
static jlong CallNonvirtualLongMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualLongMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 80;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 80
static jlong CallNonvirtualLongMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualLongMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 80;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 81
static jfloat CallNonvirtualFloatMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualFloatMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 81;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 81
static jfloat CallNonvirtualFloatMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualFloatMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 81;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 81
static jfloat CallNonvirtualFloatMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualFloatMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 81;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 82
static jdouble CallNonvirtualDoubleMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualDoubleMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 82;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 82
static jdouble CallNonvirtualDoubleMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualDoubleMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 82;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 82
static jdouble CallNonvirtualDoubleMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualDoubleMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 82;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 83
static void CallNonvirtualVoidMethod(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallNonvirtualVoidMethod(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, ...",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 83;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 83
static void CallNonvirtualVoidMethodV(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallNonvirtualVoidMethodV(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, va_list",
		(int)env, (int)jobj, (int)jc, (int)methodID);
	CALL_COPY_NONVIRTUAL();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 83;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 83
static void CallNonvirtualVoidMethodA(JNIEnvMod* env, jobject jobj, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallNonvirtualVoidMethodA(env=%08x, jobject=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x", (int)env, (int)jobj, (int)jc, (int)methodID, (int)args);
	CALL_COPY_NONVIRTUAL();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 83;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 52
static void CallStaticVoidMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticVoidMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 52;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 52
static void CallStaticVoidMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticVoidMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 52;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 52
static void CallStaticVoidMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticVoidMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 52;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 88
static jobject CallStaticObjectMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticObjectMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 88;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 88
static jobject CallStaticObjectMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticObjectMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 88;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 88
static jobject CallStaticObjectMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticObjectMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 88;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 87
static jboolean CallStaticBooleanMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticBooleanMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 87;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 87
static jboolean CallStaticBooleanMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticBooleanMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 87;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 87
static jboolean CallStaticBooleanMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticBooleanMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 87;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 86
static jbyte CallStaticByteMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticByteMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 86;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 86
static jbyte CallStaticByteMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticByteMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 86;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 86
static jbyte CallStaticByteMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticByteMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 86;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 85
static jchar CallStaticCharMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticCharMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 85;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 85
static jchar CallStaticCharMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticCharMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 85;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 85
static jchar CallStaticCharMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticCharMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 85;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 51
static jshort CallStaticShortMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticShortMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 51;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 51
static jshort CallStaticShortMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticShortMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 51;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 51
static jshort CallStaticShortMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticShortMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 51;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 89
static jint CallStaticIntMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticIntMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 89;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 89
static jint CallStaticIntMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticIntMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 89;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 89
static jint CallStaticIntMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticIntMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 89;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 90
static jlong CallStaticLongMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticLongMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 90;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 90
static jlong CallStaticLongMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticLongMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 90;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 90
static jlong CallStaticLongMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticLongMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 90;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 91
static jfloat CallStaticFloatMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticFloatMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 91;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 91
static jfloat CallStaticFloatMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticFloatMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 91;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 91
static jfloat CallStaticFloatMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticFloatMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 91;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 92
static jdouble CallStaticDoubleMethod(JNIEnvMod* env, jclass jc, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallStaticDoubleMethod(env=%08x, jclass=%08x, methodID=%08x, ...)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	va_list args;
	va_start(args, methodID);
	PARAMS_FROM_ARGS();
	va_end(args);	
    ext->execManager->jniCall.function = 91;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 92
static jdouble CallStaticDoubleMethodV(JNIEnvMod* env, jclass jc, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallStaticDoubleMethodV(env=%08x, jclass=%08x, methodID=%08x, va_list)",
		(int)env, (int)jc, (int)methodID);
	CALL_COPY_STATIC();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 91;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 92
static jdouble CallStaticDoubleMethodA(JNIEnvMod* env, jclass jc, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallStaticDoubleMethodA(env=%08x, jclass=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jc, (int)methodID, (int)args);
	CALL_COPY_STATIC();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 91;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 15
static jfieldID GetFieldID(JNIEnvMod* env, jclass jclazz, const char* name, const char* sig) {
	ALOGD("jniEnvMod->GetFieldID(env=%08x, jclazz=%08x, name=%s, sig=%s)", (int)env, (int)jclazz, name, sig);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jclazz) + sizeof(char)*(strlen(name)+1) + sizeof(char)*(strlen(sig)+1);
	void* data = malloc(size);
	memcpy(data, &jclazz, sizeof(jclazz));
	void* d2 = data+sizeof(jclazz);
	memcpy(d2, name, sizeof(char)*(strlen(name)+1));
	d2 += sizeof(char)*(strlen(name)+1);
	memcpy(d2, sig, sizeof(char)*(strlen(sig)+1));
    ext->execManager->jniCall.function = 15;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfieldID result = (jfieldID)*(int*)(ext->execManager->jniCall.param_data);
	return result;
}

//code 44
static jobject GetObjectField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetObjectField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 44;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 45
static jboolean GetBooleanField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetBooleanField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 45;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 46
static jbyte GetByteField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetByteField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 46;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 47
static jchar GetCharField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetShortField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 47;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 48
static jshort GetShortField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetShortField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 48;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 30
static jint GetIntField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetIntField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 30;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 17
static jlong GetLongField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetLongField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 17;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 49
static jfloat GetFloatField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetFloatField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 49;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 50
static jdouble GetDoubleField(JNIEnvMod* env, jobject jobj, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetDoubleField(env=%08x, jobj=%08x, fieldID=%08x)", (int)env, (int)jobj, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 50;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);;
	return result;
}

//code 43
static void SetObjectField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jobject val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jobj, (int)fieldID, (int)val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(val);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 43;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 42
static void SetBooleanField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jboolean val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jobj, (int)fieldID, val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(val);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 42;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 41
static void SetByteField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jbyte val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jobj, (int)fieldID, val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(val);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 41;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 40
static void SetCharField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jchar val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jobj, (int)fieldID, val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(val);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 40;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 39
static void SetShortField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jshort val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jobj, (int)fieldID, val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(val);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 39;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 35
static void SetIntField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jint val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jobj, (int)fieldID, val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(val);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 35;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 36
static void SetLongField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jlong val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jobj, (int)fieldID, (int)val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(jlong);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 36;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 38
static void SetFloatField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jfloat val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%f)", (int)env, (int)jobj, (int)fieldID, val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(val);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 38;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 37
static void SetDoubleField(JNIEnvMod* env, jobject jobj, jfieldID fieldID, jdouble val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jobj=%08x, fieldID=%08x, val=%f)", (int)env, (int)jobj, (int)fieldID, val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(fieldID) + sizeof(jlong);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jobj)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 37;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 2*sizeof(u4);
    ext->execManager->reqJniCall();
}

//code 16
static jmethodID GetStaticMethodID(JNIEnvMod* env, jclass jclazz, const char* name, const char* sig) {
	ALOGD("jniEnvMod->GetStaticMethodID(env=%08x, jclazz=%08x, name=%s, sig=%s)", (int)env, (int)jclazz, name, sig);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jclazz) + sizeof(char)*(strlen(name)+1) + sizeof(char)*(strlen(sig)+1);
	void* data = malloc(size);
	memcpy(data, &jclazz, sizeof(jclazz));
	void* d2 = data+sizeof(jclazz);
	memcpy(d2, name, sizeof(char)*(strlen(name)+1));
	d2 += sizeof(char)*(strlen(name)+1);
	memcpy(d2, sig, sizeof(char)*(strlen(sig)+1));
    ext->execManager->jniCall.function = 16;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jmethodID result = (jmethodID)*(int*)(ext->execManager->jniCall.param_data);;
	if (result != 0) ext->execManager->addSignatureForMethod(result, sig);
	return result;
}

//code 134
static jfieldID GetStaticFieldID(JNIEnvMod* env, jclass jclazz, const char* name, const char* sig) {
	ALOGD("jniEnvMod->GetStaticFieldID(env=%08x, jclazz=%08x, name=%s, sig=%s)",
		(int)env, (int)jclazz, name, sig);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jclazz) + sizeof(char)*(strlen(name)+1) + sizeof(char)*(strlen(sig)+1);
	void* data = malloc(size);
	memcpy(data, &jclazz, sizeof(jclazz));
	void* d2 = data+sizeof(jclazz);
	memcpy(d2, name, sizeof(char)*(strlen(name)+1));
	d2 += sizeof(char)*(strlen(name)+1);
	memcpy(d2, sig, sizeof(char)*(strlen(sig)+1));
    em->jniCall.function = 134;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jfieldID result = (jfieldID)*(int*)(em->jniCall.param_data);
	return result;
}

#define GETSTATIC_COPYDATA() \
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager; \
	int size = sizeof(jclazz) + sizeof(jfieldID); \
	void* data = malloc(size); \
	memcpy(data, &jclazz, sizeof(jclazz)); \
	memcpy(data+sizeof(jclazz), &fieldID, sizeof(fieldID)); \
	
//code 135
static jint GetStaticIntField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticIntField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 135;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jint*)(em->jniCall.param_data);
}

//code 136
static jshort GetStaticShortField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticShortField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 136;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jshort*)(em->jniCall.param_data);
}

//code 137
static jchar GetStaticCharField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticCharField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 137;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jchar*)(em->jniCall.param_data);
}

//code 138
static jbyte GetStaticByteField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticByteField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 138;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jbyte*)(em->jniCall.param_data);
}

//code 139
static jboolean GetStaticBooleanField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticBooleanField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 139;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jboolean*)(em->jniCall.param_data);
}

//code 140
static jobject GetStaticObjectField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticObjectField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 140;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jobject*)(em->jniCall.param_data);
}

//code 141
static jlong GetStaticLongField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticLongField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 141;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jlong*)(em->jniCall.param_data);
}

//code 142
static jfloat GetStaticFloatField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticFloatField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 142;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jfloat*)(em->jniCall.param_data);
}

//code 143
static jdouble GetStaticDoubleField(JNIEnvMod* env, jclass jclazz, jfieldID fieldID) {
	ALOGD("jniEnvMod->GetStaticDoubleField(env=%08x, jclazz=%08x, jfieldID=%08x)",
		(int)env, (int)jclazz, (int)fieldID);
	GETSTATIC_COPYDATA();
    em->jniCall.function = 143;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	return *(jdouble*)(em->jniCall.param_data);
}

#define SETSTATIC_COPYDATA() \
	void* data = malloc(size); \
	memcpy(data, &val, sizeof(val)); \
	memcpy(data+sizeof(val), &jc, sizeof(jc)); \
	memcpy(data+sizeof(val)+sizeof(jc), &fieldID, sizeof(fieldID)); 

//code 146
static void SetStaticObjectField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jobject val) {
	ALOGD("jniEnvMod->SetStaticBooleanField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jobject);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 133;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 8;
    em->reqJniCall();
}

//code 133
static void SetStaticBooleanField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jboolean val) {
	ALOGD("jniEnvMod->SetStaticBooleanField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jboolean);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 133;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 8;
    em->reqJniCall();
}

//code 132
static void SetStaticByteField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jbyte val) {
	ALOGD("jniEnvMod->SetStaticByteField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jbyte);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 132;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 8;
    em->reqJniCall();
}

//code 131
static void SetStaticCharField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jchar val) {
	ALOGD("jniEnvMod->SetStaticCharField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jchar);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 131;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 8;
    em->reqJniCall();
}

//code 130
static void SetStaticShortField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jshort val) {
	ALOGD("jniEnvMod->SetStaticShortField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jshort);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 130;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 8;
    em->reqJniCall();
}

//code 129
static void SetStaticIntField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jint val) {
	ALOGD("jniEnvMod->SetStaticIntField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jint);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 129;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 8;
    em->reqJniCall();
}

//code 60
static void SetStaticLongField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jlong val) {
	ALOGD("jniEnvMod->SetStaticLongField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jlong);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 60;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 8;
    em->reqJniCall();
}

//code 128
static void SetStaticFloatField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jfloat val) {
	ALOGD("jniEnvMod->SetStaticFloatField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jfloat);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 128;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 0;
    em->reqJniCall();
}

//code 127
static void SetStaticDoubleField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jdouble val) {
	ALOGD("jniEnvMod->SetStaticDoubleField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jdouble);
	SETSTATIC_COPYDATA();
    em->jniCall.function = 127;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 0;
    em->reqJniCall();
}

//code 147
static jstring NewString(JNIEnvMod* env, const jchar* unicodeChars, jsize len) {
	ALOGD("NewString(env=%08x, unicodeChars=%08x, len=%d)", (int)env, (int)unicodeChars, len);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	int size = sizeof(jchar)*len + sizeof(len);
	void* data = malloc(size);
	jsize* i = (jsize*)data;
	*i = len;
	memcpy(data+sizeof(len), unicodeChars, sizeof(jchar)*len);
    em->jniCall.function = 147;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->jniCall.taintsize = 0;
    em->reqJniCall();
	jstring result = *(jstring*)(em->jniCall.param_data);
	free(data);
	return result;
}

//code 119
static jsize GetStringLength(JNIEnvMod* env, jstring jstr) {
	ALOGD("jniEnvMod->GetStringLength(env=%08x, jstr=%08x)", (int)env, (int)jstr);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 119;
	em->jniCall.length = sizeof(jstr);
	em->jniCall.param_data = &jstr;
	em->reqJniCall();
	return *(jsize*)(em->jniCall.param_data);
}

//code 55
static const jchar* GetStringChars(JNIEnvMod* env, jstring jstr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetStringChars(env=%08x, jstr=%08x, isCopy=%08x)",
		(int)env, (int)jstr, (int)isCopy);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jstr) + sizeof(jboolean);
	void* data = malloc(size);
	memcpy(data, &jstr, sizeof(jstr));
	if (isCopy != 0) {
		memcpy(data+sizeof(jstr), isCopy, sizeof(jboolean));
	} else {
		jboolean b = 0;
		memcpy(data+sizeof(jstr), &b, sizeof(jboolean));
	}
    ext->execManager->jniCall.function = 55;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	const void* r = ext->execManager->jniCall.param_data;
	int dalvikP = *((int*)r);
	r += sizeof(jchar*);
	int strlen = *((int*)r);
	r += sizeof(int);
	jchar* result = (jchar*)malloc(strlen*sizeof(jchar)+1);
	memcpy(result, r, strlen*sizeof(jchar));
	ext->execManager->addStringChars(dalvikP, strlen, result);
	ALOGD("GetStringChars: %08x -> %s", *result, (char*)result);
	return (const jchar*)result;
}

//code 56
void ReleaseStringChars(JNIEnvMod* env, jstring jstr, const jchar* chars) {
	ALOGD("jniEnvMod->ReleaseStringChars(env=%08x, jstr=%08x, jchars=%08x)",
		(int)env, (int)jstr, (int)chars);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	stringMap_t* s = ext->execManager->getDalvikChars(chars);
	if (s == 0) {
		ALOGE("Chars entry missing");
		return;
	} else if (s->length < 0) {
		ALOGE("Negative string length: %d", s->length);
		return;
	}
	ALOGD("found string map entry at %08x", (int)s);
	ALOGD("with length=%d, dalvikP=%08x, chars=%s", s->length, (int)(s->dalvikP), s->wrapperP);
	int size = sizeof(jstr) + sizeof(s->dalvikP) + sizeof(s->length) + sizeof(jchar)*s->length;
	void* data = malloc(size);
	ALOGD("allocated memory of size %d to %08x", size, (int*)data);
	memcpy(data, &jstr, sizeof(jstr));
	memcpy(data+sizeof(jstr), &(s->dalvikP), sizeof(s->dalvikP));
	memcpy(data+sizeof(jstr)+sizeof(s->dalvikP), &(s->length), sizeof(s->length));
	memcpy(data+sizeof(jstr)+sizeof(s->dalvikP)+sizeof(s->length), chars, sizeof(jchar)*s->length);
	ALOGD("copied all data");
    ext->execManager->jniCall.function = 56;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	ALOGD("ReleaseStringChars: Now deleting entry in table (chars=%08x)", (int)chars);
	ALOGD("execManager=%08x", (int)(ext->execManager));
	ext->execManager->deleteCharsEntry(chars);
}

//code 2
static jstring NewStringUTF(JNIEnvMod* env, const char* bytes) {
    ALOGD("jniEnvMod->NewStringUTF(env=%08x, bytes=%08x (%s))", (int)env, (int)bytes, bytes);
    JNIEnvModExt* ext = (JNIEnvModExt*)env;
    int size = strlen(bytes)+1; //+1 for terminating 0
    ext->execManager->jniCall.function = 2;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = bytes;
    ext->execManager->reqJniCall();
    jstring strResult = (jstring)*(int*)(ext->execManager->jniCall.param_data);
    ALOGD("returning %08x", (int)strResult);
    return strResult;
}

//code 
static jsize GetStringUTFLength(JNIEnvMod* env, jstring jstr) {
	ALOGD("jniEnvMod->GetStringUTFLength(env=%08x, jstr=%08x)", (int)env, (int)jstr);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 999;
	em->jniCall.length = sizeof(jstr);
	em->jniCall.param_data = &jstr;
	em->reqJniCall();
	return *(jsize*)(em->jniCall.param_data);
}

//code 1
static const char* GetStringUTFChars(JNIEnvMod* env, jstring jstr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetStringUTFChars(env=%08x, jstr=%08x, isCopy=%08x)",
		(int)env, (int)jstr, (int)isCopy);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	ALOGD("JniEnv->execManager=%p with status %s",
		ext->execManager, ExecutionManager::strStatus(ext->execManager->mStatus));
	int size = sizeof(jstring), sizeb = sizeof(jboolean)*sizeof(char);
	//void* data = malloc(size+sizeb); // freed after reqJniCall()
	void* data = malloc(size); // freed after reqJniCall()
	memcpy(data, &jstr, size);
	//if (isCopy != NULL) memcpy(data+size, isCopy, sizeb);
	//else memset(data+size, 0, sizeb);
	ALOGD("jstr=%p, iscopy=%d", jstr, isCopy);
	ext->execManager->jniCall.function = 1;
	ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
	ext->execManager->jniCall.length = size;
	ext->execManager->reqJniCall();
	free(data);
    if (isCopy != NULL) {
        *isCopy = JNI_TRUE;
    }
	//ext->execManager->reqJniCall(42);
	int dalvikP = *(int*)(ext->execManager->jniCall.param_data);
	const char* d = (char*)(ext->execManager->jniCall.param_data+sizeof(char*));
	ext->execManager->addStringChars(dalvikP, 0, (const jchar*)d);
	return d;
}

//code 67
static void ReleaseStringUTFChars(JNIEnvMod* env, jstring jstr, const char* utf) {
	ALOGD("jniEnvMod->ReleaseStringUTFChars(env=%08x, jstr=%08x, utf=%s)",
		(int)env, (int)jstr, utf);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	stringMap_t* s = ext->execManager->getDalvikChars((const jchar*)utf);
	int length = strlen(utf);
	int size = sizeof(char*) + sizeof(int) + length*8;
	void* data = malloc(size);
	memcpy(data, &s->dalvikP, sizeof(char*));
	memcpy(data+sizeof(char*), &length, sizeof(int));
	memcpy(data+sizeof(char*)+sizeof(int), &jstr, sizeof(jstring));
	memcpy(data+sizeof(char*)+2*sizeof(int), utf, length*8);
	ext->execManager->jniCall.function = 67;
	ext->execManager->jniCall.param_data = data;
	ext->execManager->jniCall.length = size;
	ext->execManager->reqJniCall();
	free(data);
}

//code 148
static jsize GetArrayLength(JNIEnvMod* env, jarray jarr) {
	ALOGD("GetArrayLength(env=%08x, jarr=%08x)", (int)env, (int)jarr);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 148;
    em->jniCall.length = sizeof(jarr);
    em->jniCall.taintsize = 0;
    em->jniCall.param_data = &jarr;
    em->reqJniCall();
	return *(jsize*)(em->jniCall.param_data);
}

//code 122
static jobjectArray NewObjectArray(JNIEnvMod* env, jsize length, jclass jelementClass, jobject jinitialElement) {
	ALOGD("jniEnvMod->NewObjectArray(env=%08x, length=%d, jelementClass=%08x, jinitialElement=%08x)",
		(int)env, length, (int)jelementClass, (int)jinitialElement);
	int size = sizeof(length) + sizeof(jelementClass) + sizeof(jinitialElement);
	void* data = malloc(size);
	memcpy(data, &length, sizeof(length));
	memcpy(data+sizeof(length), &jelementClass, sizeof(jelementClass));
	memcpy(data+sizeof(length)+sizeof(jelementClass), &jinitialElement, sizeof(jinitialElement));
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 122;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobjectArray result = *(jobjectArray*)(em->jniCall.param_data);
	free(data);
	return result;
}

//code 149
static jobject GetObjectArrayElement(JNIEnvMod* env, jobjectArray jarr, jsize index) {
	ALOGD("GetObjectArrayElement(env=%08x, jarr=%08x, index=%d)",
		(int)env, (int)jarr, index);
	int size = sizeof(jarr) + sizeof(index);
	int* data = (int*)malloc(size);
	data[0] = (int)jarr;
	data[1] = index;
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 149;
    em->jniCall.length = size;
    em->jniCall.taintsize = 0;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)(em->jniCall.param_data);
	ALOGD("GetObjectArrayElement->%08x", (int)result);
	free(data);
	return result;
}

//code 150
static void SetObjectArrayElement(JNIEnvMod* env, jobjectArray jarr, jsize index, jobject jobj) {
	ALOGD("SetObjectArrayElement(env=%08x, jarr=%08x, index=%d, jobj=%08x)",
		(int)env, (int)jarr, index, (int)jobj);
	int size = sizeof(jarr) + sizeof(index) + sizeof(jobj);
	int* data = (int*)malloc(size);
	data[0] = (int)jarr;
	data[1] = index;
	data[2] = (int)jobj;
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 150;
    em->jniCall.length = size;
    em->jniCall.taintsize = 0;
    em->jniCall.param_data = data;
    em->reqJniCall();
	free(data);
}

//code 110
static jbooleanArray NewBooleanArray(JNIEnvMod* env, jsize length) {
	ALOGD("jniEnvMod->NewBooleanArray(env=%08x, length=%08x)", (int)env, length);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 110;
    em->jniCall.length = sizeof(length);
    em->jniCall.param_data = &length;
    em->reqJniCall();
	jbooleanArray result = *(jbooleanArray*)(em->jniCall.param_data);
	return result;
}

//code 111
static jbyteArray NewByteArray(JNIEnvMod* env, jsize length) {
	ALOGD("jniEnvMod->NewByteArray(env=%08x, length=%08x)", (int)env, length);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 111;
    em->jniCall.length = sizeof(length);
    em->jniCall.param_data = &length;
    em->reqJniCall();
	jbyteArray result = *(jbyteArray*)(em->jniCall.param_data);
	return result;
}

//code 112
static jcharArray NewCharArray(JNIEnvMod* env, jsize length) {
	ALOGD("jniEnvMod->NewCharArray(env=%08x, length=%08x)", (int)env, length);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 113;
    em->jniCall.length = sizeof(length);
    em->jniCall.param_data = &length;
    em->reqJniCall();
	jcharArray result = *(jcharArray*)(em->jniCall.param_data);
	return result;
}

//code 113
static jshortArray NewShortArray(JNIEnvMod* env, jsize length) {
	ALOGD("jniEnvMod->NewShortArray(env=%08x, length=%08x)", (int)env, length);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 113;
    em->jniCall.length = sizeof(length);
    em->jniCall.param_data = &length;
    em->reqJniCall();
	jshortArray result = *(jshortArray*)(em->jniCall.param_data);
	return result;
}

//code 114
static jintArray NewIntArray(JNIEnvMod* env, jsize length) {
	ALOGD("jniEnvMod->NewIntArray(env=%08x, length=%08x)", (int)env, length);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 114;
    em->jniCall.length = sizeof(length);
    em->jniCall.param_data = &length;
    em->reqJniCall();
	jintArray result = *(jintArray*)(em->jniCall.param_data);
	return result;
}

//code 115
static jlongArray NewLongArray(JNIEnvMod* env, jsize length) {
	ALOGD("jniEnvMod->NewLongArray(env=%08x, length=%08x)", (int)env, length);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 115;
    em->jniCall.length = sizeof(length);
    em->jniCall.param_data = &length;
    em->reqJniCall();
	jlongArray result = *(jlongArray*)(em->jniCall.param_data);
	return result;
}

//code 116
static jfloatArray NewFloatArray(JNIEnvMod* env, jsize length) {
	ALOGD("jniEnvMod->NewFloatArray(env=%08x, length=%08x)", (int)env, length);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 116;
    em->jniCall.length = sizeof(length);
    em->jniCall.param_data = &length;
    em->reqJniCall();
	jfloatArray result = *(jfloatArray*)(em->jniCall.param_data);
	return result;
}

//code 117
static jdoubleArray NewDoubleArray(JNIEnvMod* env, jsize length) {
	ALOGD("jniEnvMod->NewDoubleArray(env=%08x, length=%08x)", (int)env, length);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 117;
    em->jniCall.length = sizeof(length);
    em->jniCall.param_data = &length;
    em->reqJniCall();
	jdoubleArray result = *(jdoubleArray*)(em->jniCall.param_data);
	return result;
}

#define GET_GENERIC_ARRAYELEMENTS() \
	size = *((int*)(ext->execManager->jniCall.param_data));\
	int dalvikP = *((int*)(ext->execManager->jniCall.param_data+sizeof(int)));\
	ALOGD("GetPrimitiveArrayCritical received size=%d, dalvikP=%08x", size, dalvikP);\
	ext->execManager->addArray(jarr, size, dalvikP);\
	void* result = malloc(size);\
	memcpy(result, ext->execManager->jniCall.param_data+sizeof(int)+sizeof(dalvikP), size);

//code 151
static jboolean* GetBooleanArrayElements(JNIEnvMod* env, jbooleanArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetBooleanArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 151;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	GET_GENERIC_ARRAYELEMENTS();
	return (jboolean*)result;
}

//code 70
static jbyte* GetByteArrayElements(JNIEnvMod* env, jbyteArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetByteArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 70;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	GET_GENERIC_ARRAYELEMENTS();
	return (jbyte*)result;
}

//code 152
static jchar* GetCharArrayElements(JNIEnvMod* env, jcharArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetCharArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 152;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	GET_GENERIC_ARRAYELEMENTS();
	return (jchar*)result;
}

//code 99
static jshort* GetShortArrayElements(JNIEnvMod* env, jshortArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetShortArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 99;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	GET_GENERIC_ARRAYELEMENTS();
	return (jshort*)result;
}

//code 153
static jint* GetIntArrayElements(JNIEnvMod* env, jintArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetShortArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 153;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	GET_GENERIC_ARRAYELEMENTS();
	return (jint*)result;
}

//code 97
static jlong* GetLongArrayElements(JNIEnvMod* env, jlongArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetLongArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 97;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	GET_GENERIC_ARRAYELEMENTS();
	return (jlong*)result;
}

//code 154
static jfloat* GetFloatArrayElements(JNIEnvMod* env, jfloatArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetFloatArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 154;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	GET_GENERIC_ARRAYELEMENTS();
	return (jfloat*)result;
}

//code 98
static jdouble* GetDoubleArrayElements(JNIEnvMod* env, jdoubleArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetDoubleArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 98;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	GET_GENERIC_ARRAYELEMENTS();
	return (jdouble*)result;
}

//code 102
static void ReleaseBooleanArrayElements(JNIEnvMod* env, jbooleanArray jarr, jboolean* elems, jint mode) {
	ALOGD("jniEnvMod->ReleaseByteArrayElements(env=%08x, jarr=%08x, elems=%08x, mode=%08x)",
		(int)env, (int)jarr, (int)elems, mode);
	if (mode == 1) return; // 1 = JNI_COMMIT
	//TODO: copy out data?
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 102;
    ext->execManager->jniCall.length = sizeof(jarr);
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
}

//code 101
static void ReleaseByteArrayElements(JNIEnvMod* env, jbyteArray jarr, jbyte* elems, jint mode) {
	ALOGD("jniEnvMod->ReleaseByteArrayElements(env=%08x, jarr=%08x, elems=%08x, mode=%08x)",
		(int)env, (int)jarr, (int)elems, mode);
	if (mode == 1) return; // 1 = JNI_COMMIT
	//TODO: copy out data?
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 101;
    ext->execManager->jniCall.length = sizeof(jarr);
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
}

//code 103
static void ReleaseCharArrayElements(JNIEnvMod* env, jcharArray jarr, jchar* elems, jint mode) {
	ALOGD("jniEnvMod->ReleaseCharArrayElements(env=%08x, jarr=%08x, elems=%08x, mode=%08x)",
		(int)env, (int)jarr, (int)elems, mode);
	if (mode == 1) return; // 1 = JNI_COMMIT
	//TODO: copy out data?
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 103;
    ext->execManager->jniCall.length = sizeof(jarr);
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
}

//code 104
static void ReleaseShortArrayElements(JNIEnvMod* env, jshortArray jarr, jshort* elems, jint mode) {
	ALOGD("jniEnvMod->ReleaseShortArrayElements(env=%08x, jarr=%08x, elems=%08x, mode=%08x)",
		(int)env, (int)jarr, (int)elems, mode);
	if (mode == 1) return; // 1 = JNI_COMMIT
	//TODO: copy out data?
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 104;
    ext->execManager->jniCall.length = sizeof(jarr);
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
}

//code 105
static void ReleaseIntArrayElements(JNIEnvMod* env, jintArray jarr, jint* elems, jint mode) {
	ALOGD("jniEnvMod->ReleaseIntArrayElements(env=%08x, jarr=%08x, elems=%08x, mode=%08x)",
		(int)env, (int)jarr, (int)elems, mode);
	//TODO: copy out data?
	if (mode == 1) return; // 1 = JNI_COMMIT
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 105;
    ext->execManager->jniCall.length = sizeof(jarr);
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
}

//code 106
static void ReleaseLongArrayElements(JNIEnvMod* env, jlongArray jarr, jlong* elems, jint mode) {
	ALOGD("jniEnvMod->ReleaseLongArrayElements(env=%08x, jarr=%08x, elems=%08x, mode=%08x)",
		(int)env, (int)jarr, (int)elems, mode);
	if (mode == 1) return; // 1 = JNI_COMMIT
	//TODO: copy out data?
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 106;
    ext->execManager->jniCall.length = sizeof(jarr);
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
}

//code 107
static void ReleaseFloatArrayElements(JNIEnvMod* env, jfloatArray jarr, jfloat* elems, jint mode) {
	ALOGD("jniEnvMod->ReleaseFloatArrayElements(env=%08x, jarr=%08x, elems=%08x, mode=%08x)",
		(int)env, (int)jarr, (int)elems, mode);
	if (mode == 1) return; // 1 = JNI_COMMIT
	//TODO: copy out data?
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 107;
    ext->execManager->jniCall.length = sizeof(jarr);
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
}

//code 108
static void ReleaseDoubleArrayElements(JNIEnvMod* env, jdoubleArray jarr, jdouble* elems, jint mode) {
	ALOGD("jniEnvMod->ReleaseDoubleArrayElements(env=%08x, jarr=%08x, elems=%08x, mode=%08x)",
		(int)env, (int)jarr, (int)elems, mode);
	if (mode == 1) return; // 1 = JNI_COMMIT
	//TODO: copy out data?
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 108;
    ext->execManager->jniCall.length = sizeof(jarr);
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
}

#define GENERIC_GETARRAYREGION() \
	JNIEnvModExt* ext = (JNIEnvModExt*)env;\
	int size = sizeof(jarr) + 2*sizeof(jsize);\
	void* data = malloc(size);\
	memcpy(data, &jarr, sizeof(jarr));\
	void* d2 = data + sizeof(jarr);\
	memcpy(d2, &start, sizeof(start));\
	d2 += sizeof(start);\
	memcpy(d2, &len, sizeof(len));

//code 155
static void GetBooleanArrayRegion(JNIEnvMod* env, jbooleanArray jarr, jsize start, jsize len, jboolean* buf) {
	ALOGD("jniEnvMod->GetBooleanArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	GENERIC_GETARRAYREGION();
    ext->execManager->jniCall.function = 155;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jboolean)*len);
	free(data);
}

//code 54
static void GetByteArrayRegion(JNIEnvMod* env, jbyteArray jarr, jsize start, jsize len, jbyte* buf) {
	ALOGD("jniEnvMod->GetByteArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	GENERIC_GETARRAYREGION();
    ext->execManager->jniCall.function = 54;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jbyte)*len);
	free(data);
}

//code 156
static void GetCharArrayRegion(JNIEnvMod* env, jcharArray jarr, jsize start, jsize len, jchar* buf) {
	ALOGD("jniEnvMod->GetCharArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	GENERIC_GETARRAYREGION();
    ext->execManager->jniCall.function = 156;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jchar)*len);
	free(data);
}

//code 157
static void GetShortArrayRegion(JNIEnvMod* env, jshortArray jarr, jsize start, jsize len, jshort* buf) {
	ALOGD("jniEnvMod->GetShortArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	GENERIC_GETARRAYREGION();
    ext->execManager->jniCall.function = 157;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jshort)*len);
	free(data);
}

//code 158
static void GetIntArrayRegion(JNIEnvMod* env, jintArray jarr, jsize start, jsize len, jint* buf) {
	ALOGD("jniEnvMod->GetIntArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	GENERIC_GETARRAYREGION();
    ext->execManager->jniCall.function = 158;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jint)*len);
	free(data);
}

//code 159
static void GetLongArrayRegion(JNIEnvMod* env, jlongArray jarr, jsize start, jsize len, jlong* buf) {
	ALOGD("jniEnvMod->GetLongArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	GENERIC_GETARRAYREGION();
    ext->execManager->jniCall.function = 159;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jlong)*len);
	free(data);
}

//code 160
static void GetFloatArrayRegion(JNIEnvMod* env, jfloatArray jarr, jsize start, jsize len, jfloat* buf) {
	ALOGD("jniEnvMod->GetFloatArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	GENERIC_GETARRAYREGION();
    ext->execManager->jniCall.function = 160;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jfloat)*len);
	free(data);
}

//code 100
static void GetDoubleArrayRegion(JNIEnvMod* env, jdoubleArray jarr, jsize start, jsize len, jdouble* buf) {
	ALOGD("jniEnvMod->GetDoubleArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	GENERIC_GETARRAYREGION();
    ext->execManager->jniCall.function = 100;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jdouble)*len);
	free(data);
}

#define ARRAYREGION_COPYOUT() \
	JNIEnvModExt* ext = (JNIEnvModExt*)env; \
	int size = sizeof(jarr) + 2*sizeof(jsize) + len*sizeof(*buf);\
	void* data = malloc(size);\
	memcpy(data, &jarr, sizeof(jarr));\
	void* d2 = data + sizeof(jarr);\
	memcpy(d2, &start, sizeof(start));\
	d2 += sizeof(start);\
	memcpy(d2, &len, sizeof(len));\
	d2 += sizeof(len);

//code 161
static void SetBooleanArrayRegion(JNIEnvMod* env, jbooleanArray jarr, jsize start, jsize len, const jboolean* buf) {
	ALOGD("jniEnvMod->SetBooleanArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	ARRAYREGION_COPYOUT();
	memcpy(d2, buf, len*sizeof(jboolean));
    ext->execManager->jniCall.function = 161;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 94
static void SetByteArrayRegion(JNIEnvMod* env, jbyteArray jarr, jsize start, jsize len, const jbyte* buf) {
	ALOGD("jniEnvMod->SetByteArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	ARRAYREGION_COPYOUT();
	memcpy(d2, buf, len*sizeof(jbyte));
    ext->execManager->jniCall.function = 94;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 95
static void SetCharArrayRegion(JNIEnvMod* env, jcharArray jarr, jsize start, jsize len, const jchar* buf) {
	ALOGD("jniEnvMod->SetCharArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	ARRAYREGION_COPYOUT();
	memcpy(d2, buf, len*sizeof(jchar));
    ext->execManager->jniCall.function = 95;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 53
static void SetShortArrayRegion(JNIEnvMod* env, jshortArray jarr, jsize start, jsize len, const jshort* buf) {
	ALOGD("jniEnvMod->SetShortArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	ARRAYREGION_COPYOUT();
	memcpy(d2, buf, len*sizeof(jshort));
    ext->execManager->jniCall.function = 53;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 96
static void SetIntArrayRegion(JNIEnvMod* env, jintArray jarr, jsize start, jsize len, const jint* buf) {
	ALOGD("SetIntArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	ARRAYREGION_COPYOUT();
	memcpy(d2, buf, len*sizeof(jint));
    ext->execManager->jniCall.function = 96;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 162
static void SetLongArrayRegion(JNIEnvMod* env, jlongArray jarr, jsize start, jsize len, const jlong* buf) {
	ALOGD("SetIntArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	ARRAYREGION_COPYOUT();
	memcpy(d2, buf, len*sizeof(jlong));
    ext->execManager->jniCall.function = 162;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 93
static void SetFloatArrayRegion(JNIEnvMod* env, jfloatArray jarr, jsize start, jsize len, const jfloat* buf) {
	ALOGD("SetFloatArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	ARRAYREGION_COPYOUT();
	memcpy(d2, buf, len*sizeof(jfloat));
    ext->execManager->jniCall.function = 93;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 163
static void SetDoubleArrayRegion(JNIEnvMod* env, jdoubleArray jarr, jsize start, jsize len, const jdouble* buf) {
	ALOGD("SetFloatArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	ARRAYREGION_COPYOUT();
	memcpy(d2, buf, len*sizeof(jdouble));
    ext->execManager->jniCall.function = 163;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 145
static jint RegisterNatives(JNIEnvMod* env, jclass jclazz, const JNINativeMethod* methods, jint nMethods) {
	ALOGD("RegisterNatives(env=%08x, jclazz=%08x, methods=%08x, nMenthods=%08x)",
		(int)env, (int)jclazz, (int)methods, nMethods);
	int size = sizeof(jclazz) + sizeof(nMethods) + nMethods*(2*sizeof(int)+sizeof(void*));
	for (int i=0; i<nMethods; i++)
		size += (strlen(methods[i].name)+strlen(methods[i].signature)+2)*sizeof(char);
	void* data = malloc(size);
	memcpy(data, &jclazz, sizeof(jclazz));
	memcpy(data+sizeof(jclazz), &nMethods, sizeof(nMethods));
	void* d2 = data+sizeof(jclazz)+sizeof(nMethods);
	for (int i=0; i<nMethods; i++) {
		int j = strlen(methods[i].name)+1;
		memcpy(d2, &j, sizeof(j));
		d2 += sizeof(j);
		memcpy(d2, methods[i].name, j*sizeof(char));
		d2 += j*sizeof(char);
		j = strlen(methods[i].signature)+1;
		memcpy(d2, &j, sizeof(j));
		d2 += sizeof(j);
		memcpy(d2, methods[i].signature, j*sizeof(char));
		d2 += j*sizeof(char);
		int v = (int)methods[i].fnPtr;
		ALOGD("v=%08x, fnPtr=%08x", v, methods[i].fnPtr);
		memcpy(d2, &v, sizeof(v));
		d2 += sizeof(v);
		ALOGD("copied out %s (%s) @ %08x", methods[i].name, methods[i].signature, (int)methods[i].fnPtr);
	}
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 145;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jint result = *(jint*)em->jniCall.param_data;
	free(data);
	return result;
}

//code 124
static jint UnregisterNatives(JNIEnvMod* env, jclass jclazz) {
	ALOGD("UnregisterNatives(env=%08x, jclazz=%08x)", (int)env, (int)jclazz);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 124;
	em->jniCall.length = sizeof(jclazz);
	em->jniCall.param_data = &jclazz;
	em->reqJniCall();
	return *(jint*)(em->jniCall.param_data);
}

//code 164
static jint MonitorEnter(JNIEnvMod* env, jobject jobj) {
	ALOGD("MonitorEnter(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 164;
	em->jniCall.length = sizeof(jobj);
	em->jniCall.param_data = &jobj;
	em->reqJniCall();
	return *(jint*)(em->jniCall.param_data);
}

//code 165
static jint MonitorExit(JNIEnvMod* env, jobject jobj) {
	ALOGD("MonitorExit(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 165;
	em->jniCall.length = sizeof(jobj);
	em->jniCall.param_data = &jobj;
	em->reqJniCall();
	return *(jint*)(em->jniCall.param_data);
}

static jint GetJavaVM(JNIEnvMod* env, JavaVM** vm) {
	ALOGD("GetJavaVM(env=%08x, vm=%08x)", (int)env, (int)vm);
	*vm = (JavaVM*)&gInvokeInterface;
    jint result = (*vm == NULL) ? JNI_ERR : JNI_OK;
	ALOGD("GetJavaVM -> %d", result);
	return result;
}

//code 120
static void GetStringRegion(JNIEnvMod* env, jstring jstr, jsize start, jsize len, jchar* buf) {
	ALOGD("GetStringRegion(env=%08x, jstr=%08x, start=%d, len=%d, buf=%08x)",
		(int)env, (int)jstr, (int)start, (int)len, (int)buf);
	int size = sizeof(jstr) + sizeof(start) + sizeof(len);
	void* data = malloc(size);
	memcpy(data, &jstr, sizeof(jstr));
	memcpy(data+sizeof(jstr), &start, sizeof(start));
	memcpy(data+sizeof(jstr)+sizeof(len), &len, sizeof(len));
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 120;
	em->jniCall.length = size;
	em->jniCall.param_data = data;
	em->reqJniCall();
	memcpy(buf, em->jniCall.param_data, sizeof(jchar)*len);
	free(data);
}

//code 121
static void GetStringUTFRegion(JNIEnvMod* env, jstring jstr, jsize start, jsize len, char* buf) {
	ALOGD("GetStringUTFRegion(env=%08x, jstr=%08x, start=%d, len=%d, buf=%08x)",
		(int)env, (int)jstr, (int)start, (int)len, (int)buf);
	int size = sizeof(jstr) + sizeof(start) + sizeof(len);
	void* data = malloc(size);
	memcpy(data, &jstr, sizeof(jstr));
	memcpy(data+sizeof(jstr), &start, sizeof(start));
	memcpy(data+sizeof(jstr)+sizeof(len), &len, sizeof(len));
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
	em->jniCall.function = 120;
	em->jniCall.length = size;
	em->jniCall.param_data = data;
	em->reqJniCall();
	memcpy(buf, em->jniCall.param_data, 8*len); //UTF-chars are 8bit
	free(data);
}

/*static void* GetPrimitiveArrayCritical(JNIEnvMod* env, jshortArray jarr, jboolean* isCopy) {
	ALOGD("GetShortArrayCritical");
	void* result = malloc(8);
	return result;
}

static void* GetPrimitiveArrayCritical(JNIEnvMod* env, jbyteArray jarr, jboolean* isCopy) {
	ALOGD("GetByteArrayCritical");
	void* result = malloc(8);
	return result;
}*/

//code 65
static void* GetPrimitiveArrayCritical(JNIEnvMod* env, jarray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetPrimitiveArrayCritical(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 65;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	size = *((int*)(ext->execManager->jniCall.param_data));
	int dalvikP = *((int*)(ext->execManager->jniCall.param_data+sizeof(int)));
	ALOGD("GetPrimitiveArrayCritical received size=%d, dalvikP=%08x", size, dalvikP);
	ext->execManager->addArray(jarr, size, dalvikP);
	//TODO: too fragile? void* might be freed by BnWrapper?
	//void* result = (void*)(ext->execManager->jniCall.param_data+sizeof(int)+sizeof(dalvikP));
	void* result = malloc(size);
	memcpy(result, ext->execManager->jniCall.param_data+sizeof(int)+sizeof(dalvikP), size);
	return result;
}

//code 66
static void ReleasePrimitiveArrayCritical(JNIEnvMod* env, jarray jarr, void* array, jint mode) {
	ALOGD("jniEnvMod->ReleasePrimitiveArrayCritical(env=%08x, jarr=%08x, array=%08x, mode=%08x)", (int)env, (int)jarr, (int)array, mode);
	if (mode == 1) return; 
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	arrayList_t* at = ext->execManager->getArrayLength(jarr);
	ALOGD("address of length: %08x", (int)&(at->length));
	ALOGD("address of dalvikP: %08x", (int)&(at->dalvikP));
	ALOGD("dalvikP=%08x", at->dalvikP);
	int size = sizeof(jarr) + sizeof(int) + at->length;
	if (size<0) {
		ALOGE("Array lost (negative size)"); return;
	}
	jshort* iarr = (jshort*)array;
	ALOGD("i[0]=%d, i[1]=%d, i[2]=%d, i[3]=%d, i[4]=%d, i[5]=%d, i[6]=%d",
		iarr[0], iarr[1], iarr[2], iarr[3], iarr[4], iarr[5], iarr[6]);
	void* data = malloc(size);
	memcpy(data, &jarr, sizeof(jarr));
	memcpy(data+sizeof(jarr), &(at->length), 2*sizeof(int));
	memcpy(data+sizeof(jarr)+2*sizeof(int), array, at->length);
    ext->execManager->jniCall.function = 66;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	//free(array); crashes! TODO: but would we need to free this?
}

//code 68
static const jchar* GetStringCritical(JNIEnvMod* env, jstring jstr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetStringCritical(env=%08x, jstr=%08x, isCopy=%08x)",
		(int)env, (int)jstr, (int)isCopy);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jstr) + sizeof(jboolean);
	void* data = malloc(size);
	memcpy(data, &jstr, sizeof(jstr));
	if (isCopy != 0) {
		memcpy(data+sizeof(jstr), isCopy, sizeof(jboolean));
	} else {
		jboolean b = 0;
		memcpy(data+sizeof(jstr), &b, sizeof(jboolean));
	}
    ext->execManager->jniCall.function = 68;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	const void* r = ext->execManager->jniCall.param_data;
	int dalvikP = *((int*)r);
	r += sizeof(jchar*);
	int strlen = *((int*)r);
	r += sizeof(int);
	jchar* result = (jchar*)malloc(strlen*sizeof(jchar)+1);
	memcpy(result, r, strlen*sizeof(jchar));
	ext->execManager->addStringChars(dalvikP, strlen, result);
	ALOGD("GetStringCritical: %08x -> %s", *result, (char*)result);
	return (const jchar*)result;
}

//code 57
void ReleaseStringCritical(JNIEnvMod* env, jstring jstr, const jchar* chars) {
	ALOGD("jniEnvMod->ReleaseStringCritical(env=%08x, jstr=%08x, jchars=%08x)",
		(int)env, (int)jstr, (int)chars);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	stringMap_t* s = ext->execManager->getDalvikChars(chars);
	if (s == 0) {
		ALOGE("Chars entry missing");
		return;
	} else if (s->length < 0) {
		ALOGE("Negative string length: %d", s->length);
		return;
	}
	//ALOGD("found string map entry at %08x", s);
	//ALOGD("with length=%d, dalvikP=%08x, chars=%s", s->length, s->dalvikP, s->wrapperP);
	int size = sizeof(jstr) + sizeof(s->dalvikP) + sizeof(s->length) + sizeof(jchar)*s->length;
	void* data = malloc(size);
	//ALOGD("allocated memory of size %d to %08x", size, (int*)data);
	memcpy(data, &jstr, sizeof(jstr));
	memcpy(data+sizeof(jstr), &(s->dalvikP), sizeof(s->dalvikP));
	memcpy(data+sizeof(jstr)+sizeof(s->dalvikP), &(s->length), sizeof(s->length));
	memcpy(data+sizeof(jstr)+sizeof(s->dalvikP)+sizeof(s->length), chars, sizeof(jchar)*s->length);
	ext->execManager->deleteCharsEntry(chars);
    ext->execManager->jniCall.function = 57;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	const jchar* result = (jchar*)ext->execManager->jniCall.param_data;
}

//code 109
static jweak NewWeakGlobalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->NewWeakGlobalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 109;
    em->jniCall.length = sizeof(jobj);
    em->jniCall.param_data = &jobj;
    em->reqJniCall();
	jweak result = *(jweak*)(em->jniCall.param_data);
	return result;
}

//code 69
static void DeleteWeakGlobalRef(JNIEnvMod* env, jweak jw) {
	ALOGD("jniEnvMod->DeleteWeakGlobalRef(env=%08x, jweak=%08x)", (int)env, (int)jw);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jw);
	void* data = malloc(size);
	memcpy(data, &jw, size);
    ext->execManager->jniCall.function = 69;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 13
static jboolean ExceptionCheck(JNIEnvMod* env) {
	ALOGD("jniEnvMod->ExceptionCheck(env=%08x)", (int)env);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 13;
    ext->execManager->jniCall.length = 0;
    ext->execManager->jniCall.param_data = 0;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	ALOGD("returning %08x", (int)result);
	return result;
}

//code 166
static jobject NewDirectByteBuffer(JNIEnvMod* env, void* address, jlong capacity) {
	ALOGD("NewDirectByteBuffer(env=%08x, address=%08x, capacity=%lld)", (int)env, (int)address, capacity);
	ALOGW("Direct Byte Buffer not fully supported!"); //TODO
	int size = sizeof(capacity) + capacity*sizeof(address);
	jlong* data = (jlong*)malloc(size);
	data[0] = capacity;
	memcpy(&data[1], address, capacity*sizeof(address));
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 166;
    em->jniCall.taintsize = 0;
    em->jniCall.length = size;
    em->jniCall.param_data = data;
    em->reqJniCall();
	jobject result = *(jobject*)(em->jniCall.param_data);
	free(data);
	return result;
}

//code 167
static void* GetDirectBufferAddress(JNIEnvMod* env, jobject jbuf) {
	ALOGD("GetDirectBufferAddress(env=%08x, jbuf=%08x)", (int)env, (int)jbuf);
	ALOGW("Direct Byte Buffer not fully supported!"); //TODO
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 167;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jbuf);
    em->jniCall.param_data = &jbuf;
    em->reqJniCall();
	jlong* resultsize = (jlong*)em->jniCall.param_data;
	void* result = malloc(resultsize[0]);
	memcpy(result, &resultsize[1], resultsize[0]);
	return result;
}

//code 168
static jlong GetDirectBufferCapacity(JNIEnvMod* env, jobject jbuf) {
	ALOGD("GetDirectBufferCapacity(env=%08x, jbuf=%08x)", (int)env, (int)jbuf);
	ALOGW("Direct Byte Buffer not fully supported!"); //TODO
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 168;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jbuf);
    em->jniCall.param_data = &jbuf;
    em->reqJniCall();
	return *(jlong*)em->jniCall.param_data;
}

//code 118
static jobjectRefType GetObjectRefType(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->GetObjectRefType(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	ExecutionManager* em = ((JNIEnvModExt*)env)->execManager;
    em->jniCall.function = 118;
    em->jniCall.taintsize = 0;
    em->jniCall.length = sizeof(jobj);
    em->jniCall.param_data = &jobj;
    em->reqJniCall();
	jobjectRefType result = *(jobjectRefType*)(em->jniCall.param_data);
	return result;
}

//function table just like in dalvik/vm/Jni.cpp
static const struct JNINativeInterfaceMod gNativeInterface = {
    NULL,
    NULL,
    NULL,
    NULL,
    GetVersion,
    DefineClass,
    FindClass,
    FromReflectedMethod,
    FromReflectedField,
    ToReflectedMethod,
    GetSuperclass,
    IsAssignableFrom,
    ToReflectedField,
    Throw,
    ThrowNew,
    ExceptionOccurred,
    ExceptionDescribe,
    ExceptionClear,
    FatalError,
    PushLocalFrame,
    PopLocalFrame,
    NewGlobalRef,
    DeleteGlobalRef,
    DeleteLocalRef,
    IsSameObject,
    NewLocalRef,
    EnsureLocalCapacity,
    AllocObject,
    NewObject,
    NewObjectV,
    NewObjectA,
    GetObjectClass,
    IsInstanceOf,
    GetMethodID,
    CallObjectMethod,
    CallObjectMethodV,
    CallObjectMethodA,
    CallBooleanMethod,
    CallBooleanMethodV,
    CallBooleanMethodA,
    CallByteMethod,
    CallByteMethodV,
    CallByteMethodA,
    CallCharMethod,
    CallCharMethodV,
    CallCharMethodA,
    CallShortMethod,
    CallShortMethodV,
    CallShortMethodA,
    CallIntMethod,
    CallIntMethodV,
    CallIntMethodA,
    CallLongMethod,
    CallLongMethodV,
    CallLongMethodA,
    CallFloatMethod,
    CallFloatMethodV,
    CallFloatMethodA,
    CallDoubleMethod,
    CallDoubleMethodV,
    CallDoubleMethodA,
    CallVoidMethod,
    CallVoidMethodV,
    CallVoidMethodA,
    CallNonvirtualObjectMethod,
    CallNonvirtualObjectMethodV,
    CallNonvirtualObjectMethodA,
    CallNonvirtualBooleanMethod,
    CallNonvirtualBooleanMethodV,
    CallNonvirtualBooleanMethodA,
    CallNonvirtualByteMethod,
    CallNonvirtualByteMethodV,
    CallNonvirtualByteMethodA,
    CallNonvirtualCharMethod,
    CallNonvirtualCharMethodV,
    CallNonvirtualCharMethodA,
    CallNonvirtualShortMethod,
    CallNonvirtualShortMethodV,
    CallNonvirtualShortMethodA,
    CallNonvirtualIntMethod,
    CallNonvirtualIntMethodV,
    CallNonvirtualIntMethodA,
    CallNonvirtualLongMethod,
    CallNonvirtualLongMethodV,
    CallNonvirtualLongMethodA,
    CallNonvirtualFloatMethod,
    CallNonvirtualFloatMethodV,
    CallNonvirtualFloatMethodA,
    CallNonvirtualDoubleMethod,
    CallNonvirtualDoubleMethodV,
    CallNonvirtualDoubleMethodA,
    CallNonvirtualVoidMethod,
    CallNonvirtualVoidMethodV,
    CallNonvirtualVoidMethodA,
    GetFieldID,
    GetObjectField,
    GetBooleanField,
    GetByteField,
    GetCharField,
    GetShortField,
    GetIntField,
    GetLongField,
    GetFloatField,
    GetDoubleField,
    SetObjectField,
    SetBooleanField,
    SetByteField,
    SetCharField,
    SetShortField,
    SetIntField,
    SetLongField,
    SetFloatField,
    SetDoubleField,
    GetStaticMethodID,
    CallStaticObjectMethod,
    CallStaticObjectMethodV,
    CallStaticObjectMethodA,
    CallStaticBooleanMethod,
    CallStaticBooleanMethodV,
    CallStaticBooleanMethodA,
    CallStaticByteMethod,
    CallStaticByteMethodV,
    CallStaticByteMethodA,
    CallStaticCharMethod,
    CallStaticCharMethodV,
    CallStaticCharMethodA,
    CallStaticShortMethod,
    CallStaticShortMethodV,
    CallStaticShortMethodA,
    CallStaticIntMethod,
    CallStaticIntMethodV,
    CallStaticIntMethodA,
    CallStaticLongMethod,
    CallStaticLongMethodV,
    CallStaticLongMethodA,
    CallStaticFloatMethod,
    CallStaticFloatMethodV,
    CallStaticFloatMethodA,
    CallStaticDoubleMethod,
    CallStaticDoubleMethodV,
    CallStaticDoubleMethodA,
    CallStaticVoidMethod,
    CallStaticVoidMethodV,
    CallStaticVoidMethodA,
    GetStaticFieldID,
    GetStaticObjectField,
    GetStaticBooleanField,
    GetStaticByteField,
    GetStaticCharField,
    GetStaticShortField,
    GetStaticIntField,
    GetStaticLongField,
    GetStaticFloatField,
    GetStaticDoubleField,
    SetStaticObjectField,
    SetStaticBooleanField,
    SetStaticByteField,
    SetStaticCharField,
    SetStaticShortField,
    SetStaticIntField,
    SetStaticLongField,
    SetStaticFloatField,
    SetStaticDoubleField,
    NewString,
    GetStringLength,
    GetStringChars,
    ReleaseStringChars,
    NewStringUTF,
    GetStringUTFLength,
    GetStringUTFChars,
    ReleaseStringUTFChars,
    GetArrayLength,
    NewObjectArray,
    GetObjectArrayElement,
    SetObjectArrayElement,
    NewBooleanArray,
    NewByteArray,
    NewCharArray,
    NewShortArray,
    NewIntArray,
    NewLongArray,
    NewFloatArray,
    NewDoubleArray,
    GetBooleanArrayElements,
    GetByteArrayElements,
    GetCharArrayElements,
    GetShortArrayElements,
    GetIntArrayElements,
    GetLongArrayElements,
    GetFloatArrayElements,
    GetDoubleArrayElements,
    ReleaseBooleanArrayElements,
    ReleaseByteArrayElements,
    ReleaseCharArrayElements,
    ReleaseShortArrayElements,
    ReleaseIntArrayElements,
    ReleaseLongArrayElements,
    ReleaseFloatArrayElements,
    ReleaseDoubleArrayElements,
    GetBooleanArrayRegion,
    GetByteArrayRegion,
    GetCharArrayRegion,
    GetShortArrayRegion,
    GetIntArrayRegion,
    GetLongArrayRegion,
    GetFloatArrayRegion,
    GetDoubleArrayRegion,
    SetBooleanArrayRegion,
    SetByteArrayRegion,
    SetCharArrayRegion,
    SetShortArrayRegion,
    SetIntArrayRegion,
    SetLongArrayRegion,
    SetFloatArrayRegion,
    SetDoubleArrayRegion,
    RegisterNatives,
    UnregisterNatives,
    MonitorEnter,
    MonitorExit,
    GetJavaVM, //TODO: full implementation
    GetStringRegion,
    GetStringUTFRegion,
    GetPrimitiveArrayCritical,
    ReleasePrimitiveArrayCritical,
    GetStringCritical,
    ReleaseStringCritical,
    NewWeakGlobalRef,
    DeleteWeakGlobalRef,
    ExceptionCheck,
    NewDirectByteBuffer,
    GetDirectBufferAddress,
    GetDirectBufferCapacity,
    GetObjectRefType,

    // following fields are initializers for
    // the tainted JNI method calls
    // as they should not be usable by native code
    // outside the dalvik process, we initialize
    // them with NULL
    NULL, // GetArrayType

    NULL, // NewTaintedObjectA,
    
	NULL, // CallObjectTaintedMethodA,
	NULL, // CallBooleanTaintedMethodA,
	NULL, // CallByteTaintedMethodA,
	NULL, // CallCharTaintedMethodA,
	NULL, // CallShortTaintedMethodA,
	NULL, // CallIntTaintedMethodA,
	NULL, // CallLongTaintedMethodA,
	NULL, // CallFloatTaintedMethodA,
	NULL, // CallDoubleTaintedMethodA,
	NULL, // CallVoidTaintedMethodA,

	NULL, // CallNonvirtualObjectTaintedMethodA,
	NULL, // CallNonvirtualBooleanTaintedMethodA,
	NULL, // CallNonvirtualByteTaintedMethodA,
	NULL, // CallNonvirtualCharTaintedMethodA,
	NULL, // CallNonvirtualShortTaintedMethodA,
	NULL, // CallNonvirtualIntTaintedMethodA,
	NULL, // CallNonvirtualLongTaintedMethodA,
	NULL, // CallNonvirtualFloatTaintedMethodA,
	NULL, // CallNonvirtualDoubleTaintedMethodA,
	NULL, // CallNonvirtualVoidTaintedMethodA,

    NULL, // GetObjectTaintedField,
	NULL, // GetBooleanTaintedField,
	NULL, // GetByteTaintedField,
	NULL, // GetCharTaintedField,
	NULL, // GetShortTaintedField,
	NULL, // GetIntTaintedField,
	NULL, // GetLongTaintedField,
	NULL, // GetFloatTaintedField,
	NULL, // GetDoubleTaintedField,

	NULL, // SetObjectTaintedField,
	NULL, // SetBooleanTaintedField,
	NULL, // SetByteTaintedField,
	NULL, // SetCharTaintedField,
	NULL, // SetShortTaintedField,
	NULL, // SetIntTaintedField,
	NULL, // SetLongTaintedField,
	NULL, // SetFloatTaintedField,
	NULL, // SetDoubleTaintedField,

	NULL, // CallStaticObjectTaintedMethodA,
	NULL, // CallStaticBooleanTaintedMethodA,
	NULL, // CallStaticByteTaintedMethodA,
	NULL, // CallStaticCharTaintedMethodA,
	NULL, // CallStaticShortTaintedMethodA,
	NULL, // CallStaticIntTaintedMethodA,
	NULL, // CallStaticLongTaintedMethodA,
	NULL, // CallStaticFloatTaintedMethodA,
	NULL, // CallStaticDoubleTaintedMethodA,
	NULL, // CallStaticVoidTaintedMethodA,

	NULL, // GetStaticObjectTaintedField,
	NULL, // GetStaticBooleanTaintedField,
	NULL, // GetStaticByteTaintedField,
	NULL, // GetStaticCharTaintedField,
	NULL, // GetStaticShortTaintedField,
	NULL, // GetStaticIntTaintedField,
	NULL, // GetStaticLongTaintedField,
	NULL, // GetStaticFloatTaintedField,
	NULL, // GetStaticDoubleTaintedField,

	NULL, // SetStaticObjectTaintedField,
	NULL, // SetStaticBooleanTaintedField,
	NULL, // SetStaticByteTaintedField,
	NULL, // SetStaticCharTaintedField,
	NULL, // SetStaticShortTaintedField,
	NULL, // SetStaticIntTaintedField,
	NULL, // SetStaticLongTaintedField,
	NULL, // SetStaticFloatTaintedField,
	NULL, // SetStaticDoubleTaintedField
    
    NULL, // NewTaintedStringUTF,
	NULL  // GetTaintedStringUTFChars,

};

JNIEnvMod* dvmCreateJNIEnvMod() {
    JNIEnvModExt* newEnv = (JNIEnvModExt*) calloc(1, sizeof(JNIEnvModExt));
    newEnv->funcTable = &gNativeInterface;

    //create Execution Manager
    newEnv->execManager = new ExecutionManager();
    return (JNIEnvMod*)newEnv;
}

JavaVM* wrCreateJavaVM() {
	JavaVM* vm = (JavaVM*) malloc(sizeof(JavaVM));
	vm->functions = &gInvokeInterface;
	return vm;
}

}

//#endif
