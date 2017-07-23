#include "Clipboard_Lite.h"
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

// THESE LIBRARIES ARE HERE FOR CONVINIENCE!! They are SLOW and ONLY USED FOR
// HOW THE LIBRARY WORKS!
#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"
#define LODEPNG_COMPILE_PNG
#define LODEPNG_COMPILE_DISK
#include "lodepng.h"
/////////////////////////////////////////////////////////////////////////

using namespace std::chrono_literals;

int main(int argc, char* argv[]) {

    auto counter = 0;
    std::cout << "Clipboard Example running" << std::endl;
    auto clipboard = SL::Clipboard_Lite::CreateClipboard()
        .onText([](const std::string& text) {
        std::cout << "onText" << std::endl;
        std::cout << text << std::endl;
    }).onImage([&](const SL::Clipboard_Lite::Image& image) {
        counter++;
        auto s = std::to_string(counter) + std::string(" M");

        if (image.PixelStride == 4) {
            s+= std::string(".png");
            lodepng::encode(s.c_str(), image.Data.get(), image.Width, image.Height);
        }
        else if (image.PixelStride == 3) {
            s+= std::string(".jpg");
            tje_encode_to_file(s.c_str(), image.Width, image.Height, 3, image.Data.get());
        }
        std::cout << "onImage Height=" << image.Height << " Width=" << image.Width << std::endl;
    })
        .run();

    std::this_thread::sleep_for(1s);
    std::cout << "Copying Text to clipboard" << std::endl;
    std::string txt = "pasted text";
    clipboard.copy(txt);


    std::this_thread::sleep_for(2s);
    auto i = 0;
    while (i++ < 10) {

        std::cout << "Copying Text to clipboard a " << i++ << " time" << std::endl;
        txt = "pasted text a time ";
        txt += std::to_string(i);
        clipboard.copy(txt);
        std::this_thread::sleep_for(10s);
    }


    return 0;
}
