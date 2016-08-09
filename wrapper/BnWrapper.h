#include <IWrapper.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <BpWrapper.h>
#include "ExecutionManager.h"

namespace android {

// Server
class BnWrapper : public BnInterface<IWrapper> {
    public:
    BnWrapper(int pid, int uid);
    virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);

    protected:
	int callerPid, callerUid;
//#ifdef WITH_DALVIK_BINDER_SERVICE
	JNIEnvMod* jniEnv;
//#endif

    private:
	bool checkAuthorization(int pid, int uid);
};

}

