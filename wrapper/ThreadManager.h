#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <utils/Log.h>
#include <map>

#include "ExecutionManager.h"

namespace android {
 
  class ThreadManager {
  public:
    ThreadManager();
    ~ThreadManager();
    
    ExecutionManager* getExecutionManager(long threadId);
    ExecutionManager* addExecutionManager(long threadId);
    void delExecutionManager(long threadId);
    
  private:
    std::map <long, ExecutionManager*> managers;
  };
}

#endif
