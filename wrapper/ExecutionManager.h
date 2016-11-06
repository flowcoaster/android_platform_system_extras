#ifndef EXECUTION_MANAGER_H
#define EXECUTION_MANAGER_H
#include <semaphore.h>
#include <pthread.h>
#include <utils/Log.h>
#include "JniEnvMod.h"

namespace android {

  union jvalue;
  struct JNIEnvModExt;
  
  typedef struct {
    JNIEnvModExt* jniEnv;
    void* clazz;
    int argInfo;
    int argc;
    const u4* argv;
    const char* shorty;
    void* funcHandle;
    jvalue* pResult;
  } platformInvoke_t;
    
  typedef struct {
    int function;
    int length;
    const void* param_data;
    const void* taint_data;
	int taintsize;
  } jniCall_t;

	//struct sigList_t;

	struct sigList_t {
		jmethodID methodID;
		const char* sig;
		sigList_t* next;
	};
    
	struct arrayList_t {
		jarray jarr;
		int length;
		int dalvikP;
		arrayList_t* next;
	};
    
	struct stringMap_t {
		int dalvikP;
		int length;
		const jchar* wrapperP;
		stringMap_t* next;
	};
    
  static void *runNative(void *platformInvoke);

  class ExecutionManager {
    public:
        enum {
            INIT,
            WAIT4EXEC,
            WAIT4JNI,
            FINISHED,
            ERROR
        };

        sem_t mReplyReady;
        sem_t mJniReady;
        sem_t mExecReady;

        // the thread executing the native function
        pthread_t mNativeThread;

        int mStatus;

        int id;

        // details for the actual native execution
        void *mResult;

		// struct with data for exec (because the thread only gets a void*)
		platformInvoke_t platformInvoke;

		JavaVM* vm;

		// struct with data for calling JNI functions
		jniCall_t jniCall;

		sigList_t* sigList;
		arrayList_t* arrayList;
		stringMap_t* charsList;

        ExecutionManager() {
            ALOGD("================ CREATE EXECUTIONMANAGER ================ ");

            mStatus = INIT;

            id = 0;
			sigList = 0;
			arrayList = 0;
			charsList = 0;

            sem_init(&mReplyReady, 1, 0);
            sem_init(&mJniReady, 1, 0);
            sem_init(&mExecReady, 1, 1);
        }

        ~ExecutionManager() {
            sem_destroy(&mReplyReady);
            sem_destroy(&mJniReady);
        }


        int startExec(jvalue* result);
        int reqJniCall();
        int setJniResult(void *r);

        void signalResult();

        void *getResult();

        void reset();

        void waitForThread();
        bool isReady();
		void addSignatureForMethod(jmethodID methodID, const char* sig);
		const char* getSignatureForMethod(jmethodID methodID);
		int getNumParams(jmethodID methodID);
		int getMethodParamSize(jmethodID methodID);
		void addArray(jarray jarr, int length, int dalvikP);
		arrayList_t* getArrayLength(jarray jarr);
		void addStringChars(int dalvik, int length, const jchar* wrapperP);
		stringMap_t* getDalvikChars(const jchar* wrapperP);
		void deleteCharsEntry(const jchar* wrapperP);

        static const char * strStatus(int status) {
            switch(status) {
            case INIT:
                return "INIT";
            case WAIT4EXEC:
                return "WAIT4EXEC";
            case WAIT4JNI:
                return "WAIT4JNI";
            case FINISHED:
                return "FINISHED";
            case ERROR:
                return "ERROR";
            default:
                return "undefined";
            }
        }
    };
}
#endif
