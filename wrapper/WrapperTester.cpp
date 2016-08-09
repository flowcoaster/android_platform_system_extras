#include <binder/IServiceManager.h>
#include <utils/Timers.h>
#include <ctime>
#include <time.h>
#include "IWrapper.h"
#include "utils.h"

namespace android {
  void sendCmd1(const char *name)
  {
    printf("Try to connect to connect to service: %s ... ", name);

    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder = sm->getService(String16(name));
    sp<IWrapper> wrapper;
    
    if (binder != 0) {
      printf("Success\n");
      wrapper = android::interface_cast<IWrapper>(binder);
      if(wrapper != 0) {
	/* Vector<void*> params;
	Vector<int> sizes; 
	Vector<int> taints; */

	ALOGD("SEND _TAINTCALL_ TO WRAPPER");

	int32_t libRef = wrapper->addLib("/data/test/libhello-jni.so");

	int32_t funcRef = wrapper->addFunc("foo", libRef);

	void *clazz = 0x0;
	int argInfo = 0;
	int argc = 3;
	const uint32_t taints = 0;
	const uint32_t argv = 0;
	const char *shorty = "(LLL)V";
	int32_t libHandle = libRef;
	int32_t funcHandle = funcRef;
	const char* funcName = "foo";

	JValTaint *result = wrapper->taintCall(0, clazz, argInfo, argc, &taints, &argv, shorty, libHandle, funcHandle, funcName);
	// int result = wrapper->call("LIBR", "METH", params, sizes, taints);
	
      }	else {
	ALOGE("ERROR: Cannot communicate to service!");
      }
    } else {
      printf("Failed\n");
      ALOGE("ERROR: Cannot find/bind to service!");
    }
  }

  void sendCmd2(const char *name) {
    printf("Try to connect to connect to service: %s ... ", name);

    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder = sm->getService(String16(name));
    sp<IWrapper> wrapper;
    
    if (binder != 0) {
      printf("Success\n");
      wrapper = android::interface_cast<IWrapper>(binder);
      if(wrapper != 0) {
	ALOGD("SEND _TAINTCALL_ TO WRAPPER");

	int32_t libRef = wrapper->addLib("/data/test/libhello-jni.so");

	int32_t funcRef = wrapper->addFunc("simpleFoo", libRef);

	void *clazz = 0x0;
	int argInfo = 0;
	int argc = 2;
	const uint32_t taints[2] = {0, 0};
	const uint32_t argv[2] = {5, 10};
	const char *shorty = "(II)I";
	int32_t libHandle = libRef;
	int32_t funcHandle = funcRef;
	const char* funcName = "simpleFoo";

	JValTaint *result = wrapper->taintCall(0, clazz, argInfo, argc, taints, argv, shorty, libHandle, funcHandle, funcName);
	// int result = wrapper->call("LIBR", "METH", params, sizes, taints);
	
      }	else {
	ALOGE("ERROR: Cannot communicate to service!");
      }
    } else {
      printf("Failed\n");
      ALOGE("ERROR: Cannot find/bind to service!");
    }
  }
}

using namespace android;

int main(unsigned int argc, char** argv)
{
  if(argc != 2) {
    printf("Wrong number of arguments. Wrapper name required\n");
    return 2;
  }

  sendCmd2(argv[1]);
  return 0;
}
