#include "internal/ServerHub.h"
#include "Input.h"
#include "WS_Lite.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <assert.h>

using namespace std::chrono_literals;

namespace SL {
	namespace RAT {

		class ServerHubImpl {
		public:
			std::shared_ptr<Server_Config> Config_;
			WS_LITE::WSListener h;

			std::atomic_int ClientCount;
			IServerDriver * IServerDriver_;

            bool thisclosed = false;

			ServerHubImpl(IServerDriver* r, std::shared_ptr<Server_Config> config) : IServerDriver_(r), Config_(config) {
                h = WS_LITE::WSListener::CreateListener(config->WebSocketTLSLPort);
				ClientCount = 0;

				h.onConnection([&](const WS_LITE::WSocket& socket, const std::unordered_map<std::string, std::string>& map) {
					static int counter = 0;
					if (ClientCount + 1 > Config_->MaxNumConnections) {
                        h.close(socket, 1000, "Closing due to max number of connections!");
					}
					else {
						int t = counter++ % Config_->MaxWebSocketThreads;
						SL_RAT_LOG(Logging_Levels::INFO_log_level, "Transfering connection to thread " << t);
						ClientCount += 1;
						IServerDriver_->onConnection(socket);
					}
				});


				// register our events

				h.onDisconnection([&](const WS_LITE::WSocket& ws, unsigned short code, const std::string& msg) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "onDisconnection  ");
					ClientCount -= 1;
					IServerDriver_->onDisconnection(ws, code, msg);
				});
				h.onMessage([&](const WS_LITE::WSocket&ws, const WS_LITE::WSMessage& msg) {
                    auto length = msg.len;
					auto pactype = PACKET_TYPES::INVALID;
					assert(length >= sizeof(pactype));

					pactype = *reinterpret_cast<const PACKET_TYPES*>(msg.data);
					length -= sizeof(pactype);
					auto data = msg.data + sizeof(pactype);
					switch (pactype) {
					case PACKET_TYPES::MOUSEEVENT:
						assert(length == sizeof(MouseEvent));
						IServerDriver_->onReceive_Mouse(reinterpret_cast<const MouseEvent*>(data));
						break;
					case PACKET_TYPES::KEYEVENT:
						assert(length == sizeof(KeyEvent));
						IServerDriver_->onReceive_Key(reinterpret_cast<const KeyEvent*>(data));
						break;
					case PACKET_TYPES::CLIPBOARDTEXTEVENT:
						IServerDriver_->onReceive_ClipboardText(data, length);
						break;
					default:
						IServerDriver_->onMessage(ws, msg);
						break;
					}
                });
			}

			~ServerHubImpl() {

			}

			void Broadcast(char * data, size_t len) {
				if (ClientCount <= 0) return;//no one here to send to

			}


		};

	}
}

SL::RAT::ServerHub::ServerHub(IServerDriver * r, std::shared_ptr<Server_Config> config)
{
	ServerHubImpl_ = std::make_unique<ServerHubImpl>(r, config);
}

SL::RAT::ServerHub::~ServerHub()
{
}

void SL::RAT::ServerHub::Run()
{
    ServerHubImpl_->h.startlistening();
}

void SL::RAT::ServerHub::Broadcast(char * data, size_t len)
{
	ServerHubImpl_->Broadcast(data, len);
}

int SL::RAT::ServerHub::get_ClientCount() const
{
	return ServerHubImpl_->ClientCount;
}
