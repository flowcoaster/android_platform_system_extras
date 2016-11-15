#include "ExecutionManager.h"
#include "ThreadManager.h"

namespace android {
  
#define LOG_TAG "ThreadManager"

  ThreadManager::ThreadManager() {
    ALOGD("ThreadManager created");
  }

  ThreadManager::~ThreadManager() {
    for(std::map<long, ExecutionManager*>::iterator it = managers.begin(); it != managers.end(); it++) {
      delete managers[it->first];
      managers.erase(it->first);
    }
  }

  void ThreadManager::delExecutionManager(long threadId) {
    ALOGD("ThreadManager::delExecutionManager(%ld)", threadId);
    if(managers.find(threadId) != managers.end()) {
      delete managers[threadId];
      managers.erase(threadId);
    }
  }

  ExecutionManager* ThreadManager::getExecutionManager(long threadId) {
    ALOGD("ThreadManager::getExecutionManager(%ld)", threadId);
    if(managers.find(threadId) == managers.end())
      return NULL;
    else
      return managers[threadId];
  }

  ExecutionManager* ThreadManager::addExecutionManager(long threadId) {
    ALOGD("ThreadManager::addExecutionManager(%ld)", threadId);
    if(managers.find(threadId) == managers.end())
      managers[threadId] = new ExecutionManager();
  
    return managers[threadId];
  }

}
