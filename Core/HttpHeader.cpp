#include "stdafx.h"
#include "HttpHeader.h"

std::unordered_map<std::string, std::string> SL::Remote_Access_Library::Network::HttpHeader::Parse(std::string defaultheaderversion, std::istream& stream)
{
	std::unordered_map<std::string, std::string> header;
	std::string line;
	std::getline(stream, line);
	size_t method_end;
	if ((method_end = line.find(' ')) != std::string::npos) {
		size_t path_end;
		if ((path_end = line.find(' ', method_end + 1)) != std::string::npos) {
			header[HTTP_METHOD] = line.substr(0, method_end);
			header[HTTP_PATH] = line.substr(method_end + 1, path_end - method_end - 1);
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
	return header;
}
