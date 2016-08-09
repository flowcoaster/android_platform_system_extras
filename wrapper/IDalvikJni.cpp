//#ifdef WITH_DALVIK_BINDER_SERVICE

#include <binder/Parcel.h>
#include <utils/Errors.h> // status_t
#include "IDalvikJni.h"
#include "BpDalvikJni.h"

namespace android {

/*jint BnDalvikJni::getVersion(JNIEnvMod* env) {
	ALOGD("Warning: Server should not do this");	
	return -1;
}*/


	IMPLEMENT_META_INTERFACE(DalvikJni, "DalvikJni");
    /*const android::String16 IDalvikJni::descriptor("DvmJniCallbackService");             
    const android::String16& IDalvikJni::getInterfaceDescriptor() const {              
        return IDalvikJni::descriptor;                                
    }                                                                   
    android::sp<IDalvikJni> IDalvikJni::asInterface(                
            const android::sp<android::IBinder>& obj)                   
    {                                                                   
        android::sp<IDalvikJni> intr;                                 
        if (obj != NULL) {                                              
            intr = static_cast<IDalvikJni*>(                          
                obj->queryLocalInterface(                               
                        IDalvikJni::descriptor).get());               
            if (intr == NULL) {                                         
                intr = new BpDalvikJni(obj);                          
            }                                                           
        }                                                               
        return intr;                                                    
    }                                                                   
    IDalvikJni::IDalvikJni() { }                                    
    IDalvikJni::~IDalvikJni() { }    */                              

}

//#endif
