/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_photon_ArrayListSerializer */

#ifndef _Included_org_photon_ArrayListSerializer
#define _Included_org_photon_ArrayListSerializer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_photon_ArrayListSerializer
 * Method:    psoPackObject
 * Signature: (Ljava/util/ArrayList;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_photon_ArrayListSerializer_psoPackObject
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_photon_ArrayListSerializer
 * Method:    psoUnpackObject
 * Signature: ([B)Ljava/util/ArrayList;
 */
JNIEXPORT jobject JNICALL Java_org_photon_ArrayListSerializer_psoUnpackObject
  (JNIEnv *, jobject, jbyteArray);

#ifdef __cplusplus
}
#endif
#endif
