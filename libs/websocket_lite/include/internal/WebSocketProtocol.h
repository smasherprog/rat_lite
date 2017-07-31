#pragma once
#include "WS_Lite.h"
#include "DataStructures.h"
#include "Utils.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <random>
#include <deque>

#if WIN32
#include <SDKDDKVer.h>
#endif

#include "asio.hpp"
#include "asio/ssl.hpp"
#include "asio/deadline_timer.hpp"

namespace SL {
    namespace WS_LITE {


        template<class PARENTTYPE, class SOCKETTYPE> void readexpire_from_now(const PARENTTYPE& parent, const SOCKETTYPE& socket, std::chrono::seconds secs)
        {
            std::error_code ec;
            if (secs.count() == 0) socket->read_deadline.cancel(ec);
            socket->read_deadline.expires_from_now(secs, ec);
            if (ec) {
                SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, ec.message());
            }
            else if (secs.count() > 0) {
                socket->read_deadline.async_wait([parent, socket](const std::error_code& ec) {
                    if (ec != asio::error::operation_aborted) {
                        return sendclosemessage(parent, socket, 1001, "read timer expired on the socket ");
                    }
                });
            }
        }
        template<class PARENTTYPE, class SOCKETTYPE> void writeexpire_from_now(const PARENTTYPE& parent, const SOCKETTYPE& socket, std::chrono::seconds secs)
        {

            std::error_code ec;
            if (secs.count() == 0) socket->write_deadline.cancel(ec);
            socket->write_deadline.expires_from_now(secs, ec);
            if (ec) {
                SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, ec.message());
            }
            else if (secs.count() > 0) {
                socket->write_deadline.async_wait([parent, socket](const std::error_code& ec) {
                    if (ec != asio::error::operation_aborted) {
                        return sendclosemessage(parent, socket, 1001, "write timer expired on the socket ");
                    }
                });
            }
        }
        template<class PARENTTYPE, class SOCKETTYPE>inline void startwrite(const PARENTTYPE& parent, const SOCKETTYPE& socket) {
            if (!socket->SendMessageQueue.empty()) {
                socket->Writing = true;
                auto msg(socket->SendMessageQueue.front());
                write(parent, socket, msg.msg);
            }
            else {
                socket->Writing = false;
                writeexpire_from_now(parent, socket, std::chrono::seconds(0));// make sure the write timer doesnt kick off
            }
        }
        template<class PARENTTYPE, class SOCKETTYPE>void sendImpl(const PARENTTYPE& parent, const SOCKETTYPE& socket, WSMessage& msg, bool compressmessage) {
            if (compressmessage) {
                assert(msg.code == OpCode::BINARY || msg.code == OpCode::TEXT);
            }

            socket->strand.post([socket, msg, parent, compressmessage]() {
                if (socket->SocketStatus_ == SocketStatus::CONNECTED) {
                    //update the socket status to reflect it is closing to prevent other messages from being sent.. this is the last valid message
                    //make sure to do this after a call to write so the write process sends the close message, but no others
                    if (msg.code == OpCode::CLOSE) {
                        socket->SocketStatus_ = SocketStatus::CLOSING;
                    }
                    socket->SendMessageQueue.emplace_back(SendQueueItem{ msg, compressmessage });
                    if (!socket->Writing) {
                        SL::WS_LITE::startwrite(parent, socket);
                    }
                }
            });
        }
        template<class PARENTTYPE, class SOCKETTYPE>void sendclosemessage(const PARENTTYPE& parent, const SOCKETTYPE& socket, unsigned short code, const std::string& msg) {
            SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "closeImpl " << msg);
            WSMessage ws;
            ws.code = OpCode::CLOSE;
            ws.len = sizeof(code) + msg.size();
            ws.Buffer = std::shared_ptr<unsigned char>(new unsigned char[ws.len], [](unsigned char* p) { delete[] p; });
            *reinterpret_cast<unsigned short*>(ws.Buffer.get()) = ntoh(code);
            memcpy(ws.Buffer.get() + sizeof(code), msg.c_str(), msg.size());
            ws.data = ws.Buffer.get();
            sendImpl(parent, socket, ws, false);
        }

        template<class PARENTTYPE, class SOCKETTYPE, class SENDBUFFERTYPE>inline void handleclose(const PARENTTYPE& parent, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {
            SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Closed: " << msg.code);
            socket->SocketStatus_ = SocketStatus::CLOSED;
            if (parent->onDisconnection) {
                parent->onDisconnection(socket, msg.code, "");
            }
            socket->SendMessageQueue.clear();//clear all outbound messages
            socket->canceltimers();
            std::error_code ec;
            socket->Socket.lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            ec.clear();
            socket->Socket.lowest_layer().close(ec);
        }

        template<class PARENTTYPE, class SOCKETTYPE, class SENDBUFFERTYPE>void closeImpl(const PARENTTYPE& parent, const SOCKETTYPE& socket, unsigned short code, const std::string& msg, const SENDBUFFERTYPE& networkmsg) {
            socket->Writing = false;
            if (networkmsg.code == OpCode::CLOSE) {
                //failed when sending a close message... get out and notify
                handleclose(parent, socket, networkmsg);
            }
            else {
                sendclosemessage(parent, socket, code, msg);
            }
        }
        template<class PARENTTYPE, class SOCKETTYPE, class SENDBUFFERTYPE>inline void write_end(const PARENTTYPE& parent, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {

            asio::async_write(socket->Socket, asio::buffer(msg.data, msg.len), socket->strand.wrap([parent, socket, msg](const std::error_code& ec, size_t bytes_transferred) {
                if (!socket->SendMessageQueue.empty()) {
                    socket->SendMessageQueue.pop_front();
                }
                UNUSED(bytes_transferred);
                if (ec)
                {
                    return closeImpl(parent, socket, 1002, "write header failed " + ec.message(), msg);
                }
                assert(msg.len == bytes_transferred);
                if (msg.code == OpCode::CLOSE) {
                    handleclose(parent, socket, msg);
                }
                else {
                    startwrite(parent, socket);
                }
            }));
        }

        template<class SOCKETTYPE, class SENDBUFFERTYPE>inline void writeend(const std::shared_ptr<WSClientImpl>& parent, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {
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
            auto bytes_transferred = asio::write(socket->Socket, asio::buffer(mask, 4), ec);
            if (ec)
            {
                if (msg.code == OpCode::CLOSE) {
                    handleclose(parent, socket, msg);
                }
                else {
                    return closeImpl(parent, socket, 1002, "write mask failed " + ec.message(), msg);
                }
            }
            else {
                UNUSED(bytes_transferred);
                assert(bytes_transferred == 4);
                write_end(parent, socket, msg);
            }
        }
        template<class SOCKETTYPE, class SENDBUFFERTYPE>inline void writeend(const std::shared_ptr<WSListenerImpl>& parent, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {
            write_end(parent, socket, msg);
        }

        template<class PARENTTYPE, class SOCKETTYPE, class SENDBUFFERTYPE>inline void write(const PARENTTYPE& parent, const SOCKETTYPE& socket, const SENDBUFFERTYPE& msg) {
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
            writeexpire_from_now(parent, socket, parent->WriteTimeout);
            std::error_code ec;
            auto bytes_transferred = asio::write(socket->Socket, asio::buffer(header, sendsize), ec);
            UNUSED(bytes_transferred);
            if (!ec)
            {
                assert(sendsize == bytes_transferred);
                writeend(parent, socket, msg);
            }
            else {
                return closeImpl(parent, socket, 1002, "write header failed   " + ec.message(), msg);
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
        template <class PARENTTYPE, class SOCKETTYPE>inline void ProcessMessage(const PARENTTYPE& parent, const SOCKETTYPE& socket) {

            auto opcode = getOpCode(socket->ReceiveHeader);

            if (!getFin(socket->ReceiveHeader)) {
                if (socket->LastOpCode == OpCode::INVALID) {
                    if (opcode != OpCode::BINARY && opcode != OpCode::TEXT) {
                        return sendclosemessage(parent, socket, 1002, "First Non Fin Frame must be binary or text");
                    }
                    socket->LastOpCode = opcode;
                }
                else if (opcode != OpCode::CONTINUATION) {
                    return sendclosemessage(parent, socket, 1002, "Continuation Received without a previous frame");
                }
                ReadHeaderNext(parent, socket);
            }
            else {

                if (socket->LastOpCode != OpCode::INVALID && opcode != OpCode::CONTINUATION) {
                    return sendclosemessage(parent, socket, 1002, "Continuation Received without a previous frame");
                }
                else if (socket->LastOpCode == OpCode::INVALID && opcode == OpCode::CONTINUATION) {
                    return sendclosemessage(parent, socket, 1002, "Continuation Received without a previous frame");
                }
                else if (socket->LastOpCode == OpCode::TEXT || opcode == OpCode::TEXT) {
                    if (!isValidUtf8(socket->ReceiveBuffer, socket->ReceiveBufferSize)) {
                        return sendclosemessage(parent, socket, 1007, "Frame not valid utf8");
                    }
                }
                if (parent->onMessage) {
                    auto unpacked = WSMessage{ socket->ReceiveBuffer,   socket->ReceiveBufferSize, socket->LastOpCode != OpCode::INVALID ? socket->LastOpCode : opcode };
                    parent->onMessage(socket, unpacked);
                }
                ReadHeaderStart(parent, socket);
            }
        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void SendPong(const PARENTTYPE& parent, const SOCKETTYPE& socket, const std::shared_ptr<unsigned char>& buffer, size_t size) {
            WSMessage msg;
            msg.Buffer = buffer;
            msg.len = size;
            msg.code = OpCode::PONG;
            msg.data = msg.Buffer.get();

            sendImpl(parent, socket, msg, false);
        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ProcessClose(const PARENTTYPE& parent, const SOCKETTYPE& socket, const std::shared_ptr<unsigned char>& buffer, size_t size) {
            if (size >= 2) {
                auto closecode = hton(*reinterpret_cast<unsigned short*>(buffer.get()));
                if (size > 2) {
                    if (!isValidUtf8(buffer.get() + sizeof(closecode), size - sizeof(closecode))) {
                        return sendclosemessage(parent, socket, 1007, "Frame not valid utf8");
                    }
                }

                if (((closecode >= 1000 && closecode <= 1011) || (closecode >= 3000 && closecode <= 4999)) && closecode != 1004 && closecode != 1005 && closecode != 1006) {
                    return sendclosemessage(parent, socket, 1000, "");
                }
                else
                {
                    return sendclosemessage(parent, socket, 1002, "");
                }
            }
            else if (size != 0) {
                return sendclosemessage(parent, socket, 1002, "");
            }
            return sendclosemessage(parent, socket, 1000, "");
        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ProcessControlMessage(const PARENTTYPE& parent, const SOCKETTYPE& socket, const std::shared_ptr<unsigned char>& buffer, size_t size) {
            if (!getFin(socket->ReceiveHeader)) {
                return sendclosemessage(parent, socket, 1002, "Closing connection. Control Frames must be Fin");
            }
            auto opcode = getOpCode(socket->ReceiveHeader);

            switch (opcode)
            {
            case OpCode::PING:
                if (parent->onPing) {
                    parent->onPing(socket, buffer.get(), size);
                }
                SendPong(parent, socket, buffer, size);
                break;
            case OpCode::PONG:
                if (parent->onPong) {
                    parent->onPong(socket, buffer.get(), size);
                }

                break;
            case OpCode::CLOSE:
                return ProcessClose(parent, socket, buffer, size);

            default:
                return sendclosemessage(parent, socket, 1002, "Closing connection. nonvalid op code");
            }
            ReadHeaderNext(parent, socket);

        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ReadBody(const PARENTTYPE& parent, const SOCKETTYPE& socket) {

            if (!DidPassMaskRequirement<PARENTTYPE>(socket->ReceiveHeader)) {//Close connection if it did not meet the mask requirement. 
                return sendclosemessage(parent, socket, 1002, "Closing connection because mask requirement not met");
            }

            if (getrsv2(socket->ReceiveHeader) || getrsv3(socket->ReceiveHeader) || (getrsv1(socket->ReceiveHeader) && !socket->CompressionEnabled)) {
                return sendclosemessage(parent, socket, 1002, "Closing connection. rsv bit set");
            }

            auto opcode = getOpCode(socket->ReceiveHeader);

            size_t size = getpayloadLength1(socket->ReceiveHeader);
            switch (size) {
            case 126:
                size = ntoh(getpayloadLength2(socket->ReceiveHeader));
                break;
            case 127:
                size = static_cast<size_t>(ntoh(getpayloadLength8(socket->ReceiveHeader)));
                if (size > std::numeric_limits<std::size_t>::max()) {
                    return sendclosemessage(parent, socket, 1009, "Payload exceeded MaxPayload size");
                }
                break;
            default:
                break;
            }

            size += AdditionalBodyBytesToRead<PARENTTYPE>();
            if (opcode == OpCode::PING || opcode == OpCode::PONG || opcode == OpCode::CLOSE) {
                if (size - AdditionalBodyBytesToRead<PARENTTYPE>() > CONTROLBUFFERMAXSIZE) {
                    return sendclosemessage(parent, socket, 1002, "Payload exceeded for control frames. Size requested " + std::to_string(size));
                }
                else if (size > 0) {
                    auto buffer = std::shared_ptr<unsigned char>(new unsigned char[size], [](auto p) { delete[] p; });
                    asio::async_read(socket->Socket, asio::buffer(buffer.get(), size), [parent, socket, buffer, size](const std::error_code& ec, size_t bytes_transferred) {

                        if (!ec || bytes_transferred != size) {
                            if (size != bytes_transferred) {
                                return sendclosemessage(parent, socket, 1002, "Did not receive all bytes ... ");
                            }
                            UnMaskMessage(parent, size, buffer.get());

                            auto tempsize = size - AdditionalBodyBytesToRead<PARENTTYPE>();
                            ProcessControlMessage(parent, socket, buffer, tempsize);
                        }
                        else {
                            return sendclosemessage(parent, socket, 1002, "ReadBody Error " + ec.message());
                        }
                    });
                }
                else {
                    std::shared_ptr<unsigned char> ptr;
                    ProcessControlMessage(parent, socket, ptr, 0);
                }
            }

            else if (opcode == OpCode::TEXT || opcode == OpCode::BINARY || opcode == OpCode::CONTINUATION) {
                auto addedsize = socket->ReceiveBufferSize + size;
                if (addedsize > std::numeric_limits<std::size_t>::max()) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "payload exceeds memory on system!!! ");
                    return sendclosemessage(parent, socket, 1009, "Payload exceeded MaxPayload size");
                }
                socket->ReceiveBufferSize = addedsize;

                if (socket->ReceiveBufferSize > parent->MaxPayload) {
                    return sendclosemessage(parent, socket, 1009, "Payload exceeded MaxPayload size");
                }
                if (socket->ReceiveBufferSize > std::numeric_limits<std::size_t>::max()) {
                    SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "payload exceeds memory on system!!! ");
                    return sendclosemessage(parent, socket, 1009, "Payload exceeded MaxPayload size");
                }

                if (size > 0) {
                    socket->ReceiveBuffer = static_cast<unsigned char*>(realloc(socket->ReceiveBuffer, socket->ReceiveBufferSize));
                    if (!socket->ReceiveBuffer) {
                        SL_WS_LITE_LOG(Logging_Levels::ERROR_log_level, "MEMORY ALLOCATION ERROR!!! Tried to realloc " << socket->ReceiveBufferSize);
                        return sendclosemessage(parent, socket, 1009, "Payload exceeded MaxPayload size");
                    }
                    asio::async_read(socket->Socket, asio::buffer(socket->ReceiveBuffer + socket->ReceiveBufferSize - size, size), [parent, socket, size](const std::error_code& ec, size_t bytes_transferred) {

                        if (!ec || bytes_transferred != size) {
                            if (size != bytes_transferred) {
                                return sendclosemessage(parent, socket, 1002, "Did not receive all bytes ... ");
                            }
                            auto buffer = socket->ReceiveBuffer + socket->ReceiveBufferSize - size;
                            UnMaskMessage(parent, size, buffer);
                            socket->ReceiveBufferSize -= AdditionalBodyBytesToRead<PARENTTYPE>();
                            ProcessMessage(parent, socket);
                        }
                        else {
                            return sendclosemessage(parent, socket, 1002, "ReadBody Error " + ec.message());
                        }
                    });
                }
                else {
                    ProcessMessage(parent, socket);
                }
            }
            else {
                return sendclosemessage(parent, socket, 1002, "Closing connection. nonvalid op code");
            }

        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ReadHeaderStart(const PARENTTYPE& parent, const SOCKETTYPE& socket) {
            free(socket->ReceiveBuffer);
            socket->ReceiveBuffer = nullptr;
            socket->ReceiveBufferSize = 0;
            socket->LastOpCode = OpCode::INVALID;
            ReadHeaderNext(parent, socket);
        }
        template <class PARENTTYPE, class SOCKETTYPE>inline void ReadHeaderNext(const PARENTTYPE& parent, const SOCKETTYPE& socket) {
            readexpire_from_now(parent, socket, parent->ReadTimeout);
            asio::async_read(socket->Socket, asio::buffer(socket->ReceiveHeader, 2), [parent, socket](const std::error_code& ec, size_t bytes_transferred) {
                UNUSED(bytes_transferred);
                if (!ec || bytes_transferred != 2) {
                    size_t readbytes = getpayloadLength1(socket->ReceiveHeader);
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
                        asio::async_read(socket->Socket, asio::buffer(socket->ReceiveHeader + 2, readbytes), [parent, socket](const std::error_code& ec, size_t) {
                            if (!ec) {
                                ReadBody(parent, socket);
                            }
                            else {
                                return sendclosemessage(parent, socket, 1002, "readheader ExtendedPayloadlen " + ec.message());
                            }
                        });
                    }
                    else {
                        ReadBody(parent, socket);
                    }

                }
                else {
                    return sendclosemessage(parent, socket, 1002, "WebSocket ReadHeader failed " + ec.message());
                }
            });
        }
    }
}