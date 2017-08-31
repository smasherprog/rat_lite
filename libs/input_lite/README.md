# Input_lite
<p>Linux/Mac <img src="https://travis-ci.org/smasherprog/input_lite.svg?branch=master" /></p>
<p>Windows <img src="https://ci.appveyor.com/api/projects/status/j80n5swr918y8tts"/><p>
<p>cross platform input library</p>

<h2>USAGE</h2>
https://github.com/smasherprog/input_lite/blob/master/Test/main.cpp

```c++

    std::cout << "Simulating the A key on keyboard is being pressed " << std::endl;
    SL::Input_Lite::SendInput(SL::Input_Lite::KeyEvent{true, SL::Input_Lite::KeyCodes::KEY_A});
    SL::Input_Lite::SendInput(SL::Input_Lite::KeyEvent{false, SL::Input_Lite::KeyCodes::KEY_A});

	std::cout << "Starting Mouse move tests by Offset" << std::endl;
    SL::Input_Lite::SendInput(SL::Input_Lite::MousePositionAbsoluteEvent{100, 100});
    for (auto x = 0; x < 500; x++) {
        SL::Input_Lite::SendInput(SL::Input_Lite::MousePositionOffsetEvent{1, 0});
        std::this_thread::sleep_for(10ms);
    }
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendInput(SL::Input_Lite::MousePositionOffsetEvent{0, 1});
        std::this_thread::sleep_for(10ms);
    }
    std::cout << "Starting Mouse move tests by Absolute" << std::endl;
    for (auto x = 0; x < 500; x++) {
        SL::Input_Lite::SendInput(SL::Input_Lite::MousePositionAbsoluteEvent{x, 300});
        std::this_thread::sleep_for(10ms);
    }
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendInput(SL::Input_Lite::MousePositionAbsoluteEvent{500, y});
        std::this_thread::sleep_for(10ms);
    }
    std::cout << "Starting Mouse Click tests right down up" << std::endl;
    SL::Input_Lite::SendInput(SL::Input_Lite::MouseButtonEvent{true, SL::Input_Lite::MouseButtons::RIGHT});
    SL::Input_Lite::SendInput(SL::Input_Lite::MouseButtonEvent{false, SL::Input_Lite::MouseButtons::RIGHT});

    std::cout << "Starting Mouse Click tests left down up" << std::endl;
    SL::Input_Lite::SendInput(SL::Input_Lite::MouseButtonEvent{true, SL::Input_Lite::MouseButtons::LEFT});
    SL::Input_Lite::SendInput(SL::Input_Lite::MouseButtonEvent{false, SL::Input_Lite::MouseButtons::LEFT});

    std::cout << "Starting Mouse wheel tests " << std::endl;
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendInput(SL::Input_Lite::MouseScrollEvent{1});
        std::this_thread::sleep_for(10ms);
    }
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendInput(SL::Input_Lite::MouseScrollEvent{-1});
        std::this_thread::sleep_for(10ms);
    }

```
