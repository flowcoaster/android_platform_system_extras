#ifndef JNIENVMOD_H
#define JNIENVMOD_H

//#ifdef WITH_DALVIK_BINDER_SERVICE

#include <stdarg.h>	// for va_list etc.
#include <stdint.h> // for int types
//#include "ExecutionManager.h"
#include "utils.h"

namespace android {

/* Primitive types that match up with Java equivalents. */
typedef uint8_t  jboolean; /* unsigned 8 bits */
typedef int8_t   jbyte;    /* signed 8 bits */
typedef uint16_t jchar;    /* unsigned 16 bits */
typedef int16_t  jshort;   /* signed 16 bits */
typedef int32_t  jint;     /* signed 32 bits */
typedef int64_t  jlong;    /* signed 64 bits */
typedef float    jfloat;   /* 32-bit IEEE 754 */
typedef double   jdouble;  /* 64-bit IEEE 754 */

/* "cardinal indices and sizes" */
typedef jint     jsize;

/*
 * Reference types, in C++
 */
class _jobject {};
class _jclass : public _jobject {};
class _jstring : public _jobject {};
class _jarray : public _jobject {};
class _jobjectArray : public _jarray {};
class _jbooleanArray : public _jarray {};
class _jbyteArray : public _jarray {};
class _jcharArray : public _jarray {};
class _jshortArray : public _jarray {};
class _jintArray : public _jarray {};
class _jlongArray : public _jarray {};
class _jfloatArray : public _jarray {};
class _jdoubleArray : public _jarray {};
class _jthrowable : public _jobject {};

typedef _jobject*       jobject;
typedef _jclass*        jclass;
typedef _jstring*       jstring;
typedef _jarray*        jarray;
typedef _jobjectArray*  jobjectArray;
typedef _jbooleanArray* jbooleanArray;
typedef _jbyteArray*    jbyteArray;
typedef _jcharArray*    jcharArray;
typedef _jshortArray*   jshortArray;
typedef _jintArray*     jintArray;
typedef _jlongArray*    jlongArray;
typedef _jfloatArray*   jfloatArray;
typedef _jdoubleArray*  jdoubleArray;
typedef _jthrowable*    jthrowable;
typedef _jobject*       jweak;

struct _jfieldID;                       /* opaque structure */
typedef struct _jfieldID* jfieldID;     /* field IDs */

struct _jmethodID;                      /* opaque structure */
typedef struct _jmethodID* jmethodID;   /* method IDs */

typedef union jvalue {
    jboolean    z;
    jbyte       b;
    jchar       c;
    jshort      s;
    jint        i;
    jlong       j;
    jfloat      f;
    jdouble     d;
    jobject     l;
} jvalue;

typedef enum jobjectRefType {
    JNIInvalidRefType = 0,
    JNILocalRefType = 1,
    JNIGlobalRefType = 2,
    JNIWeakGlobalRefType = 3
} jobjectRefType;

typedef struct {
    const char* name;
    const char* signature;
    void*       fnPtr;
} JNINativeMethod;

class ExecutionManager;

struct JNIEnvModExt {
    const struct JNINativeInterfaceMod* funcTable;     /* must be first */

    ExecutionManager* execManager;
};


//copied from nativehelper/jni.h
struct _JNIEnvMod;
struct _JavaVM;
typedef const struct JNINativeInterfaceMod* C_JNIEnvMod;

typedef _JNIEnvMod JNIEnvMod;
typedef _JavaVM JavaVM;


/*
 * Table of interface function pointers.
 */
struct JNINativeInterfaceMod {
    void*       reserved0;
    void*       reserved1;
    void*       reserved2;
    void*       reserved3;

    jint        (*GetVersion)(JNIEnvMod *);

    jclass      (*DefineClass)(JNIEnvMod*, const char*, jobject, const jbyte*,
                        jsize);
    jclass      (*FindClass)(JNIEnvMod*, const char*);

    jmethodID   (*FromReflectedMethod)(JNIEnvMod*, jobject);
    jfieldID    (*FromReflectedField)(JNIEnvMod*, jobject);
    /* spec doesn't show jboolean parameter */
    jobject     (*ToReflectedMethod)(JNIEnvMod*, jclass, jmethodID, jboolean);

    jclass      (*GetSuperclass)(JNIEnvMod*, jclass);
    jboolean    (*IsAssignableFrom)(JNIEnvMod*, jclass, jclass);

    /* spec doesn't show jboolean parameter */
    jobject     (*ToReflectedField)(JNIEnvMod*, jclass, jfieldID, jboolean);

    jint        (*Throw)(JNIEnvMod*, jthrowable);
    jint        (*ThrowNew)(JNIEnvMod *, jclass, const char *);
    jthrowable  (*ExceptionOccurred)(JNIEnvMod*);
    void        (*ExceptionDescribe)(JNIEnvMod*);
    void        (*ExceptionClear)(JNIEnvMod*);
    void        (*FatalError)(JNIEnvMod*, const char*);

    jint        (*PushLocalFrame)(JNIEnvMod*, jint);
    jobject     (*PopLocalFrame)(JNIEnvMod*, jobject);

    jobject     (*NewGlobalRef)(JNIEnvMod*, jobject);
    void        (*DeleteGlobalRef)(JNIEnvMod*, jobject);
    void        (*DeleteLocalRef)(JNIEnvMod*, jobject);
    jboolean    (*IsSameObject)(JNIEnvMod*, jobject, jobject);

    jobject     (*NewLocalRef)(JNIEnvMod*, jobject);
    jint        (*EnsureLocalCapacity)(JNIEnvMod*, jint);

    jobject     (*AllocObject)(JNIEnvMod*, jclass);
    jobject     (*NewObject)(JNIEnvMod*, jclass, jmethodID, ...);
    jobject     (*NewObjectV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jobject     (*NewObjectA)(JNIEnvMod*, jclass, jmethodID, jvalue*);

    jclass      (*GetObjectClass)(JNIEnvMod*, jobject);
    jboolean    (*IsInstanceOf)(JNIEnvMod*, jobject, jclass);
    jmethodID   (*GetMethodID)(JNIEnvMod*, jclass, const char*, const char*);

    jobject     (*CallObjectMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jobject     (*CallObjectMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jobject     (*CallObjectMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    jboolean    (*CallBooleanMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jboolean    (*CallBooleanMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jboolean    (*CallBooleanMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    jbyte       (*CallByteMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jbyte       (*CallByteMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jbyte       (*CallByteMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    jchar       (*CallCharMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jchar       (*CallCharMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jchar       (*CallCharMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    jshort      (*CallShortMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jshort      (*CallShortMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jshort      (*CallShortMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    jint        (*CallIntMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jint        (*CallIntMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jint        (*CallIntMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    jlong       (*CallLongMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jlong       (*CallLongMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jlong       (*CallLongMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    jfloat      (*CallFloatMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jfloat      (*CallFloatMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jfloat      (*CallFloatMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    jdouble     (*CallDoubleMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    jdouble     (*CallDoubleMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    jdouble     (*CallDoubleMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);
    void        (*CallVoidMethod)(JNIEnvMod*, jobject, jmethodID, ...);
    void        (*CallVoidMethodV)(JNIEnvMod*, jobject, jmethodID, va_list);
    void        (*CallVoidMethodA)(JNIEnvMod*, jobject, jmethodID, jvalue*);

    jobject     (*CallNonvirtualObjectMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jobject     (*CallNonvirtualObjectMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    jobject     (*CallNonvirtualObjectMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);
    jboolean    (*CallNonvirtualBooleanMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jboolean    (*CallNonvirtualBooleanMethodV)(JNIEnvMod*, jobject, jclass,
                         jmethodID, va_list);
    jboolean    (*CallNonvirtualBooleanMethodA)(JNIEnvMod*, jobject, jclass,
                         jmethodID, jvalue*);
    jbyte       (*CallNonvirtualByteMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jbyte       (*CallNonvirtualByteMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    jbyte       (*CallNonvirtualByteMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);
    jchar       (*CallNonvirtualCharMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jchar       (*CallNonvirtualCharMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    jchar       (*CallNonvirtualCharMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);
    jshort      (*CallNonvirtualShortMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jshort      (*CallNonvirtualShortMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    jshort      (*CallNonvirtualShortMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);
    jint        (*CallNonvirtualIntMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jint        (*CallNonvirtualIntMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    jint        (*CallNonvirtualIntMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);
    jlong       (*CallNonvirtualLongMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jlong       (*CallNonvirtualLongMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    jlong       (*CallNonvirtualLongMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);
    jfloat      (*CallNonvirtualFloatMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jfloat      (*CallNonvirtualFloatMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    jfloat      (*CallNonvirtualFloatMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);
    jdouble     (*CallNonvirtualDoubleMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    jdouble     (*CallNonvirtualDoubleMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    jdouble     (*CallNonvirtualDoubleMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);
    void        (*CallNonvirtualVoidMethod)(JNIEnvMod*, jobject, jclass,
                        jmethodID, ...);
    void        (*CallNonvirtualVoidMethodV)(JNIEnvMod*, jobject, jclass,
                        jmethodID, va_list);
    void        (*CallNonvirtualVoidMethodA)(JNIEnvMod*, jobject, jclass,
                        jmethodID, jvalue*);

    jfieldID    (*GetFieldID)(JNIEnvMod*, jclass, const char*, const char*);

    jobject     (*GetObjectField)(JNIEnvMod*, jobject, jfieldID);
    jboolean    (*GetBooleanField)(JNIEnvMod*, jobject, jfieldID);
    jbyte       (*GetByteField)(JNIEnvMod*, jobject, jfieldID);
    jchar       (*GetCharField)(JNIEnvMod*, jobject, jfieldID);
    jshort      (*GetShortField)(JNIEnvMod*, jobject, jfieldID);
    jint        (*GetIntField)(JNIEnvMod*, jobject, jfieldID);
    jlong       (*GetLongField)(JNIEnvMod*, jobject, jfieldID);
    jfloat      (*GetFloatField)(JNIEnvMod*, jobject, jfieldID);
    jdouble     (*GetDoubleField)(JNIEnvMod*, jobject, jfieldID);

    void        (*SetObjectField)(JNIEnvMod*, jobject, jfieldID, jobject);
    void        (*SetBooleanField)(JNIEnvMod*, jobject, jfieldID, jboolean);
    void        (*SetByteField)(JNIEnvMod*, jobject, jfieldID, jbyte);
    void        (*SetCharField)(JNIEnvMod*, jobject, jfieldID, jchar);
    void        (*SetShortField)(JNIEnvMod*, jobject, jfieldID, jshort);
    void        (*SetIntField)(JNIEnvMod*, jobject, jfieldID, jint);
    void        (*SetLongField)(JNIEnvMod*, jobject, jfieldID, jlong);
    void        (*SetFloatField)(JNIEnvMod*, jobject, jfieldID, jfloat);
    void        (*SetDoubleField)(JNIEnvMod*, jobject, jfieldID, jdouble);

    jmethodID   (*GetStaticMethodID)(JNIEnvMod*, jclass, const char*, const char*);

    jobject     (*CallStaticObjectMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jobject     (*CallStaticObjectMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jobject     (*CallStaticObjectMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);
    jboolean    (*CallStaticBooleanMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jboolean    (*CallStaticBooleanMethodV)(JNIEnvMod*, jclass, jmethodID,
                        va_list);
    jboolean    (*CallStaticBooleanMethodA)(JNIEnvMod*, jclass, jmethodID,
                        jvalue*);
    jbyte       (*CallStaticByteMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jbyte       (*CallStaticByteMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jbyte       (*CallStaticByteMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);
    jchar       (*CallStaticCharMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jchar       (*CallStaticCharMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jchar       (*CallStaticCharMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);
    jshort      (*CallStaticShortMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jshort      (*CallStaticShortMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jshort      (*CallStaticShortMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);
    jint        (*CallStaticIntMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jint        (*CallStaticIntMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jint        (*CallStaticIntMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);
    jlong       (*CallStaticLongMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jlong       (*CallStaticLongMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jlong       (*CallStaticLongMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);
    jfloat      (*CallStaticFloatMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jfloat      (*CallStaticFloatMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jfloat      (*CallStaticFloatMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);
    jdouble     (*CallStaticDoubleMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    jdouble     (*CallStaticDoubleMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    jdouble     (*CallStaticDoubleMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);
    void        (*CallStaticVoidMethod)(JNIEnvMod*, jclass, jmethodID, ...);
    void        (*CallStaticVoidMethodV)(JNIEnvMod*, jclass, jmethodID, va_list);
    void        (*CallStaticVoidMethodA)(JNIEnvMod*, jclass, jmethodID, jvalue*);

    jfieldID    (*GetStaticFieldID)(JNIEnvMod*, jclass, const char*,
                        const char*);

    jobject     (*GetStaticObjectField)(JNIEnvMod*, jclass, jfieldID);
    jboolean    (*GetStaticBooleanField)(JNIEnvMod*, jclass, jfieldID);
    jbyte       (*GetStaticByteField)(JNIEnvMod*, jclass, jfieldID);
    jchar       (*GetStaticCharField)(JNIEnvMod*, jclass, jfieldID);
    jshort      (*GetStaticShortField)(JNIEnvMod*, jclass, jfieldID);
    jint        (*GetStaticIntField)(JNIEnvMod*, jclass, jfieldID);
    jlong       (*GetStaticLongField)(JNIEnvMod*, jclass, jfieldID);
    jfloat      (*GetStaticFloatField)(JNIEnvMod*, jclass, jfieldID);
    jdouble     (*GetStaticDoubleField)(JNIEnvMod*, jclass, jfieldID);

    void        (*SetStaticObjectField)(JNIEnvMod*, jclass, jfieldID, jobject);
    void        (*SetStaticBooleanField)(JNIEnvMod*, jclass, jfieldID, jboolean);
    void        (*SetStaticByteField)(JNIEnvMod*, jclass, jfieldID, jbyte);
    void        (*SetStaticCharField)(JNIEnvMod*, jclass, jfieldID, jchar);
    void        (*SetStaticShortField)(JNIEnvMod*, jclass, jfieldID, jshort);
    void        (*SetStaticIntField)(JNIEnvMod*, jclass, jfieldID, jint);
    void        (*SetStaticLongField)(JNIEnvMod*, jclass, jfieldID, jlong);
    void        (*SetStaticFloatField)(JNIEnvMod*, jclass, jfieldID, jfloat);
    void        (*SetStaticDoubleField)(JNIEnvMod*, jclass, jfieldID, jdouble);

    jstring     (*NewString)(JNIEnvMod*, const jchar*, jsize);
    jsize       (*GetStringLength)(JNIEnvMod*, jstring);
    const jchar* (*GetStringChars)(JNIEnvMod*, jstring, jboolean*);
    void        (*ReleaseStringChars)(JNIEnvMod*, jstring, const jchar*);
    jstring     (*NewStringUTF)(JNIEnvMod*, const char*);
    jsize       (*GetStringUTFLength)(JNIEnvMod*, jstring);
    /* JNI spec says this returns const jbyte*, but that's inconsistent */
    const char* (*GetStringUTFChars)(JNIEnvMod*, jstring, jboolean*);
    void        (*ReleaseStringUTFChars)(JNIEnvMod*, jstring, const char*);
    jsize       (*GetArrayLength)(JNIEnvMod*, jarray);
    jobjectArray (*NewObjectArray)(JNIEnvMod*, jsize, jclass, jobject);
    jobject     (*GetObjectArrayElement)(JNIEnvMod*, jobjectArray, jsize);
    void        (*SetObjectArrayElement)(JNIEnvMod*, jobjectArray, jsize, jobject);

    jbooleanArray (*NewBooleanArray)(JNIEnvMod*, jsize);
    jbyteArray    (*NewByteArray)(JNIEnvMod*, jsize);
    jcharArray    (*NewCharArray)(JNIEnvMod*, jsize);
    jshortArray   (*NewShortArray)(JNIEnvMod*, jsize);
    jintArray     (*NewIntArray)(JNIEnvMod*, jsize);
    jlongArray    (*NewLongArray)(JNIEnvMod*, jsize);
    jfloatArray   (*NewFloatArray)(JNIEnvMod*, jsize);
    jdoubleArray  (*NewDoubleArray)(JNIEnvMod*, jsize);

    jboolean*   (*GetBooleanArrayElements)(JNIEnvMod*, jbooleanArray, jboolean*);
    jbyte*      (*GetByteArrayElements)(JNIEnvMod*, jbyteArray, jboolean*);
    jchar*      (*GetCharArrayElements)(JNIEnvMod*, jcharArray, jboolean*);
    jshort*     (*GetShortArrayElements)(JNIEnvMod*, jshortArray, jboolean*);
    jint*       (*GetIntArrayElements)(JNIEnvMod*, jintArray, jboolean*);
    jlong*      (*GetLongArrayElements)(JNIEnvMod*, jlongArray, jboolean*);
    jfloat*     (*GetFloatArrayElements)(JNIEnvMod*, jfloatArray, jboolean*);
    jdouble*    (*GetDoubleArrayElements)(JNIEnvMod*, jdoubleArray, jboolean*);

    void        (*ReleaseBooleanArrayElements)(JNIEnvMod*, jbooleanArray,
                        jboolean*, jint);
    void        (*ReleaseByteArrayElements)(JNIEnvMod*, jbyteArray,
                        jbyte*, jint);
    void        (*ReleaseCharArrayElements)(JNIEnvMod*, jcharArray,
                        jchar*, jint);
    void        (*ReleaseShortArrayElements)(JNIEnvMod*, jshortArray,
                        jshort*, jint);
    void        (*ReleaseIntArrayElements)(JNIEnvMod*, jintArray,
                        jint*, jint);
    void        (*ReleaseLongArrayElements)(JNIEnvMod*, jlongArray,
                        jlong*, jint);
    void        (*ReleaseFloatArrayElements)(JNIEnvMod*, jfloatArray,
                        jfloat*, jint);
    void        (*ReleaseDoubleArrayElements)(JNIEnvMod*, jdoubleArray,
                        jdouble*, jint);

    void        (*GetBooleanArrayRegion)(JNIEnvMod*, jbooleanArray,
                        jsize, jsize, jboolean*);
    void        (*GetByteArrayRegion)(JNIEnvMod*, jbyteArray,
                        jsize, jsize, jbyte*);
    void        (*GetCharArrayRegion)(JNIEnvMod*, jcharArray,
                        jsize, jsize, jchar*);
    void        (*GetShortArrayRegion)(JNIEnvMod*, jshortArray,
                        jsize, jsize, jshort*);
    void        (*GetIntArrayRegion)(JNIEnvMod*, jintArray,
                        jsize, jsize, jint*);
    void        (*GetLongArrayRegion)(JNIEnvMod*, jlongArray,
                        jsize, jsize, jlong*);
    void        (*GetFloatArrayRegion)(JNIEnvMod*, jfloatArray,
                        jsize, jsize, jfloat*);
    void        (*GetDoubleArrayRegion)(JNIEnvMod*, jdoubleArray,
                        jsize, jsize, jdouble*);

    /* spec shows these without const; some jni.h do, some don't */
    void        (*SetBooleanArrayRegion)(JNIEnvMod*, jbooleanArray,
                        jsize, jsize, const jboolean*);
    void        (*SetByteArrayRegion)(JNIEnvMod*, jbyteArray,
                        jsize, jsize, const jbyte*);
    void        (*SetCharArrayRegion)(JNIEnvMod*, jcharArray,
                        jsize, jsize, const jchar*);
    void        (*SetShortArrayRegion)(JNIEnvMod*, jshortArray,
                        jsize, jsize, const jshort*);
    void        (*SetIntArrayRegion)(JNIEnvMod*, jintArray,
                        jsize, jsize, const jint*);
    void        (*SetLongArrayRegion)(JNIEnvMod*, jlongArray,
                        jsize, jsize, const jlong*);
    void        (*SetFloatArrayRegion)(JNIEnvMod*, jfloatArray,
                        jsize, jsize, const jfloat*);
    void        (*SetDoubleArrayRegion)(JNIEnvMod*, jdoubleArray,
                        jsize, jsize, const jdouble*);

    jint        (*RegisterNatives)(JNIEnvMod*, jclass, const JNINativeMethod*,
                        jint);
    jint        (*UnregisterNatives)(JNIEnvMod*, jclass);
    jint        (*MonitorEnter)(JNIEnvMod*, jobject);
    jint        (*MonitorExit)(JNIEnvMod*, jobject);
    jint        (*GetJavaVM)(JNIEnvMod*, JavaVM**);

    void        (*GetStringRegion)(JNIEnvMod*, jstring, jsize, jsize, jchar*);
    void        (*GetStringUTFRegion)(JNIEnvMod*, jstring, jsize, jsize, char*);

    void*       (*GetPrimitiveArrayCritical)(JNIEnvMod*, jarray, jboolean*);
    void        (*ReleasePrimitiveArrayCritical)(JNIEnvMod*, jarray, void*, jint);

    const jchar* (*GetStringCritical)(JNIEnvMod*, jstring, jboolean*);
    void        (*ReleaseStringCritical)(JNIEnvMod*, jstring, const jchar*);

    jweak       (*NewWeakGlobalRef)(JNIEnvMod*, jobject);
    void        (*DeleteWeakGlobalRef)(JNIEnvMod*, jweak);

    jboolean    (*ExceptionCheck)(JNIEnvMod*);

    jobject     (*NewDirectByteBuffer)(JNIEnvMod*, void*, jlong);
    void*       (*GetDirectBufferAddress)(JNIEnvMod*, jobject);
    jlong       (*GetDirectBufferCapacity)(JNIEnvMod*, jobject);

    /* added in JNI 1.6 */
    jobjectRefType (*GetObjectRefType)(JNIEnvMod*, jobject);

//#ifdef WITH_TAINT_TRACKING
	const char*	(*GetArrayType)(JNIEnvMod*, jarray);

    jstring     (*NewTaintedStringUTF)(JNIEnvMod*, const char*, u4);
    const char* (*GetTaintedStringUTFChars)(JNIEnvMod*, jstring, jboolean*, u4*);

    jobject     (*GetObjectTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);
    jboolean    (*GetBooleanTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);
    jbyte       (*GetByteTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);
    jchar       (*GetCharTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);
    jshort      (*GetShortTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);
    jint        (*GetIntTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);
    jlong       (*GetLongTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);
    jfloat      (*GetFloatTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);
    jdouble     (*GetDoubleTaintedField)(JNIEnvMod*, jobject, jfieldID, u4*);

    void        (*SetObjectTaintedField)(JNIEnvMod*, jobject, jfieldID, jobject, u4);
    void        (*SetBooleanTaintedField)(JNIEnvMod*, jobject, jfieldID, jboolean, u4);
    void        (*SetByteTaintedField)(JNIEnvMod*, jobject, jfieldID, jbyte, u4);
    void        (*SetCharTaintedField)(JNIEnvMod*, jobject, jfieldID, jchar, u4);
    void        (*SetShortTaintedField)(JNIEnvMod*, jobject, jfieldID, jshort, u4);
    void        (*SetIntTaintedField)(JNIEnvMod*, jobject, jfieldID, jint, u4);
    void        (*SetLongTaintedField)(JNIEnvMod*, jobject, jfieldID, jlong, u4);
    void        (*SetFloatTaintedField)(JNIEnvMod*, jobject, jfieldID, jfloat, u4);
    void        (*SetDoubleTaintedField)(JNIEnvMod*, jobject, jfieldID, jdouble, u4);

    jobject     (*CallObjectTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    jboolean    (*CallBooleanTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    jbyte       (*CallByteTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    jchar       (*CallCharTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    jshort      (*CallShortTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    jint        (*CallIntTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    jlong       (*CallLongTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    jfloat      (*CallFloatTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    jdouble     (*CallDoubleTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);
    void        (*CallVoidTaintedMethodA)(JNIEnvMod*, jobject, u4, jmethodID, u4*, jvalue*, u4*);

    jobject     (*CallNonvirtualObjectTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    jboolean    (*CallNonvirtualBooleanTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    jbyte       (*CallNonvirtualByteTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    jchar       (*CallNonvirtualCharTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    jshort      (*CallNonvirtualShortTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    jint        (*CallNonvirtualIntTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    jlong       (*CallNonvirtualLongTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    jfloat      (*CallNonvirtualFloatTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    jdouble     (*CallNonvirtualDoubleTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);
    void        (*CallNonvirtualVoidTaintedMethodA)(JNIEnvMod*, jobject, u4, jclass, jmethodID, u4*, jvalue*, u4*);

    jobject     (*CallStaticObjectTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    jboolean    (*CallStaticBooleanTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    jbyte       (*CallStaticByteTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    jchar       (*CallStaticCharTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    jshort      (*CallStaticShortTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    jint        (*CallStaticIntTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    jlong       (*CallStaticLongTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    jfloat      (*CallStaticFloatTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    jdouble     (*CallStaticDoubleTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);
    void        (*CallStaticVoidTaintedMethodA)(JNIEnvMod*, jclass, jmethodID, u4*, jvalue*, u4*);

    jobject     (*GetStaticObjectTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);
    jboolean    (*GetStaticBooleanTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);
    jbyte       (*GetStaticByteTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);
    jchar       (*GetStaticCharTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);
    jshort      (*GetStaticShortTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);
    jint        (*GetStaticIntTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);
    jlong       (*GetStaticLongTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);
    jfloat      (*GetStaticFloatTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);
    jdouble     (*GetStaticDoubleTaintedField)(JNIEnvMod*, jclass, jfieldID, u4*);

    void        (*SetStaticObjectTaintedField)(JNIEnvMod*, jclass, jfieldID, jobject, u4);
    void        (*SetStaticBooleanTaintedField)(JNIEnvMod*, jclass, jfieldID, jboolean, u4);
    void        (*SetStaticByteTaintedField)(JNIEnvMod*, jclass, jfieldID, jbyte, u4);
    void        (*SetStaticCharTaintedField)(JNIEnvMod*, jclass, jfieldID, jchar, u4);
    void        (*SetStaticShortTaintedField)(JNIEnvMod*, jclass, jfieldID, jshort, u4);
    void        (*SetStaticIntTaintedField)(JNIEnvMod*, jclass, jfieldID, jint, u4);
    void        (*SetStaticLongTaintedField)(JNIEnvMod*, jclass, jfieldID, jlong, u4);
    void        (*SetStaticFloatTaintedField)(JNIEnvMod*, jclass, jfieldID, jfloat, u4);
    void        (*SetStaticDoubleTaintedField)(JNIEnvMod*, jclass, jfieldID, jdouble, u4);
  
//#endif
};

/*
 * C++ object wrapper.
 *
 * This is usually overlaid on a C struct whose first element is a
 * JNINativeInterface*.  We rely somewhat on compiler behavior.
 */
struct _JNIEnvMod {
    /* do not rename this; it does not seem to be entirely opaque */
    const struct JNINativeInterfaceMod* functions;

#if defined(__cplusplus)

    jint GetVersion()
    { return functions->GetVersion(this); }

    jclass DefineClass(const char *name, jobject loader, const jbyte* buf,
        jsize bufLen)
    { return functions->DefineClass(this, name, loader, buf, bufLen); }

    jclass FindClass(const char* name)
    { return functions->FindClass(this, name); }

    jmethodID FromReflectedMethod(jobject method)
    { return functions->FromReflectedMethod(this, method); }

    jfieldID FromReflectedField(jobject field)
    { return functions->FromReflectedField(this, field); }

    jobject ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic)
    { return functions->ToReflectedMethod(this, cls, methodID, isStatic); }

    jclass GetSuperclass(jclass clazz)
    { return functions->GetSuperclass(this, clazz); }

    jboolean IsAssignableFrom(jclass clazz1, jclass clazz2)
    { return functions->IsAssignableFrom(this, clazz1, clazz2); }

    jobject ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic)
    { return functions->ToReflectedField(this, cls, fieldID, isStatic); }

    jint Throw(jthrowable obj)
    { return functions->Throw(this, obj); }

    jint ThrowNew(jclass clazz, const char* message)
    { return functions->ThrowNew(this, clazz, message); }

    jthrowable ExceptionOccurred()
    { return functions->ExceptionOccurred(this); }

    void ExceptionDescribe()
    { functions->ExceptionDescribe(this); }

    void ExceptionClear()
    { functions->ExceptionClear(this); }

    void FatalError(const char* msg)
    { functions->FatalError(this, msg); }

    jint PushLocalFrame(jint capacity)
    { return functions->PushLocalFrame(this, capacity); }

    jobject PopLocalFrame(jobject result)
    { return functions->PopLocalFrame(this, result); }

    jobject NewGlobalRef(jobject obj)
    { return functions->NewGlobalRef(this, obj); }

    void DeleteGlobalRef(jobject globalRef)
    { functions->DeleteGlobalRef(this, globalRef); }

    void DeleteLocalRef(jobject localRef)
    { functions->DeleteLocalRef(this, localRef); }

    jboolean IsSameObject(jobject ref1, jobject ref2)
    { return functions->IsSameObject(this, ref1, ref2); }

    jobject NewLocalRef(jobject ref)
    { return functions->NewLocalRef(this, ref); }

    jint EnsureLocalCapacity(jint capacity)
    { return functions->EnsureLocalCapacity(this, capacity); }

    jobject AllocObject(jclass clazz)
    { return functions->AllocObject(this, clazz); }

    jobject NewObject(jclass clazz, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        jobject result = functions->NewObjectV(this, clazz, methodID, args);
        va_end(args);
        return result;
    }

    jobject NewObjectV(jclass clazz, jmethodID methodID, va_list args)
    { return functions->NewObjectV(this, clazz, methodID, args); }

    jobject NewObjectA(jclass clazz, jmethodID methodID, jvalue* args)
    { return functions->NewObjectA(this, clazz, methodID, args); }

    jclass GetObjectClass(jobject obj)
    { return functions->GetObjectClass(this, obj); }

    jboolean IsInstanceOf(jobject obj, jclass clazz)
    { return functions->IsInstanceOf(this, obj, clazz); }

    jmethodID GetMethodID(jclass clazz, const char* name, const char* sig)
    { return functions->GetMethodID(this, clazz, name, sig); }

#define CALL_TYPE_METHOD(_jtype, _jname)                                    \
    _jtype Call##_jname##Method(jobject obj, jmethodID methodID, ...)       \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->Call##_jname##MethodV(this, obj, methodID,      \
                    args);                                                  \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_TYPE_METHODV(_jtype, _jname)                                   \
    _jtype Call##_jname##MethodV(jobject obj, jmethodID methodID,           \
        va_list args)                                                       \
    { return functions->Call##_jname##MethodV(this, obj, methodID, args); }
#define CALL_TYPE_METHODA(_jtype, _jname)                                   \
    _jtype Call##_jname##MethodA(jobject obj, jmethodID methodID,           \
        jvalue* args)                                                       \
    { return functions->Call##_jname##MethodA(this, obj, methodID, args); } \
    _jtype Call##_jname##TaintedMethodA(jobject obj, u4 objTaint,           \
             jmethodID methodID, u4* resultTaint, jvalue* args, u4* taints) \
   { return functions->Call##_jname##TaintedMethodA(this, obj, objTaint,    \
                                    methodID, resultTaint, args, taints); }

#define CALL_TYPE(_jtype, _jname)                                           \
    CALL_TYPE_METHOD(_jtype, _jname)                                        \
    CALL_TYPE_METHODV(_jtype, _jname)                                       \
    CALL_TYPE_METHODA(_jtype, _jname)

    CALL_TYPE(jobject, Object)
    CALL_TYPE(jboolean, Boolean)
    CALL_TYPE(jbyte, Byte)
    CALL_TYPE(jchar, Char)
    CALL_TYPE(jshort, Short)
    CALL_TYPE(jint, Int)
    CALL_TYPE(jlong, Long)
    CALL_TYPE(jfloat, Float)
    CALL_TYPE(jdouble, Double)

    void CallVoidMethod(jobject obj, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallVoidMethodV(this, obj, methodID, args);
        va_end(args);
    }
    void CallVoidMethodV(jobject obj, jmethodID methodID, va_list args)
    { functions->CallVoidMethodV(this, obj, methodID, args); }
    void CallVoidMethodA(jobject obj, jmethodID methodID, jvalue* args)
    { functions->CallVoidMethodA(this, obj, methodID, args); }
    void CallVoidTaintedMethodA(jobject obj, u4 objTaint, jmethodID methodID, u4* resultTaint, jvalue* args, u4* taints)
    { functions->CallVoidTaintedMethodA(this, obj, objTaint, methodID, resultTaint, args, taints); }

#define CALL_NONVIRT_TYPE_METHOD(_jtype, _jname)                            \
    _jtype CallNonvirtual##_jname##Method(jobject obj, jclass clazz,        \
        jmethodID methodID, ...)                                            \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->CallNonvirtual##_jname##MethodV(this, obj,      \
                    clazz, methodID, args);                                 \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_NONVIRT_TYPE_METHODV(_jtype, _jname)                           \
    _jtype CallNonvirtual##_jname##MethodV(jobject obj, jclass clazz,       \
        jmethodID methodID, va_list args)                                   \
    { return functions->CallNonvirtual##_jname##MethodV(this, obj, clazz,   \
        methodID, args); }
#define CALL_NONVIRT_TYPE_METHODA(_jtype, _jname)                           \
    _jtype CallNonvirtual##_jname##MethodA(jobject obj, jclass clazz,       \
        jmethodID methodID, jvalue* args)                                   \
    { return functions->CallNonvirtual##_jname##MethodA(this, obj, clazz,   \
                                                        methodID, args); }  \
    _jtype CallNonvirtual##_jname##TaintedMethodA(jobject obj, u4 objTaint, \
         jclass jclazz, jmethodID methodID, u4* resultTaint, jvalue* args,  \
         u4* taints)                                                        \
  { return functions->CallNonvirtual##_jname##TaintedMethodA(this, obj,     \
                 objTaint, jclazz, methodID, resultTaint, args, taints); }

#define CALL_NONVIRT_TYPE(_jtype, _jname)                                   \
    CALL_NONVIRT_TYPE_METHOD(_jtype, _jname)                                \
    CALL_NONVIRT_TYPE_METHODV(_jtype, _jname)                               \
    CALL_NONVIRT_TYPE_METHODA(_jtype, _jname)

    CALL_NONVIRT_TYPE(jobject, Object)
    CALL_NONVIRT_TYPE(jboolean, Boolean)
    CALL_NONVIRT_TYPE(jbyte, Byte)
    CALL_NONVIRT_TYPE(jchar, Char)
    CALL_NONVIRT_TYPE(jshort, Short)
    CALL_NONVIRT_TYPE(jint, Int)
    CALL_NONVIRT_TYPE(jlong, Long)
    CALL_NONVIRT_TYPE(jfloat, Float)
    CALL_NONVIRT_TYPE(jdouble, Double)

    void CallNonvirtualVoidMethod(jobject obj, jclass clazz, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallNonvirtualVoidMethodV(this, obj, clazz, methodID, args);
        va_end(args);
    }
    void CallNonvirtualVoidMethodV(jobject obj, jclass clazz,
        jmethodID methodID, va_list args)
    { functions->CallNonvirtualVoidMethodV(this, obj, clazz, methodID, args); }
    void CallNonvirtualVoidMethodA(jobject obj, jclass clazz,
        jmethodID methodID, jvalue* args)
    { functions->CallNonvirtualVoidMethodA(this, obj, clazz, methodID, args); }
    void CallNonvirtualVoidTaintedMethodA(jobject obj, u4 objTaint,
         jclass jclazz, jmethodID methodID, u4* resultTaint, jvalue* args,
         u4* taints)
    { functions->CallNonvirtualVoidTaintedMethodA(this, obj,
                      objTaint, jclazz, methodID, resultTaint, args, taints); }
  
    jfieldID GetFieldID(jclass clazz, const char* name, const char* sig)
    { return functions->GetFieldID(this, clazz, name, sig); }

    jobject GetObjectField(jobject obj, jfieldID fieldID)
    { return functions->GetObjectField(this, obj, fieldID); }
    jboolean GetBooleanField(jobject obj, jfieldID fieldID)
    { return functions->GetBooleanField(this, obj, fieldID); }
    jbyte GetByteField(jobject obj, jfieldID fieldID)
    { return functions->GetByteField(this, obj, fieldID); }
    jchar GetCharField(jobject obj, jfieldID fieldID)
    { return functions->GetCharField(this, obj, fieldID); }
    jshort GetShortField(jobject obj, jfieldID fieldID)
    { return functions->GetShortField(this, obj, fieldID); }
    jint GetIntField(jobject obj, jfieldID fieldID)
    { return functions->GetIntField(this, obj, fieldID); }
    jlong GetLongField(jobject obj, jfieldID fieldID)
    { return functions->GetLongField(this, obj, fieldID); }
    jfloat GetFloatField(jobject obj, jfieldID fieldID)
    { return functions->GetFloatField(this, obj, fieldID); }
    jdouble GetDoubleField(jobject obj, jfieldID fieldID)
    { return functions->GetDoubleField(this, obj, fieldID); }

    void SetObjectField(jobject obj, jfieldID fieldID, jobject value)
    { functions->SetObjectField(this, obj, fieldID, value); }
    void SetBooleanField(jobject obj, jfieldID fieldID, jboolean value)
    { functions->SetBooleanField(this, obj, fieldID, value); }
    void SetByteField(jobject obj, jfieldID fieldID, jbyte value)
    { functions->SetByteField(this, obj, fieldID, value); }
    void SetCharField(jobject obj, jfieldID fieldID, jchar value)
    { functions->SetCharField(this, obj, fieldID, value); }
    void SetShortField(jobject obj, jfieldID fieldID, jshort value)
    { functions->SetShortField(this, obj, fieldID, value); }
    void SetIntField(jobject obj, jfieldID fieldID, jint value)
    { functions->SetIntField(this, obj, fieldID, value); }
    void SetLongField(jobject obj, jfieldID fieldID, jlong value)
    { functions->SetLongField(this, obj, fieldID, value); }
    void SetFloatField(jobject obj, jfieldID fieldID, jfloat value)
    { functions->SetFloatField(this, obj, fieldID, value); }
    void SetDoubleField(jobject obj, jfieldID fieldID, jdouble value)
    { functions->SetDoubleField(this, obj, fieldID, value); }

    jmethodID GetStaticMethodID(jclass clazz, const char* name, const char* sig)
    { return functions->GetStaticMethodID(this, clazz, name, sig); }

#define CALL_STATIC_TYPE_METHOD(_jtype, _jname)                             \
    _jtype CallStatic##_jname##Method(jclass clazz, jmethodID methodID,     \
        ...)                                                                \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->CallStatic##_jname##MethodV(this, clazz,        \
                    methodID, args);                                        \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_STATIC_TYPE_METHODV(_jtype, _jname)                            \
    _jtype CallStatic##_jname##MethodV(jclass clazz, jmethodID methodID,    \
        va_list args)                                                       \
    { return functions->CallStatic##_jname##MethodV(this, clazz, methodID,  \
        args); }
#define CALL_STATIC_TYPE_METHODA(_jtype, _jname)                            \
    _jtype CallStatic##_jname##MethodA(jclass clazz, jmethodID methodID,    \
        jvalue* args)                                                       \
    { return functions->CallStatic##_jname##MethodA(this, clazz, methodID,  \
        args); }                                                            \
    _jtype CallStatic##_jname##TaintedMethodA(jclass clazz, jmethodID methodID, \
                             u4* resultTaint, jvalue* args, u4* taints)     \
    { return functions->CallStatic##_jname##TaintedMethodA(this, clazz,     \
                         methodID, resultTaint, args, taints); }

#define CALL_STATIC_TYPE(_jtype, _jname)                                    \
    CALL_STATIC_TYPE_METHOD(_jtype, _jname)                                 \
    CALL_STATIC_TYPE_METHODV(_jtype, _jname)                                \
    CALL_STATIC_TYPE_METHODA(_jtype, _jname)

    CALL_STATIC_TYPE(jobject, Object)
    CALL_STATIC_TYPE(jboolean, Boolean)
    CALL_STATIC_TYPE(jbyte, Byte)
    CALL_STATIC_TYPE(jchar, Char)
    CALL_STATIC_TYPE(jshort, Short)
    CALL_STATIC_TYPE(jint, Int)
    CALL_STATIC_TYPE(jlong, Long)
    CALL_STATIC_TYPE(jfloat, Float)
    CALL_STATIC_TYPE(jdouble, Double)

    void CallStaticVoidMethod(jclass clazz, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallStaticVoidMethodV(this, clazz, methodID, args);
        va_end(args);
    }
    void CallStaticVoidMethodV(jclass clazz, jmethodID methodID, va_list args)
    { functions->CallStaticVoidMethodV(this, clazz, methodID, args); }
    void CallStaticVoidMethodA(jclass clazz, jmethodID methodID, jvalue* args)
    { functions->CallStaticVoidMethodA(this, clazz, methodID, args); }
    void CallStaticVoidTaintedMethodA(jclass clazz, jmethodID methodID, u4* resultTaint, jvalue* args, u4* taints)
    { functions->CallStaticVoidTaintedMethodA(this, clazz, methodID, NULL, args, taints); }

    jfieldID GetStaticFieldID(jclass clazz, const char* name, const char* sig)
    { return functions->GetStaticFieldID(this, clazz, name, sig); }

    jobject GetStaticObjectField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticObjectField(this, clazz, fieldID); }
    jboolean GetStaticBooleanField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticBooleanField(this, clazz, fieldID); }
    jbyte GetStaticByteField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticByteField(this, clazz, fieldID); }
    jchar GetStaticCharField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticCharField(this, clazz, fieldID); }
    jshort GetStaticShortField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticShortField(this, clazz, fieldID); }
    jint GetStaticIntField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticIntField(this, clazz, fieldID); }
    jlong GetStaticLongField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticLongField(this, clazz, fieldID); }
    jfloat GetStaticFloatField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticFloatField(this, clazz, fieldID); }
    jdouble GetStaticDoubleField(jclass clazz, jfieldID fieldID)
    { return functions->GetStaticDoubleField(this, clazz, fieldID); }

    void SetStaticObjectField(jclass clazz, jfieldID fieldID, jobject value)
    { functions->SetStaticObjectField(this, clazz, fieldID, value); }
    void SetStaticBooleanField(jclass clazz, jfieldID fieldID, jboolean value)
    { functions->SetStaticBooleanField(this, clazz, fieldID, value); }
    void SetStaticByteField(jclass clazz, jfieldID fieldID, jbyte value)
    { functions->SetStaticByteField(this, clazz, fieldID, value); }
    void SetStaticCharField(jclass clazz, jfieldID fieldID, jchar value)
    { functions->SetStaticCharField(this, clazz, fieldID, value); }
    void SetStaticShortField(jclass clazz, jfieldID fieldID, jshort value)
    { functions->SetStaticShortField(this, clazz, fieldID, value); }
    void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value)
    { functions->SetStaticIntField(this, clazz, fieldID, value); }
    void SetStaticLongField(jclass clazz, jfieldID fieldID, jlong value)
    { functions->SetStaticLongField(this, clazz, fieldID, value); }
    void SetStaticFloatField(jclass clazz, jfieldID fieldID, jfloat value)
    { functions->SetStaticFloatField(this, clazz, fieldID, value); }
    void SetStaticDoubleField(jclass clazz, jfieldID fieldID, jdouble value)
    { functions->SetStaticDoubleField(this, clazz, fieldID, value); }

    jstring NewString(const jchar* unicodeChars, jsize len)
    { return functions->NewString(this, unicodeChars, len); }

    jsize GetStringLength(jstring string)
    { return functions->GetStringLength(this, string); }

    const jchar* GetStringChars(jstring string, jboolean* isCopy)
    { return functions->GetStringChars(this, string, isCopy); }

    void ReleaseStringChars(jstring string, const jchar* chars)
    { functions->ReleaseStringChars(this, string, chars); }

    jstring NewStringUTF(const char* bytes)
    { return functions->NewStringUTF(this, bytes); }

    jsize GetStringUTFLength(jstring string)
    { return functions->GetStringUTFLength(this, string); }

    const char* GetStringUTFChars(jstring string, jboolean* isCopy)
    { return functions->GetStringUTFChars(this, string, isCopy); }

    void ReleaseStringUTFChars(jstring string, const char* utf)
    { functions->ReleaseStringUTFChars(this, string, utf); }

    jsize GetArrayLength(jarray array)
    { return functions->GetArrayLength(this, array); }

    jobjectArray NewObjectArray(jsize length, jclass elementClass,
        jobject initialElement)
    { return functions->NewObjectArray(this, length, elementClass,
        initialElement); }

    jobject GetObjectArrayElement(jobjectArray array, jsize index)
    { return functions->GetObjectArrayElement(this, array, index); }

    void SetObjectArrayElement(jobjectArray array, jsize index, jobject value)
    { functions->SetObjectArrayElement(this, array, index, value); }

    jbooleanArray NewBooleanArray(jsize length)
    { return functions->NewBooleanArray(this, length); }
    jbyteArray NewByteArray(jsize length)
    { return functions->NewByteArray(this, length); }
    jcharArray NewCharArray(jsize length)
    { return functions->NewCharArray(this, length); }
    jshortArray NewShortArray(jsize length)
    { return functions->NewShortArray(this, length); }
    jintArray NewIntArray(jsize length)
    { return functions->NewIntArray(this, length); }
    jlongArray NewLongArray(jsize length)
    { return functions->NewLongArray(this, length); }
    jfloatArray NewFloatArray(jsize length)
    { return functions->NewFloatArray(this, length); }
    jdoubleArray NewDoubleArray(jsize length)
    { return functions->NewDoubleArray(this, length); }

    jboolean* GetBooleanArrayElements(jbooleanArray array, jboolean* isCopy)
    { return functions->GetBooleanArrayElements(this, array, isCopy); }
    jbyte* GetByteArrayElements(jbyteArray array, jboolean* isCopy)
    { return functions->GetByteArrayElements(this, array, isCopy); }
    jchar* GetCharArrayElements(jcharArray array, jboolean* isCopy)
    { return functions->GetCharArrayElements(this, array, isCopy); }
    jshort* GetShortArrayElements(jshortArray array, jboolean* isCopy)
    { return functions->GetShortArrayElements(this, array, isCopy); }
    jint* GetIntArrayElements(jintArray array, jboolean* isCopy)
    { return functions->GetIntArrayElements(this, array, isCopy); }
    jlong* GetLongArrayElements(jlongArray array, jboolean* isCopy)
    { return functions->GetLongArrayElements(this, array, isCopy); }
    jfloat* GetFloatArrayElements(jfloatArray array, jboolean* isCopy)
    { return functions->GetFloatArrayElements(this, array, isCopy); }
    jdouble* GetDoubleArrayElements(jdoubleArray array, jboolean* isCopy)
    { return functions->GetDoubleArrayElements(this, array, isCopy); }

    void ReleaseBooleanArrayElements(jbooleanArray array, jboolean* elems,
        jint mode)
    { functions->ReleaseBooleanArrayElements(this, array, elems, mode); }
    void ReleaseByteArrayElements(jbyteArray array, jbyte* elems,
        jint mode)
    { functions->ReleaseByteArrayElements(this, array, elems, mode); }
    void ReleaseCharArrayElements(jcharArray array, jchar* elems,
        jint mode)
    { functions->ReleaseCharArrayElements(this, array, elems, mode); }
    void ReleaseShortArrayElements(jshortArray array, jshort* elems,
        jint mode)
    { functions->ReleaseShortArrayElements(this, array, elems, mode); }
    void ReleaseIntArrayElements(jintArray array, jint* elems,
        jint mode)
    { functions->ReleaseIntArrayElements(this, array, elems, mode); }
    void ReleaseLongArrayElements(jlongArray array, jlong* elems,
        jint mode)
    { functions->ReleaseLongArrayElements(this, array, elems, mode); }
    void ReleaseFloatArrayElements(jfloatArray array, jfloat* elems,
        jint mode)
    { functions->ReleaseFloatArrayElements(this, array, elems, mode); }
    void ReleaseDoubleArrayElements(jdoubleArray array, jdouble* elems,
        jint mode)
    { functions->ReleaseDoubleArrayElements(this, array, elems, mode); }

    void GetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
        jboolean* buf)
    { functions->GetBooleanArrayRegion(this, array, start, len, buf); }
    void GetByteArrayRegion(jbyteArray array, jsize start, jsize len,
        jbyte* buf)
    { functions->GetByteArrayRegion(this, array, start, len, buf); }
    void GetCharArrayRegion(jcharArray array, jsize start, jsize len,
        jchar* buf)
    { functions->GetCharArrayRegion(this, array, start, len, buf); }
    void GetShortArrayRegion(jshortArray array, jsize start, jsize len,
        jshort* buf)
    { functions->GetShortArrayRegion(this, array, start, len, buf); }
    void GetIntArrayRegion(jintArray array, jsize start, jsize len,
        jint* buf)
    { functions->GetIntArrayRegion(this, array, start, len, buf); }
    void GetLongArrayRegion(jlongArray array, jsize start, jsize len,
        jlong* buf)
    { functions->GetLongArrayRegion(this, array, start, len, buf); }
    void GetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
        jfloat* buf)
    { functions->GetFloatArrayRegion(this, array, start, len, buf); }
    void GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
        jdouble* buf)
    { functions->GetDoubleArrayRegion(this, array, start, len, buf); }

    void SetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
        const jboolean* buf)
    { functions->SetBooleanArrayRegion(this, array, start, len, buf); }
    void SetByteArrayRegion(jbyteArray array, jsize start, jsize len,
        const jbyte* buf)
    { functions->SetByteArrayRegion(this, array, start, len, buf); }
    void SetCharArrayRegion(jcharArray array, jsize start, jsize len,
        const jchar* buf)
    { functions->SetCharArrayRegion(this, array, start, len, buf); }
    void SetShortArrayRegion(jshortArray array, jsize start, jsize len,
        const jshort* buf)
    { functions->SetShortArrayRegion(this, array, start, len, buf); }
    void SetIntArrayRegion(jintArray array, jsize start, jsize len,
        const jint* buf)
    { functions->SetIntArrayRegion(this, array, start, len, buf); }
    void SetLongArrayRegion(jlongArray array, jsize start, jsize len,
        const jlong* buf)
    { functions->SetLongArrayRegion(this, array, start, len, buf); }
    void SetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
        const jfloat* buf)
    { functions->SetFloatArrayRegion(this, array, start, len, buf); }
    void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
        const jdouble* buf)
    { functions->SetDoubleArrayRegion(this, array, start, len, buf); }

    jint RegisterNatives(jclass clazz, const JNINativeMethod* methods,
        jint nMethods)
    { return functions->RegisterNatives(this, clazz, methods, nMethods); }

    jint UnregisterNatives(jclass clazz)
    { return functions->UnregisterNatives(this, clazz); }

    jint MonitorEnter(jobject obj)
    { return functions->MonitorEnter(this, obj); }

    jint MonitorExit(jobject obj)
    { return functions->MonitorExit(this, obj); }

    jint GetJavaVM(JavaVM** vm)
    { return functions->GetJavaVM(this, vm); }

    void GetStringRegion(jstring str, jsize start, jsize len, jchar* buf)
    { functions->GetStringRegion(this, str, start, len, buf); }

    void GetStringUTFRegion(jstring str, jsize start, jsize len, char* buf)
    { return functions->GetStringUTFRegion(this, str, start, len, buf); }

    void* GetPrimitiveArrayCritical(jarray array, jboolean* isCopy)
    { return functions->GetPrimitiveArrayCritical(this, array, isCopy); }

    void ReleasePrimitiveArrayCritical(jarray array, void* carray, jint mode)
    { functions->ReleasePrimitiveArrayCritical(this, array, carray, mode); }

    const jchar* GetStringCritical(jstring string, jboolean* isCopy)
    { return functions->GetStringCritical(this, string, isCopy); }

    void ReleaseStringCritical(jstring string, const jchar* carray)
    { functions->ReleaseStringCritical(this, string, carray); }

    jweak NewWeakGlobalRef(jobject obj)
    { return functions->NewWeakGlobalRef(this, obj); }

    void DeleteWeakGlobalRef(jweak obj)
    { functions->DeleteWeakGlobalRef(this, obj); }

    jboolean ExceptionCheck()
    { return functions->ExceptionCheck(this); }

    jobject NewDirectByteBuffer(void* address, jlong capacity)
    { return functions->NewDirectByteBuffer(this, address, capacity); }

    void* GetDirectBufferAddress(jobject buf)
    { return functions->GetDirectBufferAddress(this, buf); }

    jlong GetDirectBufferCapacity(jobject buf)
    { return functions->GetDirectBufferCapacity(this, buf); }

    /* added in JNI 1.6 */
    jobjectRefType GetObjectRefType(jobject obj)
    { return functions->GetObjectRefType(this, obj); }
//#ifdef WITH_TAINT_TRACKING
	const char*	GetArrayType(jarray jarr)
	{ return functions->GetArrayType(this, jarr); }

    jstring NewTaintedStringUTF(const char* bytes, u4 taint)
    { return functions->NewTaintedStringUTF(this, bytes, taint); }
    const char* GetTaintedStringUTFChars(jstring string, jboolean* isCopy, u4* taint)
    { return functions->GetTaintedStringUTFChars(this, string, isCopy, taint); }

	jobject GetObjectTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetObjectTaintedField(this, obj, field, taint); }
	jboolean GetBooleanTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetBooleanTaintedField(this, obj, field, taint); }
	jbyte GetByteTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetByteTaintedField(this, obj, field, taint); }
	jchar GetCharTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetCharTaintedField(this, obj, field, taint); }
	jshort GetShortTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetShortTaintedField(this, obj, field, taint); }
	jint GetIntTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetIntTaintedField(this, obj, field, taint); }
	jlong GetLongTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetLongTaintedField(this, obj, field, taint); }
	jfloat GetFloatTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetFloatTaintedField(this, obj, field, taint); }
	jdouble GetDoubleTaintedField(jobject obj, jfieldID field, u4* taint)
	{ return functions->GetDoubleTaintedField(this, obj, field, taint); }

	void SetObjectTaintedField(jobject obj, jfieldID field, jobject val, u4 taint)
	{ return functions->SetObjectTaintedField(this, obj, field, val, taint); }
	void SetBooleanTaintedField(jobject obj, jfieldID field, jboolean val, u4 taint)
	{ return functions->SetBooleanTaintedField(this, obj, field, val, taint); }
	void SetByteTaintedField(jobject obj, jfieldID field, jbyte val, u4 taint)
	{ return functions->SetByteTaintedField(this, obj, field, val, taint); }
	void SetCharTaintedField(jobject obj, jfieldID field, jchar val, u4 taint)
	{ return functions->SetCharTaintedField(this, obj, field, val, taint); }
	void SetShortTaintedField(jobject obj, jfieldID field, jshort val, u4 taint)
	{ return functions->SetShortTaintedField(this, obj, field, val, taint); }
	void SetIntTaintedField(jobject obj, jfieldID field, jint val, u4 taint)
	{ return functions->SetIntTaintedField(this, obj, field, val, taint); }
	void SetLongTaintedField(jobject obj, jfieldID field, jlong val, u4 taint)
	{ return functions->SetLongTaintedField(this, obj, field, val, taint); }
	void SetFloatTaintedField(jobject obj, jfieldID field, jfloat val, u4 taint)
	{ return functions->SetFloatTaintedField(this, obj, field, val, taint); }
	void SetDoubleTaintedField(jobject obj, jfieldID field, jdouble val, u4 taint)
	{ return functions->SetDoubleTaintedField(this, obj, field, val, taint); }
  
//#endif // WITH_TAINT_TRACKING

#endif /*__cplusplus*/

};

/*
 * JNI invocation interface.
 */
struct JNIInvokeInterface {
    void*       reserved0;
    void*       reserved1;
    void*       reserved2;

    jint        (*DestroyJavaVM)(JavaVM*);
    jint        (*AttachCurrentThread)(JavaVM*, JNIEnvMod**, void*);
    jint        (*DetachCurrentThread)(JavaVM*);
    jint        (*GetEnv)(JavaVM*, void**, jint);
    jint        (*AttachCurrentThreadAsDaemon)(JavaVM*, JNIEnvMod**, void*);
};

struct _JavaVM {
    const struct JNIInvokeInterface* functions;

    jint DestroyJavaVM()
    { return functions->DestroyJavaVM(this); }
    jint AttachCurrentThread(JNIEnvMod** p_env, void* thr_args)
    { return functions->AttachCurrentThread(this, p_env, thr_args); }
    jint DetachCurrentThread()
    { return functions->DetachCurrentThread(this); }
    jint GetEnv(void** env, jint version)
    { return functions->GetEnv(this, env, version); }
    jint AttachCurrentThreadAsDaemon(JNIEnvMod** p_env, void* thr_args)
    { return functions->AttachCurrentThreadAsDaemon(this, p_env, thr_args); }
};


JNIEnvMod* dvmCreateJNIEnvMod();

struct Callback {
	int code;
	void* data;
	int size;
};

extern struct Callback jniCallback;

}

//#endif //ifdef WITH_DALVIK_BINDER_SERVICE

#endif //ifndef JNIENVMOD_H
