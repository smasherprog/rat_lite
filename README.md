# websocket_lite

<h3>Websocket_Lite</h3>
<p>Linux/Mac <img src="https://travis-ci.org/smasherprog/websocket_lite.svg?branch=master"/><p>
<p>Windows <img src="https://ci.appveyor.com/api/projects/status/kqa94n7p8se05vi9/branch/master?svg=true"/><p>
<p>Websocket <a href="https://tools.ietf.org/html/rfc6455">rfc6455</a> Compliant. Working on rfc 7692 -- which supports compression<p>
<p>This library is intended to be a fully compliant websocket implementation <a href="http://htmlpreview.github.io/?https://github.com/smasherprog/websocket_lite/blob/master/Test/autobahn/index.html">Autobahn complete </a> (minus rfc 7692--in progress), but with a few design goals:
<h3>To get started, all you need is <a href="https://cmake.org/download/">cmake</a></h3>
<ul>
<li>
Cross-platform: Asio https://github.com/chriskohlhoff/asio/
</li>
<li>
Performance 
</li>
<li>
Simple and easy to use
</li>
<li>
Latest standards: c++ 17 
</li>
<li>
All functions are thread-safe and can be called from any thread at any time
</li>
</ul>
<h2>USAGE</h2>
<p>To get started check out the example here<p>
https://github.com/smasherprog/websocket_lite/blob/master/Test/main.cpp

```c++
auto listener = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
  .CreateListener(SL::WS_LITE::PortNumber(3000))
  .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        
  }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {

  }).listen();
    
auto clientctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
  .CreateClient()
  .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
        
  }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {

  }).connect("localhost", SL::WS_LITE::PortNumber(3000));

```
