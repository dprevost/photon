/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_photon_PhotonQueue */

#ifndef _Included_org_photon_PhotonQueue
#define _Included_org_photon_PhotonQueue
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_photon_PhotonQueue
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_photon_PhotonQueue_initIDs
  (JNIEnv *, jclass);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    fini
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_fini
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    getDefinition
 * Signature: (JLorg/photon/ObjectDefinition;I[Lorg/photon/FieldDefinition;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_getDefinition
  (JNIEnv *, jobject, jlong, jobject, jint, jobjectArray);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    getFirst
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_getFirst
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    getNext
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_getNext
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    init
 * Signature: (Lorg/photon/PhotonSession;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_init
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    pop
 * Signature: (J[BI[I)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_pop
  (JNIEnv *, jobject, jlong, jbyteArray, jint, jintArray);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    push
 * Signature: (J[BI)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_push
  (JNIEnv *, jobject, jlong, jbyteArray, jint);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    pushNow
 * Signature: (J[BI)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_pushNow
  (JNIEnv *, jobject, jlong, jbyteArray, jint);

/*
 * Class:     org_photon_PhotonQueue
 * Method:    getStatus
 * Signature: (JLorg/photon/ObjStatus;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_PhotonQueue_getStatus
  (JNIEnv *, jobject, jlong, jobject);

#ifdef __cplusplus
}
#endif
#endif
