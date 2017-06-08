#include "WS_Lite.h"
#include "Logging.h"

#include <thread>
#include <chrono>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <atomic>

using namespace std::chrono_literals;

void wssautobahntest() {

    auto lastheard = std::chrono::high_resolution_clock::now();

    auto listener = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
        .CreateListener(SL::WS_LITE::PortNumber(3000))
        .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "listener::onConnection");
    }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL::WS_LITE::WSMessage msg;
        msg.Buffer = std::shared_ptr<unsigned char>(new unsigned char[message.len], [](unsigned char* p) { delete[] p; });
        msg.len = message.len;
        msg.code = message.code;
        msg.data = msg.Buffer.get();
        memcpy(msg.data, message.data, message.len);
        socket->send(msg, false);
    }).listen();


    listener.set_ReadTimeout(std::chrono::seconds(100));
    listener.set_WriteTimeout(std::chrono::seconds(100));

    auto tlslistener = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
        .CreateTLSListener(SL::WS_LITE::PortNumber(3001), TEST_CERTIFICATE_PRIVATE_PASSWORD, TEST_CERTIFICATE_PRIVATE_PATH, TEST_CERTIFICATE_PUBLIC_PATH, TEST_DH_PATH)
        .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "tlslistener::onConnection");
    }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL::WS_LITE::WSMessage msg;
        msg.Buffer = std::shared_ptr<unsigned char>(new unsigned char[message.len], [](unsigned char* p) { delete[] p; });
        msg.len = message.len;
        msg.code = message.code;
        msg.data = msg.Buffer.get();
        memcpy(msg.data, message.data, message.len);
        socket->send(msg, false);
    }).listen();

    tlslistener.set_ReadTimeout(std::chrono::seconds(100));
    tlslistener.set_WriteTimeout(std::chrono::seconds(100));

    std::string cmd = "wstest -m fuzzingclient -s ";
    cmd += TEST_FUZZING_PATH;
    system(cmd.c_str());
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastheard).count() < 2000) {
        std::this_thread::sleep_for(200ms);
    }
    std::cout << "Exiting autobahn test..." << std::endl;
}
void generaltest() {
    std::cout << "Starting General test..." << std::endl;
    auto lastheard = std::chrono::high_resolution_clock::now();

    SL::WS_LITE::PortNumber port(3002);
    auto listenerct = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
        .CreateListener(port)
        .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "listener::onConnection");

    }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL::WS_LITE::WSMessage msg;
        msg.Buffer = std::shared_ptr<unsigned char>(new unsigned char[message.len], [](unsigned char* p) { delete[] p; });
        msg.len = message.len;
        msg.code = message.code;
        msg.data = msg.Buffer.get();
        memcpy(msg.data, message.data, message.len);
        socket->send( msg, false);
    }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "listener::onDisconnection");
    }).listen();

    auto clientctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
        .CreateClient()
        .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "Client::onConnection");
    }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "client::onDisconnection");
    }).connect("localhost", port);

    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastheard).count() < 2000) {
        std::this_thread::sleep_for(200ms);
    }
}
void generalTLStest() {
    std::cout << "Starting General TLS test..." << std::endl;
    auto lastheard = std::chrono::high_resolution_clock::now();
    SL::WS_LITE::PortNumber port(3005);
    auto listenerctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
        .CreateTLSListener(port, TEST_CERTIFICATE_PRIVATE_PASSWORD, TEST_CERTIFICATE_PRIVATE_PATH, TEST_CERTIFICATE_PUBLIC_PATH, TEST_DH_PATH)
        .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "listener::onConnection");

    }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL::WS_LITE::WSMessage msg;
        msg.Buffer = std::shared_ptr<unsigned char>(new unsigned char[message.len], [](unsigned char* p) { delete[] p; });
        msg.len = message.len;
        msg.code = message.code;
        msg.data = msg.Buffer.get();
        memcpy(msg.data, message.data, message.len);
        socket->send( msg, false);
    }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "listener::onDisconnection");
    }).listen();

    auto clientctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
        .CreateTLSClient(TEST_CERTIFICATE_PUBLIC_PATH)
        .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "Client::onConnection");
    }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL_WS_LITE_LOG(SL::WS_LITE::Logging_Levels::INFO_log_level, "client::onDisconnection");
    }).connect("localhost", port);

    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastheard).count() < 2000) {
        std::this_thread::sleep_for(200ms);
    }
}

void multithreadtest() {
    std::cout << "Starting Multi threaded test..." << std::endl;
    auto lastheard = std::chrono::high_resolution_clock::now();

    SL::WS_LITE::PortNumber port(3003);
    auto listenerctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(2))
        .CreateListener(port)
        .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        lastheard = std::chrono::high_resolution_clock::now();
    }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
        lastheard = std::chrono::high_resolution_clock::now();
    }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
        lastheard = std::chrono::high_resolution_clock::now();
        SL::WS_LITE::WSMessage msg;
        msg.Buffer = std::shared_ptr<unsigned char>(new unsigned char[message.len], [](unsigned char* p) { delete[] p; });
        msg.len = message.len;
        msg.code = message.code;
        msg.data = msg.Buffer.get();
        memcpy(msg.data, message.data, message.len);
        socket->send(msg, false);
    }).listen();

    std::vector<SL::WS_LITE::WSClient> clients;
    auto clientctx(SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(2)));
    clients.reserve(50);
    for (auto i = 0; i < 50; i++) {
        auto c = clientctx.CreateClient()
            .onConnection([&clients, &lastheard, i](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
            lastheard = std::chrono::high_resolution_clock::now();
            SL::WS_LITE::WSMessage msg;
            std::string txtmsg = "testing msg";
            txtmsg += std::to_string(i);
            msg.Buffer = std::shared_ptr<unsigned char>(new unsigned char[txtmsg.size()], [](unsigned char* p) { delete[] p; });
            msg.len = txtmsg.size();
            msg.code = SL::WS_LITE::OpCode::TEXT;
            msg.data = msg.Buffer.get();
            memcpy(msg.data, txtmsg.data(), txtmsg.size());
            socket->send(msg, false);
        }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
            lastheard = std::chrono::high_resolution_clock::now();
        }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
            lastheard = std::chrono::high_resolution_clock::now();
        }).connect("localhost", port);
        clients.push_back(c);
    }

    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastheard).count() < 2000) {
        std::this_thread::sleep_for(200ms);
    }
}
void multithreadthroughputtest() {
    std::cout << "Starting Multi threaded throughput test" << std::endl;
    std::vector<SL::WS_LITE::WSClient> clients;
    clients.reserve(50);//this should use about 1 GB of memory between sending and receiving
    auto recvtimer = std::chrono::high_resolution_clock::now();
    auto lastheard = std::chrono::high_resolution_clock::now();
    std::atomic<unsigned long long> mbsreceived;
    mbsreceived = 0;
    const auto bufferesize = 1024 * 1024 * 10;

    SL::WS_LITE::PortNumber port(3004);
    auto listenerctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(2))
        .CreateListener(port)
        .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        lastheard = std::chrono::high_resolution_clock::now();
    }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
        lastheard = std::chrono::high_resolution_clock::now();
    }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
        lastheard = std::chrono::high_resolution_clock::now();
        mbsreceived += message.len;
        if (mbsreceived == bufferesize* clients.capacity()) {
            std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - recvtimer).count() << "ms to receive " << bufferesize * clients.capacity() << " bytes" << std::endl;
        }
    }).listen();

    std::atomic<unsigned long long> mbssent;
    mbssent = 0;

    auto  clientctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(2));
    auto sendtimer = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < clients.capacity(); i++) {
        auto c = clientctx.CreateClient()
            .onConnection([&clients, &lastheard, i, &mbssent, &sendtimer, bufferesize](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
            lastheard = std::chrono::high_resolution_clock::now();
            SL::WS_LITE::WSMessage msg;
            msg.Buffer = std::shared_ptr<unsigned char>(new unsigned char[bufferesize], [&](unsigned char* p) {
                mbssent += bufferesize;
                if (mbssent == bufferesize * clients.capacity()) {
                    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - sendtimer).count() << "ms to send " << bufferesize * clients.capacity() << " bytes" << std::endl;
                }
                delete[] p;
            });
            msg.len = bufferesize;//10MB
            msg.code = SL::WS_LITE::OpCode::BINARY;
            msg.data = msg.Buffer.get();
            socket->send(msg, false);
        }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
            lastheard = std::chrono::high_resolution_clock::now();
        }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
            lastheard = std::chrono::high_resolution_clock::now();
        }).connect("localhost", port);
        clients.push_back(c);
    }

    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastheard).count() < 5000) {
        std::this_thread::sleep_for(200ms);
    }
    std::cout << "Received " << mbsreceived << "  bytes" << std::endl;
}
int main(int argc, char* argv[]) {
    wssautobahntest();
    std::this_thread::sleep_for(1s);
    generaltest();
    std::this_thread::sleep_for(1s);
    generalTLStest();
    std::this_thread::sleep_for(1s);
    multithreadtest();
    std::this_thread::sleep_for(1s);
    multithreadthroughputtest();
    return 0;
}
