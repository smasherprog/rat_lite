#include "WS_Lite.h"
#include "internal/WebSocketProtocol.h"
#include <memory>

namespace SL {
    namespace WS_LITE {

        WSContext CreateContext(ThreadCount threadcount) {
            return WSContext(std::make_shared<WSContextImpl>(threadcount));
        }
    }
}
