#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <utils/Log.h>
#include <map>

#include "JniEnvMod.h"

namespace android {
 
  class ThreadManager {
  public:
    ThreadManager();
    ~ThreadManager();
    
    JNIEnvModExt* getJniEnv(long threadId);
    JNIEnvModExt* addJniEnv(long threadId);
    void delJniEnv(long threadId);

  private:
    std::map <long, JNIEnvModExt*> jniEnvs;
  };
}

#endif
