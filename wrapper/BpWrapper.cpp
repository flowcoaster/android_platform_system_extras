#include <binder/Parcel.h>
#include <BpWrapper.h>
#include <IWrapper.h>
#include "utils.h"
#include "JniEnvMod.h"

#define LOG_TAG "BpWrapper=Client"

namespace android{

	void setResult(JValTaint* res, Parcel* reply) {
	    status_t status = reply->readInt64(&res->val.j);
	    int32_t taint = reply->readInt32();
	    res->taint = taint;
	    ALOGD("setResult(value=%d, taint=%d)", res->val.i, taint);
	}

	//blueprint
	//unpack data from void* replydata
	//call jniEnv->function
	//malloc void* callbackdata
	//copy result into callbackdata

	void BpWrapper::callGetStringUTFChars() {
		jstring jstr = *((jstring*)replydata);
		jboolean isCopy = *((jboolean*)(replydata+(sizeof(jstr)*sizeof(char))));
		ALOGD("jstr=%p, isCopy=%d", jstr, isCopy);
		const char* chars = jniEnv->GetStringUTFChars(jstr, &isCopy);
		ALOGD("GetStringUTFChars returns %s; sizeof(*chars)=%d; strlen(chars)=%d",
			chars, sizeof(*chars), strlen(chars));
		size = sizeof(*chars)*strlen(chars)+1;
		taintsize = 0;
		callbackdata = malloc(size); //freed by CALLBACK
		memcpy(callbackdata, chars, size);
	}

	void BpWrapper::callNewStringUTF() {
		*((char*)(replydata+replylength-1)) = '\0';
		const char* chars = (char*)replydata;
		ALOGD("read: %s", chars);
		jstring result = jniEnv->NewStringUTF(chars);
		size = sizeof(jstring);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("NewStringUTF->%08x", (int)result);
	}

	void BpWrapper::callGetVersion() {
		jint result = jniEnv->GetVersion();
		size = sizeof(jstring);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("GetVersion->%d", result);
	}

	void BpWrapper::callFindClass() {
		*((char*)(replydata+replylength-1)) = '\0';
		const char* chars = (char*)replydata;
		ALOGD("read: %s", chars);
		jclass result = jniEnv->FindClass(chars);
		size = sizeof(jclass);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("FindClass->%08x", (int)result);
	}

	void BpWrapper::callGetSuperclass() {
		jclass jclazz = *((jclass*)replydata);
		jclass result = jniEnv->GetSuperclass(jclazz);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("GetSuperclass->%08x", (int)result);
	}

	void BpWrapper::callIsAssignableFrom() {
		jclass jclazz1 = *((jclass*)replydata);
		jclass jclazz2 = *((jclass*)(replydata+sizeof(jclass)));
		jboolean result = jniEnv->IsAssignableFrom(jclazz1, jclazz2);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("IsAssignableFrom->%08x", (int)result);
	}

	void BpWrapper::callAllocObject() {
		jclass jclazz = *((jclass*)replydata);
		jobject result = jniEnv->AllocObject(jclazz);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("AllocObject()->%08x", (int)result);
	}

	void BpWrapper::callThrow() {
		jthrowable jt = *((jthrowable*)replydata);
		jint result = jniEnv->Throw(jt);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("Throw()->%08x", (int)result);
	}

	void BpWrapper::callThrowNew() {
		jclass jclazz = *((jclass*)replydata);
		char* message = (char*)(replydata+sizeof(jclazz));
		jint result = jniEnv->ThrowNew(jclazz, message);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("ThrowNew()->%08x", (int)result);
	}
	
	void BpWrapper::callExceptionOccurred() {
		jthrowable result = jniEnv->ExceptionOccurred();
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("ExceptionOccurred()->%08x", (int)result);
	}

	void BpWrapper::callExceptionDescribe() {
		jniEnv->ExceptionDescribe();
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
		ALOGD("ExceptionDescribe()->.");
	}

	void BpWrapper::callExceptionClear() {
		jniEnv->ExceptionClear();
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
		ALOGD("ExceptionClear()->.");
	}

	void BpWrapper::callExceptionCheck() {
		jboolean result = jniEnv->ExceptionCheck();
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(result);
		memcpy(callbackdata, &result, size);
		ALOGD("ExceptionCheck()->%08x", (int)result);
	}

	void BpWrapper::callGetMethodID() {
		jclass jclazz = *((jclass*)replydata);
		void* r2 = replydata+sizeof(jclazz);
		char* name = (char*)(r2);
		r2+= sizeof(char)*(strlen(name)+1);
		char* sig = (char*)(r2);
		jmethodID result = jniEnv->GetMethodID(jclazz, name, sig);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("GetMethodID()->%08x", (int)result);
	}

	void BpWrapper::callGetFieldID() {
		jclass jclazz = *((jclass*)replydata);
		void* r2 = replydata+sizeof(jclazz);
		char* name = (char*)(r2);
		r2+= sizeof(char)*(strlen(name)+1);
		char* sig = (char*)(r2);
		jfieldID result = jniEnv->GetFieldID(jclazz, name, sig);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("GetFieldID()->%08x", (int)result);
	}

	void BpWrapper::callGetStaticMethodID() {
		jclass jclazz = *((jclass*)replydata);
		void* r2 = replydata+sizeof(jclazz);
		char* name = (char*)(r2);
		r2+= sizeof(char)*(strlen(name)+1);
		char* sig = (char*)(r2);
		jmethodID result = jniEnv->GetStaticMethodID(jclazz, name, sig);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("GetMethodID()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetObjectField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jobject result = jniEnv->GetObjectTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetObjectField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetBooleanField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jboolean result = jniEnv->GetBooleanTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetBooleanField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetByteField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jbyte result = jniEnv->GetByteTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetByteField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetCharField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jchar result = jniEnv->GetCharTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetCharField()->%c", result);
	}

	//with taint support
	void BpWrapper::callGetShortField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jshort result = jniEnv->GetShortTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetShortField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetIntField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jint result = jniEnv->GetIntTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetIntField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetLongField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jlong result = jniEnv->GetLongTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetLongField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetFloatField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jfloat result = jniEnv->GetFloatTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetFloatField()->%f", result);
	}

	//with taint support
	void BpWrapper::callGetDoubleField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
		jdouble result = jniEnv->GetDoubleTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));
		ALOGD("GetDoubleField()->%f", result);
	}

	void BpWrapper::callSetObjectField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jobject value = *((jobject*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetObjectTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, (int)value, taint);
		jniEnv->SetObjectTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetBooleanField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jboolean value = *((jboolean*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetBooleanTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetBooleanTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetByteField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jbyte value = *((jbyte*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetByteTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetByteTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetCharField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jchar value = *((jchar*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetCharTaintedField: Field %08x to %c with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetCharTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetShortField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jshort value = *((jshort*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetShortTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetShortTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetIntField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jint value = *((jint*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetIntTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetIntTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetLongField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jlong value = *((jlong*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetLongTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, (int)value, taint);
		jniEnv->SetLongTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetFloatField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jfloat value = *((jfloat*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetFloatTaintedField: Field %08x to %f with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetFloatTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetDoubleField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		r2 += sizeof(fieldID);
		jdouble value = *((jdouble*)r2);
		r2 += sizeof(value);
		u4 taint = *((u4*)r2);
		ALOGD("SetDoubleTaintedField: Field %08x to %f with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetDoubleTaintedField(jobj, fieldID, value, taint);
		size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callGetObjectClass() {
		jobject jobj = *((jobject*)replydata);
		jclass result = jniEnv->GetObjectClass(jobj);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("GetObjectClass()->%08x", (int)result);
	}

	void BpWrapper::callIsInstanceOf() {
		jobject jobj = *((jobject*)replydata);
		jclass jclazz = *((jclass*)(replydata+sizeof(jobj)));
		jboolean result = jniEnv->IsInstanceOf(jobj, jclazz);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewLocalRef() {
		jobject jobj = *((jobject*)replydata);
		jobject result = jniEnv->NewLocalRef(jobj);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewGlobalRef() {
		jobject jobj = *((jobject*)replydata);
		jobject result = jniEnv->NewGlobalRef(jobj);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callDeleteGlobalRef() {
		jobject jobj = *((jobject*)replydata);
		jniEnv->DeleteGlobalRef(jobj);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callDeleteLocalRef() {
		jobject jobj = *((jobject*)replydata);
		jniEnv->DeleteGlobalRef(jobj);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callIsSameObject() {
		jobject jobj1 = *((jobject*)replydata);
		jobject jobj2 = *((jobject*)(replydata+sizeof(jobj1)));
		jboolean result = jniEnv->IsSameObject(jobj1, jobj2);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callCallBooleanMethod() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID));
		jboolean result = jniEnv->CallBooleanMethodA(jobj, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callCallBooleanMethodV() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID));
		jboolean result = jniEnv->CallBooleanMethodA(jobj, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callCallBooleanMethodA() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID));
		jboolean result = jniEnv->CallBooleanMethodA(jobj, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callCallIntMethod() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID));
		jint result = jniEnv->CallIntMethodA(jobj, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callCallIntMethodV() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID));
		jint result = jniEnv->CallIntMethodA(jobj, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callCallIntMethodA() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID));
		jint result = jniEnv->CallIntMethodA(jobj, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callCallObjectMethod() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID));
		jobject result = jniEnv->CallObjectMethodA(jobj, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	//with taint support
	void BpWrapper::callCallVoidMethod() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jniEnv->CallVoidTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callCallNonvirtualObjectMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc));
		jobject result = jniEnv->CallNonvirtualObjectMethodA(jobj, jc, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callCallStaticVoidMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		jvalue* args = (jvalue*)(replydata+sizeof(methodID)+sizeof(jc));
		jniEnv->CallStaticVoidMethodA(jc, methodID, args);
		taintsize = size = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callCallStaticShortMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		jvalue* args = (jvalue*)(replydata+sizeof(methodID)+sizeof(jc));
		jshort result = jniEnv->CallStaticShortMethodA(jc, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callSetShortArrayRegion() {
		jshortArray jarr = *((jshortArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		const jshort* buf = (jshort*)(replydata+sizeof(jarr)+sizeof(start)+sizeof(len));
		//ALOGD("buf[0]=%08x, buf[1]=%08x, buf[2]=%08x", buf[0], buf[1], buf[2]);
		//ALOGD("calling SetShortArrayRegion(jarr=%08x, start=%08x, len=%08x, buf=%08x",
		//	jarr, start, len, buf);
		jniEnv->SetShortArrayRegion(jarr, start, len, buf);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callGetByteArrayRegion() {
		jbyteArray jarr = *((jbyteArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		jbyte* buf = (jbyte*)malloc(len*sizeof(jbyte));
		jniEnv->GetByteArrayRegion(jarr, start, len, buf);
		size = len*sizeof(jbyte);
		taintsize = 0;
		callbackdata = buf;
	}

	void BpWrapper::callGetStringChars() {
		jstring jstr = *((jstring*)replydata);
		ALOGD("GetStringChars: jstr=%08x", jstr);
		jboolean isCopy = *((jboolean*)(replydata+sizeof(jstr)));
		ALOGD("GetStringChars: isCopy=%08x", isCopy);
		const jchar* result = jniEnv->GetStringChars(jstr, &isCopy);
		//ALOGD("GetStringChars: %08x -> %s", result, *result);
		ALOGD("GetStringChars: %08x", result);
		int strlen = jniEnv->GetStringLength(jstr);
		size = sizeof(jchar*)+sizeof(int)+strlen*sizeof(jchar);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(jchar*));
		memcpy(callbackdata+sizeof(jchar*), &strlen, sizeof(jchar*));
		memcpy(callbackdata+sizeof(jchar*)+sizeof(strlen), result, size);
	}

	void BpWrapper::callReleaseStringChars() {
		jstring jstr = *((jstring*)replydata);
		jchar* dalvikP = (jchar*)(replydata+sizeof(jstr));
		int strlen = *((int*)(replydata+sizeof(jstr)+sizeof(dalvikP)));
		const jchar* chars = (jchar*)(replydata+sizeof(jstr)+sizeof(dalvikP)+sizeof(strlen));
		memcpy(dalvikP, chars, strlen*sizeof(jchar));
		jniEnv->ReleaseStringChars(jstr, dalvikP);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseStringCritical() {
		jstring jstr = *((jstring*)replydata);
		jchar* dalvikP = (jchar*)(replydata+sizeof(jstr));
		int strlen = *((int*)(replydata+sizeof(jstr)+sizeof(dalvikP)));
		const jchar* chars = (jchar*)(replydata+sizeof(jstr)+sizeof(dalvikP)+sizeof(strlen));
		memcpy(dalvikP, chars, strlen*sizeof(jchar));
		jniEnv->ReleaseStringCritical(jstr, dalvikP);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callFromReflectedField() {
		jobject jfield = *((jobject*)replydata);
		jfieldID result = jniEnv->FromReflectedField(jfield);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewObjectA() {
		jclass jc = *((jclass*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		jvalue* args = (jvalue*)(replydata+sizeof(methodID)+sizeof(jc));
		jobject result = jniEnv->NewObjectA(jc, methodID, args);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	int BpWrapper::handleJNIRequest(JValTaint* res, Parcel* reply) {
	    int function, taintsize;
	    reply->readInt32(&function);
	    reply->readInt32(&replylength);
	    reply->readInt32(&taintsize);
	    ALOGD("function=%d, length=%d, taintsize=%d", function, replylength, taintsize);
	    replydata = malloc(replylength); //freed by individual JNI functions
	    reply->read(replydata, replylength);
		if (taintsize != 0) {
			replytaint = malloc(taintsize);
			reply->read(replytaint, taintsize);
		} else replytaint = 0;
		switch (function) {
			case 1: callGetStringUTFChars(); break;
	    	case 2: callNewStringUTF(); break;
			case 3: callGetVersion(); break;
			case 4: callFindClass(); break;
			case 5: callGetSuperclass(); break;
			case 6: callIsAssignableFrom(); break;
			case 7: callAllocObject(); break;
			case 8: callThrow(); break;
			case 9: callThrowNew(); break;
			case 10: callExceptionOccurred(); break;
			case 11: callExceptionDescribe(); break;
			case 12: callExceptionClear(); break;
			case 13: callExceptionCheck(); break;
			case 14: callGetMethodID(); break;
			case 15: callGetFieldID(); break;
			case 16: callGetStaticMethodID(); break;
			case 17: callGetLongField(); break;
			case 18: callGetObjectClass(); break;
			case 19: callIsInstanceOf(); break;
			case 20: callNewLocalRef(); break;
			case 21: callNewGlobalRef(); break;
			case 22: callDeleteGlobalRef(); break;
			case 23: callIsSameObject(); break;
			case 24: callCallBooleanMethod(); break;
			case 25: //callCallBooleanMethodV(); break;
			case 26: callCallBooleanMethodA(); break;
			case 27: callCallIntMethod(); break;
			case 28: callCallIntMethodV(); break;
			case 29: callCallIntMethodA(); break;
			case 30: callGetIntField(); break;
			case 31: callCallNonvirtualObjectMethodA(); break;
			case 32: callDeleteLocalRef(); break;
			case 33: callCallObjectMethod(); break;
			case 34: callCallVoidMethod(); break;
			case 35: callSetIntField(); break;
			case 36: callSetLongField(); break;
			case 37: callSetDoubleField(); break;
			case 38: callSetFloatField(); break;
			case 39: callSetShortField(); break;
			case 40: callSetCharField(); break;
			case 41: callSetByteField(); break;
			case 42: callSetBooleanField(); break;
			case 43: callSetObjectField(); break;
			case 44: callGetObjectField(); break;
			case 45: callGetBooleanField(); break;
			case 46: callGetByteField(); break;
			case 47: callGetCharField(); break;
			case 48: callGetShortField(); break;
			case 49: callGetFloatField(); break;
			case 50: callGetDoubleField(); break;
			case 51: callCallStaticShortMethodA(); break;
			case 52: callCallStaticVoidMethodA(); break;
			case 53: callSetShortArrayRegion(); break;
			case 54: callGetByteArrayRegion(); break;
			case 55: callGetStringChars(); break;
			case 56: callReleaseStringChars(); break;
			case 57: callReleaseStringCritical(); break;
			case 58: callFromReflectedField(); break;
			case 59: callNewObjectA(); break;
	    	default: 
			ALOGE("Unknown function: %d", function);
			free(replydata);
			return -1;
	    }
		free(replytaint);
		free(replydata);
	    return callback(function, size, callbackdata, taintsize, res);
	}

// Client
        BpWrapper::BpWrapper(const sp<IBinder>& impl) : BpInterface<IWrapper>(impl) {
            ALOGD("BpWrapper::BpWrapper()");
        }

	void BpWrapper::setJniEnv(JNIEnvMod* env) {
	    jniEnv = env;
	}

	int32_t BpWrapper::call(char* library, char* method, Vector<void*> params, Vector<int> sizeofparam, Vector<int> taints) {
#ifdef ASSERT
	    ASSERT(params.size() == taints.size());
	    ASSERT(params.size() == sizeofparam.size());
#else
	    assert(params.size() == taints.size());
	    assert(params.size() == sizeofparam.size());
#endif
	    Parcel data, reply;
            data.writeInterfaceToken(IWrapper::getInterfaceDescriptor());
            data.writeCString(library);
            data.writeCString(method);
	    int numParams = params.size();
	    data.writeInt32(numParams);
	    for (int i=0; i<numParams; i++) {
		data.writeInt32(sizeofparam.array()[i]);
		data.writeInt32(taints.array()[i]);
		data.write(&(params.array()[i]), sizeofparam.array()[i]);
	    }
            remote()->transact(CALL, data, &reply);    // asynchronous call
            int32_t res;
            status_t status = reply.readInt32(&res);
            ALOGD("BpWrapper::call = %i (status: %i)", res, status);
            return res;
	}

	int32_t BpWrapper::addLib(const char* pathName) {
	    Parcel data, reply;
            data.writeInterfaceToken(IWrapper::getInterfaceDescriptor());
	    data.writeCString(pathName);
            remote()->transact(ADD_LIB, data, &reply);    // asynchronous call
	    int32_t res;
	    status_t status = reply.readInt32(&res);
	    return res;
	}

	int32_t BpWrapper::addFunc(const char* funcName, int32_t libRef) {
	    ALOGD("BpWrapper::addFunc(funcName=%s, libRef=%d)", funcName, libRef);
	    Parcel data, reply;
	    data.writeInterfaceToken(IWrapper::getInterfaceDescriptor());
	    data.writeCString(funcName);
	    data.writeInt32(libRef);
	    remote()->transact(ADD_FUNC, data, &reply);
	    return reply.readInt32();
	}

	JValTaint* BpWrapper::taintCall(JNIEnvMod* pEnv, void* clazz, int argInfo, int argc, const uint32_t* taints,
    	    const uint32_t* argv, const char* shorty, int32_t libHandle, int32_t funcHandle, const char* funcName) {
	    ALOGD("BpWrapper::taintcall(clazz=%p, argInfo=%d, argc=%d, taints=%p, argv=%p, shorty=%s, libHandle=%d, funcHandle=%d, funcName=%s)", clazz, argInfo, argc, taints, argv, shorty, libHandle, funcHandle, funcName);
	    setJniEnv(pEnv);
	    Parcel data, reply;
	    Parcel* replyPtr = &reply;
	    //ALOGD("Parcel &data=%p, replyPtr=%p", &data, replyPtr);
	    //if (replyPtr != 0) ALOGD("replyPtr stats: dataSize=%d, availableData=%d", replyPtr->dataSize(), replyPtr->dataAvail());
            data.writeInterfaceToken(IWrapper::getInterfaceDescriptor());
	    data.writeInt32(argc);
	    //data.write(taints, argc);
	    for (int i=0; i<argc; i++) data.writeInt32(taints[i]);
	    for (int i=0; i<argc; i++) data.writeInt32(argv[i]); //data.write(argv, argc);
	    //if (libHandle != 0) data.write(libHandle, 4); else data.writeInt32(0);
	    data.writeCString(shorty);
	    data.writeInt32(libHandle);
	    //if (funcHandle != 0) data.write(funcHandle, 4); else data.writeInt32(0);
	    data.writeInt32(funcHandle);
	    if (clazz != 0) data.write(clazz, 4); else data.writeInt32(0);
	    data.writeInt32(argInfo);
	    //ALOGD("about to transact");
            remote()->transact(TAINT_CALL, data, replyPtr);    // asynchronous call
	    int execStatus;
	    reply.readInt32(&execStatus);
	    //ALOGD("execStatus=%d", execStatus);
	    bool usedJni = false;
	    JValTaint resVal;
	    JValTaint* res = &resVal;
	    resVal.val.i=-1;

	    while (execStatus != 3) {
			usedJni = true;
			if (execStatus == 2) {
		    	ALOGD("Execution needs JNI, handling request");
		    	execStatus = handleJNIRequest(res, replyPtr);
	    	} else {
		    	ALOGE("Unexpected Execution Manager status: %d", execStatus);
		    	return res;
			}
	    }

	    //ExecutionManager::FINISHED
	    ALOGD("Execution finished, processing result");
	    if (!usedJni) {
		ALOGD("No JNI interruptions -> setting result from taintCall");
		setResult(res, &reply);
	    }
	    ALOGD("returning result=long%lld_int%d", res->val.j, res->val.i);
	    return res;
	}

	int BpWrapper::callback(int function, int length, void* rawdata, int taintlength, JValTaint* res) {
	    Parcel data, reply;
	    data.writeInterfaceToken(IWrapper::getInterfaceDescriptor());
	    data.writeInt32(function);
	    data.writeInt32(length);
		data.writeInt32(taintlength);
	    data.write(rawdata, length);
		if (taintlength > 0) data.write(rawdata+length, taintlength);
	    remote()->transact(CALLBACK, data, &reply);
	    free (rawdata);
	    int execStatus = reply.readInt32();
	    ALOGD("Status of execution manager is now: %d", execStatus);
	    if (execStatus == 3) {
			ALOGD("finished after JNI Call -> setting result from callback");
			setResult(res, &reply);
	    } else if (execStatus == 2) {
			ALOGD("need to handle another JNI request");
			execStatus = handleJNIRequest(res, &reply);
	    } else {
			ALOGE("Unexpected status: %d", execStatus);
	    }
	    return execStatus;
	}

}
