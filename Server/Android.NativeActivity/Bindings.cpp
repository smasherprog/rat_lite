
#include <stdio.h>
#include "Bindings.h"

JNIEXPORT jlong JNICALL Java_com_RDPBinding_RDPService_StartService(JNIEnv* env, jobject thiz) {
	printf("Hello World!\n");
	return 57;
}
JNIEXPORT jboolean JNICALL Java_com_RDPBinding_RDPService_StopService(JNIEnv* env, jobject thiz, jlong ptr) {
	printf("Hello World!\n");
	return 57 == ptr;
}