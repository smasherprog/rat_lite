#include "Android_RDPServerBindings.h"
#include <stdio.h>
#include "Server.h"
#include "Server_Config.h"
#include <thread>

std::shared_ptr<SL::Remote_Access_Library::Server> _Server;
std::mutex _ServerLock;

void  my_load(void)
{
}

void my_unload(void)
{
	{
		std::lock_guard<std::mutex> lock(_ServerLock);
		if(_Server) _Server->Stop(true);
		_Server = nullptr;
	}
}

JNIEXPORT void JNICALL Java_com_Android_1Server_Android_1Server_StartService(JNIEnv* env, jobject thiz) {

	SL::Remote_Access_Library::Network::Server_Config config;
	config.WebSocketListenPort = 6001;// listen for websockets
	config.HttpListenPort = 8080;

	{
		std::lock_guard<std::mutex> lock(_ServerLock);
		if (_Server) _Server->Stop(true);//block while waiting for any previous server to stop
		_Server = std::make_shared<SL::Remote_Access_Library::Server>(config);
		auto tmp = _Server.get();
		std::thread th = std::thread([tmp]() { tmp->Run(); });
		th.detach();//let the thread run for ever until stop is called on the Server
	}
}
JNIEXPORT void JNICALL Java_com_Android_1Server_Android_1Server_StopService(JNIEnv* env, jobject thiz) {
	{
		std::lock_guard<std::mutex> lock(_ServerLock);
		if (_Server) _Server->Stop(true);
		_Server = nullptr;
	}
}
JNIEXPORT void JNICALL Java_com_Android_1Server_Android_1Server_OnImage(JNIEnv* env, jobject thiz, jobject buf, int width, int height) {

	auto dBuf = (char*)env->GetDirectBufferAddress(buf);
	_Server->OnImage(dBuf, width, height);
}