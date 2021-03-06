#include <binder/Parcel.h>
#include <BpWrapper.h>
#include <IWrapper.h>
#include "utils.h"
#include "JniEnvMod.h"
#include <utils/CallStack.h>
#include <pthread.h>
#include <binder/IServiceManager.h>

#define LOG_TAG "BpWrapper=Client"

#ifndef FLOWCOASTER_DEBUG
	#define ALOGD(...) void();
#endif

namespace android{

	bool BpWrapper::restartService() {
		ALOGD("restarting service %s", serviceName);
		sp<IServiceManager> sm = 0;
		sm = defaultServiceManager();
		ASSERT(sm != 0);
		sp<IBinder> binder = 0;
		const char* part1 = "/system/bin/tgwrapper  ";
		pid_t pid = getpid();
		char cpid[50];
		struct timespec res;
		clock_gettime(CLOCK_MONOTONIC, &res);
		sprintf(cpid, "%d %ld %ld", pid, res.tv_sec, res.tv_nsec);
		const char* part2 = " &";
		char* result = (char*) malloc(1+strlen(part1)+strlen(part2)+strlen(cpid));
		strcpy(result, part1);
		strcat(result, cpid);
		strcat(result, part2);
		ALOGD("calling %s", result);
		int sysresult = system(result);
		ALOGD("result code for attempt to start taintgrind: %d", sysresult);
		while (binder == 0)
			binder = sm->getService(String16(serviceName));
		ALOGD("Found service: %s", serviceName);
        wrapper = interface_cast<IWrapper>(binder);
        ASSERT(wrapper != 0);
		return true;
		//registerDeathObserver(wrapper);
		//	wrapper->setServiceState(true);
	}

	int BpWrapper::doCallbackTransaction(int function, int length, void* rawdata,
                                         int taintlength, JValTaint* res, Parcel* reply, u4 threadId) {
      ALOGD("JNI request: callback function=%d, length=%d, data=%08x, taintlength=%d",
            function, length, (int)rawdata, taintlength);
      /* ALOGD("Data to transact: ");
      for(int i = 0; i < length; i++)
      ALOGD("\t%d: %02x", i, ((char*)rawdata)[i]);*/
      Parcel data;

		// TODO: put this somewhere else to avoid doing this several times ...    
		data.writeInterfaceToken(IWrapper::getInterfaceDescriptor());
		data.writeInt32(threadId);
		data.writeInt32(function);
		data.writeInt32(length);
		data.writeInt32(taintlength);
		if(length > 0) {
        	data.write(rawdata, length);
        	if (taintlength > 0)
				data.write(rawdata + length, taintlength);
		}
		while (!serviceAlive) {
			ALOGD("waiting for service to come alive again...");
			sleep(1);
		}        
		remote()->transact(CALLBACK, data, reply);
		if (!serviceAlive) ALOGW("Caution: results cannot be trusted because service died.");
		if(length > 0) {
			ALOGD("now freeing rawdata @%08x", (int)rawdata);
			free (rawdata);
		} else {
			ALOGD("no rawdata - nothing to be freed");
		}
		return reply->readInt32();
	}

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
		u4 taint = 0;
		const char* chars = jniEnv->GetTaintedStringUTFChars(jstr, 0, &taint);
		ALOGD("GetTaintedStringUTFChars returns %s; sizeof(*chars)=%d; strlen(chars)=%d (taint=%08x)",
			chars, sizeof(*chars), strlen(chars), taint);
		//size = sizeof(*chars)*strlen(chars)+1;
		ALOGD("jniEnv=%08x", (int)jniEnv);
		int strsize = jniEnv->GetUTFCharsByteLength(jstr);
		//int strsize = 8*strlen(chars);
		ALOGD("chars needs %d bytes", strsize);
		size = 2*strsize+sizeof(char*)+sizeof(strsize);
		taintsize = size;
		callbackdata = malloc(size+taintsize); //freed by CALLBACK
		int* idata = (int*)callbackdata;
		idata[0] = (int)chars;
		idata[1] = strsize;
		ALOGD("dalvikP=%08x", idata[0]);
		memcpy(&idata[2], chars, strsize);
		if (taint != 0) {
			memset(&idata[4+(strsize+3)/4], taint, strsize/4);
			memset(&idata[2+(strsize+3)/4], 0, 4);
		} else
			memset(&idata[2+(strsize+3)/4], 0, 2+strsize/4);
	}

	void BpWrapper::callNewStringUTF() {
		*((char*)(replydata+replylength-1)) = '\0';
		const char* chars = (char*)replydata;
		ALOGD("read: %s", chars);
		u4 taint = 0;
		int* tdata = (int*)replytaint;
		for (int i=0; i<(replylength / 4); i++) taint |= tdata[i];
		jstring result = jniEnv->NewTaintedStringUTF(chars, taint);
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
		callbackdata = NULL; // malloc(size);
		ALOGD("ExceptionDescribe()->.");
	}

	void BpWrapper::callExceptionClear() {
		jniEnv->ExceptionClear();
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
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
		ALOGD("calling GetMethodID(jclazz=%08x, name=%s, sig=%s)", (int)jclazz, name, sig);
		jmethodID result = jniEnv->GetMethodID(jclazz, name, sig);
		size = sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
		ALOGD("GetMethodID()->%08x (size = %d)", (int)result, size);
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

#define UNPACK_GETFIELD() \
		jobject* jdata = (jobject*)replydata; \
		jobject jobj = jdata[0]; \
		jfieldID fieldID = (jfieldID)jdata[1]; \
		u4 taint = 0;

#define WRITEOUT_GETFIELD() \
		size = sizeof(result) + sizeof(taint); \
		callbackdata = malloc(size); \
		taintsize = sizeof(taint); \
		int* rdata = (int*)callbackdata; \
		rdata[0] = (int)result; \
		rdata[1] = taint; 

	//with taint support
	void BpWrapper::callGetObjectField() {
		UNPACK_GETFIELD();
		jobject result = jniEnv->GetObjectTaintedField(jobj, fieldID, &taint);
		WRITEOUT_GETFIELD();
		ALOGD("GetObjectField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetBooleanField() {
		UNPACK_GETFIELD();
		ALOGD("call GetBooleanTaintedField(jobj=%08x, fieldID=%08x, taint=0)", (int)jobj, (int)fieldID);
		jboolean result = jniEnv->GetBooleanTaintedField(jobj, fieldID, &taint);
		WRITEOUT_GETFIELD();
		ALOGD("GetBooleanField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetByteField() {
		UNPACK_GETFIELD();
		jbyte result = jniEnv->GetByteTaintedField(jobj, fieldID, &taint);
		WRITEOUT_GETFIELD();
		ALOGD("GetByteField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetCharField() {
		UNPACK_GETFIELD();
		jchar result = jniEnv->GetCharTaintedField(jobj, fieldID, &taint);
		WRITEOUT_GETFIELD();
		ALOGD("GetCharField()->%c", result);
	}

	//with taint support
	void BpWrapper::callGetShortField() {
		UNPACK_GETFIELD();
		jshort result = jniEnv->GetShortTaintedField(jobj, fieldID, &taint);
		WRITEOUT_GETFIELD();
		ALOGD("GetShortField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetIntField() {
		UNPACK_GETFIELD();
		jint result = jniEnv->GetIntTaintedField(jobj, fieldID, &taint);
		WRITEOUT_GETFIELD();
		ALOGD("GetIntField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetLongField() {
		UNPACK_GETFIELD();
		jlong result = jniEnv->GetLongTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		jlong* ldata = (jlong*)callbackdata;
		int* idata = (int*)callbackdata;
		ldata[0] = result;
		idata[2] = taint;
		ALOGD("GetLongField()->%08x", (int)result);
	}

	//with taint support
	void BpWrapper::callGetFloatField() {
		UNPACK_GETFIELD();
		jfloat result = jniEnv->GetFloatTaintedField(jobj, fieldID, &taint);
		WRITEOUT_GETFIELD();
		ALOGD("GetFloatField()->%f", result);
	}

	//with taint support
	void BpWrapper::callGetDoubleField() {
		UNPACK_GETFIELD();
		jdouble result = jniEnv->GetDoubleTaintedField(jobj, fieldID, &taint);
		size = sizeof(result) + sizeof(taint);
		callbackdata = malloc(size);
		taintsize = sizeof(taint);
		jdouble* rdata = (jdouble*)callbackdata;
		int* idata = (int*)callbackdata;
		rdata[0] = result;
		idata[2] = taint;
		ALOGD("GetDoubleField()->%f", result);
	}

#define UNPACK_SETFIELD() \
	int* idata = (int*)replydata; \
	jfieldID fieldID = (jfieldID)idata[1]; \
	jobject jobj = (jobject)idata[2]; \
	u4 taint = idata[3];
	
#define UNPACK_SETFIELDL() \
	int* idata = (int*)replydata; \
	jlong* ldata = (jlong*)replydata; \
	jfieldID fieldID = (jfieldID)idata[2]; \
	jobject jobj = (jobject)idata[3]; \
	u4 taint = idata[4];
	
	//with taint support
	void BpWrapper::callSetObjectField() {
		UNPACK_SETFIELD();
		jobject value = (jobject)idata[0];
		ALOGD("SetObjectTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, (int)value, taint);
		jniEnv->SetObjectTaintedField(jobj, fieldID, value, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	//with taint support
	void BpWrapper::callSetBooleanField() {
		UNPACK_SETFIELD();
		jboolean value = (jboolean)idata[0];
		ALOGD("SetBooleanTaintedField: Field %08x to %08x with taint %08x in object %08x",
			(int)fieldID, value, taint, (int)jobj);
		jniEnv->SetBooleanTaintedField(jobj, fieldID, value, taint);
		ALOGD("Field is set, returning...");
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetByteField() {
		UNPACK_SETFIELD();
		jbyte value = (jbyte)idata[0];
		ALOGD("SetByteTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetByteTaintedField(jobj, fieldID, value, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetCharField() {
		UNPACK_SETFIELD();
		jchar value = (jchar)idata[0];
		ALOGD("SetCharTaintedField: Field %08x to %c with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetCharTaintedField(jobj, fieldID, value, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetShortField() {
		UNPACK_SETFIELD();
		jshort value = (jshort)idata[0];
		ALOGD("SetShortTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetShortTaintedField(jobj, fieldID, value, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetIntField() {
		UNPACK_SETFIELD();
		jint value = (jint)idata[0];
		ALOGD("SetIntTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetIntTaintedField(jobj, fieldID, value, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetLongField() {
		UNPACK_SETFIELDL();
		jlong value = ldata[0];
		ALOGD("SetLongTaintedField: Field %08x to %08x with taint %08x", (int)fieldID, (int)value, taint);
		jniEnv->SetLongTaintedField(jobj, fieldID, value, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetFloatField() {
		UNPACK_SETFIELD();
		jfloat value = (jfloat)idata[0];
		ALOGD("SetFloatTaintedField: Field %08x to %f with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetFloatTaintedField(jobj, fieldID, value, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetDoubleField() {
		UNPACK_SETFIELDL();
		jdouble value = (jdouble)ldata[0];
		ALOGD("SetDoubleTaintedField: Field %08x to %f with taint %08x", (int)fieldID, value, taint);
		jniEnv->SetDoubleTaintedField(jobj, fieldID, value, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
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
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetStaticBooleanField() {
		jboolean val = *((jboolean*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticBooleanTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticBooleanField(jc, fieldID, val);
		jniEnv->SetStaticBooleanTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetStaticByteField() {
		jbyte val = *((jbyte*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticByteTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticByteField(jc, fieldID, val);
		jniEnv->SetStaticByteTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetStaticCharField() {
		jchar val = *((jchar*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticCharTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticCharField(jc, fieldID, val);
		jniEnv->SetStaticCharTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetStaticShortField() {
		jshort val = *((jshort*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticShortTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticShortField(jc, fieldID, val);
		jniEnv->SetStaticShortTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetStaticIntField() {
		jint val = *((jint*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticIntTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticIntField(jc, fieldID, val);
		jniEnv->SetStaticIntTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetStaticLongField() {
		jlong val = *((jlong*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticLongTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticLongField(jc, fieldID, val);
		jniEnv->SetStaticLongTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetStaticFloatField() {
		jfloat val = *((jfloat*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticFloatTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticFloatField(jc, fieldID, val);
		jniEnv->SetStaticFloatTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetStaticDoubleField() {
		jdouble val = *((jdouble*)replydata);
		SETSTATIC_GETDATA();
		ALOGD("SetStaticDoubleTaintedField: Field %08x to %08x", (int)fieldID, (int)val);
		//jniEnv->SetStaticDoubleField(jc, fieldID, val);
		jniEnv->SetStaticDoubleTaintedField(jc, fieldID, val, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
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
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callDeleteLocalRef() {
		jobject jobj = *(jobject*)replydata;
		ALOGD("deleting local ref for jobj=%08x", (int)jobj);
		jniEnv->DeleteLocalRef(jobj);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
        ALOGD("callDeleteLocalRef: callbackdata == %p", callbackdata);
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

#define UNPACK_CALLMETHOD() \
		int* data = (int*)replydata; \
		jobject jobj = (jobject)data[0]; \
		jmethodID methodID = (jmethodID)data[1]; \
		int paramSize = data[2]; \
		ALOGD("paramSize = %08x", paramSize); \
		jvalue* args = (jvalue*)&data[3]; \
		void* vargs = (void*)args; \
		u4 objTaint = *((u4*)(vargs+paramSize)); \
		u4* paramTaints = (u4*)(vargs+paramSize+3*sizeof(int)); \
		u4 resultTaint = 0;

#define WRITE_CALLRESULT() \
		size = sizeof(result); \
		taintsize = sizeof(resultTaint); \
		callbackdata = malloc(size+taintsize); \
		int* aresult = (int*)callbackdata; \
		aresult[0] = (int)result; \
		aresult[1] = resultTaint;


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

	//with taint support
	void BpWrapper::callCallBooleanMethodA() {
		UNPACK_CALLMETHOD();
		jboolean result = jniEnv->CallBooleanTaintedMethodA(jobj,
			objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallByteMethodA() {
		UNPACK_CALLMETHOD();
		jbyte result = jniEnv->CallByteTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallCharMethodA() {
		UNPACK_CALLMETHOD();
		jchar result = jniEnv->CallCharTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallShortMethodA() {
		UNPACK_CALLMETHOD();
		jshort result = jniEnv->CallShortTaintedMethodA(jobj, 
			objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
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

	//with taint support
	void BpWrapper::callCallIntMethodA() {
		UNPACK_CALLMETHOD();
		jint result = jniEnv->CallIntTaintedMethodA(jobj,
			objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallLongMethodA() {
		UNPACK_CALLMETHOD();
		jlong result = jniEnv->CallLongTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallFloatMethodA() {
		UNPACK_CALLMETHOD();
		jfloat result = jniEnv->CallFloatTaintedMethodA(jobj,
			objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallDoubleMethodA() {
		UNPACK_CALLMETHOD();
		jdouble result = jniEnv->CallDoubleTaintedMethodA(jobj,
			objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallObjectMethod() {
		UNPACK_CALLMETHOD();
		jobject result = jniEnv->CallObjectTaintedMethodA(jobj,
			objTaint, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallVoidMethod() {
		UNPACK_CALLMETHOD();
		/*ALOGD("jvalue[0]=%08x / %lld", args[0].i, args[0].j);
		ALOGD("jvalue[1]=%08x / %lld", args[1].i, args[1].j);
		ALOGD("jvalue[2]=%08x / %lld", args[2].i, args[2].j);
		int* iargs = (int*)args;
		for (int i=0; i<7; i++) ALOGD("iargs[i]=%08x", iargs[i]);*/
		jniEnv->CallVoidTaintedMethodA(jobj, objTaint, methodID, &resultTaint, args, paramTaints);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

#define UNPACK_CALLVIRTUALMETHOD() \
		int* idata = (int*)replydata; \
		jobject jobj = (jobject)idata[0]; \
		jclass jc = (jclass)idata[1]; \
		jmethodID methodID = (jmethodID)idata[2]; \
		int paramSize = idata[3]; \
		jvalue* args = (jvalue*)&idata[4]; \
		void* vargs = (void*)args; \
		u4 objTaint = *((u4*)(vargs+paramSize)); \
		u4* paramTaints = (u4*)(args+paramSize+4*sizeof(int)); \
		u4 resultTaint = 0;

	//with taint support
	void BpWrapper::callCallNonvirtualObjectMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jobject result = jniEnv->CallNonvirtualObjectTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualBooleanMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jboolean result = jniEnv->CallNonvirtualBooleanTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualByteMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jbyte result = jniEnv->CallNonvirtualByteTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualCharMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jchar result = jniEnv->CallNonvirtualCharTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualShortMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jshort result = jniEnv->CallNonvirtualShortTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualIntMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jint result = jniEnv->CallNonvirtualIntTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualLongMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jlong result = jniEnv->CallNonvirtualLongTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualFloatMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jfloat result = jniEnv->CallNonvirtualFloatTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualDoubleMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jdouble result = jniEnv->CallNonvirtualDoubleTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallNonvirtualVoidMethodA() {
		UNPACK_CALLVIRTUALMETHOD();
		jniEnv->CallNonvirtualVoidTaintedMethodA(jobj, objTaint, jc, 
			methodID, &resultTaint, args, paramTaints);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

#define UNPACK_CALLSTATICMETHOD() \
		int* idata = (int*)replydata; \
		jclass jc = (jclass)idata[0]; \
		jmethodID methodID = (jmethodID)idata[1]; \
		int paramSize = idata[2]; \
		jvalue* args = (jvalue*)&idata[3]; \
		void* vargs = (void*)args; \
		u4* paramTaints = (u4*)(vargs+paramSize+3*sizeof(int)); \
		u4 resultTaint = 0;

	//with taint support
	void BpWrapper::callCallStaticVoidMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jniEnv->CallStaticVoidTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		taintsize = size = 0;
		callbackdata = NULL; // malloc(size);
	}

	//with taint support
	void BpWrapper::callCallStaticObjectMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jobject result = jniEnv->CallStaticObjectTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallStaticBooleanMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jboolean result = jniEnv->CallStaticBooleanTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallStaticByteMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jbyte result = jniEnv->CallStaticByteTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallStaticCharMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jchar result = jniEnv->CallStaticCharTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallStaticShortMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jshort result = jniEnv->CallStaticShortTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallStaticIntMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jint result = jniEnv->CallStaticIntTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallStaticLongMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jlong result = jniEnv->CallStaticLongTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallStaticFloatMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jfloat result = jniEnv->CallStaticFloatTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	//with taint support
	void BpWrapper::callCallStaticDoubleMethodA() {
		UNPACK_CALLSTATICMETHOD();
		jdouble result = jniEnv->CallStaticDoubleTaintedMethodA(jc, methodID, &resultTaint, args, paramTaints);
		WRITE_CALLRESULT();
	}

	void BpWrapper::callSetBooleanArrayRegion() {
		int* idata = (int*)replydata;
		jsize len = idata[0];
		const jboolean* buf = (jboolean*)&idata[1];
		jbooleanArray jarr = (jbooleanArray)idata[len*sizeof(buf[0])/4+1];
		jsize start = idata[len*sizeof(buf[0])/4+2];
		u4 taint = 0;
		for (int j=0; j<len*sizeof(buf[0])/4; j++) taint |= idata[len*sizeof(buf[0])/4+4+j];
		jniEnv->SetTaintedBooleanArrayRegion(jarr, start, len, buf, taint);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetByteArrayRegion() {
		int* idata = (int*)replydata;
		jsize len = idata[0];
		const jbyte* buf = (jbyte*)&idata[1];
		jbyteArray jarr = (jbyteArray)idata[len*sizeof(buf[0])/4+1];
		jsize start = idata[len*sizeof(buf[0])/4+2];
		u4 taint = 0;
		for (int j=0; j<len*sizeof(buf[0])/4; j++) taint |= idata[len*sizeof(buf[0])/4+4+j];
		jniEnv->SetTaintedByteArrayRegion(jarr, start, len, buf, taint);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetCharArrayRegion() {
		int* idata = (int*)replydata;
		jsize len = idata[0];
		const jchar* buf = (jchar*)&idata[1];
		jcharArray jarr = (jcharArray)idata[len*sizeof(buf[0])/4+1];
		jsize start = idata[len*sizeof(buf[0])/4+2];
		u4 taint = 0;
		for (int j=0; j<len*sizeof(buf[0])/4; j++) taint |= idata[len*sizeof(buf[0])/4+4+j];
		jniEnv->SetTaintedCharArrayRegion(jarr, start, len, buf, taint);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetShortArrayRegion() {
		int* idata = (int*)replydata;
		jsize len = idata[0];
		const jshort* buf = (jshort*)&idata[1];
		jshortArray jarr = (jshortArray)idata[len*sizeof(buf[0])/4+1];
		jsize start = idata[len*sizeof(buf[0])/4+2];
		u4 taint = 0;
		for (int j=0; j<len*sizeof(buf[0])/4; j++) taint |= idata[len*sizeof(buf[0])/4+4+j];
		jniEnv->SetTaintedShortArrayRegion(jarr, start, len, buf, taint);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetIntArrayRegion() {
		int* idata = (int*)replydata;
		jsize len = idata[0];
		const jint* buf = (jint*)&idata[1];
		jintArray jarr = (jintArray)idata[len*sizeof(buf[0])/4+1];
		jsize start = idata[len*sizeof(buf[0])/4+2];
		u4 taint = 0;
		for (int j=0; j<len*sizeof(buf[0])/4; j++) taint |= idata[len*sizeof(buf[0])/4+4+j];
		jniEnv->SetTaintedIntArrayRegion(jarr, start, len, buf, taint);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetLongArrayRegion() {
		int* idata = (int*)replydata;
		jsize len = idata[0];
		const jlong* buf = (jlong*)&idata[1];
		jlongArray jarr = (jlongArray)idata[len*sizeof(buf[0])/4+1];
		jsize start = idata[len*sizeof(buf[0])/4+2];
		u4 taint = 0;
		for (int j=0; j<len*sizeof(buf[0])/4; j++) taint |= idata[len*sizeof(buf[0])/4+4+j];
		jniEnv->SetTaintedLongArrayRegion(jarr, start, len, buf, taint);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetFloatArrayRegion() {
		int* idata = (int*)replydata;
		jsize len = idata[0];
		const jfloat* buf = (jfloat*)&idata[1];
		jfloatArray jarr = (jfloatArray)idata[len*sizeof(buf[0])/4+1];
		jsize start = idata[len*sizeof(buf[0])/4+2];
		u4 taint = 0;
		for (int j=0; j<len*sizeof(buf[0])/4; j++) taint |= idata[len*sizeof(buf[0])/4+4+j];
		jniEnv->SetTaintedFloatArrayRegion(jarr, start, len, buf, taint);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callSetDoubleArrayRegion() {
		int* idata = (int*)replydata;
		jsize len = idata[0];
		const jdouble* buf = (jdouble*)&idata[1];
		jdoubleArray jarr = (jdoubleArray)idata[len*sizeof(buf[0])/4+1];
		jsize start = idata[len*sizeof(buf[0])/4+2];
		u4 taint = 0;
		for (int j=0; j<len*sizeof(buf[0])/4; j++) taint |= idata[len*sizeof(buf[0])/4+4+j];
		jniEnv->SetTaintedDoubleArrayRegion(jarr, start, len, buf, taint);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

#define COPYOUT_GETARRAYREGION() \
		size = len*sizeof(buf[0]); \
		taintsize = size; \
		callbackdata = buf; \
		idata = (int*)callbackdata; \
		for (int j=0; j<(len*sizeof(buf[0])/4); j++) idata[size/4+j]=taint; \

	void BpWrapper::callGetBooleanArrayRegion() {
		int* idata = (int*)replydata;
		jbooleanArray jarr = (jbooleanArray)idata[0];
		jsize start = idata[1];
		jsize len = idata[2];
		u4 taint = 0;
		callbackdata = malloc(2*len*sizeof(jboolean));
		jboolean* buf = (jboolean*)callbackdata;
		jniEnv->GetTaintedBooleanArrayRegion(jarr, start, len, buf, &taint);
		COPYOUT_GETARRAYREGION();
	}

	void BpWrapper::callGetByteArrayRegion() {
		int* idata = (int*)replydata;
		jbyteArray jarr = (jbyteArray)idata[0];
		jsize start = idata[1];
		jsize len = idata[2];
		u4 taint = 0;
		callbackdata = malloc(2*len*sizeof(jbyte));
		jbyte* buf = (jbyte*)callbackdata;
		jniEnv->GetTaintedByteArrayRegion(jarr, start, len, buf, &taint);
		COPYOUT_GETARRAYREGION();
	}

	void BpWrapper::callGetCharArrayRegion() {
		int* idata = (int*)replydata;
		jcharArray jarr = (jcharArray)idata[0];
		jsize start = idata[1];
		jsize len = idata[2];
		u4 taint = 0;
		callbackdata = malloc(2*len*sizeof(jchar));
		jchar* buf = (jchar*)callbackdata;
		jniEnv->GetTaintedCharArrayRegion(jarr, start, len, buf, &taint);
		COPYOUT_GETARRAYREGION();
	}

	void BpWrapper::callGetShortArrayRegion() {
		int* idata = (int*)replydata;
		jshortArray jarr = (jshortArray)idata[0];
		jsize start = idata[1];
		jsize len = idata[2];
		u4 taint = 0;
		callbackdata = malloc(2*len*sizeof(jshort));
		jshort* buf = (jshort*)callbackdata;
		jniEnv->GetTaintedShortArrayRegion(jarr, start, len, buf, &taint);
		COPYOUT_GETARRAYREGION();
	}

	void BpWrapper::callGetIntArrayRegion() {
		int* idata = (int*)replydata;
		jintArray jarr = (jintArray)idata[0];
		jsize start = idata[1];
		jsize len = idata[2];
		u4 taint = 0;
		callbackdata = malloc(2*len*sizeof(jint));
		jint* buf = (jint*)callbackdata;
		jniEnv->GetTaintedIntArrayRegion(jarr, start, len, buf, &taint);
		COPYOUT_GETARRAYREGION();
	}

	void BpWrapper::callGetLongArrayRegion() {
		int* idata = (int*)replydata;
		jlongArray jarr = (jlongArray)idata[0];
		jsize start = idata[1];
		jsize len = idata[2];
		u4 taint = 0;
		callbackdata = malloc(2*len*sizeof(jlong));
		jlong* buf = (jlong*)callbackdata;
		jniEnv->GetTaintedLongArrayRegion(jarr, start, len, buf, &taint);
		COPYOUT_GETARRAYREGION();
	}

	void BpWrapper::callGetFloatArrayRegion() {
		int* idata = (int*)replydata;
		jfloatArray jarr = (jfloatArray)idata[0];
		jsize start = idata[1];
		jsize len = idata[2];
		u4 taint = 0;
		callbackdata = malloc(2*len*sizeof(jfloat));
		jfloat* buf = (jfloat*)callbackdata;
		jniEnv->GetTaintedFloatArrayRegion(jarr, start, len, buf, &taint);
		COPYOUT_GETARRAYREGION();
	}

	void BpWrapper::callGetDoubleArrayRegion() {
		int* idata = (int*)replydata;
		jdoubleArray jarr = (jdoubleArray)idata[0];
		jsize start = idata[1];
		jsize len = idata[2];
		u4 taint = 0;
		callbackdata = malloc(2*len*sizeof(jdouble));
		jdouble* buf = (jdouble*)callbackdata;
		jniEnv->GetTaintedDoubleArrayRegion(jarr, start, len, buf, &taint);
		COPYOUT_GETARRAYREGION();
	}

	void BpWrapper::callGetStringChars() {
		jstring jstr = *(jstring*)replydata;
		ALOGD("GetStringChars: jstr=%08x", (int)jstr);
		const jchar* result = jniEnv->GetStringChars(jstr, 0);
		ALOGD("GetStringChars: %08x", (int)result);
		int strlen = jniEnv->GetStringLength(jstr);
		size = sizeof(jchar*)+sizeof(int)+strlen*sizeof(jchar);
		taintsize = 0;
		callbackdata = malloc(size);
		int* idata = (int*)callbackdata;
		idata[0] = (int)result;
		idata[1] = strlen;
		ALOGD("writing out dalvikP=%08x; strlen=%08x", idata[0], idata[1]);
		memcpy(&idata[2], result, strlen*sizeof(jchar));
	}

	void BpWrapper::callReleaseStringChars() {
		int* idata = (int*)replydata;
		jstring jstr = (jstring)idata[0];
		jchar* dalvikP = (jchar*)idata[1];
		int strlen = idata[2];
		const jchar* chars = (jchar*)&idata[3];
		int* tdata = (int*)replytaint;
		int charlength = (replylength - 12)/4;
		ALOGD("charlength = %d", charlength);
		u4 taint = 0;
		for (int i=0; i<(charlength+1)/2; i++)
			taint |= tdata[i+3];
		memcpy(dalvikP, chars, strlen*sizeof(jchar));
		jniEnv->ReleaseTaintedStringChars(jstr, taint, dalvikP);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseStringCritical() {
		int* idata = (int*)replydata;
		int strlen = idata[0];
		const jchar* chars = (const jchar*)&idata[1];
		jstring jstr = (jstring)idata[1+(strlen+1)/2];
		jchar* dalvikP = (jchar*)idata[2+(strlen+1)/2];
		ALOGD("releaseStringCritical: received strlen=%d, jstr=%08x, dalvikP=%08x",
			strlen, (int)jstr, (int)dalvikP);
		memcpy(dalvikP, chars, strlen*sizeof(jchar));
		u4 taint = 0;
		for (int j=0; j<(strlen+1)/2; j++) taint |= idata[3+(strlen+1)/2+j];
		jniEnv->ReleaseTaintedStringCritical(jstr, taint, dalvikP);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
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
		void* result = jniEnv->GetPrimitiveArrayCritical(jarr, 0);
		ALOGD("dalvik void pointer: %08x", (int)result);
		int arraysize = jniEnv->GetArrayLength(jarr)*elementSize;
		size = arraysize + sizeof(size) + sizeof(result);
		taintsize = 0;
		callbackdata = malloc(size);
		int* idata = (int*)callbackdata;
		idata[0] = arraysize;
		idata[1] = (int)result;
		memcpy(&idata[2], result, arraysize);
	}

	void BpWrapper::callReleasePrimitiveArrayCritical() {
		int* idata = (int*)replydata;
		jarray jarr = (jarray)idata[0];
		int arraysize = idata[1];
		void* dalvikP = (void*)idata[2];
		//int dalvikPP = *((int*)(replydata+sizeof(jarr)+sizeof(int)));
		//dalvikP = (void*)dalvikPP;
		//void* data = replydata + sizeof(jarr) + 2*sizeof(int);
		//ALOGD("callReleasePrimitiveArrayCritical: jarr=%08x, size=%d, dalvikP=%08x, dalvikPP=%08x, data=%08x",
		//	(int)jarr, size, (int)dalvikP, dalvikPP, (int)data);
		ALOGD("callReleasePrimitiveArrayCritical: jarr=%08x, size=%d, dalvikP=%08x, data=%08x",
			(int)jarr, arraysize, (int)dalvikP, (int)&idata[3]);
		jshort* iarr = (jshort*)&idata[3];
		ALOGD("i[0]=%d, i[1]=%d, i[2]=%d, i[3]=%d, i[4]=%d, i[5]=%d, i[6]=%d",
			iarr[0], iarr[1], iarr[2], iarr[3], iarr[4], iarr[5], iarr[6]);
		memcpy(dalvikP, &idata[3], arraysize);
        ALOGD("-1-");
		jniEnv->ReleasePrimitiveArrayCritical(jarr, dalvikP, 2);
        ALOGD("-2-");
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseStringUTFChars() {
		int* idata = (int*)replydata;
		char* dalvikP = (char*)idata[0];
		int length = idata[1];
		jstring jstr = (jstring)idata[2];
		char* utf = (char*)&idata[3];
		u4 taint = 0;
		ALOGD("ReleaseStringUTFChars received dalvikP=%08x, length=%d, utf=%s", (int)dalvikP, length, utf);
		memcpy(dalvikP, utf, length);
		ALOGD("copied chars back");
		for (int j=0; j<length/4; j++) taint |= idata[j+length/4+6];
		ALOGD("chars taint=%08x", taint);
		jniEnv->ReleaseTaintedStringUTFChars(jstr, taint, dalvikP);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
		ALOGD("<- release string utf chars");
	}

	void BpWrapper::callGetStringCritical() {
		int* idata = (int*)replydata;
		jstring jstr = (jstring)idata[0];
		ALOGD("GetStringCritical: jstr=%08x", (int)jstr);
		u4 taint = 0;
		const jchar* result = jniEnv->GetTaintedStringCritical(jstr, &taint, 0);
		ALOGD("GetTaintedStringCritical: %08x (taint=%08x)", (int)result, taint);
		int strlen = jniEnv->GetStringLength(jstr);
		size = 2*sizeof(int)+strlen*sizeof(jchar);
		taintsize = size;
		callbackdata = malloc(size+taintsize);
		idata = (int*)callbackdata;
		idata[0] = (int)result;
		idata[1] = strlen;
		memcpy(&idata[2], result, strlen*sizeof(jchar));
		for (int j=0; j<strlen/2; j++)
			idata[4+strlen/2+j] = taint;
		ALOGD("<- callGetStringCritical (taintsize=%d @%08x)", taintsize, &taintsize);
	}

	void BpWrapper::callDeleteWeakGlobalRef() {
		jweak jw = *((jweak*)replydata);
		jniEnv->DeleteWeakGlobalRef(jw);
		size = 0;
		taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

#define GENERIC_GETARRAYELEMENTS() \
  taintsize = size + sizeof(size) + sizeof(result);                     \
  callbackdata = malloc(2*(size + sizeof(size) + sizeof(result)));      \
  int* idata = (int*)callbackdata;                                      \
  idata[0] = size;                                                      \
  idata[1] = (int) result;                                              \
  //ALOGD("Copy following %d bytes to offset %p", size, &idata[2]);     \
  // for(int i = 0; i < size; i++)                                      \
  //   ALOGD("\t1 %d: %02x", i, ((char*)result)[i]);                    \
  memcpy(&idata[2], result, size);                                      \
  // ALOGD("Set 8 bytes to 0 at offset %p", &idata[2+size/4]);           \
  memset(&idata[2+size/4], 0, 8);                                       \
  // for(int i = 0; i < size; i++)                                       \
  //  ALOGD("\t2 %d: %02x", i, ((char*)(&idata[2]))[i]);                \
  for (int j=0; j<=size/4; j++) idata[4+size/4+j] = taint;              \
  // for(int i = 0; i < size; i++)                                       \
  // ALOGD("\t3 %d: %02x", i, ((char*)(&idata[2]))[i]);                  \
  size += 2 * sizeof(int);
  
	void BpWrapper::callGetBooleanArrayElements() {
		jbooleanArray jarr = *((jbooleanArray*)replydata);
		jboolean fake = 0;
		u4 taint = 0;
		void* result = jniEnv->GetTaintedBooleanArrayElements(jarr, &fake, &taint);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jboolean);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetByteArrayElements() {
		jbyteArray jarr = *((jbyteArray*)replydata);
		jboolean fake = 0;
		u4 taint = 0;
		void* result = jniEnv->GetTaintedByteArrayElements(jarr, &fake, &taint);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jbyte);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetCharArrayElements() {
		jcharArray jarr = *((jcharArray*)replydata);
		jboolean fake = 0;
		u4 taint = 0;
		void* result = jniEnv->GetTaintedCharArrayElements(jarr, &fake, &taint);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jchar);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetShortArrayElements() {
		jshortArray jarr = *((jshortArray*)replydata);
		jboolean fake = 0;
		u4 taint = 0;
		void* result = jniEnv->GetTaintedShortArrayElements(jarr, &fake, &taint);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jshort);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetIntArrayElements() {
		jintArray jarr = *((jintArray*)replydata);
		jboolean fake = 0;
		u4 taint = 0;
		void* result = jniEnv->GetTaintedIntArrayElements(jarr, &fake, &taint);
		ALOGD("dalvik void pointer: %08x", (int)result);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jint);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetLongArrayElements() {
		jlongArray jarr = *((jlongArray*)replydata);
		jboolean fake = 0;
		u4 taint = 0;
		void* result = jniEnv->GetTaintedLongArrayElements(jarr, &fake, &taint);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jlong);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetFloatArrayElements() {
		jfloatArray jarr = *((jfloatArray*)replydata);
		jboolean fake = 0;
		u4 taint = 0;
		void* result = jniEnv->GetTaintedFloatArrayElements(jarr, &fake, &taint);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jfloat);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callGetDoubleArrayElements() {
		jdoubleArray jarr = *((jdoubleArray*)replydata);
		jboolean fake = 0;
		u4 taint = 0;
		void* result = jniEnv->GetTaintedDoubleArrayElements(jarr, &fake, &taint);
		size = jniEnv->GetArrayLength(jarr)*sizeof(jdouble);
		GENERIC_GETARRAYELEMENTS();
	}

	void BpWrapper::callReleaseBooleanArrayElements() {
		jbooleanArray jarr = *((jbooleanArray*)replydata);
		jniEnv->ReleaseBooleanArrayElements(jarr, (jboolean*)replydata, 0);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseByteArrayElements() {
		jbyteArray jarr = *((jbyteArray*)replydata);
		jniEnv->ReleaseByteArrayElements(jarr, (jbyte*)replydata, 0);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseCharArrayElements() {
		jcharArray jarr = *((jcharArray*)replydata);
		jniEnv->ReleaseCharArrayElements(jarr, (jchar*)replydata, 0);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseShortArrayElements() {
		jshortArray jarr = *((jshortArray*)replydata);
		ALOGD("ReleaseShortArrayElements(%08x)", (int)jarr);
		jniEnv->ReleaseShortArrayElements(jarr, (jshort*)replydata, 0);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseIntArrayElements() {
		jintArray jarr = *((jintArray*)replydata);
		jniEnv->ReleaseIntArrayElements(jarr, 0, 0);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseLongArrayElements() {
		jlongArray jarr = *((jlongArray*)replydata);
        ALOGD("ReleaseLongArrayElements(%08x)", (int)jarr);
		jniEnv->ReleaseLongArrayElements(jarr, (jlong*)replydata, 0);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseFloatArrayElements() {
		jfloatArray jarr = *((jfloatArray*)replydata);
		jniEnv->ReleaseFloatArrayElements(jarr, (jfloat*)replydata, 0);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callReleaseDoubleArrayElements() {
		jdoubleArray jarr = *((jdoubleArray*)replydata);
		jniEnv->ReleaseDoubleArrayElements(jarr, (jdouble*)replydata, 0);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
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
		int* idata = (int*)replydata;
		jobject jinitialElement = (jobject)idata[0];
		jsize length = idata[1];
		jclass jelementClass = (jclass)idata[2];
		u4 taint = idata[3];
		jobjectArray result = jniEnv->NewTaintedObjectArray(length, jelementClass, jinitialElement, taint);
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
			int code = *(int*)r2;
			meth->fnPtr = (void*)code;
			r2 += sizeof(void*);
			ALOGD("registering native method from library: %s (%s), code at %08x",
				meth->name, meth->signature, (int)meth->fnPtr);
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
        ALOGD("callGetArrayLenth: %d", (int)result);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetObjectArrayElement() {
		int* idata = (int*)replydata;
		jobjectArray jarr = (jobjectArray)idata[0];
		jsize index = idata[1];
		u4 taint = 0;
		ALOGD("call GetObjectArrayElement(jarr=%08x, index=%d)", (int)jarr, index);
		jobject result = jniEnv->GetTaintedObjectArrayElement(jarr, index, &taint);
		ALOGD("callGetObjectArrayElement->%08x (taint=%08x)", (int)result, taint);
		taintsize = sizeof(taint);
		size = sizeof(result);
		callbackdata = malloc(size + taintsize);
		idata = (int*)callbackdata;
		idata[0] = (int)result;
		idata[1] = taint;
        ALOGD("end callGetObjectArrayElement");
	}

	void BpWrapper::callSetObjectArrayElement() {
		int* idata = (int*)replydata;
		jobject jobj = (jobject)idata[0];
		jsize index = idata[1];
		jobjectArray jarr = (jobjectArray)idata[2];
		u4 taint = idata[3];
		ALOGD("call SetTaintedObjectArrayElement(jarr=%08x, index=%d, jobj=%08x, taint=%08x)", 
			(int)jarr, index, (int)jobj, taint);
		jniEnv->SetTaintedObjectArrayElement(jarr, index, jobj, taint);
		size = taintsize = 0;
		callbackdata = NULL; // malloc(size);
	}

	void BpWrapper::callMonitorEnter() {
		jobject jobj = *((jobject*)replydata);
		jint result = jniEnv->MonitorEnter(jobj);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callMonitorExit() {
		jobject jobj = *((jobject*)replydata);
		jint result = jniEnv->MonitorExit(jobj);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callNewDirectByteBuffer() {
		jlong* jl = (jlong*)replydata;
		void* address = malloc(jl[0]*sizeof(address));
		memcpy(address, &jl[1], jl[0]*sizeof(address));
		ALOGD("callNewDirectByteBuffer(capacity=%lld, address=%08x)", jl[0], (int)address);
		jobject result = jniEnv->NewDirectByteBuffer(address, jl[0]);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		memcpy(callbackdata, &result, size);
	}

	void BpWrapper::callGetDirectBufferAddress() {
		jobject jbuf = *((jobject*)replydata);
		ALOGD("callGetDirectBufferAddress(jbuf=%08x)", (int)jbuf);
		void* result = jniEnv->GetDirectBufferAddress(jbuf);
		jlong resultsize = jniEnv->GetDirectBufferCapacity(jbuf);
		taintsize = 0;
		size = sizeof(resultsize) + resultsize;
		callbackdata = malloc(size);
		jlong* callbackdatap = (jlong*)callbackdata;
		callbackdatap[0] = resultsize;
		memcpy(&callbackdatap[1], result, resultsize);
	}

	void BpWrapper::callGetDirectBufferCapacity() {
		jobject jbuf = *((jobject*)replydata);
		ALOGD("callGetDirectBufferCapacity(jbuf=%08x)", (int)jbuf);
		jlong result = jniEnv->GetDirectBufferCapacity(jbuf);
		taintsize = 0;
		size = sizeof(result);
		callbackdata = malloc(size);
		jlong* callbackdatap = (jlong*)callbackdata;
		callbackdatap[0] = result;
	}

	void BpWrapper::handleJNIRequest(JValTaint* res, Parcel* reply) {
	    int function;

        //cs->update(0, 100);
        //cs->dump("BpWrapper: ");
        
	    reply->readInt32(&function);
	    reply->readInt32(&replylength);
	    reply->readInt32(&taintsize);
	    //ALOGD("function=%d, length=%d, taintsize=%d", function, replylength, taintsize);
	    replydata = malloc(replylength); //freed by individual JNI functions
	    reply->read(replydata, replylength);
		ALOGD("function=%d, length=%d, taintsize=%d, replydata=%08x",
			function, replylength, taintsize, (int)(int*)replydata);
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
			case 161: callSetBooleanArrayRegion(); break;
			case 162: callSetLongArrayRegion(); break;
			case 163: callSetDoubleArrayRegion(); break;
			case 164: callMonitorEnter(); break;
			case 165: callMonitorExit(); break;
			case 166: callNewDirectByteBuffer(); break;
			case 167: callGetDirectBufferAddress(); break;
	    	default: 
			ALOGE("Unknown function: %d", function);
			free(replydata);
			//return -1;
	    }
		//ALOGD("taintsize=%d @%08x", taintsize, &taintsize);
		free(replytaint);
		free(replydata);
		ALOGD("handleJNIRequest setting cbdata: function=%d, callbackdata=%08x, length=%d, taintlength=%d",
			function, (int)callbackdata, size, taintsize);
		cbdata->function = function;
		cbdata->rawdata = callbackdata;
		cbdata->length = size;
		cbdata->taintlength = taintsize;
		cbdata->res = res;
	}

// Client
        BpWrapper::BpWrapper(const sp<IBinder>& impl) : BpInterface<IWrapper>(impl) {
            ALOGD("BpWrapper::BpWrapper()");
            cs = new CallStack();
			cbdata = (callback_t*)malloc(sizeof(callback_t));
        }

	void BpWrapper::setJniEnv(JNIEnvMod* env) {
		//ALOGD("Wrapper setting jniEnv=%08x", (int)env);
	    jniEnv = env;
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
                                    const uint32_t* argv, const char* shorty, int32_t libHandle,
									int32_t funcHandle, const char* funcName, u4 threadId) {
    	ALOGD("BpWrapper::taintcall(clazz=%08x, argInfo=%d, argc=%d, taints=%p, argv=%p, shorty=%s, libHandle=%d, funcHandle=%d, funcName=%s, threadId=%d)",
			clazz, argInfo, argc, taints, argv, shorty, libHandle, funcHandle, funcName, threadId);
    	setJniEnv(pEnv);
    	Parcel data, reply;
		ALOGD("Thread id: %d; jniEnv=%08x", threadId, (int)pEnv);
        //ALOGD("Parcel &data=%p, replyPtr=%p", &data, replyPtr);
		//if (replyPtr != 0)
			//ALOGD("replyPtr stats: dataSize=%d, availableData=%d", replyPtr->dataSize(), replyPtr->dataAvail());
		while (!serviceAlive) {
			ALOGD("Taintcall: waiting for service to come alive again...");
			sleep(1);
		}        
		data.writeInterfaceToken(IWrapper::getInterfaceDescriptor());
        data.writeInt32(argc);
        for (int i=0; i<argc; i++) data.writeInt32(taints[i]);
        for (int i=0; i<argc; i++) {
        	ALOGD("\targv[%d] = 0x%08x\n", i, argv[i]);
        	data.writeInt32(argv[i]);
        }
        data.writeCString(shorty);
        data.writeInt32(libHandle);
        data.writeInt32(funcHandle);
        if (clazz != 0) data.writeInt32((int)clazz); else data.writeInt32(0);
        data.writeInt32(argInfo);
		data.writeInt32(threadId);
        //ALOGD("about to transact");
        remote()->transact(TAINT_CALL, data, &reply);    // asynchronous call
		//int exceptionCode = reply.readExceptionCode();
		//ALOGD("exceptionCode=%d", exceptionCode);
		if (!serviceAlive) ALOGW("Caution: results cannot be trusted because service died.");
        //ALOGD("transaction done");
	    int execStatus;
	    reply.readInt32(&execStatus);
	    ALOGD("transaction finished with execStatus=%d", execStatus);
	    bool usedJni = false;
	    JValTaint resVal;
	    JValTaint* res = &resVal;
	    resVal.val.i=-1;

	    while (execStatus != 3) {
			usedJni = true;
			if (execStatus == 2) {
		    	//ALOGD("Execution needs JNI, handling request");
		    	//execStatus = handleJNIRequest(res, replyPtr);
				handleJNIRequest(res, &reply);
				execStatus = doCallbackTransaction(cbdata->function, cbdata->length, cbdata->rawdata,
                                                   cbdata->taintlength, cbdata->res, &reply, threadId);
	    	} else {
		    	ALOGE("Unexpected Execution Manager status: %d", execStatus);
		    	return res;
			}
	    }

	    //ExecutionManager::FINISHED
	    ALOGD("Execution finished, processing result");
	    //if (!usedJni) {
		//ALOGD("No JNI interruptions -> setting result from taintCall");
		setResult(res, &reply);
	    //}
	    ALOGD("returning result=long%lld_int%d", res->val.j, res->val.i);
	    return res;
	}

	int32_t BpWrapper::changeFunc(int32_t oldHandle, int32_t newHandle) {
		Parcel data, reply;
		data.writeInterfaceToken(IWrapper::getInterfaceDescriptor());
		data.writeInt32(oldHandle);
		data.writeInt32(newHandle);
		remote()->transact(CHANGE_FUNC, data, &reply);
		int funcref = reply.readInt32();
		return funcref;
	}

	void BpWrapper::setServiceState(bool online) {
		serviceAlive = online;
		if (!online) {
			ALOGD("Taintgrind Wrapper is not available anymore, restarting...");
			if (this->restartService()) {
				ALOGD("Taintgrind Wrapper is alive again.");
				serviceAlive = true;
			}
		} else ALOGD("Taintgrind Wrapper is now available.");
	}

	void BpWrapper::setServiceName(char* name) {
		serviceName = name;
	}

	void BpWrapper::setWrapperPointer(sp<IWrapper> wrapperPointer) {
		wrapper = wrapperPointer;
	}
}

