# Input_lite
<p>Linux/Mac <img src="https://travis-ci.org/smasherprog/input_lite.svg?branch=master" /></p>
<p>Windows <img src="https://ci.appveyor.com/api/projects/status/j80n5swr918y8tts"/><p>
<p>cross platform input library</p>

<h2>USAGE</h2>
https://github.com/smasherprog/input_lite/blob/master/Test/main.cpp

```c++
    for (auto c = ' '; c < 127; c++) {
        SL::Input_Lite::SendKey(c);
    }

    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ENTER);
    // UNICODE SUPPORT!
    SL::Input_Lite::SendKeyDown((wchar_t)294); // the letter Ħ
    SL::Input_Lite::SendKeyDown((wchar_t)274); // the letter Ē
    SL::Input_Lite::SendKeyDown((wchar_t)315); // the letter Ļ
    SL::Input_Lite::SendKeyDown((wchar_t)315); // the letter Ļ
    SL::Input_Lite::SendKeyDown((wchar_t)526); // the letter Ȏ
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ENTER);
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::TAB);
    SL::Input_Lite::SendKeys(std::string("this is a std::string"));
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ENTER);
    // unicode
    SL::Input_Lite::SendKeys(L"ĦĒĻĻȎ");
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ENTER);
    SL::Input_Lite::SendKeys(std::wstring(L"ĦĒĻĻȎ"));
    // SPECIAL KEYS
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ENTER);
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::NUMPAD0);
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::CAPSLOCK);
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::TAB);

    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ARROWLEFT);
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ARROWLEFT);
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ARROWLEFT);
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ARROWLEFT);
    SL::Input_Lite::SendKey(SL::Input_Lite::SpecialKeyCodes::ARROWLEFT);


    std::cout << "Starting Mouse move tests by Offset" << std::endl;
    SL::Input_Lite::SendMousePosition(SL::Input_Lite::Absolute{ 100, 100 });
    for (auto x = 0; x < 500; x++) {
        SL::Input_Lite::SendMousePosition(SL::Input_Lite::Offset{ 1, 0 });
        std::this_thread::sleep_for(10ms);
    }
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendMousePosition(SL::Input_Lite::Offset{ 0, 1 });
        std::this_thread::sleep_for(10ms);
    }
    std::cout << "Starting Mouse move tests by Absolute" << std::endl;
    for (auto x = 0; x < 500; x++) {
        SL::Input_Lite::SendMousePosition(SL::Input_Lite::Absolute{ x, 300 });
        std::this_thread::sleep_for(10ms);
    }
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendMousePosition(SL::Input_Lite::Absolute{ 500, y });
        std::this_thread::sleep_for(10ms);
    }
    std::cout << "Starting Mouse Click tests " << std::endl;
    SL::Input_Lite::SendMouseClick(SL::Input_Lite::MouseButtons::RIGHT);
    SL::Input_Lite::SendMouseClick(SL::Input_Lite::MouseButtons::LEFT);
    std::cout << "Starting Mouse wheel tests " << std::endl;
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendMouseScroll(1);
        std::this_thread::sleep_for(10ms);
    }
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendMouseScroll(-1);
        std::this_thread::sleep_for(10ms);
    }

```
