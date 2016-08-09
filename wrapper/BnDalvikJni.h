#include <binder/Parcel.h>
#include <binder/IInterface.h>
#include <IDalvikJni.h>

namespace android {

//Server
class BnDalvikJni : public BnInterface<IDalvikJni> {
	public:
	virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
	//virtual jint getVersion(JNIEnvMod* env);
};

}

