//#ifdef WITH_DALVIK_BINDER_SERVICE

#include <JniEnvMod.h>
#include <stddef.h> // for NULL
#include <utils/Log.h>
#include "ExecutionManager.h"

#define LOG_TAG "JNI Remote"

namespace android {

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
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	ext->execManager->jniCall.function = 3;
	ext->execManager->jniCall.length = 0;
	ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	return result;
}

//code 4
static jclass FindClass(JNIEnvMod* env, const char* name) {
	ALOGD("jniEnvMod->FindClass(env=%08x, name=%s)", (int)env, name);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = strlen(name)+1;
    ext->execManager->jniCall.function = 4;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = name;
    ext->execManager->reqJniCall();
    int* intResult = (int*)(ext->execManager->jniCall.param_data);
    jclass resultClass = (jclass)(*intResult);
    ALOGD("returning %08x", (int)resultClass);
    return resultClass;
}

//code 64
static jmethodID FromReflectedMethod(JNIEnvMod* env, jobject jmethod) {
	ALOGD("jniEnvMod->FromReflectedMethod(env=%08x, jmethod=%08x)", (int)env, (int)jmethod);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jmethod);
    ext->execManager->jniCall.function = 64;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jmethod;
    ext->execManager->reqJniCall();
	return *((jmethodID*)(ext->execManager->jniCall.param_data));
}

//code 58
static jfieldID FromReflectedField(JNIEnvMod* env, jobject jfield) {
	ALOGD("jniEnvMod->FromReflectedField(env=%08x, jfield=%08x)", (int)env, (int)jfield);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jfield);
    ext->execManager->jniCall.function = 58;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jfield;
    ext->execManager->reqJniCall();
	return *((jfieldID*)(ext->execManager->jniCall.param_data));
}

//code 61
static jobject ToReflectedMethod(JNIEnvMod* env, jclass jcls, jmethodID methodID, jboolean isStatic) {
	ALOGD("jniEnvMod->ToReflectedMethod(env=%08x, jclass=%08x, methodID=%08x)",
		(int)env, (int)jcls, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jcls) + sizeof(methodID);
	void* data = malloc(size);
	memcpy(data, &jcls, sizeof(jcls));
	memcpy(data+sizeof(jcls), &methodID, sizeof(methodID));
    ext->execManager->jniCall.function = 61;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = *((jobject*)(ext->execManager->jniCall.param_data));
	free(data);
	return result;
}

static jobject ToReflectedField(JNIEnvMod* env, jclass jcls, jfieldID fieldID, jboolean isStatic) {
	ALOGD("jniEnvMod->ToReflectedField(env=%08x, jclass=%08x, fieldID=%08x)",
		(int)env, (int)jcls, (int)fieldID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jcls) + sizeof(fieldID);
	void* data = malloc(size);
	memcpy(data, &jcls, sizeof(jcls));
	memcpy(data+sizeof(jcls), &fieldID, sizeof(fieldID));
    ext->execManager->jniCall.function = 61;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = *((jobject*)(ext->execManager->jniCall.param_data));
	free(data);
	return result;
}

//code 5
static jclass GetSuperclass(JNIEnvMod* env, jclass jclazz) {
	ALOGD("jniEnvMod->GetSuperclass(env=%08x, jclazz=%08x)", (int)env, (int)jclazz);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jclazz);
	//void* data = malloc(size);
	//memcpy(data, &jclazz, size);
    ext->execManager->jniCall.function = 5;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jclazz;
    ext->execManager->reqJniCall();
	jclass resultClass = (jclass)*(int*)(ext->execManager->jniCall.param_data);
	//free(data);
    ALOGD("returning %08x", (int)resultClass);
    return resultClass;
}

//code6
static jboolean IsAssignableFrom(JNIEnvMod* env, jclass jclazz1, jclass jclazz2) {
	ALOGD("jniEnvMod->IsAssignableFrom(env=%08x, jclazz1=%08x, jclazz2=%08x)", (int)env, (int)jclazz1, (int)jclazz2);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = 2*sizeof(jclazz1);
	void* data = malloc(size);
	memcpy(data, &jclazz1, sizeof(jclazz1));
	memcpy(data+sizeof(jclazz1), &jclazz2, sizeof(jclazz2));
    ext->execManager->jniCall.function = 6;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	ALOGD("returning %08x", (int)result);
	return result;
}

//code8
static jint Throw(JNIEnvMod* env, jthrowable jt) {
	ALOGD("jniEnvMod->Throw(env=%08x, jthrowable=%08x)", (int)env, (int)jt);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jt);
    ext->execManager->jniCall.function = 8;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jt;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	ALOGD("returning %08x", (int)result);
	return result;
}

//code 9
static jint ThrowNew(JNIEnvMod* env, jclass jclazz, const char* message) {
	ALOGD("jniEnvMod->ThrowNew(env=%08x, jclazz=%08x, message='%s')", (int)env, (int)jclazz, message);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jclazz)+sizeof(char)*(strlen(message)+1);
	void* data = malloc(size);
	memcpy(data, &jclazz, sizeof(jclazz));
	memcpy(data+sizeof(jclazz), message, sizeof(char)*(strlen(message)+1));
    ext->execManager->jniCall.function = 9;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	ALOGD("returning %08x", (int)result);
	return result;
}

//code 10
static jthrowable ExceptionOccurred(JNIEnvMod* env) {
	ALOGD("jniEnvMod->ExceptionOccurred(env=%08x)", (int)env);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 10;
    ext->execManager->jniCall.length = 0;
    ext->execManager->jniCall.param_data = 0;
    ext->execManager->reqJniCall();
	jthrowable result = (jthrowable)*(int*)(ext->execManager->jniCall.param_data);
	ALOGD("returning %08x", (int)result);
	return result;
}

//code 11
static void ExceptionDescribe(JNIEnvMod* env) {
	ALOGD("jniEnvMod->ExceptionDescribe(env=%08x)", (int)env);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 11;
    ext->execManager->jniCall.length = 0;
    ext->execManager->jniCall.param_data = 0;
    ext->execManager->reqJniCall();
	ALOGD("returning after ExceptionDescribe");
}	

//code 12
static void ExceptionClear(JNIEnvMod* env) {
	ALOGD("jniEnvMod->ExceptionDescribe(env=%08x)", (int)env);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
    ext->execManager->jniCall.function = 12;
    ext->execManager->jniCall.length = 0;
    ext->execManager->jniCall.param_data = 0;
    ext->execManager->reqJniCall();
	ALOGD("returning after ExceptionClear");
}	

//code 21
static jobject NewGlobalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->NewGlobalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj);
	void* data = malloc(size);
	memcpy(data, &jobj, size);
    ext->execManager->jniCall.function = 21;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 22
static void DeleteGlobalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->DeleteGlobalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj);
	void* data = malloc(size);
	memcpy(data, &jobj, size);
    ext->execManager->jniCall.function = 22;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 32
static void DeleteLocalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->DeleteLocalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj);
	void* data = malloc(size);
	memcpy(data, &jobj, size);
    ext->execManager->jniCall.function = 32;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 23
static jboolean IsSameObject(JNIEnvMod* env, jobject jobj1, jobject jobj2) {
	ALOGD("jniEnvMod->IsSameObject(env=%08x, jobj1=%08x, jobj2=%08x)", (int)env, (int)jobj1, (int)jobj2);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = 2*sizeof(jobj1);
	void* data = malloc(size);
	memcpy(data, &jobj1, sizeof(jobj1));
	memcpy(data+sizeof(jobj1), &jobj1, sizeof(jobj1));
    ext->execManager->jniCall.function = 23;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 20
static jobject NewLocalRef(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->NewLocalRef(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj);
	void* data = malloc(size);
	memcpy(data, &jobj, size);
    ext->execManager->jniCall.function = 20;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 7
static jobject AllocObject(JNIEnvMod* env, jclass jclazz) {
	ALOGD("jniEnvMod->AllocObject(env=%08x, jclazz=%08x)", (int)env, (int)jclazz);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jclazz);
	void* data = malloc(size);
	memcpy(data, &jclazz, size);
    ext->execManager->jniCall.function = 7;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	ALOGD("returning %08x", (int)result);
	return result;
}

#define CALL_COPY() \
	JNIEnvModExt* ext = (JNIEnvModExt*)env; \
	int paramSize = 8*ext->execManager->getNumParams(methodID); \
	ALOGD("paramSize=%d", paramSize); \
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize; \
	void* data = malloc(size); \
	memcpy(data, &jobj, sizeof(jobj)); \
	void* d2 = data + sizeof(jobj); \
	memcpy(d2, &methodID, sizeof(methodID)); \
	d2 += sizeof(methodID); \
	const char* sig = ext->execManager->getSignatureForMethod(methodID);

#define NEW_COPY() \
	JNIEnvModExt* ext = (JNIEnvModExt*)env; \
	int paramSize = 8*ext->execManager->getNumParams(methodID); \
	ALOGD("paramSize=%d", paramSize); \
	int size = sizeof(jclass) + sizeof(jmethodID) + paramSize; \
	void* data = malloc(size); \
	memcpy(data, &jclazz, sizeof(jclazz)); \
	void* d2 = data + sizeof(jclazz); \
	memcpy(d2, &methodID, sizeof(methodID)); \
	d2 += sizeof(methodID); \
	const char* sig = ext->execManager->getSignatureForMethod(methodID);

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
			case 'D': { \
				v.d = va_arg(args, jdouble); \
				memcpy(d2, &v, sizeof(v)); \
				ALOGD("copying jdouble %f", v.d); \
				d2 += sizeof(v); \
				break; } \
			case 'J': { \
				v.j = va_arg(args, jlong); \
				memcpy(d2, &v, sizeof(v)); \
				ALOGD("copying jlong %lld", v.j); \
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
    ext->execManager->jniCall.function = 59;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 59
static jobject NewObjectV(JNIEnvMod* env, jclass jclazz, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->NewObject(env=%08x, jclazz=%08x, method=%08x, va_list)", (int)env, (int)jclazz, (int)methodID);
	NEW_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 59;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 59
static jobject NewObjectA(JNIEnvMod* env, jclass jclazz, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->NewObjectA(env=%08x, jclazz=%08x, method=%08x, args=%08x)",
		(int)env, (int)jclazz, (int)methodID, (int)args);
	NEW_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 59;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 18
static jclass GetObjectClass(JNIEnvMod* env, jobject jobj) {
	ALOGD("jniEnvMod->GetObjectClass(env=%08x, jobj=%08x)", (int)env, (int)jobj);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj);
	void* data = malloc(size);
	memcpy(data, &jobj, size);
	ALOGD("param_data=%08x", (int)(int*)data);
    ext->execManager->jniCall.function = 18;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jclass result = (jclass)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 19
static jboolean IsInstanceOf(JNIEnvMod* env, jobject jobj, jclass jclazz) {
	ALOGD("jniEnvMod->IsInstanceOf(env=%08x, jobj=%08x, jclazz=%08x)", (int)env, (int)jobj, (int)jclazz);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jobj) + sizeof(jclazz);
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	memcpy(data+sizeof(jobj), &jclazz, sizeof(jclazz));
    ext->execManager->jniCall.function = 19;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 14
static jmethodID GetMethodID(JNIEnvMod* env, jclass jclazz, const char* name, const char* sig) {
	ALOGD("jniEnvMod->GetMethodID(env=%08x, jclazz=%08x, name=%s, sig=%s)", (int)env, (int)jclazz, name, sig);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jclazz) + sizeof(char)*(strlen(name)+1) + sizeof(char)*(strlen(sig)+1);
	void* data = malloc(size);
	memcpy(data, &jclazz, sizeof(jclazz));
	void* d2 = data+sizeof(jclazz);
	memcpy(d2, name, sizeof(char)*(strlen(name)+1));
	d2 += sizeof(char)*(strlen(name)+1);
	memcpy(d2, sig, sizeof(char)*(strlen(sig)+1));
    ext->execManager->jniCall.function = 14;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jmethodID result = (jmethodID)*(int*)(ext->execManager->jniCall.param_data);
	if (result != 0) ext->execManager->addSignatureForMethod(result, sig);
	free(data);
	return result;
}

//code 33
static jobject CallObjectMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 33;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 33
static jobject CallObjectMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 33;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 33
static jobject CallObjectMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 33;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jobject result = (jobject)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 24
static jboolean CallBooleanMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 24;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 25
static jboolean CallBooleanMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallBooleanMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 25;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 26
static jboolean CallBooleanMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallBooleanMethod(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 26;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jboolean result = (jboolean)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 71
static jbyte CallByteMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallByteMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args)",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 71;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = *((jbyte*)(ext->execManager->jniCall.param_data));
	free(data);
	return result;
}

//code 71
static jbyte CallByteMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallByteMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 71;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 71
static jbyte CallByteMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallByteMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 71;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jbyte result = (jbyte)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 72
static jchar CallCharMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallCharMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args)",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 72;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = *((jchar*)(ext->execManager->jniCall.param_data));
	free(data);
	return result;
}

//code 72
static jchar CallCharMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallCharMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 72;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 72
static jchar CallCharMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallCharMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x)",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 72;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jchar result = (jchar)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 84
static jshort CallShortMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallShortMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 84;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = *((jshort*)(ext->execManager->jniCall.param_data));
	free(data);
	return result;
}

//code 84
static jshort CallShortMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallShortMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 84;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 84
static jshort CallShortMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallShortMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 84;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jshort result = (jshort)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 27
static jint CallIntMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallIntMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 27;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 28
static jint CallIntMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallIntMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 28;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 29
static jint CallIntMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallIntMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 29;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jint result = (jint)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 73
static jlong CallLongMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallLongMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 73;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = *((jlong*)(ext->execManager->jniCall.param_data));
	free(data);
	return result;
}

//code 73
static jlong CallLongMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallLongMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 73;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 73
static jlong CallLongMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallLongMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 73;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jlong result = (jlong)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 62
static jfloat CallFloatMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallFloatMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 62;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = *((jfloat*)(ext->execManager->jniCall.param_data));
	free(data);
	return result;
}

//code 62
static jfloat CallFloatMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallFloatMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 62;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 62
static jfloat CallFloatMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallFloatMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 62;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jfloat result = (jfloat)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 74
static jdouble CallDoubleMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallDoubleMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 74;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = *((jdouble*)(ext->execManager->jniCall.param_data));
	free(data);
	return result;
}

//code 74
static jdouble CallDoubleMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallDoubleMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 74;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 74
static jdouble CallDoubleMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallDoubleMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 74;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	jdouble result = (jdouble)*(int*)(ext->execManager->jniCall.param_data);
	free(data);
	return result;
}

//code 34
static void CallVoidMethod(JNIEnvMod* env, jobject jobj, jmethodID methodID, ...) {
	ALOGD("jniEnvMod->CallIntMethod(env=%08x, jobject=%08x, methodID=%08x, and some var args",
		(int)env, (int)jobj, (int)methodID);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	va_list args;
	va_start(args, methodID);
	int firstParam = va_arg(args, int);
	int paramSize = ext->execManager->getMethodParamSize(methodID);
	int size = sizeof(jobject) + sizeof(jmethodID) + sizeof(paramSize) + paramSize;
	void* data = malloc(size);
	memcpy(data, &jobj, sizeof(jobj));
	void* d2 = data + sizeof(jobj);
	memcpy(d2, &methodID, sizeof(methodID));
	d2 += sizeof(methodID);
	memcpy(d2, &paramSize, sizeof(paramSize));
	d2 += sizeof(paramSize);
	memcpy(d2, &firstParam, paramSize);
	va_end(args);
    ext->execManager->jniCall.function = 34;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 34
static void CallVoidMethodV(JNIEnvMod* env, jobject jobj, jmethodID methodID, va_list args) {
	ALOGD("jniEnvMod->CallVoidMethodV(env=%08x, jobject=%08x, methodID=%08x, va_list=%08x)",
		(int)env, (int)jobj, (int)methodID, args);
	CALL_COPY();
	PARAMS_FROM_ARGS();
    ext->execManager->jniCall.function = 34;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code 34
static void CallVoidMethodA(JNIEnvMod* env, jobject jobj, jmethodID methodID, jvalue* args) {
	ALOGD("jniEnvMod->CallVoidMethodA(env=%08x, jobject=%08x, methodID=%08x, jvalue*=%08x",
		(int)env, (int)jobj, (int)methodID, (int)args);
	CALL_COPY();
	memcpy(d2, args, paramSize);
    ext->execManager->jniCall.function = 34;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
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

//code 60
static void SetStaticLongField(JNIEnvMod* env, jclass jc, jfieldID fieldID, jlong val) {
	ALOGD("jniEnvMod->SetIntField(env=%08x, jclass=%08x, fieldID=%08x, val=%08x)", (int)env, (int)jc, (int)fieldID, (int)val);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jc) + sizeof(fieldID) + sizeof(jlong);
	void* data = malloc(size);
	memcpy(data, &jc, sizeof(jc));
	memcpy(data+sizeof(jc), &fieldID, sizeof(fieldID));
	memcpy(data+sizeof(jc)+sizeof(fieldID), &val, sizeof(val));
    ext->execManager->jniCall.function = 60;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->jniCall.taintsize = 0;
    ext->execManager->reqJniCall();
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

//code 1
static const char* GetStringUTFChars(JNIEnvMod* env, jstring jstr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetStringUTFChars(env=%08x, jstr=%08x, isCopy=%08x)",
		(int)env, (int)jstr, (int)isCopy);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	ALOGD("JniEnv->execManager=%p with status %s",
		ext->execManager, ExecutionManager::strStatus(ext->execManager->mStatus));
	int size = sizeof(jstring)*sizeof(char), sizeb = sizeof(jboolean)*sizeof(char);
	void* data = malloc(size+sizeb); // freed after reqJniCall()
	memcpy(data, &jstr, size);
	if (isCopy != NULL) memcpy(data+size, isCopy, sizeb);
	else memset(data+size, 0, sizeb);
	ALOGD("jstr=%p, iscopy=%d", jstr, *isCopy);
	ext->execManager->jniCall.function = 1;
	ext->execManager->jniCall.param_data = data;
	ext->execManager->jniCall.length = size+sizeb;
	ext->execManager->reqJniCall();
	free(data);
	
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

//code 69
static void DeleteWeakGlobalRef(JNIEnvMod* env, jweak jw) {
	ALOGD("jniEnvMod->DeleteGlobalRef(env=%08x, jweak=%08x)", (int)env, (int)jw);
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

//code 70
static jbyte* GetByteArrayElements(JNIEnvMod* env, jbyteArray jarr, jboolean* isCopy) {
	ALOGD("jniEnvMod->GetByteArrayElements(env=%08x, jarr=%08x, isCopy)", (int)env, (int)jarr);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr);
    ext->execManager->jniCall.function = 70;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = &jarr;
    ext->execManager->reqJniCall();
	size = *((int*)(ext->execManager->jniCall.param_data));
	int dalvikP = *((int*)(ext->execManager->jniCall.param_data+sizeof(int)));
	ALOGD("GetPrimitiveArrayCritical received size=%d, dalvikP=%08x", size, dalvikP);
	ext->execManager->addArray(jarr, size, dalvikP);
	void* result = malloc(size);
	memcpy(result, ext->execManager->jniCall.param_data+sizeof(int)+sizeof(dalvikP), size);
	return (jbyte*)result;
}


//code 54
static void GetByteArrayRegion(JNIEnvMod* env, jbyteArray jarr, jsize start, jsize len, jbyte* buf) {
	ALOGD("jniEnvMod->GetByteArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr) + 2*sizeof(jsize);
	void* data = malloc(size);
	memcpy(data, &jarr, sizeof(jarr));
	void* d2 = data + sizeof(jarr);
	memcpy(d2, &start, sizeof(start));
	d2 += sizeof(start);
	memcpy(d2, &len, sizeof(len));
    ext->execManager->jniCall.function = 54;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	memcpy(buf, ext->execManager->jniCall.param_data, sizeof(jbyte)*len);
	free(data);
}

//code 53
static void SetShortArrayRegion(JNIEnvMod* env, jshortArray jarr, jsize start, jsize len, const jshort* buf) {
	ALOGD("jniEnvMod->SetShortArrayRegion(env=%08x, jarr=%08x, start=%08x, len=%08x, buf=%08x)",
		(int)env, (int)jarr, start, len, (int) buf);
	JNIEnvModExt* ext = (JNIEnvModExt*)env;
	int size = sizeof(jarr) + 2*sizeof(jsize) + len*sizeof(*buf);
	void* data = malloc(size);
	memcpy(data, &jarr, sizeof(jarr));
	void* d2 = data + sizeof(jarr);
	memcpy(d2, &start, sizeof(start));
	d2 += sizeof(start);
	memcpy(d2, &len, sizeof(len));
	d2 += sizeof(len);
	//ALOGD("buf[0]=%08x, buf[1]=%08x, buf[2]=%08x", buf[0], buf[1], buf[2]);
	memcpy(d2, buf, len*sizeof(jshort));
    ext->execManager->jniCall.function = 53;
    ext->execManager->jniCall.length = size;
    ext->execManager->jniCall.param_data = data;
    ext->execManager->reqJniCall();
	free(data);
}

//code ?
static void GetStringUTFRegion(JNIEnvMod* env, jstring jstr, jsize start, jsize len, char* buf) {
	ALOGD("GetStringUTFRegion");
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
	ALOGD("address of length: %08x", &(at->length));
	ALOGD("address of dalvikP: %08x", &(at->dalvikP));
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


//function table just like in dalvik/vm/Jni.cpp
static const struct JNINativeInterfaceMod gNativeInterface = {
    NULL,
    NULL,
    NULL,
    NULL,
    GetVersion,
    NULL, //DefineClass,
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
    NULL, //FatalError,
    NULL, //PushLocalFrame,
    NULL, //PopLocalFrame,
    NewGlobalRef,
    DeleteGlobalRef,
    DeleteLocalRef,
    IsSameObject,
    NewLocalRef,
    NULL, //EnsureLocalCapacity,
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
    NULL, //CallNonvirtualVoidMethod,
    NULL, //CallNonvirtualVoidMethodV,
    NULL, //CallNonvirtualVoidMethodA,
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
    NULL, //GetStaticFieldID,
    NULL, //GetStaticObjectField,
    NULL, //GetStaticBooleanField,
    NULL, //GetStaticByteField,
    NULL, //GetStaticCharField,
    NULL, //GetStaticShortField,
    NULL, //GetStaticIntField,
    NULL, //GetStaticLongField,
    NULL, //GetStaticFloatField,
    NULL, //GetStaticDoubleField,
    NULL, //SetStaticObjectField,
    NULL, //SetStaticBooleanField,
    NULL, //SetStaticByteField,
    NULL, //SetStaticCharField,
    NULL, //SetStaticShortField,
    NULL, //SetStaticIntField,
    SetStaticLongField,
    NULL, //SetStaticFloatField,
    NULL, //SetStaticDoubleField,
    NULL, //NewString,
    NULL, //GetStringLength,
    GetStringChars,
    ReleaseStringChars,
    NewStringUTF, //2: NewStringUTF,
    NULL, //GetStringUTFLength,
    GetStringUTFChars, //1:GetStringUTFChars,
    ReleaseStringUTFChars,
    NULL, //GetArrayLength,
    NULL, //NewObjectArray,
    NULL, //GetObjectArrayElement,
    NULL, //SetObjectArrayElement,
    NULL, //NewBooleanArray,
    NULL, //NewByteArray,
    NULL, //NewCharArray,
    NULL, //NewShortArray,
    NULL, //NewIntArray,
    NULL, //NewLongArray,
    NULL, //NewFloatArray,
    NULL, //NewDoubleArray,
    NULL, //GetBooleanArrayElements,
    GetByteArrayElements,
    NULL, //GetCharArrayElements,
    NULL, //GetShortArrayElements,
    NULL, //GetIntArrayElements,
    NULL, //GetLongArrayElements,
    NULL, //GetFloatArrayElements,
    NULL, //GetDoubleArrayElements,
    NULL, //ReleaseBooleanArrayElements,
    NULL, //ReleaseByteArrayElements,
    NULL, //ReleaseCharArrayElements,
    NULL, //ReleaseShortArrayElements,
    NULL, //ReleaseIntArrayElements,
    NULL, //ReleaseLongArrayElements,
    NULL, //ReleaseFloatArrayElements,
    NULL, //ReleaseDoubleArrayElements,
    NULL, //GetBooleanArrayRegion,
    GetByteArrayRegion,
    NULL, //GetCharArrayRegion,
    NULL, //GetShortArrayRegion,
    NULL, //GetIntArrayRegion,
    NULL, //GetLongArrayRegion,
    NULL, //GetFloatArrayRegion,
    NULL, //GetDoubleArrayRegion,
    NULL, //SetBooleanArrayRegion,
    NULL, //SetByteArrayRegion,
    NULL, //SetCharArrayRegion,
    SetShortArrayRegion,
    NULL, //SetIntArrayRegion,
    NULL, //SetLongArrayRegion,
    NULL, //SetFloatArrayRegion,
    NULL, //SetDoubleArrayRegion,
    NULL, //RegisterNatives,
    NULL, //UnregisterNatives,
    NULL, //MonitorEnter,
    NULL, //MonitorExit,
    NULL, //GetJavaVM,
    NULL, //GetStringRegion,
    GetStringUTFRegion,
    GetPrimitiveArrayCritical,
    ReleasePrimitiveArrayCritical,
    GetStringCritical,
    ReleaseStringCritical,
    NULL, //NewWeakGlobalRef,
    DeleteWeakGlobalRef,
    ExceptionCheck,
    NULL, //NewDirectByteBuffer,
    NULL, //GetDirectBufferAddress,
    NULL, //GetDirectBufferCapacity,
    NULL, //GetObjectRefType

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

}

//#endif
