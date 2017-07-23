#include "internal/Utils.h"
#include "Logging.h"

#include <random>
#include <fstream>

#if WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincrypt.h>
#include "asio/ssl.hpp"
#endif

namespace SL {
    namespace WS_LITE {

        std::string url_decode(const std::string& in)
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

        bool Parse_ServerHandshake(std::istream& stream, std::unordered_map<std::string, std::string>& header)
        {
            std::string line;
            std::getline(stream, line);

            size_t path_end;
            if ((path_end = line.find(' ')) != std::string::npos) {

                if (path_end == 8)
                    header[HTTP_VERSION] = line.substr(path_end - 3, 3);
                else
                {
                    SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Parse_ServerHandshake failed due to no http version in header");
                    return false;
                }
                if (line.size() > path_end + 4) {
                    header[HTTP_METHOD] = line.substr(path_end + 1, line.size() - path_end -1);
                }

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
            return true;
        }


        bool Parse_ClientHandshake(std::istream& stream, std::unordered_map<std::string, std::string>& header)
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
                    else {
                        SL_WS_LITE_LOG(Logging_Levels::INFO_log_level, "Parse_ClientHandshake failed due to no http version in header");
                        return false;
                    }

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

        bool Generate_Handshake(std::unordered_map<std::string, std::string>& header, std::ostream & handshake)
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
            else {
                handshake << HTTP_SECWEBSOCKETEXTENSIONS << HTTP_KEYVALUEDELIM << HTTP_ENDLINE;
            }
            handshake << HTTP_SECWEBSOCKETACCEPT << HTTP_KEYVALUEDELIM << Base64encode(sha1) << HTTP_ENDLINE << HTTP_ENDLINE;

            return true;
        }

        // Based on utf8_check.c by Markus Kuhn, 2005
        // https://www.cl.cam.ac.uk/~mgk25/ucs/utf8_check.c
        // Optimized for predominantly 7-bit content by Alex Hultman, 2016
        bool isValidUtf8(unsigned char *s, size_t length)
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

        char* ZlibInflate(char *data, size_t &length, size_t maxPayload, std::string& dynamicInflationBuffer, z_stream& inflationStream, char* inflationBuffer) {
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
#if WIN32

        void add_other_root_certs(asio::ssl::context &ctx)
        {
            HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
            if (hStore == NULL) {
                return;
            }

            X509_STORE *store = X509_STORE_new();
            PCCERT_CONTEXT pContext = NULL;
            while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != NULL) {
                // convert from DER to internal format
                X509 *x509 = d2i_X509(NULL, (const unsigned char **)&pContext->pbCertEncoded, pContext->cbCertEncoded);
                if (x509 != NULL) {
                    X509_STORE_add_cert(store, x509);
                    X509_free(x509);
                }
            }

            CertFreeCertificateContext(pContext);
            CertCloseStore(hStore, 0);

            // attach X509_STORE to boost ssl context
            SSL_CTX_set_cert_store(ctx.native_handle(), store);
        }

#else 
        void add_other_root_certs(asio::ssl::context &ctx) {}
#endif
    }
}
