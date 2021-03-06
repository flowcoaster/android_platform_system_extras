#include "IWrapper.h"
#include "BnWrapper.h"

namespace android {

class Wrapper : public BnWrapper {
    public:
    Wrapper(int pid, int uid) : BnWrapper(pid, uid) {
    }

    virtual int32_t call(char* library, char* method, Vector<void*> params, Vector<int> sizeofparam, Vector<int> taints) {
        INFO("Wrapper::call()");
        return 0;
    }

    virtual int32_t addLib(const char* pathName) {
        INFO("Wrapper::addLib(pathName=%s)", pathName);
	return 0;
    }

	virtual int32_t addFunc(const char* funcName, int32_t libRef) {
        INFO("Wrapper::addFunc(funcName=%s, libRef=%d)", funcName, libRef);
	return 0;
    }

    virtual JValTaint* taintCall(JNIEnvMod* pEnv, int clazz, int argInfo, int argc, const uint32_t* taints,
                                 const uint32_t* argv, const char* shorty, int32_t libHandle, int32_t funcHandle, const char* funcName, u4 threadId) {
        INFO("Wrapper::taintCall()");
	return 0;
	}

    virtual int callback(int function, int length, void* data, int taintsize, JValTaint* res) {
	INFO("Wrapper::callBack()");
	return 0;
    }

	virtual int32_t changeFunc(int32_t oldHandle, int32_t newHandle) {
		INFO("Wrapper::changeFunc()");
		return 0;
	}

	virtual void setServiceState(bool online) {
		INFO("Wrapper::setServiceState()");
	}

	virtual void setServiceName(char* name) {
		INFO("Wrapper::setServiceName()");
	}

	virtual void setWrapperPointer(sp<IWrapper> wrapperPointer) {
		INFO("Wrapper::setWrapperPointer()");
	}

	virtual bool restartService() {
		INFO("Wrapper::restartService()");
		return false;
	}
};

int androidmain(int argc, char **argv);

} //namespace android
