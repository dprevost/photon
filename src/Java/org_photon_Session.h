/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_photon_Session */

#ifndef _Included_org_photon_Session
#define _Included_org_photon_Session
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_photon_Session
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_photon_Session_initIDs
  (JNIEnv *, jclass);

/*
 * Class:     org_photon_Session
 * Method:    psoCommit
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoCommit
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_photon_Session
 * Method:    psoCreateFolder
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoCreateFolder
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     org_photon_Session
 * Method:    psoCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;J)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoCreateObject
  (JNIEnv *, jobject, jlong, jstring, jobject, jlong);

/*
 * Class:     org_photon_Session
 * Method:    psoCreateObjectEx
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoCreateObjectEx
  (JNIEnv *, jobject, jlong, jstring, jobject, jstring);

/*
 * Class:     org_photon_Session
 * Method:    psoCreateKeyedObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;JJ)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoCreateKeyedObject
  (JNIEnv *, jobject, jlong, jstring, jobject, jlong, jlong);

/*
 * Class:     org_photon_Session
 * Method:    psoCreateKeyedObjectEx
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoCreateKeyedObjectEx
  (JNIEnv *, jobject, jlong, jstring, jobject, jstring, jstring);

/*
 * Class:     org_photon_Session
 * Method:    psoDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoDestroyObject
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     org_photon_Session
 * Method:    psoFini
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoFini
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_photon_Session
 * Method:    psoGetDataDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoGetDataDefinition
  (JNIEnv *, jobject, jlong, jstring, jobject);

/*
 * Class:     org_photon_Session
 * Method:    psoGetDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoGetDefinition
  (JNIEnv *, jobject, jlong, jstring, jobject);

/*
 * Class:     org_photon_Session
 * Method:    psoGetInfo
 * Signature: (JLorg/photon/Info;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoGetInfo
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     org_photon_Session
 * Method:    psoGetKeyDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/KeyDefinition;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoGetKeyDefinition
  (JNIEnv *, jobject, jlong, jstring, jobject);

/*
 * Class:     org_photon_Session
 * Method:    psoGetStatus
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoGetStatus
  (JNIEnv *, jobject, jlong, jstring, jobject);

/*
 * Class:     org_photon_Session
 * Method:    psoInit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoInit
  (JNIEnv *, jobject);

/*
 * Class:     org_photon_Session
 * Method:    psoRollback
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Session_psoRollback
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
