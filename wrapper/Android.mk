wrapper_sources := \
	IWrapper.cpp \
	BnWrapper.cpp \
	Wrapper.cpp \
	utils.cpp \
	JniRemote.cpp \
	CallEabi.S \
	ExecutionManager.cpp \
#	IDalvikJni.cpp \
#	BpDalvikJni.cpp \

dispatcher_sources := \
	utils.cpp \
	BpWrapper.cpp \
	IWrapper.cpp \
	Dispatcher.cpp \
	CallEabi.S \
#	BnDalvikJni.cpp \
#	BpDalvikJni.cpp \
#	DalvikJni.cpp \
#	IDalvikJni.cpp \

wrapper_tester := \
        WrapperTester.cpp \
        IWrapper.cpp \
        BpWrapper.cpp \
	utils.cpp \

test_sources := \
	Starter.cpp \
	DalvikJni.cpp \
	BnDalvikJni.cpp \
	IDalvikJni.cpp \
	BpDalvikJni.cpp \

djnitester_sources := \
	utils.cpp \
	DalvikJniTester.cpp \
	BnDalvikJni.cpp \
	BpDalvikJni.cpp \
	DalvikJni.cpp \
	IDalvikJni.cpp \

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_FLAGS += -DWITH_DALVIK_BINDER_SERVICE=1 -std=c++11
LOCAL_MODULE := tgwrapper
LOCAL_MODULE_TAGS := debug
LOCAL_SRC_FILES := $(wrapper_sources)
LOCAL_SHARED_LIBRARIES := libutils libcutils libbinder libdl
LOCAL_C_INCLUDES += frameworks/base/include system/core/include frameworks/native/include bionic/libc/include bionic/libc/arch-arm/include libnativehelper/include
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_FLAGS += -DWITH_DALVIK_BINDER_SERVICE=1
LOCAL_MODULE := dispatcher
LOCAL_MODULE_TAGS := debug
LOCAL_SRC_FILES := $(dispatcher_sources)
LOCAL_SHARED_LIBRARIES := libbinder libutils libcutils libdl
LOCAL_C_INCLUDES += frameworks/base/include system/core/include frameworks/native/include bionic/libc/include libnativehelper/include
include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_C_FLAGS += -DWITH_DALVIK_BINDER_SERVICE=1
#LOCAL_MODULE := dvmjnibindertest
#LOCAL_MODULE_TAGS := debug
#LOCAL_SRC_FILES := $(test_sources)
#LOCAL_SHARED_LIBRARIES := libbinder libutils libcutils libdl
#LOCAL_C_INCLUDES += frameworks/base/include system/core/include frameworks/native/include bionic/libc/include libnativehelper/include
#include $(BUILD_EXECUTABLE)

#include $(CLEAR_VARS)
#LOCAL_C_FLAGS += -DWITH_DALVIK_BINDER_SERVICE=1
#LOCAL_MODULE := djnitester
#LOCAL_MODULE_TAGS := debug
#LOCAL_SRC_FILES := $(djnitester_sources)
#LOCAL_SHARED_LIBRARIES := libbinder libutils libcutils libdl
#LOCAL_C_INCLUDES += frameworks/base/include system/core/include frameworks/native/include bionic/libc/include libnativehelper/include
#include $(BUILD_EXECUTABLE)

#include $(CLEAR_VARS)
#LOCAL_C_FLAGS += -DWITH_DALVIK_BINDER_SERVICE=1
#LOCAL_MODULE := testtgw
#LOCAL_MODULE_TAGS := debug
#LOCAL_SRC_FILES := $(wrapper_tester)
#LOCAL_SHARED_LIBRARIES := libutils libcutils libbinder libdl
#LOCAL_C_INCLUDES += frameworks/base/include system/core/include frameworks/native/include bionic/libc/include bionic/libc/arch-arm/include libnativehelper/include
#include $(BUILD_EXECUTABLE)
