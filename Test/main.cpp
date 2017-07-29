#include "Input_Lite.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    std::this_thread::sleep_for(4s);

    for (auto c = ' '; c < '~'; c++) {
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
    SL::Input_Lite::SendMousePosition(SL::Input_Lite::AbsolutePos{ 100, 100 });
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
        SL::Input_Lite::SendMousePosition(SL::Input_Lite::AbsolutePos{ x, 300 });
        std::this_thread::sleep_for(10ms);
    }
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendMousePosition(SL::Input_Lite::AbsolutePos{ 500, y });
        std::this_thread::sleep_for(10ms);
    }
    std::cout << "Starting Mouse Click tests " << std::endl;
    SL::Input_Lite::SendMouseClick(SL::Input_Lite::MouseButtons::RIGHT);
    SL::Input_Lite::SendMouseClick(SL::Input_Lite::MouseButtons::LEFT);
    SL::Input_Lite::SendMouseDoubleClick(SL::Input_Lite::MouseButtons::LEFT);
    std::cout << "Starting Mouse wheel tests " << std::endl;
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendMouseScroll(1);
        std::this_thread::sleep_for(10ms);
    }
    for (auto y = 0; y < 500; y++) {
        SL::Input_Lite::SendMouseScroll(-1);
        std::this_thread::sleep_for(10ms);
    }
    return 0;
}
/*
Display* dpy;
dpy = XOpenDisplay(NULL);

KeySym* keysyms = NULL;
int keysyms_per_keycode = 0;
int scratch_keycode = 0;
int keycode_low, keycode_high;
XDisplayKeycodes(dpy, &keycode_low, &keycode_high);
keysyms = XGetKeyboardMapping(dpy, keycode_low, keycode_high - keycode_low, &keysyms_per_keycode);

int i;
for(i = keycode_low; i <= keycode_high; i++) {
    int j = 0;
    int key_is_empty = 1;
    for(j = 0; j < keysyms_per_keycode; j++) {
        int symindex = (i - keycode_low) * keysyms_per_keycode + j;
        if(keysyms[symindex] != 0) {
            key_is_empty = 0;
        } else {
            break;
        }
    }
    if(key_is_empty) {
        scratch_keycode = i;
        break;
    }
}
XFree(keysyms);
XFlush(dpy);
KeySym sym = XStringToKeysym("U00c4"); // uppercase Z
KeySym keysym_list[] = { sym, sym };
XChangeKeyboardMapping(dpy, scratch_keycode, 2, keysym_list, 2);
KeyCode code = scratch_keycode;

XTestFakeKeyEvent(dpy, code, True, 0);
XTestFakeKeyEvent(dpy, code, False, 0);
keysym_list[] = { 0 };
XChangeKeyboardMapping(dpy, scratch_keycode, 1, keysym_list, 1);*/
