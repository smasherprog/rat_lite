#pragma once
#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif

	JNIEXPORT jlong JNICALL Java_com_RDPBinding_RDPService_StartService(JNIEnv* env, jobject thiz);
	JNIEXPORT jboolean JNICALL Java_com_RDPBinding_RDPService_StopService(JNIEnv* env, jobject thiz, jlong ptr);

#ifdef __cplusplus
}
#endif
