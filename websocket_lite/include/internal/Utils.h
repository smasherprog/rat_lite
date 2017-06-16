#pragma once
#include "WS_Lite.h"
#include "Logging.h"

#include <sstream>
#include <string>
#include <unordered_map>
#include <fstream>

#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

#include <zlib.h>
#include <math.h>
#include <random>

namespace asio {
    namespace ssl {
        class context;
    }
}


namespace SL {
    namespace WS_LITE {


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


        template<class PARENTTYPE>inline bool DidPassMaskRequirement(unsigned char* h) { UNUSED(h); return true; }
        template<> inline bool DidPassMaskRequirement<std::shared_ptr<WSListenerImpl>>(unsigned char* h) { return getMask(h); }
        template<> inline bool DidPassMaskRequirement<std::shared_ptr<WSClientImpl>>(unsigned char* h) { return !getMask(h); }

        template<class PARENTTYPE>inline size_t AdditionalBodyBytesToRead() { return 0; }
        template<>inline size_t AdditionalBodyBytesToRead<std::shared_ptr<WSListenerImpl>>() { return 4; }
        template<>inline size_t AdditionalBodyBytesToRead<std::shared_ptr<WSClientImpl >>() { return 0; }

        template<class PARENTTYPE>inline void set_MaskBitForSending(unsigned char* frame) { UNUSED(frame); }
        template<>inline void set_MaskBitForSending<std::shared_ptr<WSListenerImpl>>(unsigned char* frame) { setMask(frame, 0x00); }
        template<>inline void set_MaskBitForSending<std::shared_ptr<WSClientImpl >>(unsigned char* frame) { setMask(frame, 0xff); }

        template<class SENDBUFFERTYPE>size_t writeheader(unsigned char header[10], const SENDBUFFERTYPE& msg) {
            setFin(header, 0xFF);
            set_MaskBitForSending<PARENTTYPE>(header);
            setOpCode(header, msg.code);
            setrsv1(header, 0x00);
            setrsv2(header, 0x00);
            setrsv3(header, 0x00);
            if (msg.len <= 125) {
                setpayloadLength1(header, hton(static_cast<unsigned char>(msg.len)));
                return 2;
            }
            else if (msg.len > USHRT_MAX) {
                setpayloadLength8(header, hton(static_cast<unsigned long long int>(msg.len)));
                setpayloadLength1(header, 127);
                return 10;
            }
            else {
                setpayloadLength2(header, hton(static_cast<unsigned short>(msg.len)));
                setpayloadLength1(header, 126);
                return  4;
            }
        }
        template<class PARENTTYPE>void writemask(unsigned char mask[4], const WSPreparedMessage& msg) {
            UNUSED(mask);
            UNUSED(msg);
        }
        template<>void writemask<std::shared_ptr<WSListenerImpl>>(unsigned char mask[4], const WSPreparedMessage& msg) {
            UNUSED(mask);
            UNUSED(msg);
        }      
        template<>void writemask<std::shared_ptr<WSClientImpl>>(unsigned char mask[4], const WSPreparedMessage& msg) {
            std::uniform_int_distribution<unsigned int> dist(0, 255);
            std::random_device rd;

            for (auto c = 0; c < 4; c++) {
                mask[c] = static_cast<unsigned char>(dist(rd));
            }
            auto p = reinterpret_cast<unsigned char*>(msg.data);
            for (decltype(msg.len) i = 0; i < msg.len; i++) {
                *p++ ^= mask[i % 4];
            }
        }
        template<class T>std::string get_address(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket.lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.address().to_string();
            else return "";
        }
        template<class T> unsigned short get_port(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket.lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.port();
            else return static_cast<unsigned short>(-1);
        }
        template<class T> bool is_v4(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket.lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.address().is_v4();
            else return true;
        }
        template<class T> bool is_v6(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket.lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.address().is_v6();
            else return true;
        }
        template<class T> bool is_loopback(T& _socket)
        {
            std::error_code ec;
            auto rt(_socket.lowest_layer().remote_endpoint(ec));
            if (!ec) return rt.address().is_loopback();
            else return true;
        }
        template<class type>
        void SHA1(const type& input, type& hash) {
            SHA_CTX context;
            SHA1_Init(&context);
            SHA1_Update(&context, &input[0], input.size());

            hash.resize(160 / 8);
            SHA1_Final((unsigned char*)&hash[0], &context);
        }
        template<class type>
        type SHA1(const type& input) {
            type hash;
            SHA1(input, hash);
            return hash;
        }

        template<class type>
        void Base64encode(const type& ascii, type& base64) {
            BIO *bio, *b64;
            BUF_MEM *bptr;

            b64 = BIO_new(BIO_f_base64());
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
            bio = BIO_new(BIO_s_mem());
            BIO_push(b64, bio);
            BIO_get_mem_ptr(b64, &bptr);

            //Write directly to base64-buffer to avoid copy
            int base64_length = static_cast<int>(round(4 * ceil((double)ascii.size() / 3.0)));
            base64.resize(base64_length);
            bptr->length = 0;
            bptr->max = base64_length + 1;
            bptr->data = (char*)&base64[0];

            BIO_write(b64, &ascii[0], static_cast<int>(ascii.size()));
            BIO_flush(b64);

            //To keep &base64[0] through BIO_free_all(b64)
            bptr->length = 0;
            bptr->max = 0;
            bptr->data = nullptr;

            BIO_free_all(b64);
        }
        template<class type>
        type Base64encode(const type& ascii) {
            type base64;
            Base64encode(ascii, base64);
            return base64;
        }

        template<class type>
        void Base64decode(const type& base64, type& ascii) {
            //Resize ascii, however, the size is a up to two bytes too large.
            ascii.resize((6 * base64.size()) / 8);
            BIO *b64, *bio;

            b64 = BIO_new(BIO_f_base64());
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
            bio = BIO_new_mem_buf((char*)&base64[0], static_cast<int>(base64.size()));
            bio = BIO_push(b64, bio);

            int decoded_length = BIO_read(bio, &ascii[0], static_cast<int>(ascii.size()));
            ascii.resize(decoded_length);

            BIO_free_all(b64);
        }
        template<class type>
        type Base64decode(const type& base64) {
            type ascii;
            Base64decode(base64, ascii);
            return ascii;
        }


        inline std::ifstream::pos_type filesize(const std::string& filename)
        {
            std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
            return in.tellg();
        }

        template <typename T>
        T ntoh(T u)
        {
            static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
            union
            {
                T u;
                unsigned char u8[sizeof(T)];
            } source, dest;
            source.u = u;
            for (size_t k = 0; k < sizeof(T); k++)
                dest.u8[k] = source.u8[sizeof(T) - k - 1];

            return dest.u;
        }
        template <typename T>
        T hton(T u)
        {
            static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
            union
            {
                T u;
                unsigned char u8[sizeof(T)];
            } source, dest;
            source.u = u;
            for (size_t k = 0; k < sizeof(T); k++)
                dest.u8[sizeof(T) - k - 1] = source.u8[k];

            return dest.u;
        }
        std::string url_decode(const std::string& in);

        bool Parse_ClientHandshake(std::istream& stream, std::unordered_map<std::string, std::string>& header);
        bool Parse_ServerHandshake(std::istream& stream, std::unordered_map<std::string, std::string>& header);

        const std::string ws_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        const int LARGE_BUFFER_SIZE = 300 * 1024;

        bool Generate_Handshake(std::unordered_map<std::string, std::string>& header, std::ostream & handshake);
        bool isValidUtf8(unsigned char *s, size_t length);
        char* ZlibInflate(char *data, size_t &length, size_t maxPayload, std::string& dynamicInflationBuffer, z_stream& inflationStream, char* inflationBuffer);

        void add_other_root_certs(asio::ssl::context &ctx);
    }
}