#include <binder/IBinder.h>
#include <IDalvikJni.h>

namespace android {

//Client
class BpDalvikJni : public BpInterface<IDalvikJni> {
	public:
	BpDalvikJni(const sp<IBinder>& impl);
	jint getVersion(JNIEnvMod* env);
};

}
