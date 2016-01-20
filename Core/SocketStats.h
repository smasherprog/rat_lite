#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct SocketStats {
				//total bytes that the Socket layer received from the upper layer. This is not the actual amount of data send across the network due to compressoin, encryption, etc
				long long TotalBytesSent;
				//total bytes that the Socket layer seent to the network layer
				long long NetworkBytesSent;
				//total number of packets sent
				long long TotalPacketSent;
				//total bytes that the upper layer received from the socket layer after decompression, decryption, etc
				long long TotalBytesReceived;
				//total bytes that the Socket layer received from the network layer
				long long NetworkBytesReceived;
				//total number of packets received
				long long TotalPacketReceived;

			};
		}
	}
}