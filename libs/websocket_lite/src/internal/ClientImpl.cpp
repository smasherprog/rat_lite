#include "WS_Lite.h"
#include "Logging.h"
#include "internal/WebSocketProtocol.h"

#include <fstream>
#include <string>

namespace SL {
    namespace WS_LITE {

        bool verify_certificate(bool preverified, asio::ssl::verify_context& ctx)
        {
            char subject_name[256];
            X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
            X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
            SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Verifying " << subject_name);

            return preverified;
        }


        template<class PARENTTYPE, class SOCKETTYPE>void ConnectHandshake(PARENTTYPE& self, SOCKETTYPE& socket, const std::string& host, const std::string& endpoint, const std::unordered_map<std::string, std::string>& extraheaders) {
            auto write_buffer(std::make_shared<asio::streambuf>());
            std::ostream request(write_buffer.get());

            request << "GET " << endpoint << " HTTP/1.1" << HTTP_ENDLINE;
            request << HTTP_HOST << HTTP_KEYVALUEDELIM << host << HTTP_ENDLINE;
            request << "Upgrade: websocket" << HTTP_ENDLINE;
            request << "Connection: Upgrade" << HTTP_ENDLINE;

            //Make random 16-byte nonce
            std::string nonce;
            nonce.resize(16);
            std::uniform_int_distribution<unsigned int> dist(0, 255);
            std::random_device rd;
            for (int c = 0; c < 16; c++) {
                nonce[c] = static_cast<unsigned char>(dist(rd));
            }

            auto nonce_base64 = Base64encode(nonce);
            request << HTTP_SECWEBSOCKETKEY << HTTP_KEYVALUEDELIM << nonce_base64 << HTTP_ENDLINE;
            request << "Sec-WebSocket-Version: 13" << HTTP_ENDLINE;
            for (auto& h : extraheaders) {
                request << h.first << HTTP_KEYVALUEDELIM << h.second << HTTP_ENDLINE;
            }
            //  request << "" << HTTP_ENDLINE;
            //  request << HTTP_SECWEBSOCKETEXTENSIONS << HTTP_KEYVALUEDELIM << PERMESSAGEDEFLATE << HTTP_ENDLINE;
            request << HTTP_ENDLINE << HTTP_ENDLINE;


            auto accept_sha1 = SHA1(nonce_base64 + ws_magic_string);

            asio::async_write(socket->Socket, *write_buffer, [write_buffer, accept_sha1, socket, self](const std::error_code& ec, size_t bytes_transferred) {
                UNUSED(bytes_transferred);
                if (!ec) {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Sent Handshake bytes " << bytes_transferred);
                    auto read_buffer(std::make_shared<asio::streambuf>());
                    asio::async_read_until(socket->Socket, *read_buffer, "\r\n\r\n", [read_buffer, accept_sha1, socket, self](const std::error_code& ec, size_t bytes_transferred) {
                        if (!ec) {
                            SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Read Handshake bytes " << bytes_transferred);
                            std::istream stream(read_buffer.get());

                            std::unordered_map<std::string, std::string> header;
                            if (Parse_ServerHandshake(stream, header) && Base64decode(header[HTTP_SECWEBSOCKETACCEPT]) == accept_sha1) {


                                SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Connected ");
                        
                                if (header.find(PERMESSAGEDEFLATE) != header.end()) {
                                    socket->CompressionEnabled = true;
                                }
                                socket->SocketStatus_ = SocketStatus::CONNECTED;
                                if (self->onConnection) {
                                    self->onConnection(socket, header);
                                }/*
                                if (read_buffer->size() > bytes_transferred) {
                                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Read Extra Data " << read_buffer->size() - bytes_transferred);
                                }*/
                                ReadHeaderStart(self, socket);
                            }
                            else {
                                socket->SocketStatus_ = SocketStatus::CLOSED;
                                SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "WebSocket handshake failed  ");
                                if (self->onDisconnection) {
                                    self->onDisconnection(socket, 1002, "WebSocket handshake failed  ");
                                }
                            }
                        }
                        else {
                            socket->SocketStatus_ = SocketStatus::CLOSED;
                            SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "async_read_until failed  " << ec.message());
                            if (self->onDisconnection) {
                                self->onDisconnection(socket, 1002, "async_read_until failed  " + ec.message());
                            }
                        }
                    });
                }
                else {
                    socket->SocketStatus_ = SocketStatus::CLOSED;
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Failed sending handshake" << ec.message());
                    if (self->onDisconnection) {
                        self->onDisconnection(socket, 1002, "Failed sending handshake" + ec.message());
                    }
                }
            });

        }
        void async_handshake(std::shared_ptr<WSClientImpl> self, std::shared_ptr<WSocket<asio::ip::tcp::socket, WSClientImpl>> socket, const std::string& host, const std::string& endpoint, const std::unordered_map<std::string, std::string>& extraheaders) {
            ConnectHandshake(self, socket, host, endpoint, extraheaders);
        }
        void async_handshake(std::shared_ptr<WSClientImpl> self, std::shared_ptr<WSocket<asio::ssl::stream<asio::ip::tcp::socket>, WSClientImpl>> socket, const std::string& host, const std::string& endpoint, const std::unordered_map<std::string, std::string>& extraheaders) {
            socket->Socket.async_handshake(asio::ssl::stream_base::client, [socket, self, host, endpoint, extraheaders](const std::error_code& ec) {
                if (!ec)
                {
                    ConnectHandshake(self, socket, host, endpoint, extraheaders);
                }
                else {
                    socket->SocketStatus_ = SocketStatus::CLOSED;
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Failed async_handshake " << ec.message());
                    if (self->onDisconnection) {
                        self->onDisconnection(socket, 1002, "Failed async_handshake " + ec.message());
                    }
                }
            });
        }
        template<class PARENTTYPE, typename SOCKETCREATOR>void Connect(PARENTTYPE self, const std::string& host, PortNumber port, bool no_delay, SOCKETCREATOR&& socketcreator, const std::string& endpoint, const std::unordered_map<std::string, std::string>& extraheaders) {

            auto socket = socketcreator(self);
            socket->SocketStatus_ = SocketStatus::CONNECTING;
            std::error_code ec;
            asio::ip::tcp::resolver resolver(self->WSContextImpl_->io_service);
            auto portstr = std::to_string(port.value);
            asio::ip::tcp::resolver::query query(host, portstr.c_str());

            auto resolvedendpoint = resolver.resolve(query, ec);

            if (ec) {
                SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "resolve error " << ec.message());
                socket->SocketStatus_ = SocketStatus::CLOSED;
                if (self->onDisconnection) {
                    self->onDisconnection(socket, 1002, "resolve error " + ec.message());
                }
            }
            else {
                asio::async_connect(socket->Socket.lowest_layer(), resolvedendpoint, [socket, self, host, no_delay, endpoint, extraheaders](const std::error_code& ec, asio::ip::tcp::resolver::iterator)
                {
                    std::error_code e;
                    socket->Socket.lowest_layer().set_option(asio::ip::tcp::no_delay(no_delay), e);
                    if (e) {
                        SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "set_option error " << e.message());
                        e.clear();
                    }
                    if (!ec)
                    {
                        async_handshake(self, socket, host, endpoint, extraheaders);
                    }
                    else {
                        SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Failed async_connect " << ec.message());
                        socket->SocketStatus_ = SocketStatus::CLOSED;
                        if (self->onDisconnection) {
                            self->onDisconnection(socket, 1002, "Failed async_connect " + ec.message());
                        }
                    }
                });
            }

        }

        void WSClient::set_ReadTimeout(std::chrono::seconds seconds) {
            Impl_->ReadTimeout = seconds;
        }
        std::chrono::seconds WSClient::get_ReadTimeout() {
            return  Impl_->ReadTimeout;
        }
        void WSClient::set_WriteTimeout(std::chrono::seconds seconds) {
            Impl_->WriteTimeout = seconds;
        }
        std::chrono::seconds WSClient::get_WriteTimeout() {
            return  Impl_->WriteTimeout;
        }
        void WSClient::set_MaxPayload(size_t bytes) {
            Impl_->MaxPayload = bytes;
        }
        size_t WSClient::get_MaxPayload() {
            return  Impl_->MaxPayload;
        }


        WSClient_Configuration WSClient_Configuration::onConnection(const std::function<void(const std::shared_ptr<IWSocket>&, const std::unordered_map<std::string, std::string>&)>& handle) {
            assert(!Impl_->onConnection);
            Impl_->onConnection = handle;
            return WSClient_Configuration(Impl_);
        }
        WSClient_Configuration WSClient_Configuration::onMessage(const std::function<void(const std::shared_ptr<IWSocket>&, const WSMessage&)>& handle) {
            assert(!Impl_->onMessage);
            Impl_->onMessage = handle;
            return WSClient_Configuration(Impl_);
        }
        WSClient_Configuration WSClient_Configuration::onDisconnection(const std::function<void(const std::shared_ptr<IWSocket>&, unsigned short, const std::string&)>& handle) {
            assert(!Impl_->onDisconnection);
            Impl_->onDisconnection = handle;
            return WSClient_Configuration(Impl_);
        }
        WSClient_Configuration WSClient_Configuration::onPing(const std::function<void(const std::shared_ptr<IWSocket>&, const unsigned char *, size_t)>& handle) {
            assert(!Impl_->onPing);
            Impl_->onPing = handle;
            return WSClient_Configuration(Impl_);
        }
        WSClient_Configuration WSClient_Configuration::onPong(const std::function<void(const std::shared_ptr<IWSocket>&, const unsigned char *, size_t)>& handle) {
            assert(!Impl_->onPong);
            Impl_->onPong = handle;
            return WSClient_Configuration(Impl_);
        }

        WSClient WSClient_Configuration::connect(const std::string& host, PortNumber port,bool no_delay, const std::string& endpoint, const std::unordered_map<std::string, std::string>& extraheaders) {
            if (Impl_->TLSEnabled) {
                auto createsocket = [](auto c) {
                    auto socket = std::make_shared<WSocket<asio::ssl::stream<asio::ip::tcp::socket>, WSClientImpl>>(c, c->sslcontext);
                    std::error_code e;
                    if (c->onVerifyPeer) {
                        socket->Socket.set_verify_mode(asio::ssl::verify_peer);
                        socket->Socket.set_verify_callback([c](bool preverified, asio::ssl::verify_context& ctx) { return c->onVerifyPeer(preverified, ctx.native_handle()); }, e);
                    }
                    else {
                        socket->Socket.set_verify_mode(asio::ssl::verify_fail_if_no_peer_cert);
                        socket->Socket.set_verify_callback(std::bind(&verify_certificate, std::placeholders::_1, std::placeholders::_2), e);
                    }
                    return socket;
                };
                Connect(Impl_, host, port,  no_delay, createsocket, endpoint, extraheaders);
            }
            else {
                auto createsocket = [](auto c) {
                    return std::make_shared<WSocket<asio::ip::tcp::socket, WSClientImpl>>(c);
                };
                Connect(Impl_, host, port, no_delay,createsocket,   endpoint, extraheaders);
            }
            return WSClient(Impl_);
        }

        WSClient_Configuration WSSClient_Configuration::onVerifyPeer(const std::function<bool(bool, X509_STORE_CTX*)>& handle) {
            assert(Impl_->onVerifyPeer);
            Impl_->onVerifyPeer = handle;
            return WSClient_Configuration(Impl_);
        }
        WSSClient_Configuration WSContext::CreateTLSClient(std::string Publiccertificate_File, ExtensionOptions options) {
            UNUSED(options);
            return WSSClient_Configuration(std::make_shared<WSClientImpl>(Impl_, Publiccertificate_File));
        }
        WSSClient_Configuration WSContext::CreateTLSClient(ExtensionOptions options) {
            UNUSED(options);
            return WSSClient_Configuration(std::make_shared<WSClientImpl>(Impl_, true));
        }
        WSClient_Configuration WSContext::CreateClient(ExtensionOptions options) {
            UNUSED(options);
            return WSClient_Configuration(std::make_shared<WSClientImpl>(Impl_));
        }
    }
}
