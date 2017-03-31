project (uWS)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)


if(WIN32)
	find_package(LibUV REQUIRED)
	add_definitions(-DUSE_LIBUV)
elseif(APPLE)
	find_package(LibUV REQUIRED)
	add_definitions(-DUSE_LIBUV)
else()
	add_definitions(-DUSE_ASIO)
endif()


add_library(uWS WebSocket.cpp Socket.cpp Node.cpp Networking.cpp Hub.cpp HTTPSocket.cpp Group.cpp Extensions.cpp Epoll.cpp)

target_include_directories(uWS PUBLIC ${LIBUV_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}  ${OPENSSL_INCLUDE_DIR})
target_link_libraries (uWS LINK_PUBLIC ${LIBUV_LIBRARIES}  ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${ZLIB_LIBRARY})



