#pragma once
#include <unordered_map>
#include <string>
#include <istream>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			namespace HttpHeader {
				const auto HTTP_METHOD = "Method";
				const auto HTTP_PATH = "Path";
				const auto HTTP_VERSION = "Http_Version";
				const auto HTTP_STATUSCODE = "Http_StatusCode";
				const auto HTTP_CONTENTLENGTH = "Content-Length";
				const auto HTTP_CONTENTTYPE = "Content-Type";
				const auto HTTP_CACHECONTROL = "Cache-Control";
				const auto HTTP_ENDLINE = "\r\n";
				const auto HTTP_KEYVALUEDELIM = ": ";

				std::unordered_map<std::string, std::string> Parse(std::string defaultheaderversion, std::istream& stream);

			}
		}
	}
}