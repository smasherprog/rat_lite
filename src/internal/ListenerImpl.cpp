#include "WS_Lite.h"
#include "Logging.h"
#include "internal/WebSocketProtocol.h"

namespace SL {
    namespace WS_LITE {

        template<class SOCKETTYPE>void read_handshake(std::shared_ptr<WSListenerImpl> listener, const SOCKETTYPE& socket) {
            auto handshakecontainer(std::make_shared<HandshakeContainer>());
            asio::async_read_until(socket->Socket, handshakecontainer->Read, "\r\n\r\n", [listener, socket, handshakecontainer](const std::error_code& ec, size_t bytes_transferred) {
                UNUSED(bytes_transferred);
                if (!ec) {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Read Handshake bytes " << bytes_transferred);

                    std::istream headerdata(&handshakecontainer->Read);

                    if (!Parse_ClientHandshake(headerdata, handshakecontainer->Header)) {
                        SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Parse Handshake failed ");
                        return;
                    }
               /*     if (handshakecontainer->Read.size() > 0) {

                        SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "READ MORE DATA " << handshakecontainer->Read.size());
                        return;
                    }*/

                    std::ostream handshake(&handshakecontainer->Write);
                    if (Generate_Handshake(handshakecontainer->Header, handshake)) {
                        if (handshakecontainer->Header.find(PERMESSAGEDEFLATE) != handshakecontainer->Header.end()) {
                            socket->CompressionEnabled = true;
                        }

                        asio::async_write(socket->Socket, handshakecontainer->Write, [listener, socket, handshakecontainer](const std::error_code& ec, size_t bytes_transferred) {
                            UNUSED(bytes_transferred);
                            if (!ec) {
                                SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Connected: Sent Handshake bytes " << bytes_transferred);
                                if (listener->onConnection) {
                                    listener->onConnection(socket, handshakecontainer->Header);
                                }
                                ReadHeaderStart(listener, socket);
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
        void async_handshake(std::shared_ptr<WSListenerImpl> listener, std::shared_ptr<WSocket<asio::ip::tcp::socket, WSListenerImpl>> socket) {
            read_handshake(listener, socket);
        }
        void async_handshake(std::shared_ptr<WSListenerImpl> listener, std::shared_ptr<WSocket<asio::ssl::stream<asio::ip::tcp::socket>, WSListenerImpl>> socket) {
            socket->Socket.async_handshake(asio::ssl::stream_base::server, [listener, socket](const std::error_code& ec) {
                if (!ec) {
                    read_handshake(listener, socket);
                }
                else {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "async_handshake failed " << ec.message());
                }
            });

        }

        template<class PARENTTYPE, typename SOCKETCREATOR>void Listen(PARENTTYPE& listener, SOCKETCREATOR&& socketcreator, bool no_delay, bool reuse_address) {

            auto socket = socketcreator(listener);
            listener->acceptor.async_accept(socket->Socket.lowest_layer(), [listener, socket, socketcreator, no_delay, reuse_address](const std::error_code& ec)
            {   
                std::error_code e;
                socket->Socket.lowest_layer().set_option(asio::socket_base::reuse_address(reuse_address), e);
                if (e) {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "set_option reuse_address error " << e.message());
                    e.clear();
                }
                socket->Socket.lowest_layer().set_option(asio::ip::tcp::no_delay(no_delay), e);
                if (e) {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "set_option no_delay error " << e.message());
                    e.clear();
                }
                if (!ec)
                {
                    async_handshake(listener, socket);
                }
                Listen(listener, socketcreator, no_delay, reuse_address);
            });
        }


        void WSListener::set_ReadTimeout(std::chrono::seconds seconds) {
            Impl_->ReadTimeout = seconds;
        }
        std::chrono::seconds WSListener::get_ReadTimeout() {
            return  Impl_->ReadTimeout;
        }
        void WSListener::set_WriteTimeout(std::chrono::seconds seconds) {
            Impl_->WriteTimeout = seconds;
        }
        std::chrono::seconds WSListener::get_WriteTimeout() {
            return  Impl_->WriteTimeout;
        }
        void WSListener::set_MaxPayload(size_t bytes) {
            Impl_->MaxPayload = bytes;
        }
        size_t WSListener::get_MaxPayload() {
            return  Impl_->MaxPayload;
        }
        WSListener_Configuration WSListener_Configuration::onConnection(const std::function<void(const std::shared_ptr<IWSocket>&, const std::unordered_map<std::string, std::string>&)>& handle) {
            assert(!Impl_->onConnection);
            Impl_->onConnection = handle;
            return WSListener_Configuration(Impl_);
        }
        WSListener_Configuration WSListener_Configuration::onMessage(const std::function<void(const std::shared_ptr<IWSocket>&, const WSMessage&)>& handle) {
            assert(!Impl_->onMessage);
            Impl_->onMessage = handle;
            return WSListener_Configuration(Impl_);
        }
        WSListener_Configuration WSListener_Configuration::onDisconnection(const std::function<void(const std::shared_ptr<IWSocket>&, unsigned short, const std::string&)>& handle) {
            assert(!Impl_->onDisconnection);
            Impl_->onDisconnection = handle;
            return WSListener_Configuration(Impl_);
        }
        WSListener_Configuration WSListener_Configuration::onPing(const std::function<void(const std::shared_ptr<IWSocket>&, const unsigned char *, size_t)>& handle) {
            assert(!Impl_->onPing);
            Impl_->onPing = handle;
            return WSListener_Configuration(Impl_);
        }
        WSListener_Configuration WSListener_Configuration::onPong(const std::function<void(const std::shared_ptr<IWSocket>&, const unsigned char *, size_t)>& handle) {
            assert(!Impl_->onPong);
            Impl_->onPong = handle;
            return WSListener_Configuration(Impl_);
        }
        WSListener WSListener_Configuration::listen(bool no_delay, bool reuse_address)
        {
            if (Impl_->TLSEnabled) {
                auto createsocket = [](auto c) {
                    return std::make_shared<WSocket<asio::ssl::stream<asio::ip::tcp::socket>, WSListenerImpl>>(c, c->sslcontext);
                };
                Listen(Impl_, createsocket, no_delay, reuse_address);
            }
            else {
                auto createsocket = [](auto c) {
                    return std::make_shared<WSocket<asio::ip::tcp::socket, WSListenerImpl>>(c);
                };
                Listen(Impl_, createsocket, no_delay, reuse_address);
            }
            return WSListener(Impl_);
        }

        WSListener_Configuration WSContext::CreateListener(PortNumber port, ExtensionOptions options)
        {
            UNUSED(options);
            return WSListener_Configuration(std::make_shared<WSListenerImpl>(Impl_, port));
        }

        WSListener_Configuration WSContext::CreateTLSListener(
            PortNumber port,
            std::string Password,
            std::string Privatekey_File,
            std::string Publiccertificate_File,
            std::string dh_File, 
            ExtensionOptions options)
        {
            UNUSED(options);
            return WSListener_Configuration(std::make_shared<WSListenerImpl>(Impl_, port, Password, Privatekey_File, Publiccertificate_File, dh_File));
        }
    }
}
