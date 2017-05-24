#pragma once
#include "WS_Lite.h"
#include "Utils.h"
#if WIN32
#include <SDKDDKVer.h>
#endif

#include <unordered_map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <memory>
#include <thread>
#include <random>
#include <deque>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include "asio/deadline_timer.hpp"

#include <zlib.h>

namespace SL {
    namespace WS_LITE {

        inline char* ZlibInflate(char *data, size_t &length, size_t maxPayload, std::string& dynamicInflationBuffer, z_stream& inflationStream, char* inflationBuffer) {
            dynamicInflationBuffer.clear();

            inflationStream.next_in = (Bytef *)data;
            inflationStream.avail_in = length;

            int err;
            do {
                inflationStream.next_out = (Bytef *)inflationBuffer;
                inflationStream.avail_out = LARGE_BUFFER_SIZE;
                err = ::inflate(&inflationStream, Z_FINISH);
                if (!inflationStream.avail_in) {
                    break;
                }

                dynamicInflationBuffer.append(inflationBuffer, LARGE_BUFFER_SIZE - inflationStream.avail_out);
            } while (err == Z_BUF_ERROR && dynamicInflationBuffer.length() <= maxPayload);

            inflateReset(&inflationStream);

            if ((err != Z_BUF_ERROR && err != Z_OK) || dynamicInflationBuffer.length() > maxPayload) {
                length = 0;
                return nullptr;
            }

            if (dynamicInflationBuffer.length()) {
                dynamicInflationBuffer.append(inflationBuffer, LARGE_BUFFER_SIZE - inflationStream.avail_out);

                length = dynamicInflationBuffer.length();
                return (char *)dynamicInflationBuffer.data();
            }

            length = LARGE_BUFFER_SIZE - inflationStream.avail_out;
            return inflationBuffer;
        }

        template<class T>std::string get_address(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket->lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.address().to_string();
            else return "";
        }
        template<class T> unsigned short get_port(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket->lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.port();
            else return static_cast<unsigned short>(-1);
        }
        template<class T> bool is_v4(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket->lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.address().is_v4();
            else return true;
        }
        template<class T> bool is_v6(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket->lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.address().is_v6();
            else return true;
        }
        template<class T> bool is_loopback(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket->lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.address().is_loopback();
            else return true;
        }

        struct WSocketImpl
        {
            WSocketImpl(asio::io_service& s) :read_deadline(s), write_deadline(s) {}
            ~WSocketImpl() {
                canceltimers();
                if (ReceiveBuffer) {
                    free(ReceiveBuffer);
                }
            }
            asio::basic_waitable_timer<std::chrono::steady_clock> read_deadline;
            asio::basic_waitable_timer<std::chrono::steady_clock> write_deadline;
            unsigned char* ReceiveBuffer = nullptr;
            size_t ReceiveBufferSize = 0;
            unsigned char ReceiveHeader[14];
            bool CompressionEnabled = false;

            OpCode LastOpCode = OpCode::INVALID;
            std::shared_ptr<asio::ip::tcp::socket> Socket;
            std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> TLSSocket;
            void canceltimers() {
                read_deadline.cancel();
                write_deadline.cancel();
            }
        };
        inline void set_Socket(std::shared_ptr<WSocketImpl>& ws, std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> s) {
            ws->TLSSocket = s;
        }
        inline void set_Socket(std::shared_ptr<WSocketImpl>& ws, std::shared_ptr<asio::ip::tcp::socket>  s) {
            ws->Socket = s;
        }

        struct SendQueueItem {
            std::shared_ptr<WSocketImpl> socket;
            WSMessage msg;
            bool compressmessage;
        };
        const auto CONTROLBUFFERMAXSIZE = 125;
        struct WSContext {
            WSContext() :
                work(std::make_unique<asio::io_service::work>(io_service)) {
                inflationBuffer = std::make_unique<char[]>(LARGE_BUFFER_SIZE);
                inflateInit2(&inflationStream, -MAX_WBITS);
                io_servicethread = std::thread([&]() {
                    std::error_code ec;
                    io_service.run(ec);
                });
            }
            ~WSContext() {
                SendItems.clear();
                work.reset();
                io_service.stop();
                while (!io_service.stopped()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
                if (io_servicethread.joinable()) io_servicethread.join();
                inflateEnd(&inflationStream);
            }
            std::unique_ptr<char[]> inflationBuffer;
            unsigned int ReadTimeout = 30;
            unsigned int WriteTimeout = 30;
            size_t MaxPayload = 1024 * 1024*20;//20 MB
            std::deque<SendQueueItem> SendItems;
            z_stream inflationStream = {};
            asio::io_service io_service;
            std::thread io_servicethread;
            std::unique_ptr<asio::io_service::work> work;
            std::unique_ptr<asio::ssl::context> sslcontext;

            std::function<void(WSocket&, const std::unordered_map<std::string, std::string>&)> onConnection;
            std::function<void(WSocket&, const WSMessage&)> onMessage;
            std::function<void(WSocket&, unsigned short, const std::string&)> onDisconnection;
            std::function<void(WSocket&, const unsigned char *, size_t)> onPing;
            std::function<void(WSocket&, const unsigned char *, size_t)> onPong;
            std::function<void(WSocket&)> onHttpUpgrade;

        };

        class WSClientImpl : public WSContext {
        public:
            WSClientImpl(std::string Publiccertificate_File)
            {
                sslcontext = std::make_unique<asio::ssl::context>(asio::ssl::context::tlsv11);
                std::ifstream file(Publiccertificate_File, std::ios::binary);
                assert(file);
                std::vector<char> buf;
                buf.resize(static_cast<size_t>(filesize(Publiccertificate_File)));
                file.read(buf.data(), buf.size());
                asio::const_buffer cert(buf.data(), buf.size());
                std::error_code ec;
                sslcontext->add_certificate_authority(cert, ec);
                ec.clear();
                sslcontext->set_default_verify_paths(ec);

            }
            WSClientImpl() {  }
            ~WSClientImpl() {}
        };

        class WSListenerImpl : public WSContext {
        public:

            asio::ip::tcp::acceptor acceptor;

            WSListenerImpl(unsigned short port,
                std::string Password,
                std::string Privatekey_File,
                std::string Publiccertificate_File,
                std::string dh_File) :
                WSListenerImpl(port)
            {
                sslcontext = std::make_unique<asio::ssl::context>(asio::ssl::context::tlsv11);
                sslcontext->set_options(
                    asio::ssl::context::default_workarounds
                    | asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3
                    | asio::ssl::context::single_dh_use);
                std::error_code ec;
                sslcontext->set_password_callback([Password](std::size_t, asio::ssl::context::password_purpose) { return Password; }, ec);
                if (ec) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "set_password_callback " << ec.message());
                    ec.clear();
                }
                sslcontext->use_tmp_dh_file(dh_File, ec);
                if (ec) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "use_tmp_dh_file " << ec.message());
                    ec.clear();
                }
                sslcontext->use_certificate_chain_file(Publiccertificate_File, ec);
                if (ec) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "use_certificate_chain_file " << ec.message());
                    ec.clear();
                }
                sslcontext->set_default_verify_paths(ec);
                if (ec) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "set_default_verify_paths " << ec.message());
                    ec.clear();
                }
                sslcontext->use_private_key_file(Privatekey_File, asio::ssl::context::pem, ec);
                if (ec) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "use_private_key_file " << ec.message());
                    ec.clear();
                }

            }

            WSListenerImpl(unsigned short port) :acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) { 
            
            }

            ~WSListenerImpl() {
                std::error_code ec;
                acceptor.close(ec);
            }
        };

        template<class PARENTTYPE, class SOCKETTYPE> void readexpire_from_now(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket, int seconds)
        {

            std::error_code ec;
            if (seconds <= 0) websocket->read_deadline.cancel(ec);
            else  websocket->read_deadline.expires_from_now(std::chrono::seconds(seconds), ec);
            if (ec) {
                SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, ec.message());
            }
            else if (seconds >= 0) {
                websocket->read_deadline.async_wait([parent, websocket, socket](const std::error_code& ec) {
                    if (ec != asio::error::operation_aborted) {
                        return closeImpl(parent, websocket, 1001, "read timer expired on the socket ");
                    }
                });
            }
        }
        template<class PARENTTYPE, class SOCKETTYPE> void writeexpire_from_now(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket, int seconds)
        {
            std::error_code ec;
            if (seconds <= 0) websocket->write_deadline.cancel(ec);
            else websocket->write_deadline.expires_from_now(std::chrono::seconds(seconds), ec);
            if (ec) {
                SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, ec.message());
            }
            else if (seconds >= 0) {
                websocket->write_deadline.async_wait([parent, websocket, socket, seconds](const std::error_code& ec) {
                    if (ec != asio::error::operation_aborted) {
                        return closeImpl(parent, websocket, 1001, "write timer expired on the socket ");
                    }
                });
            }
        }
        template<class PARENTTYPE>inline void startwrite(const std::shared_ptr<PARENTTYPE>& parent) {
            if (!parent->SendItems.empty()) {
                auto msg(parent->SendItems.back());
                if (msg.socket->Socket) {
                    write(parent, msg.socket, msg.socket->Socket, msg.msg);
                }
                else {
                    write(parent, msg.socket, msg.socket->TLSSocket, msg.msg);
                }
            }
        }
        template<class PARENTTYPE>void sendImpl(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, WSMessage& msg, bool compressmessage) {
            if (compressmessage) {
                assert(msg.code == OpCode::BINARY || msg.code == OpCode::TEXT);
            }
            parent->io_service.post([websocket, msg, parent, compressmessage]() {
                if (parent->SendItems.empty()) {
                    parent->SendItems.push_front(SendQueueItem{ websocket, msg, compressmessage });
                    SL::WS_LITE::startwrite(parent);
                }
                else {
                    parent->SendItems.push_front(SendQueueItem{ websocket, msg , compressmessage });
                }
            });
        }
        template<class PARENTTYPE>void closeImpl(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, unsigned short code, const std::string& msg) {
            SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "closeImpl " << msg);
            WSMessage ws;
            ws.code = OpCode::CLOSE;
            ws.len = sizeof(code) + msg.size();
            ws.Buffer = std::shared_ptr<unsigned char>(new unsigned char[ws.len], [](unsigned char* p) { delete[] p; });
            *reinterpret_cast<unsigned short*>(ws.Buffer.get()) = ntoh(code);
            memcpy(ws.Buffer.get() + sizeof(code), msg.c_str(), msg.size());
            ws.data = ws.Buffer.get();
            sendImpl(parent, websocket, ws, false);
        }


        /*
        0                   1                   2                   3
         0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        +-+-+-+-+-------+-+-------------+-------------------------------+
        |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
        |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
        |N|V|V|V|       |S|             |   (if payload len==126/127)   |
        | |1|2|3|       |K|             |                               |
        +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
        |     Extended payload length continued, if payload len == 127  |
        + - - - - - - - - - - - - - - - +-------------------------------+
        |                               |Masking-key, if MASK set to 1  |
        +-------------------------------+-------------------------------+
        | Masking-key (continued)       |          Payload data         |
        +-------------------------------- - - - - - - - - - - - - - - - +
        :                     Payload data continued ...                :
        + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
        |                     Payload data continued ...                |
        +---------------------------------------------------------------+
        */
        inline bool getFin(unsigned char* frame) { return frame[0] & 128; }
        inline void setFin(unsigned char* frame, unsigned char val) { frame[0] = (val & 128) | (~128 & frame[0]); }

        inline bool getMask(unsigned char* frame) { return frame[1] & 128; }
        inline void setMask(unsigned char* frame, unsigned char val) { frame[1] = (val & 128) | (~128 & frame[1]); }

        inline unsigned char getpayloadLength1(unsigned char *frame) { return frame[1] & 127; }
        inline unsigned short getpayloadLength2(unsigned char *frame) { return *reinterpret_cast<unsigned short*>(frame + 2); }
        inline unsigned long long int getpayloadLength8(unsigned char *frame) { return *reinterpret_cast<unsigned long long int*>(frame + 2); }

        inline void setpayloadLength1(unsigned char *frame, unsigned char  val) { frame[1] = (val & 127) | (~127 & frame[1]); }
        inline void setpayloadLength2(unsigned char *frame, unsigned short val) { *reinterpret_cast<unsigned short*>(frame + 2) = val; }
        inline void setpayloadLength8(unsigned char *frame, unsigned long long int val) { *reinterpret_cast<unsigned long long int *>(frame + 2) = val; }

        inline OpCode getOpCode(unsigned char *frame) { return static_cast<OpCode>(*frame & 15); }
        inline void setOpCode(unsigned char *frame, OpCode val) { frame[0] = (val & 15) | (~15 & frame[0]); }

        inline bool getrsv3(unsigned char *frame) { return *frame & 16; }
        inline bool getrsv2(unsigned char *frame) { return *frame & 32; }
        //compressed?
        inline bool getrsv1(unsigned char *frame) { return *frame & 64; }

        inline void setrsv3(unsigned char *frame, unsigned char val) { frame[0] = (val & 16) | (~16 & frame[0]); }
        inline void setrsv2(unsigned char *frame, unsigned char val) { frame[0] = (val & 32) | (~32 & frame[0]); }
        inline void setrsv1(unsigned char *frame, unsigned char val) { frame[0] = (val & 64) | (~64 & frame[0]); }

        struct HandshakeContainer {
            asio::streambuf Read;
            asio::streambuf Write;
            std::unordered_map<std::string, std::string> Header;
        };
        struct WSSendMessageInternal {
            unsigned char* data;
            size_t len;
            OpCode code;
            //compress the outgoing message?
            bool Compress;
        };

        template<class PARENTTYPE>inline bool DidPassMaskRequirement(unsigned char* h) { return true; }
        template<> inline bool DidPassMaskRequirement<WSListenerImpl>(unsigned char* h) { return getMask(h); }
        template<> inline bool DidPassMaskRequirement<WSClientImpl>(unsigned char* h) { return !getMask(h); }

        template<class PARENTTYPE>inline size_t AdditionalBodyBytesToRead() { return 0; }
        template<>inline size_t AdditionalBodyBytesToRead<WSListenerImpl>() { return 4; }
        template<>inline size_t AdditionalBodyBytesToRead<WSClientImpl>() { return 0; }

        template<class PARENTTYPE>inline void set_MaskBitForSending(unsigned char* frame) {  }
        template<>inline void set_MaskBitForSending<WSListenerImpl>(unsigned char* frame) { setMask(frame, 0x00); }
        template<>inline void set_MaskBitForSending<WSClientImpl>(unsigned char* frame) { setMask(frame, 0xff); }

        template<class PARENTYPE, class SOCKETTYPE, class SENDBUFFERTYPE>inline void handleclose(const PARENTYPE& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {
            SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Closed: " << msg.code);
            if (parent->onDisconnection) {
                WSocket ws(websocket);
                parent->onDisconnection(ws, msg.code, "");

            }
            websocket->canceltimers();
            std::error_code ec;
            socket->lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            ec.clear();
            socket->lowest_layer().close(ec);
        }
        template<class PARENTYPE, class SOCKETTYPE, class SENDBUFFERTYPE>inline void write_end(const PARENTYPE& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {

            asio::async_write(*socket, asio::buffer(msg.data, msg.len), [parent, websocket, socket, msg](const std::error_code& ec, size_t bytes_transferred) {

                UNUSED(bytes_transferred);
                //   assert(msg.len == bytes_transferred);
                if (ec)
                {
                    return closeImpl(parent, websocket, 1002, "write header failed " + ec.message());
                }
                else if (!parent->SendItems.empty()) {
                    parent->SendItems.pop_back();
                }
                if (msg.code == OpCode::CLOSE) {
                    handleclose(parent, websocket, socket, msg);
                }
                else {
                    startwrite(parent);
                }
            });
        }

        template<class SOCKETTYPE, class SENDBUFFERTYPE>inline void writeend(const std::shared_ptr<WSClientImpl>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {
            std::uniform_int_distribution<unsigned int> dist(0, 255);
            std::random_device rd;

            unsigned char mask[4];
            for (auto c = 0; c < 4; c++) {
                mask[c] = static_cast<unsigned char>(dist(rd));
            }
            auto p = reinterpret_cast<unsigned char*>(msg.data);
            for (decltype(msg.len) i = 0; i < msg.len; i++) {
                *p++ ^= mask[i % 4];
            }
            std::error_code ec;
            auto bytes_transferred = asio::write(*socket, asio::buffer(mask, 4), ec);
            UNUSED(bytes_transferred);
            assert(bytes_transferred == 4);
            if (ec)
            {
                if (msg.code == OpCode::CLOSE) {
                    handleclose(parent, websocket, socket, msg);
                }
                else {
                    return closeImpl(parent, websocket, 1002, "write mask failed " + ec.message());
                }
            }
            else {
                write_end(parent, websocket, socket, msg);
            }
        }
        template<class SOCKETTYPE, class SENDBUFFERTYPE>inline void writeend(const std::shared_ptr<WSListenerImpl>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {
            write_end(parent, websocket, socket, msg);
        }

        template<class PARENTTYPE, class SOCKETTYPE, class SENDBUFFERTYPE>inline void write(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {
            size_t sendsize = 0;
            unsigned char header[10] = {};

            setFin(header, 0xFF);
            set_MaskBitForSending<PARENTTYPE>(header);
            setOpCode(header, msg.code);
            setrsv1(header, 0x00);
            setrsv2(header, 0x00);
            setrsv3(header, 0x00);


            if (msg.len <= 125) {
                setpayloadLength1(header, hton(static_cast<unsigned char>(msg.len)));
                sendsize = 2;
            }
            else if (msg.len > USHRT_MAX) {
                setpayloadLength8(header, hton(static_cast<unsigned long long int>(msg.len)));
                setpayloadLength1(header, 127);
                sendsize = 10;
            }
            else {
                setpayloadLength2(header, hton(static_cast<unsigned short>(msg.len)));
                setpayloadLength1(header, 126);
                sendsize = 4;
            }

            assert(msg.len < UINT32_MAX);
            writeexpire_from_now(parent, websocket, socket, parent->WriteTimeout);
            std::error_code ec;
            auto bytes_transferred = asio::write(*socket, asio::buffer(header, sendsize), ec);
            UNUSED(bytes_transferred);
            if (!ec)
            {
                assert(sendsize == bytes_transferred);
                writeend(parent, websocket, socket, msg);
            }
            else {
                return closeImpl(parent, websocket, 1002, "write header failed   " + ec.message());
            }

        }
        inline void UnMaskMessage(const std::shared_ptr<WSListenerImpl>& parent, size_t readsize, unsigned char* buffer) {
            UNUSED(parent);
            auto startp = buffer;
            unsigned char mask[4];
            memcpy(mask, startp, 4);
            for (size_t c = 4; c < readsize; c++) {
                startp[c - 4] = startp[c] ^ mask[c % 4];
            }
        }
        inline void UnMaskMessage(const std::shared_ptr<WSClientImpl>& parent, size_t readsize, unsigned char* buffer) {
            UNUSED(parent);
            UNUSED(readsize);
            UNUSED(buffer);
        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ProcessMessage(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket) {

            auto opcode = getOpCode(websocket->ReceiveHeader);

            if (!getFin(websocket->ReceiveHeader)) {
                if (websocket->LastOpCode == OpCode::INVALID) {
                    if (opcode != OpCode::BINARY && opcode != OpCode::TEXT) {
                        return closeImpl(parent, websocket, 1002, "First Non Fin Frame must be binary or text");
                    }
                    websocket->LastOpCode = opcode;
                }
                else if (opcode != OpCode::CONTINUATION) {
                    return closeImpl(parent, websocket, 1002, "Continuation Received without a previous frame");
                }
                ReadHeaderNext(parent, websocket, socket);
            }
            else {

                if (websocket->LastOpCode != OpCode::INVALID && opcode != OpCode::CONTINUATION) {
                    return closeImpl(parent, websocket, 1002, "Continuation Received without a previous frame");
                }
                else if (websocket->LastOpCode == OpCode::INVALID && opcode == OpCode::CONTINUATION) {
                    return closeImpl(parent, websocket, 1002, "Continuation Received without a previous frame");
                }
                else if (websocket->LastOpCode == OpCode::TEXT || opcode == OpCode::TEXT) {
                    if (!isValidUtf8(websocket->ReceiveBuffer, websocket->ReceiveBufferSize )) {
                        return closeImpl(parent, websocket, 1007, "Frame not valid utf8");
                    }
                }
                if (parent->onMessage) {
                    WSocket wsocket(websocket);

                    auto unpacked = WSMessage{ websocket->ReceiveBuffer,   websocket->ReceiveBufferSize, websocket->LastOpCode != OpCode::INVALID ? websocket->LastOpCode : opcode };
                    parent->onMessage(wsocket, unpacked);
                }
                ReadHeaderStart(parent, websocket, socket);
            }
        }
        template <class PARENTTYPE>inline void SendPong(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const std::shared_ptr<unsigned char>& buffer, size_t size) {
            WSMessage msg;
            msg.Buffer = buffer;
            msg.len = size;
            msg.code = OpCode::PONG;
            msg.data = msg.Buffer.get();

            sendImpl(parent, websocket, msg, false);
        }
        template <class PARENTTYPE>inline void ProcessClose(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const std::shared_ptr<unsigned char>& buffer, size_t size) {
            if (size >= 2) {
                auto closecode =hton(*reinterpret_cast<unsigned short*>(buffer.get()));
                if (size > 2) {
                    if (!isValidUtf8(buffer.get() + sizeof(closecode), size - sizeof(closecode))) {
                        return closeImpl(parent, websocket, 1007, "Frame not valid utf8");
                    }
                }

                if (((closecode >= 1000 && closecode <= 1011 )|| (closecode >=3000 && closecode <=4999)) && closecode != 1004 && closecode != 1005 && closecode != 1006) {
                    return closeImpl(parent, websocket, 1000, "");
                }
                else
                {
                    return closeImpl(parent, websocket, 1002, "");
                }
            }
            else if (size != 0) {
                return closeImpl(parent, websocket, 1002, "");
            }
            return closeImpl(parent, websocket, 1000, "");
        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ProcessControlMessage(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket, const std::shared_ptr<unsigned char>& buffer, size_t size) {
            if (!getFin(websocket->ReceiveHeader)) {
                return closeImpl(parent, websocket, 1002, "Closing connection. Control Frames must be Fin");
            }
            auto opcode = getOpCode(websocket->ReceiveHeader);

            WSocket wsocket(websocket);
            switch (opcode)
            {
            case OpCode::PING:
                if (parent->onPing) {
                    parent->onPing(wsocket, buffer.get(), size);
                }
                SendPong(parent, websocket, buffer, size);
                break;
            case OpCode::PONG:
                if (parent->onPong) {
                    parent->onPong(wsocket, buffer.get(), size);
                }
                // SendPong(parent, websocket);
                break;
            case OpCode::CLOSE:
                return ProcessClose(parent, websocket, buffer, size);

            default:
                return closeImpl(parent, websocket, 1002, "Closing connection. nonvalid op code");
            }
            ReadHeaderNext(parent, websocket, socket);

        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ReadBody(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket) {

            if (!DidPassMaskRequirement<PARENTTYPE>(websocket->ReceiveHeader)) {//Close connection if it did not meet the mask requirement. 
                return closeImpl(parent, websocket, 1002, "Closing connection because mask requirement not met");
            }

            if (getrsv2(websocket->ReceiveHeader) || getrsv3(websocket->ReceiveHeader)) {
                return closeImpl(parent, websocket, 1002, "Closing connection. rsv bit set");
            }
            if ((getrsv1(websocket->ReceiveHeader) && !websocket->CompressionEnabled)) {
                return closeImpl(parent, websocket, 1002, "Closing connection. rsv bit set");
            }
            auto opcode = getOpCode(websocket->ReceiveHeader);

            size_t size = getpayloadLength1(websocket->ReceiveHeader);
            switch (size) {
            case 126:
                size = ntoh(getpayloadLength2(websocket->ReceiveHeader));
                break;
            case 127:
                size = static_cast<size_t>(ntoh(getpayloadLength8(websocket->ReceiveHeader)));
                if (size > std::numeric_limits<std::size_t>::max()) {
                    return closeImpl(parent, websocket, 1009, "Payload exceeded MaxPayload size");
                }
                break;
            default:
                break;
            }
         
            size += AdditionalBodyBytesToRead<PARENTTYPE>();
            if (opcode == OpCode::PING || opcode == OpCode::PONG || opcode == OpCode::CLOSE) {
                if (size - AdditionalBodyBytesToRead<PARENTTYPE>() > CONTROLBUFFERMAXSIZE) {
                    return closeImpl(parent, websocket, 1002, "Payload exceeded for control frames. Size requested " + std::to_string(size));
                }
                else if (size > 0) {
                    auto buffer = std::shared_ptr<unsigned char>(new unsigned char[size], [](auto p) { delete[] p; });
                    asio::async_read(*socket, asio::buffer(buffer.get(), size), [parent, websocket, socket, buffer, size](const std::error_code& ec, size_t bytes_transferred) {

                        if (!ec) {
                            assert(size == bytes_transferred);
                            if (size != bytes_transferred) {
                                return closeImpl(parent, websocket, 1002, "Did not receive all bytes ... ");
                            }
                            UnMaskMessage(parent, size, buffer.get());

                            auto tempsize = size - AdditionalBodyBytesToRead<PARENTTYPE>();
                            ProcessControlMessage(parent, websocket, socket, buffer, tempsize);
                        }
                        else {
                            return closeImpl(parent, websocket, 1002, "ReadBody Error " + ec.message());
                        }
                    });
                }
                else {
                    std::shared_ptr<unsigned char> ptr;
                    ProcessControlMessage(parent, websocket, socket, ptr, 0);
                }
            }

            else if (opcode == OpCode::TEXT || opcode == OpCode::BINARY || opcode == OpCode::CONTINUATION) {
                auto addedsize = websocket->ReceiveBufferSize + size;
                if (addedsize > std::numeric_limits<std::size_t>::max()) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "payload exceeds memory on system!!! ");
                    return closeImpl(parent, websocket, 1009, "Payload exceeded MaxPayload size");
                }
                websocket->ReceiveBufferSize = addedsize;

                if (websocket->ReceiveBufferSize > parent->MaxPayload) {
                    return closeImpl(parent, websocket, 1009, "Payload exceeded MaxPayload size");
                }
                if (websocket->ReceiveBufferSize > std::numeric_limits<std::size_t>::max()) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "payload exceeds memory on system!!! ");
                    return closeImpl(parent, websocket, 1009, "Payload exceeded MaxPayload size");
                }

                if (size > 0) {
                    websocket->ReceiveBuffer = static_cast<unsigned char*>(realloc(websocket->ReceiveBuffer, websocket->ReceiveBufferSize));
                    if (!websocket->ReceiveBuffer) {
                        SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "MEMORY ALLOCATION ERROR!!!");
                        return closeImpl(parent, websocket, 1009, "Payload exceeded MaxPayload size");
                    }
                    asio::async_read(*socket, asio::buffer(websocket->ReceiveBuffer + websocket->ReceiveBufferSize - size, size), [parent, websocket, socket, size](const std::error_code& ec, size_t bytes_transferred) {

                        if (!ec) {
                            assert(size == bytes_transferred);
                            if (size != bytes_transferred) {
                                return closeImpl(parent, websocket, 1002, "Did not receive all bytes ... ");
                            }
                            auto buffer = websocket->ReceiveBuffer + websocket->ReceiveBufferSize - size;
                            UnMaskMessage(parent, size, buffer);
                            websocket->ReceiveBufferSize -= AdditionalBodyBytesToRead<PARENTTYPE>();
                            ProcessMessage(parent, websocket, socket);
                        }
                        else {
                            return closeImpl(parent, websocket, 1002, "ReadBody Error " + ec.message());
                        }
                    });
                }
                else {
                    ProcessMessage(parent, websocket, socket);
                }
            }
            else {
                return closeImpl(parent, websocket, 1002, "Closing connection. nonvalid op code");
            }

        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ReadHeaderStart(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket) {
            free(websocket->ReceiveBuffer);
            websocket->ReceiveBuffer = nullptr;
            websocket->ReceiveBufferSize = 0;
            websocket->LastOpCode = OpCode::INVALID;
            ReadHeaderNext(parent, websocket, socket);
        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ReadHeaderNext(const std::shared_ptr<PARENTTYPE>& parent, const std::shared_ptr<WSocketImpl>& websocket, const SOCKETTYPE& socket) {
            readexpire_from_now(parent, websocket, socket, parent->ReadTimeout);
            asio::async_read(*socket, asio::buffer(websocket->ReceiveHeader, 2), [parent, websocket, socket](const std::error_code& ec, size_t bytes_transferred) {
                UNUSED(bytes_transferred);
                if (!ec) {
                    assert(bytes_transferred == 2);

                    size_t readbytes = getpayloadLength1(websocket->ReceiveHeader);
                    switch (readbytes) {
                    case 126:
                        readbytes = 2;
                        break;
                    case 127:
                        readbytes = 8;
                        break;
                    default:
                        readbytes = 0;
                    }

                  
                    if (readbytes > 1) {
                        asio::async_read(*socket, asio::buffer(websocket->ReceiveHeader + 2, readbytes), [parent, websocket, socket](const std::error_code& ec, size_t) {
                            if (!ec) {
                                ReadBody(parent, websocket, socket);
                            }
                            else {
                                return closeImpl(parent, websocket, 1002, "readheader ExtendedPayloadlen " + ec.message());
                            }
                        });
                    }
                    else {
                        ReadBody(parent, websocket, socket);
                    }

                }
                else {
                    return closeImpl(parent, websocket, 1002, "WebSocket ReadHeader failed " + ec.message());
                }
            });
        }
    }
}