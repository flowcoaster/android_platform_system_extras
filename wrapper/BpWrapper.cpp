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
		//jboolean isCopy = *((jboolean*)(replydata+(sizeof(jstr)*sizeof(char))));
		//ALOGD("jstr=%p, isCopy=%d", jstr, isCopy);
		ALOGD("jstr=%08x", (int)jstr);
		const char* chars = jniEnv->GetStringUTFChars(jstr, 0);
		ALOGD("GetStringUTFChars returns %s; sizeof(*chars)=%d; strlen(chars)=%d",
			chars, sizeof(*chars), strlen(chars));
		//size = sizeof(*chars)*strlen(chars)+1;
		size = 8*(strlen(chars)+1);
		taintsize = 0;
		callbackdata = malloc(size+sizeof(char*)); //freed by CALLBACK
		memcpy(callbackdata, &chars, sizeof(char*));
		memcpy(callbackdata+sizeof(char*), chars, size);
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
		taintsize = sizeof(taint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &taint, taintsize);
		ALOGD("GetObjectField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetBooleanField() {
		jobject jobj = *((jobject*)replydata);
		void* r2 = replydata+sizeof(jobj);
		jfieldID fieldID = *((jfieldID*)r2);
		u4 taint = 0;
<<<<<<< HEAD
		/*ALOGD("call GetBooleanTaintedField(jobj=%08x, fieldID=%08x, taint=0)", (int)jobj, (int)fieldID);
		jboolean result = jniEnv->GetBooleanTaintedField(jobj, fieldID, &taint);*/
		//TODO: fix problem with invalid references on tainted call
		ALOGD("call GetBooleanField(jobj=%08x, fieldID=%08x)", (int)jobj, (int)fieldID);
		jboolean result = jniEnv->GetBooleanField(jobj, fieldID);
=======
		ALOGD("call GetBooleanTaintedField(jobj=%08x, fieldID=%08x, taint=0)", (int)jobj, (int)fieldID);
		jboolean result = jniEnv->GetBooleanTaintedField(jobj, fieldID, &taint);
		/* ALOGD("call GetBooleanField(jobj=%08x, fieldID=%08x)", (int)jobj, (int)fieldID);
           jboolean result = jniEnv->GetBooleanField(jobj, fieldID);*/
>>>>>>> bc6eacf16adf55ab9595eee56edcb3011742ab9e
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

	//with taint support
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

#define SETSTATIC_GETDATA() \
	void* r2 = replydata+sizeof(val); \
	jclass jc = *((jclass*)replydata); \
	r2 += sizeof(jc); \
	jfieldID fieldID = *((jfieldID*)r2); \
	r2 += sizeof(fieldID); \
	u4 taint = *((u4*)r2);

	void BpWrapper::callSetStaticObjectField() {
		jobject val = *((jobject*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticObjectTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		jniEnv->SetStaticObjectTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetStaticBooleanField() {
		jboolean val = *((jboolean*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticBooleanTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticBooleanField(jc, fieldID, val);
		jniEnv->SetStaticBooleanTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetStaticByteField() {
		jbyte val = *((jbyte*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticByteTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticByteField(jc, fieldID, val);
		jniEnv->SetStaticByteTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetStaticCharField() {
		jchar val = *((jchar*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticCharTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticCharField(jc, fieldID, val);
		jniEnv->SetStaticCharTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetStaticShortField() {
		jshort val = *((jshort*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticShortTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticShortField(jc, fieldID, val);
		jniEnv->SetStaticShortTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetStaticIntField() {
		jint val = *((jint*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticIntTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticIntField(jc, fieldID, val);
		jniEnv->SetStaticIntTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetStaticLongField() {
		jlong val = *((jlong*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticLongTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticLongField(jc, fieldID, val);
		jniEnv->SetStaticLongTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetStaticFloatField() {
		jfloat val = *((jfloat*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticFloatTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticFloatField(jc, fieldID, val);
		jniEnv->SetStaticFloatTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetStaticDoubleField() {
		jdouble val = *((jdouble*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticDoubleTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticDoubleField(jc, fieldID, val);
		jniEnv->SetStaticDoubleTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callGetObjectClass() {
		jobject jobj = *((jobject*)replydata);
		ALOGD("CallGetObjectClass read jobj=%08x", (int)jobj);
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
		jniEnv->DeleteLocalRef(jobj);
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

	void BpWrapper::callCallFloatMethodA() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID));
		jfloat result = jniEnv->CallFloatMethodA(jobj, methodID, args);
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
		u4 objTaint = *((u4*)(args+paramSize*sizeof(jvalue)));
		u4* paramTaints = (u4*)(args+paramSize*sizeof(jvalue)+sizeof(u4)+sizeof(methodID));
		u4 resultTaint = 0;
		ALOGD("calling VoidTaintedMethodA(jobj=%08x, objTaint=%08x, methodID=%08x, resultTaint=0, args=%08x, paramTaints=%08x)",
			(int)jobj, objTaint, (int)methodID, (int)args, (int)paramTaints);
		//jniEnv->CallVoidTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		jniEnv->CallVoidMethodA(jobj, methodID, args);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualObjectMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(args+paramSize*sizeof(jvalue)));
		u4* paramTaints = (u4*)(args+paramSize*sizeof(jvalue)+sizeof(u4)+sizeof(methodID));
		u4 resultTaint = 0;
		jobject result = jniEnv->CallNonvirtualObjectTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualBooleanMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jboolean result = jniEnv->CallNonvirtualBooleanTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualByteMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jbyte result = jniEnv->CallNonvirtualByteTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualCharMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jchar result = jniEnv->CallNonvirtualCharTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualShortMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jshort result = jniEnv->CallNonvirtualShortTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualIntMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jint result = jniEnv->CallNonvirtualIntTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualLongMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jlong result = jniEnv->CallNonvirtualLongTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualFloatMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jfloat result = jniEnv->CallNonvirtualFloatTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualDoubleMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jdouble result = jniEnv->CallNonvirtualDoubleTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallNonvirtualVoidMethodA() {
		jobject jobj = *((jobject*)replydata);
		jclass jc = *((jclass*)(replydata+sizeof(jobj)));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jniEnv->CallNonvirtualVoidTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	//with taint support
	void BpWrapper::callCallStaticVoidMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jniEnv->CallStaticVoidTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = size = 0;
		callbackdata = malloc(size);
	}

	//with taint support
	void BpWrapper::callCallStaticObjectMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jobject result = jniEnv->CallStaticObjectTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallStaticBooleanMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jboolean result = jniEnv->CallStaticBooleanTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallStaticByteMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jbyte result = jniEnv->CallStaticByteTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallStaticCharMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jchar result = jniEnv->CallStaticCharTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallStaticShortMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jshort result = jniEnv->CallStaticShortTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallStaticIntMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jint result = jniEnv->CallStaticIntTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallStaticLongMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jlong result = jniEnv->CallStaticLongTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallStaticFloatMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jfloat result = jniEnv->CallStaticFloatTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallStaticDoubleMethodA() {
		jclass jc = *((jclass*)(replydata));
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		int paramSize = *((int*)(replydata+sizeof(jc)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jc)+sizeof(methodID)+sizeof(paramSize));
		u4* paramTaints = (u4*)(args+paramSize);
		u4 resultTaint = 0;
		jdouble result = jniEnv->CallStaticDoubleTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

#define ARRAYREGION_EXTRACT() \
		jsize start = *((jsize*)(replydata+sizeof(jarr)));\
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));

	void BpWrapper::callSetByteArrayRegion() {
		jbyteArray jarr = *((jbyteArray*)(replydata));
		ARRAYREGION_EXTRACT();
		const jbyte* buf = (jbyte*)(replydata+sizeof(jarr)+sizeof(start)+sizeof(len));
		jniEnv->SetByteArrayRegion(jarr, start, len, buf);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetCharArrayRegion() {
		jcharArray jarr = *((jcharArray*)(replydata));
		ARRAYREGION_EXTRACT();
		const jchar* buf = (jchar*)(replydata+sizeof(jarr)+sizeof(start)+sizeof(len));
		jniEnv->SetCharArrayRegion(jarr, start, len, buf);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetShortArrayRegion() {
		jshortArray jarr = *((jshortArray*)(replydata));
		ARRAYREGION_EXTRACT();
		const jshort* buf = (jshort*)(replydata+sizeof(jarr)+sizeof(start)+sizeof(len));
		//ALOGD("buf[0]=%08x, buf[1]=%08x, buf[2]=%08x", buf[0], buf[1], buf[2]);
		//ALOGD("calling SetShortArrayRegion(jarr=%08x, start=%08x, len=%08x, buf=%08x",
		//	jarr, start, len, buf);
		jniEnv->SetShortArrayRegion(jarr, start, len, buf);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetIntArrayRegion() {
		jintArray jarr = *((jintArray*)(replydata));
		ARRAYREGION_EXTRACT();
		const jint* buf = (jint*)(replydata+sizeof(jarr)+sizeof(start)+sizeof(len));
		jniEnv->SetIntArrayRegion(jarr, start, len, buf);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callSetFloatArrayRegion() {
		jfloatArray jarr = *((jfloatArray*)(replydata));
		ARRAYREGION_EXTRACT();
		const jfloat* buf = (jfloat*)(replydata+sizeof(jarr)+sizeof(start)+sizeof(len));
		jniEnv->SetFloatArrayRegion(jarr, start, len, buf);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callGetBooleanArrayRegion() {
		jbooleanArray jarr = *((jbooleanArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		jboolean* buf = (jboolean*)malloc(len*sizeof(jboolean));
		jniEnv->GetBooleanArrayRegion(jarr, start, len, buf);
		size = len*sizeof(jboolean);
		taintsize = 0;
		callbackdata = buf;
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

	void BpWrapper::callGetCharArrayRegion() {
		jcharArray jarr = *((jcharArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		jchar* buf = (jchar*)malloc(len*sizeof(jchar));
		jniEnv->GetCharArrayRegion(jarr, start, len, buf);
		size = len*sizeof(jbyte);
		taintsize = 0;
		callbackdata = buf;
	}

	void BpWrapper::callGetShortArrayRegion() {
		jshortArray jarr = *((jshortArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		jshort* buf = (jshort*)malloc(len*sizeof(jshort));
		jniEnv->GetShortArrayRegion(jarr, start, len, buf);
		size = len*sizeof(jshort);
		taintsize = 0;
		callbackdata = buf;
	}

	void BpWrapper::callGetIntArrayRegion() {
		jlongArray jarr = *((jlongArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		jlong* buf = (jlong*)malloc(len*sizeof(jlong));
		jniEnv->GetLongArrayRegion(jarr, start, len, buf);
		size = len*sizeof(jlong);
		taintsize = 0;
		callbackdata = buf;
	}

	void BpWrapper::callGetLongArrayRegion() {
		jlongArray jarr = *((jlongArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		jlong* buf = (jlong*)malloc(len*sizeof(jlong));
		jniEnv->GetLongArrayRegion(jarr, start, len, buf);
		size = len*sizeof(jlong);
		taintsize = 0;
		callbackdata = buf;
	}

	void BpWrapper::callGetFloatArrayRegion() {
		jfloatArray jarr = *((jfloatArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		jfloat* buf = (jfloat*)malloc(len*sizeof(jfloat));
		jniEnv->GetFloatArrayRegion(jarr, start, len, buf);
		size = len*sizeof(jfloat);
		taintsize = 0;
		callbackdata = buf;
	}

	void BpWrapper::callGetDoubleArrayRegion() {
		jdoubleArray jarr = *((jdoubleArray*)(replydata));
		jsize start = *((jsize*)(replydata+sizeof(jarr)));
		jsize len = *((jsize*)(replydata+sizeof(jarr)+sizeof(start)));
		jdouble* buf = (jdouble*)malloc(len*sizeof(jdouble));
		jniEnv->GetDoubleArrayRegion(jarr, start, len, buf);
		size = len*sizeof(jdouble);
		taintsize = 0;
		callbackdata = buf;
	}

	void BpWrapper::callGetStringChars() {
		jstring jstr = *((jstring*)replydata);
		ALOGD("GetStringChars: jstr=%08x", (int)jstr);
		jboolean isCopy = *((jboolean*)(replydata+sizeof(jstr)));
		ALOGD("GetStringChars: isCopy=%08x", isCopy);
		const jchar* result = jniEnv->GetStringChars(jstr, &isCopy);
		//ALOGD("GetStringChars: %08x -> %s", result, *result);
		ALOGD("GetStringChars: %08x", (int)result);
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

	void BpWrapper::callFromReflectedMethod() {
		jobject jmethod = *((jobject*)replydata);
		jmethodID result = jniEnv->FromReflectedMethod(jmethod);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
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

	void BpWrapper::callToReflectedMethod() {
		jclass jc = *((jclass*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jc)));
		jobject result = jniEnv->ToReflectedMethod(jc, methodID, 0);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callToReflectedField() {
		jclass jc = *((jclass*)replydata);
		jfieldID fieldID = *((jfieldID*)(replydata+sizeof(jc)));
		jobject result = jniEnv->ToReflectedField(jc, fieldID, 0);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetPrimitiveArrayCritical() {
		jarray jarr = *((jarray*)replydata);
		jboolean fake = 0;
		//ALOGD("GetPrimitiveArrayCritical: jarr=%08x", jarr);
		const char* sig = jniEnv->GetArrayType(jarr);
		ALOGD("GetPrimitiveArrayCritical: array type = %s", sig);
		int elementSize = 0;
		switch (sig[1]) {
			case 'B':
			case 'C':
			case 'S':
			case 'I':
			case 'J':
			case 'F': elementSize = sizeof(jint); break;
			case 'L':
			case 'D': elementSize = sizeof(jlong); break;
			default: ALOGE("Unknown array type: %c; cannot determine size.", sig[1]);
		}
		void* result = jniEnv->GetPrimitiveArrayCritical(jarr, &fake);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*elementSize;
		taintsize = 0;
		callbackdata = malloc(size + sizeof(size) + sizeof(result));
		memcpy(callbackdata, &size, sizeof(size));
		memcpy(callbackdata+sizeof(size), &result, sizeof(result));
		memcpy(callbackdata+sizeof(size)+sizeof(result), result, size);
	}

	void BpWrapper::callReleasePrimitiveArrayCritical() {
		jarray jarr = *((jarray*)replydata);
		int size = *((int*)(replydata + sizeof(jarr)));
		void* dalvikP = replydata + sizeof(jarr) + sizeof(int);
		int dalvikPP = *((int*)(replydata+sizeof(jarr)+sizeof(int)));
		dalvikP = (void*)dalvikPP;
		void* data = replydata + sizeof(jarr) + 2*sizeof(int);
		ALOGD("callReleasePrimitiveArrayCritical: jarr=%08x, size=%d, dalvikP=%08x, dalvikPP=%08x, data=%08x",
			(int)jarr, size, (int)dalvikP, dalvikPP, (int)data);
		jshort* iarr = (jshort*)data;
		ALOGD("i[0]=%d, i[1]=%d, i[2]=%d, i[3]=%d, i[4]=%d, i[5]=%d, i[6]=%d",
			iarr[0], iarr[1], iarr[2], iarr[3], iarr[4], iarr[5], iarr[6]);
		memcpy(dalvikP, data, size);
		jniEnv->ReleasePrimitiveArrayCritical(jarr, data, 2);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseStringUTFChars() {
		char* dalvikP = (char*)(*((int*)replydata));
		int length = *((int*)(replydata+sizeof(int)));
		jstring jstr = *((jstring*)(replydata+2*sizeof(int)));
		char* utf = (char*)(replydata+2*sizeof(int)+sizeof(jstring));
		ALOGD("ReleaseStringUTFChars received dalvikP=%08x, length=%d, utf=%s", (int)dalvikP, length, utf);
		memcpy(dalvikP, utf, length*sizeof(char));
		jniEnv->ReleaseStringUTFChars(jstr, dalvikP);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callGetStringCritical() {
		jstring jstr = *((jstring*)replydata);
		ALOGD("GetStringCritical: jstr=%08x", (int)jstr);
		jboolean isCopy = *((jboolean*)(replydata+sizeof(jstr)));
		ALOGD("GetStringCritical: isCopy=%08x", isCopy);
		const jchar* result = jniEnv->GetStringCritical(jstr, &isCopy);
		ALOGD("GetStringCritical: %08x", (int)result);
		int strlen = jniEnv->GetStringLength(jstr);
		size = sizeof(jchar*)+sizeof(int)+strlen*sizeof(jchar);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(jchar*));
		memcpy(callbackdata+sizeof(jchar*), &strlen, sizeof(jchar*));
		memcpy(callbackdata+sizeof(jchar*)+sizeof(strlen), result, size);
	}

	void BpWrapper::callDeleteWeakGlobalRef() {
		jweak jw = *((jweak*)replydata);
		jniEnv->DeleteWeakGlobalRef(jw);
		size = 0;
		taintsize = 0;
		callbackdata = malloc(size);
	}

#define GENERIC_GETARRAYELEMENTS() \
		taintsize = 0;\
		callbackdata = malloc(size + sizeof(size) + sizeof(result));\
		memcpy(callbackdata, &size, sizeof(size));\
		memcpy(callbackdata+sizeof(size), &result, sizeof(result));\
		memcpy(callbackdata+sizeof(size)+sizeof(result), result, size);

	void BpWrapper::callGetBooleanArrayElements() {
		jbooleanArray jarr = *((jbooleanArray*)replydata);
		jboolean fake = 0;
		void* result = jniEnv->GetBooleanArrayElements(jarr, &fake);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jboolean);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetByteArrayElements() {
		jbyteArray jarr = *((jbyteArray*)replydata);
		jboolean fake = 0;
		void* result = jniEnv->GetByteArrayElements(jarr, &fake);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jbyte);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetCharArrayElements() {
		jcharArray jarr = *((jcharArray*)replydata);
		jboolean fake = 0;
		void* result = jniEnv->GetCharArrayElements(jarr, &fake);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jchar);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetShortArrayElements() {
		jshortArray jarr = *((jshortArray*)replydata);
		jboolean fake = 0;
		void* result = jniEnv->GetShortArrayElements(jarr, &fake);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jshort);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetIntArrayElements() {
		jintArray jarr = *((jintArray*)replydata);
		jboolean fake = 0;
		void* result = jniEnv->GetIntArrayElements(jarr, &fake);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jint);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetLongArrayElements() {
		jlongArray jarr = *((jlongArray*)replydata);
		jboolean fake = 0;
		void* result = jniEnv->GetLongArrayElements(jarr, &fake);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jlong);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetFloatArrayElements() {
		jfloatArray jarr = *((jfloatArray*)replydata);
		jboolean fake = 0;
		void* result = jniEnv->GetFloatArrayElements(jarr, &fake);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jfloat);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetDoubleArrayElements() {
		jdoubleArray jarr = *((jdoubleArray*)replydata);
		jboolean fake = 0;
		void* result = jniEnv->GetDoubleArrayElements(jarr, &fake);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jdouble);
		GENERIC_GETARRAYELEMENTS();
	}

	//with taint support
	void BpWrapper::callCallByteMethodA() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jbyte result = jniEnv->CallByteTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallCharMethodA() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jchar result = jniEnv->CallCharTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallLongMethodA() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jlong result = jniEnv->CallLongTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallDoubleMethodA() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jdouble result = jniEnv->CallDoubleTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	//with taint support
	void BpWrapper::callCallShortMethodA() {
		jobject jobj = *((jobject*)replydata);
		jmethodID methodID = *((jmethodID*)(replydata+sizeof(jobj)));
		int paramSize = *((int*)(replydata+sizeof(jobj)+sizeof(methodID)));
		jvalue* args = (jvalue*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize));
		u4 objTaint = *((u4*)(replydata+sizeof(jobj)+sizeof(methodID)+sizeof(paramSize)+paramSize));
		u4* paramTaints = (u4*)(args+paramSize+sizeof(u4));
		u4 resultTaint = 0;
		jshort result = jniEnv->CallShortTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		taintsize = sizeof(resultTaint);
		size = sizeof(result) + taintsize;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
		memcpy((callbackdata+sizeof(result)), &resultTaint, taintsize);
	}

	void BpWrapper::callReleaseBooleanArrayElements() {
		jbooleanArray jarr = *((jbooleanArray*)replydata);
		jniEnv->ReleaseBooleanArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseByteArrayElements() {
		jbyteArray jarr = *((jbyteArray*)replydata);
		jniEnv->ReleaseByteArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseCharArrayElements() {
		jcharArray jarr = *((jcharArray*)replydata);
		jniEnv->ReleaseCharArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseShortArrayElements() {
		jshortArray jarr = *((jshortArray*)replydata);
		jniEnv->ReleaseShortArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseIntArrayElements() {
		jintArray jarr = *((jintArray*)replydata);
		jniEnv->ReleaseIntArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseLongArrayElements() {
		jlongArray jarr = *((jlongArray*)replydata);
		jniEnv->ReleaseLongArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseFloatArrayElements() {
		jfloatArray jarr = *((jfloatArray*)replydata);
		jniEnv->ReleaseFloatArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callReleaseDoubleArrayElements() {
		jdoubleArray jarr = *((jdoubleArray*)replydata);
		jniEnv->ReleaseDoubleArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	void BpWrapper::callNewWeakGlobalRef() {
		jobject jobj = *((jobject*)replydata);
		jweak result = jniEnv->NewWeakGlobalRef(jobj);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, sizeof(result));
	}

	void BpWrapper::callNewBooleanArray() {
		jsize length = *((jsize*)replydata);
		jbooleanArray result = jniEnv->NewBooleanArray(length);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewByteArray() {
		jsize length = *((jsize*)replydata);
		jbyteArray result = jniEnv->NewByteArray(length);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewCharArray() {
		jsize length = *((jsize*)replydata);
		jcharArray result = jniEnv->NewCharArray(length);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewShortArray() {
		jsize length = *((jsize*)replydata);
		jshortArray result = jniEnv->NewShortArray(length);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewIntArray() {
		jsize length = *((jsize*)replydata);
		jintArray result = jniEnv->NewIntArray(length);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewLongArray() {
		jsize length = *((jsize*)replydata);
		jlongArray result = jniEnv->NewLongArray(length);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewFloatArray() {
		jsize length = *((jsize*)replydata);
		jfloatArray result = jniEnv->NewFloatArray(length);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewDoubleArray() {
		jsize length = *((jsize*)replydata);
		jdoubleArray result = jniEnv->NewDoubleArray(length);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetObjectRefType() {
		jobject jobj = *((jobject*)replydata);
		jobjectRefType result = jniEnv->GetObjectRefType(jobj);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetStringLength() {
		jstring jstr = *((jstring*)replydata);
		jsize result = jniEnv->GetStringLength(jstr);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetStringRegion() {
		jstring jstr = *((jstring*)replydata);
		jsize start = *((jsize*)(replydata+sizeof(jstr)));
		jsize len = *((jsize*)(replydata+sizeof(jstr)+sizeof(start)));
		callbackdata = malloc(len*sizeof(jchar));
		jniEnv->GetStringRegion(jstr, start, len, (jchar*)callbackdata);
	}

	void BpWrapper::callGetStringUTFRegion() {
		jstring jstr = *((jstring*)replydata);
		jsize start = *((jsize*)(replydata+sizeof(jstr)));
		jsize len = *((jsize*)(replydata+sizeof(jstr)+sizeof(start)));
		callbackdata = malloc(len*8); // UTF chars are 8 bit
		jniEnv->GetStringUTFRegion(jstr, start, len, (char*)callbackdata);
	}

	void BpWrapper::callNewObjectArray() {
		jsize length = *((jsize*)replydata);
		jclass jelementClass = *((jclass*)(replydata+sizeof(length)));
		jobject jinitialElement = *((jclass*)(replydata+sizeof(length)+sizeof(jelementClass)));
		jobjectArray result = jniEnv->NewObjectArray(length, jelementClass, jinitialElement);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callEnsureLocalCapacity() {
		jint capacity = *(jint*)replydata;
		jint result = jniEnv->EnsureLocalCapacity(capacity);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callUnregisterNatives() {
		jclass jclazz = *((jclass*)replydata);
		jint result = jniEnv->UnregisterNatives(jclazz);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callPushLocalFrame() {
		jint capacity = *(jint*)replydata;
		jint result = jniEnv->PushLocalFrame(capacity);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}		

	void BpWrapper::callPopLocalFrame() {
		jobject jresult = *((jobject*)replydata);
		jobject result = jniEnv->PopLocalFrame(jresult);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetStaticFieldID() {
		jclass jclazz = *((jclass*)replydata);
		void* r2 = replydata+sizeof(jclazz);
		char* name = (char*)(r2);
		r2+= sizeof(char)*(strlen(name)+1);
		char* sig = (char*)(r2);
		jfieldID result = jniEnv->GetStaticFieldID(jclazz, name, sig);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("GetStaticFieldID()->%08x", (int)result);
	}

#define UNPACK_GETSTATICFIELD() \
		jclass jclazz = *((jclass*)replydata); \
		void* r2 = replydata+sizeof(jclazz); \
		jfieldID fieldID = *((jfieldID*)r2); \
		u4 taint = 0; 

#define WRITEOUT_GETSTATICFIELD() \
		size = sizeof(result) + sizeof(taint); \
		callbackdata = malloc(size); \
		taintsize = sizeof(taint); \
		memcpy(callbackdata, &result, sizeof(result)); \
		memcpy((callbackdata+sizeof(result)), &taint, sizeof(taint));

	void BpWrapper::callGetStaticBooleanField() {
		UNPACK_GETSTATICFIELD();
		jboolean result = jniEnv->GetStaticBooleanTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticBooleanField()->%08x", (int)result);
	}

	void BpWrapper::callGetStaticByteField() {
		UNPACK_GETSTATICFIELD();
		jbyte result = jniEnv->GetStaticByteTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticByteField()->%08x", (int)result);
	}

	void BpWrapper::callGetStaticCharField() {
		UNPACK_GETSTATICFIELD();
		jchar result = jniEnv->GetStaticCharTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticCharField()->%08x", (int)result);
	}

	void BpWrapper::callGetStaticShortField() {
		UNPACK_GETSTATICFIELD();
		jshort result = jniEnv->GetStaticShortTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticShortField()->%08x", (int)result);
	}

	void BpWrapper::callGetStaticIntField() {
		UNPACK_GETSTATICFIELD();
		jint result = jniEnv->GetStaticIntTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticIntField()->%08x", (int)result);
	}

	void BpWrapper::callGetStaticLongField() {
		UNPACK_GETSTATICFIELD();
		jlong result = jniEnv->GetStaticLongTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticLongField()->%08x", (int)result);
	}

	void BpWrapper::callGetStaticFloatField() {
		UNPACK_GETSTATICFIELD();
		jfloat result = jniEnv->GetStaticFloatTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticFloatField()->%f", result);
	}

	void BpWrapper::callGetStaticDoubleField() {
		UNPACK_GETSTATICFIELD();
		jdouble result = jniEnv->GetStaticDoubleTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticDoubleField()->%f", result);
	}

	void BpWrapper::callGetStaticObjectField() {
		UNPACK_GETSTATICFIELD();
		jobject result = jniEnv->GetStaticObjectTaintedField(jclazz, fieldID, &taint);
		WRITEOUT_GETSTATICFIELD();
		ALOGD("GetStaticObjectField()->%08x", (int)result);
	}

	void BpWrapper::callGetStringUTFLength() {
		jstring jstr = *((jstring*)replydata);
		jsize result = jniEnv->GetStringUTFLength(jstr);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callRegisterNatives() {
		jclass jclazz = *((jclass*)replydata);
		jint nMethods = *((jint*)(replydata+sizeof(jclazz)));
		ALOGD("RegisterTaintedNatives: jclazz=%08x, nMethods=%08x", (int)jclazz, nMethods);
		jint result = 0; //JNI_OK
		JNINativeMethod* meth = (JNINativeMethod*)malloc(sizeof(JNINativeMethod));
		void* r2 = replydata+sizeof(jclazz)+sizeof(nMethods);
		for (int i=0; i<nMethods; i++) {
			int j = *(int*)r2;
			r2 += sizeof(j);
			char* name = (char*)r2;
			r2 += j*sizeof(char);
			j = *(int*)r2;
			r2 += sizeof(j);
			char* signature = (char*)r2;
			r2 += j*sizeof(char);
			meth->name = name;
			meth->signature = signature;
			meth->fnPtr = r2;
			int code = *(int*)r2;
			r2 += sizeof(void*);
			ALOGD("registering native method from library: %s (%s), code at %08x",
				meth->name, meth->signature, meth->fnPtr);
			ALOGD("code=%08x", code);
			if(jniEnv->RegisterTaintedNatives(jclazz, meth, 1) == -1)
				result = -1;
		}
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("RegisterTaintedNatives->%08x", result);
	}

	void BpWrapper::callNewString() {
		jsize len = *((jsize*)replydata);
		jchar* unicodeChars = (jchar*)(replydata+sizeof(len));
		jstring result = jniEnv->NewString(unicodeChars, len);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetArrayLength() {
		jarray jarr = *(jarray*)replydata;
		jsize result = jniEnv->GetArrayLength(jarr);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetObjectArrayElement() {
		jobjectArray jarr = *(jobjectArray*)replydata;
		jsize index = *(jsize*)(replydata+sizeof(jarr));
		ALOGD("call GetObjectArrayElement(jarr=%08x, index=%d)", (int)jarr, index);
		jobject result = jniEnv->GetObjectArrayElement(jarr, index);
		ALOGD("callGetObjectArrayElement->%08x", (int)result);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callSetObjectArrayElement() {
		jobjectArray jarr = *(jobjectArray*)replydata;
		jsize index = *(jsize*)(replydata+sizeof(jarr));
		jobject jobj = *(jobject*)(replydata+sizeof(jarr)+sizeof(index));
		ALOGD("call SetObjectArrayElement(jarr=%08x, index=%d, jobj=%08x)", (int)jarr, index, (int)jobj);
		jniEnv->SetObjectArrayElement(jarr, index, jobj);
		size = taintsize = 0;
		callbackdata = malloc(size);
	}

	int BpWrapper::handleJNIRequest(JValTaint* res, Parcel* reply) {
	    int function, taintsize;
	    reply->readInt32(&function);
	    reply->readInt32(&replylength);
	    reply->readInt32(&taintsize);
	    ALOGD("function=%d, length=%d, taintsize=%d", function, replylength, taintsize);
	    replydata = malloc(replylength); //freed by individual JNI functions
	    reply->read(replydata, replylength);
		ALOGD("replydata=%08x", (int)(int*)replydata);
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
			case 60: callSetStaticLongField(); break;
			case 61: callToReflectedMethod(); break;
			case 62: callCallFloatMethodA(); break;
			case 63: callToReflectedField(); break;
			case 64: callFromReflectedMethod(); break;
			case 65: callGetPrimitiveArrayCritical(); break;
			case 66: callReleasePrimitiveArrayCritical(); break;
			case 67: callReleaseStringUTFChars(); break;
			case 68: callGetStringCritical(); break;
			case 69: callDeleteWeakGlobalRef(); break;
			case 70: callGetByteArrayElements(); break;
			case 71: callCallByteMethodA(); break;
			case 72: callCallCharMethodA(); break;			
			case 73: callCallLongMethodA(); break;			
			case 74: callCallDoubleMethodA(); break;			
			case 75: callCallNonvirtualBooleanMethodA(); break;			
			case 76: callCallNonvirtualByteMethodA(); break;			
			case 77: callCallNonvirtualCharMethodA(); break;			
			case 78: callCallNonvirtualShortMethodA(); break;			
			case 79: callCallNonvirtualIntMethodA(); break;			
			case 80: callCallNonvirtualLongMethodA(); break;			
			case 81: callCallNonvirtualFloatMethodA(); break;			
			case 82: callCallNonvirtualDoubleMethodA(); break;			
			case 83: callCallNonvirtualVoidMethodA(); break;			
			case 84: callCallShortMethodA(); break;			
			case 85: callCallStaticCharMethodA(); break;			
			case 86: callCallStaticByteMethodA(); break;			
			case 87: callCallStaticBooleanMethodA(); break;			
			case 88: callCallStaticObjectMethodA(); break;			
			case 89: callCallStaticIntMethodA(); break;			
			case 90: callCallStaticLongMethodA(); break;			
			case 91: callCallStaticFloatMethodA(); break;			
			case 92: callCallStaticDoubleMethodA(); break;
			case 93: callSetFloatArrayRegion(); break;
			case 94: callSetByteArrayRegion(); break;
			case 95: callSetCharArrayRegion(); break;
			case 96: callSetIntArrayRegion(); break;
			case 97: callGetLongArrayElements(); break;
			case 98: callGetDoubleArrayElements(); break;
			case 99: callGetShortArrayElements(); break;
			case 100: callGetDoubleArrayRegion(); break;
			case 101: callReleaseByteArrayElements(); break;
			case 102: callReleaseBooleanArrayElements(); break;
			case 103: callReleaseCharArrayElements(); break;
			case 104: callReleaseShortArrayElements(); break;
			case 105: callReleaseIntArrayElements(); break;
			case 106: callReleaseLongArrayElements(); break;
			case 107: callReleaseFloatArrayElements(); break;
			case 108: callReleaseDoubleArrayElements(); break;
			case 109: callNewWeakGlobalRef(); break;
			case 110: callNewBooleanArray(); break;
			case 111: callNewByteArray(); break;
			case 112: callNewCharArray(); break;
			case 113: callNewShortArray(); break;
			case 114: callNewIntArray(); break;
			case 115: callNewLongArray(); break;
			case 116: callNewFloatArray(); break;
			case 117: callNewDoubleArray(); break;
			case 118: callGetObjectRefType(); break;
			case 119: callGetStringLength(); break;
			case 120: callGetStringRegion(); break;
			case 121: callGetStringUTFRegion(); break;
			case 122: callNewObjectArray(); break;
			case 123: callEnsureLocalCapacity(); break;
			case 124: callUnregisterNatives(); break;
			case 125: callPushLocalFrame(); break;
			case 126: callPopLocalFrame(); break;
			case 127: callSetStaticDoubleField(); break;
			case 128: callSetStaticFloatField(); break;
			case 129: callSetStaticIntField(); break;
			case 130: callSetStaticShortField(); break;
			case 131: callSetStaticCharField(); break;
			case 132: callSetStaticByteField(); break;
			case 133: callSetStaticBooleanField(); break;
			case 134: callGetStaticFieldID(); break;
			case 135: callGetStaticIntField(); break;
			case 136: callGetStaticShortField(); break;
			case 137: callGetStaticCharField(); break;
			case 138: callGetStaticByteField(); break;
			case 139: callGetStaticBooleanField(); break;
			case 140: callGetStaticObjectField(); break;
			case 141: callGetStaticLongField(); break;
			case 142: callGetStaticFloatField(); break;
			case 143: callGetStaticDoubleField(); break;
			case 144: callGetStringUTFLength(); break;
			case 145: callRegisterNatives(); break;
			case 146: callSetStaticObjectField(); break;
			case 147: callNewString(); break;
			case 148: callGetArrayLength(); break;
			case 149: callGetObjectArrayElement(); break;
			case 150: callSetObjectArrayElement(); break;
			case 151: callGetBooleanArrayElements(); break;
			case 152: callGetCharArrayElements(); break;
			case 153: callGetIntArrayElements(); break;
			case 154: callGetFloatArrayElements(); break;
			case 155: callGetBooleanArrayRegion(); break;
			case 156: callGetCharArrayRegion(); break;
			case 157: callGetShortArrayRegion(); break;
			case 158: callGetIntArrayRegion(); break;
			case 159: callGetLongArrayRegion(); break;
			case 160: callGetFloatArrayRegion(); break;
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

	JValTaint* BpWrapper::taintCall(JNIEnvMod* pEnv, int clazz, int argInfo, int argc, const uint32_t* taints,
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
	    if (clazz != 0) data.write(&clazz, 4); else data.writeInt32(0);
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
