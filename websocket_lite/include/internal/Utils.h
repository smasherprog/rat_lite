#pragma once
#include "WS_Lite.h"

#include <unordered_map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <memory>
#include <random>
#include <fstream>

#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

namespace SL {
    namespace WS_LITE {


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
        inline std::string url_decode(const std::string& in)
        {
            std::string out;
            out.reserve(in.size());
            for (std::size_t i = 0; i < in.size(); ++i)
            {
                if (in[i] == '%')
                {
                    if (i + 3 <= in.size())
                    {
                        int value = 0;
                        std::istringstream is(in.substr(i + 1, 2));
                        if (is >> std::hex >> value)
                        {
                            out += static_cast<char>(value);
                            i += 2;
                        }
                        else
                        {
                            return std::string("/");
                        }
                    }
                    else
                    {
                        return std::string("/");
                    }
                }
                else if (in[i] == '+')
                {
                    out += ' ';
                }
                else
                {
                    out += in[i];
                }
            }
            return out;
        }

        inline bool Parse_Handshake(std::string defaultheaderversion, std::istream& stream, std::unordered_map<std::string, std::string>& header)
        {
            std::string line;
            std::getline(stream, line);
            size_t method_end;
            if ((method_end = line.find(' ')) != std::string::npos) {
                size_t path_end;
                if ((path_end = line.find(' ', method_end + 1)) != std::string::npos) {
                    header[HTTP_METHOD] = line.substr(0, method_end);
                    header[HTTP_PATH] = url_decode(line.substr(method_end + 1, path_end - method_end - 1));
                    if ((path_end + 6) < line.size())
                        header[HTTP_VERSION] = line.substr(path_end + 6, line.size() - (path_end + 6) - 1);
                    else
                        header[HTTP_VERSION] = defaultheaderversion;

                    getline(stream, line);
                    size_t param_end;
                    while ((param_end = line.find(':')) != std::string::npos) {
                        size_t value_start = param_end + 1;
                        if ((value_start) < line.size()) {
                            if (line[value_start] == ' ')
                                value_start++;
                            if (value_start < line.size())
                                header.insert(std::make_pair(line.substr(0, param_end), line.substr(value_start, line.size() - value_start - 1)));
                        }

                        getline(stream, line);
                    }
                }
            }
            return true;
        }

        const std::string ws_magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        const int LARGE_BUFFER_SIZE = 300 * 1024;

        inline bool Generate_Handshake(std::unordered_map<std::string, std::string>& header, std::ostream & handshake)
        {
            auto header_it = header.find(HTTP_SECWEBSOCKETKEY);
            if (header_it == header.end()) {
                return false;
            }
            auto sha1 = SHA1(header_it->second + ws_magic_string);
            handshake << "HTTP/1.1 101 Web Socket Protocol Handshake" << HTTP_ENDLINE;
            handshake << "Upgrade: websocket" << HTTP_ENDLINE;
            handshake << "Connection: Upgrade" << HTTP_ENDLINE;
            header_it = header.find(HTTP_SECWEBSOCKETEXTENSIONS);
            if (header_it != header.end() && header_it->second.find(PERMESSAGEDEFLATE) != std::string::npos) {
                handshake << HTTP_SECWEBSOCKETEXTENSIONS << HTTP_KEYVALUEDELIM << PERMESSAGEDEFLATE << HTTP_ENDLINE;
            }
            handshake << HTTP_SECWEBSOCKETACCEPT << HTTP_KEYVALUEDELIM << Base64encode(sha1) << HTTP_ENDLINE << HTTP_ENDLINE;

            return true;
        }

        // Based on utf8_check.c by Markus Kuhn, 2005
        // https://www.cl.cam.ac.uk/~mgk25/ucs/utf8_check.c
        // Optimized for predominantly 7-bit content by Alex Hultman, 2016
        static bool isValidUtf8(unsigned char *s, size_t length)
        {
            for (unsigned char *e = s + length; s != e; ) {
                if (s + 4 <= e && ((*(uint32_t *)s) & 0x80808080) == 0) {
                    s += 4;
                }
                else {
                    while (!(*s & 0x80)) {
                        if (++s == e) {
                            return true;
                        }
                    }

                    if ((s[0] & 0x60) == 0x40) {
                        if (s + 1 >= e || (s[1] & 0xc0) != 0x80 || (s[0] & 0xfe) == 0xc0) {
                            return false;
                        }
                        s += 2;
                    }
                    else if ((s[0] & 0xf0) == 0xe0) {
                        if (s + 2 >= e || (s[1] & 0xc0) != 0x80 || (s[2] & 0xc0) != 0x80 ||
                            (s[0] == 0xe0 && (s[1] & 0xe0) == 0x80) || (s[0] == 0xed && (s[1] & 0xe0) == 0xa0)) {
                            return false;
                        }
                        s += 3;
                    }
                    else if ((s[0] & 0xf8) == 0xf0) {
                        if (s + 3 >= e || (s[1] & 0xc0) != 0x80 || (s[2] & 0xc0) != 0x80 || (s[3] & 0xc0) != 0x80 ||
                            (s[0] == 0xf0 && (s[1] & 0xf0) == 0x80) || (s[0] == 0xf4 && s[1] > 0x8f) || s[0] > 0xf4) {
                            return false;
                        }
                        s += 4;
                    }
                    else {
                        return false;
                    }
                }
            }
            return true;
        }

    }
}