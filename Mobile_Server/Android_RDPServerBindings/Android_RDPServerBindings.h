#pragma once
#include <jni.h>
#include <memory>
#include <mutex>


namespace SL {
	namespace Remote_Access_Library {
		class Server;
	}
}

extern std::shared_ptr<SL::Remote_Access_Library::Server> _Server;
extern std::mutex _ServerLock;

void __attribute__((constructor)) my_load(void);
void __attribute__((destructor)) my_unload(void);

#ifdef __cplusplus
extern "C" {
#endif

	JNIEXPORT void JNICALL Java_com_Android_1Server_Android_1Server_StartService(JNIEnv* env, jobject thiz);
	JNIEXPORT void JNICALL Java_com_Android_1Server_Android_1Server_StopService(JNIEnv* env, jobject thiz);
	JNIEXPORT void JNICALL Java_com_Android_1Server_Android_1Server_OnImage(JNIEnv* env, jobject thiz, jobject buf, int width, int height);

#ifdef __cplusplus
}
#endif
