#include <utils/Log.h>
// #include <stdio.h>
// #include <unistd.h>
#include "ExecutionManager.h"
#include "IWrapper.h"

#ifndef FLOWCOASTER_DEBUG
	#define ALOGD(...) void();
#endif

namespace android {

    #define LOG_TAG "ExecutionManager"

    static void *runNative(void *platformInvoke) {
      platformInvoke_t* pI = (platformInvoke_t *) platformInvoke;
      ALOGD("call to dvmPlatformInvoke(env=%08x, clazz=%08x, argInfo=%08x, argc=%08x, argv=%08x, shorty=%s, funcHandle=%08x, pResult=%08x",
			(int)pI->jniEnv, (int)pI->clazz, pI->argInfo, pI->argc, (int)pI->argv, pI->shorty, (int)pI->funcHandle, (int)pI->pResult);
      //ALOGD("argv[0]: %d", pI->argv[0]);
      //ALOGD("argv[1]: %d", pI->argv[1]);

      dvmPlatformInvoke(pI->jniEnv, 
			pI->clazz, 
			pI->argInfo, 
			pI->argc, 
			pI->argv, 
			pI->shorty, 
			pI->funcHandle,
			pI->pResult);

      ALOGD("STILL ALIVE AFTER PLATFORM INVOKE");

      pI->jniEnv->execManager->signalResult();

      return (void*)(pI->pResult);
    };

    int ExecutionManager::startExec(jvalue* result) {
        // sem_wait(&mExecReady);

        mStatus = WAIT4EXEC;

        mResult = result;

        id++;
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		//ALOGD("argv[0]: %d", platformInvoke.argv[0]);
		//ALOGD("argv[1]: %d", platformInvoke.argv[1]);

        int error = pthread_create(&mNativeThread, &attr, runNative, (void*)&platformInvoke);
        if(error) {
            ALOGD("OHOH");
        }
        pthread_setname_np(mNativeThread, "startExec");

        ALOGD("ExecutionManager::startExec waits for result");

        // wait until some reply to the execution request is ready
        sem_wait(&mReplyReady);

        /*if(mStatus == FINISHED) {
            waitForThread();
        }*/

        ALOGD("ExecutionManager::startExec continues with status %d", mStatus);

        return mStatus;
    }

    void ExecutionManager::waitForThread() {
        // sleep(0);
        id--;
        ALOGD("WAIT FOR THREAD WITH ID %d", id + 1);
        // pthread_join(mNativeThread, NULL);
        ALOGD("JOINED THREAD WITH ID %d", id + 1);
        // sem_post(&mExecReady);
    }

    void ExecutionManager::reset() {
        ALOGD("Current state: %s", strStatus(mStatus));
    }

    void ExecutionManager::signalResult() {
        mStatus = FINISHED;
        sem_post(&mReplyReady);
    }

    void* ExecutionManager::getResult() {
        return mResult;
    }

    int ExecutionManager::reqJniCall() {
        mStatus = WAIT4JNI;

        //ALOGD("ExecutionManager makes startExec to continue");

        // prepare the JNI object for the request to the client APP

        ALOGD("ExecutionManager waits for JNI call result");

        // wait for possibly incoming message (post another sema)
        // signal to actually send the reply
        sem_post(&mReplyReady);

        // wait for the jni call to finish
        sem_wait(&mJniReady);

        ALOGD("ExecutionManager continues to execute after JNI Call");

        mStatus = WAIT4EXEC;

        return mStatus;
    }

    int ExecutionManager::setJniResult(void *r) {
        if(mStatus != WAIT4JNI) {
            ALOGE("ExecutionManager does not wait for JNI result!");
            return ERROR;
        } else {
            ALOGD("ExecutionManager receives JNI result!");

            // TODO: set the result of the JNI execution

            //ALOGD("Signal the reqJniCall to continue");

            // signal native thread that JNI has been received
            sem_post(&mJniReady);

            //ALOGD("Continue waiting for the result of the main task (status: %s)", strStatus(mStatus));

            sem_wait(&mReplyReady);

            //if(mStatus == FINISHED) waitForThread();

            ALOGD("Task seems to be continuing: %s", strStatus(mStatus));

            return mStatus;
        }

        return ERROR;
    }

	void ExecutionManager::addSignatureForMethod(jmethodID methodID, const char* sig) {
      ALOGD("addSignatureForMethod(methodID=%08x, sig=%s)", (int)methodID, sig);
		sigList_t* newSig = (sigList_t*)malloc(sizeof(sigList_t));
		if (sigList == 0) { //no method added yet
          //*newSig = {methodID, sig, 0};
          newSig->methodID = methodID;
          newSig->sig = sig;
          newSig->next = NULL;
		} else { // push in front of other methods
          newSig->methodID = methodID;
          newSig->sig = sig;
          newSig->next = sigList;
		}
		sigList = newSig;
		ALOGD("addSignatureForMethod: %08x: methodID=%08x, sig=%s, next=%08x",
			(int)sigList, (int)sigList->methodID, sigList->sig, (int)sigList->next);
	}

	const char* ExecutionManager::getSignatureForMethod(jmethodID methodID) {
		if (sigList == 0) return NULL;
		sigList_t s = *sigList;
		while (s.methodID != methodID && s.next != 0) s = *(s.next);
		if (s.methodID == methodID) return s.sig;
		else return NULL;
	}

	int ExecutionManager::getNumParams(jmethodID methodID) {
		if (sigList == 0) return -1;
		sigList_t s = *sigList;
		while (s.methodID != methodID && s.next != 0)
			s = *(s.next);
		if (s.methodID == methodID) {
			int i = 0, size = 0;
			ALOGD("found signature for methodID %08x: %s", (int)methodID, s.sig);
			//read through method params, ignore return type at the end
			while (s.sig[i] != ')') {
				switch (s.sig[i]) {
					case '(': break;
					case 'L':
					case 'Z':
					case 'B':
					case 'C':
					case 'S':
					case 'I':
					case 'D':
					case 'J': size ++; break;
					default:
						ALOGE("Unexpected character in signature string: <%c>", s.sig[i]);
						break;
				}
				i++;
			}
			ALOGD("Method %08x with sig %s has %d params", (int)methodID, s.sig, size);
			return size;
		} else return -1;
	}

	int ExecutionManager::getMethodParamSize(jmethodID methodID) {
		if (sigList == 0) return -1;
		sigList_t s = *sigList;
		ALOGD("getMethodParamSize: head of sigList %08x->%08x", (int)&s, s);
		while (s.methodID != methodID && s.next != 0) {
			ALOGD("searching %08x != %08x, next is %08x", (int)s.methodID, (int)methodID, (int)s.next);
			s = *(s.next);
		}
		if (s.methodID == methodID) {
			int i = 0, size = 0;
			ALOGD("found signature for methodID %08x: %s", (int)methodID, s.sig);
			//read through method params, ignore return type at the end
			while (s.sig[i] != ')') {
				switch (s.sig[i]) {
					case '(': break;
					case 'L': size += sizeof(jobject); break;
					case 'Z':
					case 'B':
					case 'C':
					case 'S':
					case 'I': size += sizeof(jint); break;
					case 'D':
					case 'J': size += sizeof(jdouble); break;
					default:
						ALOGE("Unexpected character in signature string: <%c>", s.sig[i]);
						break;
				}
				i++;
			}
			ALOGD("Method %08x with sig %s has size %d", (int)methodID, s.sig, size);
			return size;
		} else return -1;
	}

	void ExecutionManager::addArray(jarray jarr, int length, int dalvikP) {
		ALOGD("addArray(jarray=%08x, length=%d, dalvikP=%08x)", (int)jarr, length, dalvikP);
		arrayList_t* newArray = (arrayList_t*)malloc(sizeof(arrayList_t));
		if (arrayList == 0) { //no array added yet
          newArray->jarr = jarr;
          newArray->length = length;
          newArray->dalvikP = dalvikP;
          newArray->next = NULL;
		} else { // push in front of other arrays
          newArray->jarr = jarr;
          newArray->length = length;
          newArray->dalvikP = dalvikP;
          newArray->next = arrayList;
		}
		arrayList = newArray;
	}

	arrayList_t* ExecutionManager::getArrayLength(jarray jarr) {
		if (arrayList == 0) return NULL;
		arrayList_t* s = arrayList;
		while (s->jarr != jarr && s->next != 0) s = s->next;
		if (s->jarr == jarr) return s;
		else return NULL;
	}

	void ExecutionManager::addStringChars(int dalvikP, int length, const jchar* wrapperP) {
		ALOGD("addStringChars(dalvikP=%08x, length=%08x, wrapperP=%s)", dalvikP, length, (const char*)wrapperP);
		stringMap_t* newCharsEntry = (stringMap_t*)malloc(sizeof(stringMap_t));
		ALOGD("sizeof(stringMap_t)=%d", sizeof(stringMap_t));
		if (charsList == 0) { //no method added yet
          newCharsEntry->dalvikP = dalvikP;
          newCharsEntry->length = length;
          newCharsEntry->wrapperP = wrapperP;
          newCharsEntry->next = NULL;
		} else { // push in front of other methods
          newCharsEntry->dalvikP = dalvikP;
          newCharsEntry->length = length;
          newCharsEntry->wrapperP = wrapperP;
          newCharsEntry->next = charsList;
		}
		charsList = newCharsEntry;
		ALOGD("added string map entry with dalvikP=%08x, length=%d, wrapperP=%s, next=%08x",
			(int)(charsList->dalvikP), charsList->length, (const char*)charsList->wrapperP, (int)(charsList->next));
	}

	stringMap_t* ExecutionManager::getDalvikChars(const jchar* wrapperP) {
		ALOGD("getDalvikChars(wrapperP=%08x)", (int)wrapperP);
		if (charsList == 0) return NULL;
		stringMap_t* s = charsList;
		while (s->wrapperP != wrapperP && s->next != 0) s = s->next;
		if (s->wrapperP == wrapperP) {
			ALOGD("found entry with dalvikP=%08x, length=%d, wrapperP=%s, next=%08x",
				(int)s->dalvikP, s->length, (const char*)s->wrapperP, (int)s->next);
			ALOGD("returning reference %08x", (int)s);
			return s;
		} else return NULL;
	}

	void ExecutionManager::deleteCharsEntry(const jchar* wrapperP) {
		ALOGD("deleteCharsEntry(wrapperP=%08x)", (int)wrapperP);
		if (charsList == 0) return;
		ALOGD("charsList contains at least one element");
		if (charsList->next == 0) {
			charsList = 0;
			return;
		}
		ALOGD("charsList contains more than one element");
		stringMap_t* s = charsList;
		if (s->wrapperP == wrapperP) {
			ALOGD("entry is at head of list");
			charsList = s->next;
			return;
		}
		while (s->next != 0 && s->next->wrapperP != wrapperP) {
			ALOGD("jumping to next=%08x", (int)s->next);
			s = s->next;
		}
		if (s->next->wrapperP == wrapperP) {
			stringMap_t* temp = s->next->next;
			free(s->next);
			s->next = temp;
			return;
		}
		ALOGE("deleteCharsEntry: entry %s not found", (const char*)wrapperP);
	}

    /*void *mainLoop(void *) {
    ExecutionManager em;
    int result = 0;

    em.setResult(&result);

    printf("Start the execution");
    while(true) {
    int status = em.startExec(runNative);

    printf("Status: %s\n", ExecutionManager::strStatus(status));

    // sleep(1);

    status = em.setJniResult(NULL);

    status = em.setJniResult(NULL);

    printf("Final: %s\n", ExecutionManager::strStatus(status));
    printf("\n");
    // sleep(1);
}
}

int main() {
pthread_t mainThread;
pthread_create(&mainThread, NULL, mainLoop, NULL);
pthread_join(mainThread, NULL);

return 0;
}*/
}
