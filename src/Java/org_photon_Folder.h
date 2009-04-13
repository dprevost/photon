/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_photon_Folder */

#ifndef _Included_org_photon_Folder
#define _Included_org_photon_Folder
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_photon_Folder
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_photon_Folder_initIDs
  (JNIEnv *, jclass);

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateFolder
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Folder_psoCreateFolder
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Folder_psoCreateObject
  (JNIEnv *, jobject, jlong, jstring, jobject, jobject);

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateKeyedObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Lorg/photon/KeyDefinition;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Folder_psoCreateKeyedObject
  (JNIEnv *, jobject, jlong, jstring, jobject, jobject, jobject);

/*
 * Class:     org_photon_Folder
 * Method:    psoDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Folder_psoDestroyObject
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     org_photon_Folder
 * Method:    psoFini
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_photon_Folder_psoFini
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_photon_Folder
 * Method:    psoGetFirst
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Folder_psoGetFirst
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     org_photon_Folder
 * Method:    psoGetNext
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Folder_psoGetNext
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     org_photon_Folder
 * Method:    psoInit
 * Signature: (Lorg/photon/Session;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Folder_psoInit
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     org_photon_Folder
 * Method:    psoStatus
 * Signature: (JLorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_Folder_psoStatus
  (JNIEnv *, jobject, jlong, jobject);

#ifdef __cplusplus
}
#endif
#endif
