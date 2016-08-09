#include <dlfcn.h> // for loading libraries
#include <binder/IPCThreadState.h>
#include <BnWrapper.h>
#include <utils/TextOutput.h> // for aout

#define LOG_TAG "BnWrapper=Server"

namespace android {

BnWrapper::BnWrapper(int pid, int uid) {
    ALOGD("BnWrapper::BnWrapper setting pid=%d, uid=%d", pid, uid);
    callerPid = pid;
    callerUid = uid;
    jniEnv = dvmCreateJNIEnvMod();
}

bool BnWrapper::checkAuthorization(int pid, int uid) {
    if (pid != callerPid || uid != callerUid) {
	ALOGD("Unauthorized Call to Binder service pid %d!=%d or uid %d!=%d", pid, callerPid, uid, callerUid);
	return false;
    } else
	return true;
}

status_t BnWrapper::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags) {
    //ALOGD("BnWrapper::onTransact(code=%i, &data=%p, reply=%p, flags=%i)", code, &data, reply, flags);
    data.checkInterface(this);

    switch(code) {
	case CALL: {
	    //ALOGD("JniEnv->execManager=%p", jniEnv->execManager);
	    jniEnv->GetStringUTFChars(0, 0);
	    const char* callName = data.readCString();
	    const char* callMethod = data.readCString();
	    int numParams = data.readInt32();
	    Vector<int> taints, paramsizes;
	    int size = 0, taint = 0;
	    Vector<int> params;
	    for (int i=0; i<numParams; i++) {
		size = data.readInt32();
		taint = data.readInt32();
		taints.push(taint);
		int temp; //void* temp;
		temp = data.readInt32(); //data.read(temp, size);
		params.push(temp);
	    }
	    void* handle = dlopen(callName, RTLD_LAZY);
	    ASSERT(handle != 0);
	    typedef int (*sum_tt)(int, int);
	    dlerror(); //flush errors
	    //aout << "flushed errors; creating symbol\n";
	    sum_tt sum = (sum_tt) dlsym(handle, callMethod);
	    const char *dl_error = dlerror();
	    if (dl_error) {
		//aout << "error: " << dl_error; endl(aout);
		dlclose(handle);
	    }
	    int x = params.top(); params.pop();
	    int y = params.top(); params.pop();
	    int i = (int) sum(x, y);
	    //aout << "sum= " << i << " \n";
	    dlclose(handle);
	    //aout << "closed library\n";
	    reply->writeInt32(i);
	    for (int ii=0; ii<numParams; ii++) reply->writeInt32(0); //write placeholders for taint
	    ALOGD("BnWrapper::onTransact call reply parcel:");
            //reply->print(PLOG); endl(PLOG);
	    return NO_ERROR;
	} break;
	case ADD_LIB: {
	    //check if caller is the correct process
	    /*if (!checkAuthorization(IPCThreadState::self()->getCallingPid(),
		IPCThreadState::self()->getCallingUid()))
		return PERMISSION_DENIED;*/
	    const char* libName = data.readCString();
	    ALOGD("onTransact: ADD_LIB: libName=%s", libName);
	    startTimer();
	    void* handle = dlopen(libName, RTLD_LAZY);
	    double time = stopTimer();
	    ALOGD("loaded handle %p(%08x) in %f ms", handle, *((int*)handle), time);
	    const char *dl_error = dlerror();
	    if (dl_error) {
		ALOGD("could not load library %s: dlerror()=%s", libName, dl_error);
		//dlclose(handle);
	    }
	    if (handle != 0) {
		// library loaded
		ssize_t index = libtable.add(handle);
		ALOGD("wrapper created new libref table library entry: %d->%p for %s", index, handle, libName);
		//ALOGD("server: size of handle void* %p is %d", handle, handleSize);
		reply->writeInt32((int)index);
	    } else {
		//loading library failed
		reply->writeInt32(0);
	    }
	    return NO_ERROR;
	} break;
	case ADD_FUNC: {
	    //ALOGD("onTransact: ADD_FUNC");
	    //check if caller is the correct process
	    /*if (!checkAuthorization(IPCThreadState::self()->getCallingPid(),
		IPCThreadState::self()->getCallingUid()))
		return PERMISSION_DENIED;*/
	    const char* funcName = data.readCString();
	    const int libref = data.readInt32();
	    void* handle = libtable[libref];
	    ALOGD("ADD_FUNC: libname=%s, libtable[%d]=%p", funcName, libref, handle);
	    if (handle != 0) {
		startTimer();
		void* func = dlsym(handle, funcName);
		double time = stopTimer();
		const char *dl_error = dlerror();
		if (dl_error) {
		    ALOGD("could not load function %s: dlerror()=%s", funcName, dl_error);
		    //dlclose(handle);
		}
		ssize_t index = libtable.add(func);
		ALOGD("wrapper created new libref table function entry: %d->%p for %s (%f s)",
		    index, func, funcName, time);
		reply->writeInt32((int)index);
	    }
	    return NO_ERROR;
	} break;
	case TAINT_CALL: {
	    ALOGD("Wrapper receiving TAINT_CALL(code=%d):", TAINT_CALL);
	    //ALOGD("parcel reply=%p", reply);
	    //check if caller is the correct process
		// no security for debugging execution manager
	    /*if (!checkAuthorization(IPCThreadState::self()->getCallingPid(),
		IPCThreadState::self()->getCallingUid()))
		return PERMISSION_DENIED;*/
	    int argc = data.readInt32();
	    int* argv = (int*)malloc(argc*sizeof(int)); //freed at the end of TAINTCALL
	    Vector<int> vectaints;
	    for (int i=0; i<argc; i++) vectaints.push(data.readInt32());
	    for (int i=0; i<argc; i++) {
		argv[i] = data.readInt32();
		//ALOGD("argv[%d]=%d", i, argv[i]);
	    }
	    const char* shorty = data.readCString();
	    //ALOGD("parcel reply=%p", reply);
	    int32_t libHandleRef = data.readInt32();
	    void* libHandle = libtable[libHandleRef];
	    int32_t funcRef = data.readInt32();
	    void* funcHandle = libtable[funcRef];
	    int clazzi = data.readInt32();
	    void* clazz = &clazzi;
	    int argInfo = data.readInt32();
	    ALOGD("read parcel: clazz=%p, argInfo=%d, argc=%d, shorty=%s, libHandle=%d, funcHandle=%d",
		clazz, argInfo, argc, shorty, libHandleRef, funcRef);
	    for (int i=0; i<argc; i++) ALOGD("taint %08x <- args[%d]=%d", vectaints[i], i, argv[i]);
	    //for (int i=0; i<argc+2; i++) ALOGD("taint %08x <- args[%d]=%08x", vectaints[i], i, argv[i]);
	    argv++; // hack for the extra value at index 0

	    // TODO: put result on stack otherwise ... ohoh
	    jvalue result;
	    jvalue* pResult = &result;
	    /*Parcel* reply2 = reply;
	    ALOGD("parcel reply=%p, reply2=%p", reply, reply2);*/
	    JNIEnvModExt* myExt = (JNIEnvModExt*)jniEnv;
	    ExecutionManager* myExecManager = myExt->execManager;
	    //myExecManager->clazz=clazz;
	    myExecManager->platformInvoke.jniEnv=myExt;
	    myExecManager->platformInvoke.argInfo=argInfo;
	    myExecManager->platformInvoke.clazz=clazz;
	    myExecManager->platformInvoke.argc=argc;
	    myExecManager->platformInvoke.argv=argv;
	    myExecManager->platformInvoke.shorty=shorty;
	    myExecManager->platformInvoke.funcHandle=funcHandle;
	    myExecManager->platformInvoke.pResult=pResult;

	    ALOGD("check: argv[0]: %d (%08x)", myExecManager->platformInvoke.argv[0], (int)(&myExecManager->platformInvoke.argv[0]));
	    ALOGD("check: argv[1]: %d (%08x)", myExecManager->platformInvoke.argv[1], (int)(&myExecManager->platformInvoke.argv[1]));

	    ALOGD("now calling myExecManager::startExec(%p)",pResult);
//#ifdef WITH_DALVIK_BINDER_SERVICE
	    //dvmPlatformInvoke((JNIEnv*)jniEnv, clazz, argInfo, argc, argv, shorty, funcHandle, pResult);
	    int status = myExecManager->startExec(pResult);
	    reply->writeInt32(status);

	    if(status == ExecutionManager::FINISHED) {
	      ALOGD("Execution is over");
	      //#else
              //dvmPlatformInvoke(0, clazz, argInfo, argc, argv, shorty, funcHandle, pResult);
	      //#endif
	      ALOGD("dvmPlatformInvoke finished. result=%lld", result.j);
	      ALOGD("wrote result, status=%d (argc=%d)", reply->writeInt64(result.j), argc); 

	      // padding for taint values
	      for (int i=0; i<argc; i++) reply->writeInt32((int)0);
	      //for (int i=0; i<argc; i++) ALOGD("wrote padding #%d, status=%d", i, reply->writeInt32((int)0));
	      //ALOGD("wrote result and taint placeholders to reply parcel");
	    } else if (status == ExecutionManager::WAIT4JNI) {
	    	ALOGD("Execution interrupted for JNI; handling JNI call now");
			ALOGD("function=%d, length=%d, param_data=%08x", myExecManager->jniCall.function,
				myExecManager->jniCall.length, (int)*(int*)(myExecManager->jniCall.param_data));
	    	reply->writeInt32(myExecManager->jniCall.function);
	    	reply->writeInt32(myExecManager->jniCall.length);
	    	reply->writeInt32(myExecManager->jniCall.taintsize);
	    	reply->write(myExecManager->jniCall.taint_data, myExecManager->jniCall.taintsize);
	    	reply->write(myExecManager->jniCall.param_data, myExecManager->jniCall.length);
			for (int i=0; i<myExecManager->jniCall.length; i++) reply->writeInt32(0); //placeholder for taint values
	    } else {
	      ALOGD("Unexpected status code from Execution Manager: %d", status);
	      free((void*)argv);
	      return -1;
	    }
	    //free((void*)argv);
	    return NO_ERROR;
	}
	case CALLBACK: {
	    //ALOGD("callback on JNI request");
	    int function, datasize, taintsize;
	    data.readInt32(&function);
	    data.readInt32(&datasize);
	    data.readInt32(&taintsize);
	    ALOGD("callback on JNI request: read function=%d, datasize=%d, taintsize=%d", function, datasize, taintsize);
	    void* rawdata = malloc(datasize); //freed at the end off CALLBACK
	    data.read(rawdata, datasize);
	    ExecutionManager* myExecManager = ((JNIEnvModExt*)jniEnv)->execManager;
	    if (function == myExecManager->jniCall.function) {
			myExecManager->jniCall.length = datasize;
			myExecManager->jniCall.param_data = rawdata;
	    } else {
			ALOGE("Function mismatch! Different JNI calls interfering.");
			free(rawdata);
			return -1;
	    }
	    myExecManager->jniCall.length = datasize;
	    int status = myExecManager->setJniResult(rawdata);
	    ALOGD("Status of ExecutionManager after setJniResult is %s", ExecutionManager::strStatus(status));
	    reply->writeInt32(status);
	    if (status == ExecutionManager::WAIT4JNI) {
			ALOGD("more JNI needed, writing out function and parameters");
	    	reply->writeInt32(myExecManager->jniCall.function);
	    	reply->writeInt32(myExecManager->jniCall.length);
	    	reply->writeInt32(myExecManager->jniCall.taintsize);
	    	reply->write(myExecManager->jniCall.taint_data, myExecManager->jniCall.taintsize);
	    	reply->write(myExecManager->jniCall.param_data, myExecManager->jniCall.length);
	    } else if (status == ExecutionManager::FINISHED) {
	      ALOGD("dvmPlatformInvoke finished. result=%lld", myExecManager->platformInvoke.pResult->j);
	      ALOGD("wrote result, status=%d (argc=%d)",
		reply->writeInt64(myExecManager->platformInvoke.pResult->j), myExecManager->platformInvoke.argc); 
	      for (int i=0; i<myExecManager->platformInvoke.argc; i++) reply->writeInt32((int)0); // padding for taint values		
	    }
		ALOGD("now freeing return data pointer");
	    free(rawdata);
	    return NO_ERROR;
	} break;
        default:
	    aout << "BnWrapper::onTransact: unknown command";
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}
