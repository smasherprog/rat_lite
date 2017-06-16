#pragma once
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <chrono>

typedef struct x509_store_ctx_st X509_STORE_CTX;

namespace SL {
    namespace WS_LITE {
        template <typename T, typename Meaning>
        struct Explicit
        {
            Explicit() { }
            Explicit(T value) : value(value) { }
            inline operator T () const { return value; }
            T value;
        };
        namespace INTERNAL {
            struct PorNumbertTag { };
            struct ThreadCountTag { };
        }

        typedef Explicit<unsigned short, INTERNAL::PorNumbertTag> PortNumber;
        typedef Explicit<unsigned short, INTERNAL::ThreadCountTag> ThreadCount;

        const auto HTTP_METHOD = "Method";
        const auto HTTP_PATH = "Path";
        const auto HTTP_HOST = "Host";
        const auto HTTP_VERSION = "Http_Version";
        const auto HTTP_STATUSCODE = "Http_StatusCode";
        const auto HTTP_CONTENTLENGTH = "Content-Length";
        const auto HTTP_CONTENTTYPE = "Content-Type";
        const auto HTTP_CACHECONTROL = "Cache-Control";
        const auto HTTP_LASTMODIFIED = "Last-Modified";
        const auto HTTP_SECWEBSOCKETKEY = "Sec-WebSocket-Key";
        const auto HTTP_SECWEBSOCKETACCEPT = "Sec-WebSocket-Accept";
        const auto HTTP_SECWEBSOCKETEXTENSIONS = "Sec-WebSocket-Extensions";
        const auto PERMESSAGEDEFLATE = "permessage-deflate";

        const auto HTTP_ENDLINE = "\r\n";
        const auto HTTP_KEYVALUEDELIM = ": ";

        enum OpCode : unsigned char {
            CONTINUATION = 0,
            TEXT = 1,
            BINARY = 2,
            CLOSE = 8,
            PING = 9,
            PONG = 10,
            INVALID = 255
        };
        enum ExtensionOptions : unsigned char {
            NO_OPTIONS = 0,
            DEFLATE = 1,
            NO_CONTEXT_TAKEOVER = 2
        };
        struct WSMessage {
            unsigned char *data;
            size_t len;
            OpCode code;
            //buffer is here to ensure the lifetime of the unsigned char *data in this structure
            //users should set the *data variable to be the beginning of the data to send. Then, set the Buffer shared ptr as well to make sure the lifetime of the data
            std::shared_ptr<unsigned char> Buffer;
        };
        struct WSPreparedMessage: public WSMessage {
            unsigned char header[10];
            unsigned char mask[4];
        };

        class IWSocket : public std::enable_shared_from_this<IWSocket> {
        public:
            virtual ~IWSocket() {}
            virtual bool is_open() const = 0;
            virtual std::string get_address() const = 0;
            virtual unsigned short get_port() const = 0;
            virtual bool is_v4() const = 0;
            virtual bool is_v6() const = 0;
            virtual bool is_loopback() const = 0;
            virtual void send(WSMessage& msg, bool compressmessage) = 0;
            virtual void send(WSPreparedMessage& msg) = 0;
            virtual WSPreparedMessage prepare(WSMessage& msg, bool compressmessage) = 0;

            //send a close message and close the socket
            virtual void close(unsigned short code = 1000, const std::string& msg = "") = 0;
        };
        class WSListenerImpl;
        class WSListener {
            std::shared_ptr<WSListenerImpl> Impl_;
        public:
            WSListener() {}
            WSListener(const std::shared_ptr<WSListenerImpl>& impl) : Impl_(impl) {}
            //the maximum payload size
            void set_MaxPayload(size_t bytes);
            //the maximum payload size
            size_t get_MaxPayload();
            //maximum time in seconds before a client is considered disconnected -- for reads
            void set_ReadTimeout(std::chrono::seconds seconds);
            //get the current read timeout in seconds
            std::chrono::seconds get_ReadTimeout();
            //maximum time in seconds before a client is considered disconnected -- for writes
            void set_WriteTimeout(std::chrono::seconds seconds);
            //get the current write timeout in seconds
            std::chrono::seconds get_WriteTimeout();
            operator bool() const { return Impl_.operator bool(); }
            //will stop the library from processing and release all memory
            void destroy() { Impl_.reset(); }
        };

        class WSListener_Configuration {
            std::shared_ptr<WSListenerImpl> Impl_;
        public:
            WSListener_Configuration(const std::shared_ptr<WSListenerImpl>& impl) : Impl_(impl) {}
            //when a connection is fully established.  If onconnect is called, then a matching onDisconnection is guaranteed
            WSListener_Configuration onConnection(const std::function<void(const std::shared_ptr<IWSocket>&, const std::unordered_map<std::string, std::string>&)>& handle);
            //when a message has been received
            WSListener_Configuration onMessage(const std::function<void(const std::shared_ptr<IWSocket>&, const WSMessage&)>& handle);
            //when a socket is closed down for ANY reason. If onconnect is called, then a matching onDisconnection is guaranteed
            WSListener_Configuration onDisconnection(const std::function<void(const std::shared_ptr<IWSocket>&, unsigned short, const std::string&)>& handle);
            //when a ping is received from a client
            WSListener_Configuration onPing(const std::function<void(const std::shared_ptr<IWSocket>&, const unsigned char *, size_t)>& handle);
            //when a pong is received from a client
            WSListener_Configuration onPong(const std::function<void(const std::shared_ptr<IWSocket>&, const unsigned char *, size_t)>& handle);
            //start the process to listen for clients. This is non-blocking and will return immediatly
            WSListener listen(bool no_delay = true, bool reuse_address = true);
        };


        class WSClientImpl;
        class WSClient {
            std::shared_ptr<WSClientImpl> Impl_;
        public:
            WSClient() {}
            WSClient(const std::shared_ptr<WSClientImpl>& impl) : Impl_(impl) {}
            //the maximum payload size
            void set_MaxPayload(size_t bytes);
            //the maximum payload size
            size_t get_MaxPayload();
            //maximum time in seconds before a client is considered disconnected -- for reads
            void set_ReadTimeout(std::chrono::seconds seconds);
            //get the current read timeout in seconds
            std::chrono::seconds get_ReadTimeout();
            //maximum time in seconds before a client is considered disconnected -- for writes
            void set_WriteTimeout(std::chrono::seconds seconds);
            //get the current write timeout in seconds
            std::chrono::seconds get_WriteTimeout();
            operator bool() const { return Impl_.operator bool(); }
            //will stop the library from processing and release all memory
            void destroy() { Impl_.reset(); }
        };
        class WSContextImpl;
        class WSClient_Configuration {
        protected:
            std::shared_ptr<WSClientImpl> Impl_;
        public:
            WSClient_Configuration(const std::shared_ptr<WSClientImpl>& impl) :Impl_(impl) {}
            //when a connection is fully established.  If onconnect is called, then a matching onDisconnection is guaranteed
            WSClient_Configuration onConnection(const std::function<void(const std::shared_ptr<IWSocket>&, const std::unordered_map<std::string, std::string>&)>& handle);
            //when a message has been received
            WSClient_Configuration onMessage(const std::function<void(const std::shared_ptr<IWSocket>&, const WSMessage&)>& handle);
            //when a socket is closed down for ANY reason. If onconnect is called, then a matching onDisconnection is guaranteed
            WSClient_Configuration onDisconnection(const std::function<void(const std::shared_ptr<IWSocket>&, unsigned short, const std::string&)>& handle);
            //when a ping is received from a client
            WSClient_Configuration onPing(const std::function<void(const std::shared_ptr<IWSocket>&, const unsigned char *, size_t)>& handle);
            //when a pong is received from a client
            WSClient_Configuration onPong(const std::function<void(const std::shared_ptr<IWSocket>&, const unsigned char *, size_t)>& handle);
            //connect to an endpoint. This is non-blocking and will return immediatly. If the library is unable to establish a connection, ondisconnection will be called. 
            WSClient connect(const std::string& host, PortNumber port, bool no_delay = true, const std::string& endpoint = "/", const std::unordered_map<std::string, std::string>& extraheaders = {});
        };

        class WSSClient_Configuration : public WSClient_Configuration {
        public:
            WSSClient_Configuration(const std::shared_ptr<WSClientImpl>& impl) :WSClient_Configuration(impl) {}
            //set this if you want to verify the server's cert
            WSClient_Configuration onVerifyPeer(const std::function<bool(bool, X509_STORE_CTX*)>& handle);
        };

        class WSContext {
            std::shared_ptr<WSContextImpl> Impl_;
        public:
            WSContext(const std::shared_ptr<WSContextImpl>& impl) :Impl_(impl) {}
            WSContext() {}
            WSListener_Configuration CreateListener(PortNumber port, ExtensionOptions options = ExtensionOptions::NO_OPTIONS);
            WSListener_Configuration CreateTLSListener(
                PortNumber port,
                std::string Password,
                std::string Privatekey_File,
                std::string Publiccertificate_File,
                std::string dh_File,
                ExtensionOptions options = ExtensionOptions::NO_OPTIONS);

            WSClient_Configuration CreateClient(ExtensionOptions options = ExtensionOptions::NO_OPTIONS);
            WSSClient_Configuration CreateTLSClient(ExtensionOptions options = ExtensionOptions::NO_OPTIONS);
            WSSClient_Configuration CreateTLSClient(std::string Publiccertificate_File, ExtensionOptions options = ExtensionOptions::NO_OPTIONS);

        };

        WSContext CreateContext(ThreadCount threadcount);
    }
}

