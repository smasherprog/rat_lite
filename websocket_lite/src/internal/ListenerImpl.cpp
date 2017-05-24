#include "WS_Lite.h"
#include "Logging.h"
#include "internal/WebSocketProtocol.h"

namespace SL {
    namespace WS_LITE {

        template<class SOCKETTYPE>void read_handshake(std::shared_ptr<WSListenerImpl> listener, SOCKETTYPE& socket) {
            auto handshakecontainer(std::make_shared<HandshakeContainer>());
            asio::async_read_until(*socket, handshakecontainer->Read, "\r\n\r\n", [listener, socket, handshakecontainer](const std::error_code& ec, size_t bytes_transferred) {
                UNUSED(bytes_transferred);
                if (!ec) {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Read Handshake bytes " << bytes_transferred);

                    std::istream headerdata(&handshakecontainer->Read);

                    if (!Parse_Handshake("1.1", headerdata, handshakecontainer->Header)) {
                        SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Parse Handshake failed ");
                        return;
                    }
                    if (handshakecontainer->Read.size() > 0) {
                        SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "READ MORE DATA " << handshakecontainer->Read.size());
                        return;
                    }

                    std::ostream handshake(&handshakecontainer->Write);
                    if (Generate_Handshake(handshakecontainer->Header, handshake)) {
                        auto sock = std::make_shared<WSocketImpl>(listener->io_service);
                        WSocket websocket(sock);
                        if (handshakecontainer->Header.find(PERMESSAGEDEFLATE) != handshakecontainer->Header.end()) {
                            sock->CompressionEnabled = true;
                        }
                        set_Socket(sock, socket);
                        if (listener->onHttpUpgrade) {
                            listener->onHttpUpgrade(websocket);
                        }

                        asio::async_write(*socket, handshakecontainer->Write, [listener, sock, socket, handshakecontainer](const std::error_code& ec, size_t bytes_transferred) {
                            UNUSED(bytes_transferred);
                            WSocket websocket(sock);
                            if (!ec) {
                                SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Connected: Sent Handshake bytes " << bytes_transferred);
                                if (listener->onConnection) {
                                    listener->onConnection(websocket, handshakecontainer->Header);
                                }
                                ReadHeaderStart(listener, sock, socket);
                            }
                            else {
                                SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "WebSocket receivehandshake failed " + ec.message());
                            }
                        });
                    }
                    else {
                        SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "WebSocket Generate_Handshake failed ");
                    }
                }
                else {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Read Handshake failed " + ec.message());
                }
            });

        }
        void async_handshake(std::shared_ptr<WSListenerImpl> listener, std::shared_ptr<asio::ip::tcp::socket> socket) {
            read_handshake(listener, socket);
        }
        void async_handshake(std::shared_ptr<WSListenerImpl> listener, std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> socket) {

            socket->async_handshake(asio::ssl::stream_base::server, [listener, socket](const std::error_code& ec) {

                if (!ec) {
                    read_handshake(listener, socket);
                }
                else {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "async_handshake failed " << ec.message());
                }
            });

        }

        template<typename SOCKETCREATOR>void Listen(std::shared_ptr<WSListenerImpl> listener, SOCKETCREATOR&& socketcreator) {

            auto socket = socketcreator(listener);
         
            listener->acceptor.async_accept(socket->lowest_layer(), [listener, socket, socketcreator](const std::error_code& ec)
            {   
                std::error_code e;
                socket->lowest_layer().set_option(asio::socket_base::reuse_address(true), e);
                if (e) {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "set_option reuse_address error " << e.message());
                    e.clear();
                }
                socket->lowest_layer().set_option(asio::ip::tcp::no_delay(true), e);
                if (e) {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "set_option no_delay error " << e.message());
                    e.clear();
                }
                if (!ec)
                {
                    async_handshake(listener, socket);
                }
                Listen(listener, socketcreator);
            });

        }
        WSListener  WSListener::CreateListener(unsigned short port)
        {
            WSListener tmp;
            tmp.Impl_ = std::make_shared<WSListenerImpl>(port);
            return tmp;
        }

        WSListener  WSListener::CreateListener(
            unsigned short port,
            std::string Password,
            std::string Privatekey_File,
            std::string Publiccertificate_File,
            std::string dh_File)
        {

            WSListener tmp;
            tmp.Impl_ = std::make_shared<WSListenerImpl>(port, Password, Privatekey_File, Publiccertificate_File, dh_File);
            return tmp;
        }
        void WSListener::startlistening()
        {
            if (Impl_->sslcontext) {
                auto createsocket = [](auto c) {
                    return std::make_shared<asio::ssl::stream<asio::ip::tcp::socket>>(c->io_service, *c->sslcontext);
                };
                Listen(Impl_, createsocket);
            }
            else {
                auto createsocket = [](auto c) {
                    return std::make_shared<asio::ip::tcp::socket>(c->io_service);
                };
                Listen(Impl_, createsocket);
            }
        }
        void WSListener::onConnection(std::function<void(WSocket&, const std::unordered_map<std::string, std::string>&)>& handle) {
            Impl_->onConnection = handle;
        }
        void WSListener::onConnection(const std::function<void(WSocket&, const std::unordered_map<std::string, std::string>&)>& handle) {
            Impl_->onConnection = handle;
        }
        void WSListener::onMessage(std::function<void(WSocket&, const WSMessage&)>& handle) {
            Impl_->onMessage = handle;
        }
        void WSListener::onMessage(const std::function<void(WSocket&, const WSMessage&)>& handle) {
            Impl_->onMessage = handle;
        }
        void WSListener::onDisconnection(std::function<void(WSocket&, unsigned short, const std::string&)>& handle) {
            Impl_->onDisconnection = handle;
        }
        void WSListener::onDisconnection(const std::function<void(WSocket&, unsigned short, const std::string&)>& handle) {
            Impl_->onDisconnection = handle;
        }
        void WSListener::onPing(std::function<void(WSocket&, const unsigned char *, size_t)>& handle) {
            Impl_->onPing = handle;
        }
        void WSListener::onPing(const std::function<void(WSocket&, const unsigned char *, size_t)>& handle) {
            Impl_->onPing = handle;
        }
        void WSListener::onPong(std::function<void(WSocket&, const unsigned char *, size_t)>& handle) {
            Impl_->onPong = handle;
        }
        void WSListener::onPong(const std::function<void(WSocket&, const unsigned char *, size_t)>& handle) {
            Impl_->onPong = handle;
        }
        void WSListener::onHttpUpgrade(std::function<void(WSocket&)>& handle) {
            Impl_->onHttpUpgrade = handle;
        }
        void WSListener::onHttpUpgrade(const std::function<void(WSocket&)>& handle) {
            Impl_->onHttpUpgrade = handle;
        }
        void WSListener::set_ReadTimeout(unsigned int seconds) {
            Impl_->ReadTimeout = seconds;
        }
        unsigned int WSListener::get_ReadTimeout() {
            return  Impl_->ReadTimeout;
        }
        void WSListener::set_WriteTimeout(unsigned int seconds) {
            Impl_->WriteTimeout = seconds;
        }
        unsigned int WSListener::get_WriteTimeout() {
            return  Impl_->WriteTimeout;
        }
        void WSListener::set_MaxPayload(size_t bytes) {
            Impl_->MaxPayload = bytes;
        }
        unsigned long long int WSListener::get_MaxPayload() {
            return  Impl_->MaxPayload;
        }

        void WSListener::send(const WSocket& s, WSMessage& msg, bool compressmessage) {
            sendImpl(Impl_, s.WSocketImpl_, msg, compressmessage);
        }
        void WSListener::close(const WSocket& s, unsigned short code, const std::string& msg)
        {
            closeImpl(Impl_, s.WSocketImpl_, code, msg);
        }
    }
}
