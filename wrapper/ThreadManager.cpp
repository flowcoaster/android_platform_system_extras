#include "JniEnvMod.h"
#include "ExecutionManager.h"
#include "ThreadManager.h"

namespace android {
  
#define LOG_TAG "ThreadManager"

  ThreadManager::ThreadManager() {
    ALOGD("ThreadManager created");
  }
  
  ThreadManager::~ThreadManager() {
    for(std::map<long, JNIEnvModExt*>::iterator it = jniEnvs.begin(); it != jniEnvs.end(); it++) {
      free(jniEnvs[it->first]->execManager->vm);
      delete jniEnvs[it->first]->execManager;
      free(jniEnvs[it->first]);
    }
  }

  // TODO: REVIEW TO REMOVE ALL GENERATED OBJECTS
  void ThreadManager::delJniEnv(long threadId) {
    ALOGD("ThreadManager::delJniEnv(%ld)", threadId);
    if(jniEnvs.find(threadId) != jniEnvs.end()) {
      free(jniEnvs[threadId]->execManager->vm);
      delete jniEnvs[threadId]->execManager;
      free(jniEnvs[threadId]);
      jniEnvs.erase(threadId);
    }
  }

  JNIEnvModExt* ThreadManager::getJniEnv(long threadId) {
    ALOGD("ThreadManager::getJniEnv(%ld)", threadId);
    if(jniEnvs.find(threadId) == jniEnvs.end())
      return addJniEnv(threadId);
    else
      return jniEnvs[threadId];
  }

  JNIEnvModExt* ThreadManager::addJniEnv(long threadId) {
    ALOGD("ThreadManager::addJniEnv(%ld)", threadId);
    if(jniEnvs.find(threadId) == jniEnvs.end()) {
      jniEnvs[threadId] = dvmCreateJNIEnvMod(new ExecutionManager());
      // TODO: Is the vm pointer in the ExecutionManager used ever
      wrCreateJavaVM(jniEnvs[threadId]);
    }
  
    return jniEnvs[threadId];
  }

}
