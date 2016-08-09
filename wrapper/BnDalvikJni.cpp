#include "BnDalvikJni.h"

namespace android {

status_t BnDalvikJni::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags) {
	return NO_ERROR;
}

}
