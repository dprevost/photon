/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_photon_DataDefBuilderODBC */

#ifndef _Included_org_photon_DataDefBuilderODBC
#define _Included_org_photon_DataDefBuilderODBC
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_photon_DataDefBuilderODBC
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_photon_DataDefBuilderODBC_initIDs
  (JNIEnv *, jclass);

/*
 * Class:     org_photon_DataDefBuilderODBC
 * Method:    psoAddField
 * Signature: (Ljava/lang/String;IIII)I
 */
JNIEXPORT jint JNICALL Java_org_photon_DataDefBuilderODBC_psoAddField
  (JNIEnv *, jobject, jstring, jint, jint, jint, jint);

/*
 * Class:     org_photon_DataDefBuilderODBC
 * Method:    psoGetLength
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_photon_DataDefBuilderODBC_psoGetLength
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif