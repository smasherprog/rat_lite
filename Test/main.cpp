
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "ClientNetworkDriver.h"
#include "ServerNetworkDriver.h"
#include "IClientDriver.h"

class TestClientDriver : public SL::RAT::IClientDriver {
public:

	std::shared_ptr<SL::RAT::Client_Config> Config_;

	SL::RAT::ClientNetworkDriver ClientNetworkDriver_;

};


TEST_CASE("Factorials are computed", "[factorial]") {




}