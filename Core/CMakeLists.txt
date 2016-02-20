project(Core)

SET(Sources
		ApplicationDirectory.cpp
		ClientNetworkDriver.cpp
		ConnectWindow.cpp
		HttpHeader.cpp
		HttpListener.cpp
		Image.cpp
		Mouse.cpp
		Screen.cpp
		Server.cpp
		ServerNetworkDriver.cpp
		Server_Impl.cpp
		SocketImpl.cpp
		stdafx.cpp
		ViewerWindow.cpp
		WebSocketListener.cpp
		IO_Runner.cpp)

SET(Headers 
		ApplicationDirectory.h
		ClientNetworkDriver.h
		ConnectWindow.h
		crypto.hpp
		HttpHeader.h
		HttpListener.h
		HttpSocket.h
		IBaseNetworkDriver.h
		IClientDriver.h
		Image.h
		index.html
		IO_Runner.h
		IServerDriver.h
		ISocket.h
		Mouse.h
		Packet.h
		Screen.h
		Server.h
		ServerNetworkDriver.h
		Server_Config.h
		Server_Impl.h
		Shapes.h
		SocketImpl.h
		SocketStats.h
		stdafx.h
		TCPListener.h
		TCPSocket.h
		ThreadPool.h
		ViewerWindow.h
		WebSocket.h
		WebSocketListener.h
		WebSocketServerNetworkDriver.h)

ADD_LIBRARY(Core STATIC ${Sources} ${Headers} ${PlatformDependResources})

SET(STATIC_LIBRARY_OUTPUT_NAME Core)
TARGET_LINK_LIBRARIES(Core zstd)
