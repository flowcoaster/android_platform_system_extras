#include <stdint.h>
#include "IWrapper.h"
#include "IDalvikJni.h"
#include <nativehelper/jni.h>

namespace android {

sp<IDalvikJni> getDalvikJniService(const char* name);
//sp<IWrapper> getDalvikJniService(const char* name);

}
