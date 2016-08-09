#include <binder/Parcel.h>
#include <binder/IPCThreadState.h> // required for obtaining caller pid
#include <binder/IInterface.h>
#include <utils/TextOutput.h> // for android 4.3 and below
//#include <binder/TextOutput.h> // for android 4.4 and above
#include <dlfcn.h> // for loading libraries
#include "IWrapper.h"
#include "BpWrapper.h"
//#include "utils.h"
#include <utils/String16.h>

namespace android {

IMPLEMENT_META_INTERFACE(Wrapper, "TaintgrindWrapper");

} // namespace android
