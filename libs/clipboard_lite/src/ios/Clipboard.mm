
#import <Foundation/Foundation.h>
#import <appkit/appkit.h>

#include "Clipboard.h"
#include "Clipboard_Lite.h"
#include <string>
#include <functional>
#include <memory>
#include <iostream>
#include <assert.h>

namespace SL {
    namespace Clipboard_Lite {
        Clipboard_ManagerImpl::Clipboard_ManagerImpl() {
            Copying = false;
        }
        Clipboard_ManagerImpl::~Clipboard_ManagerImpl() {
            
            if (BackGroundWorker.joinable()) {
                BackGroundWorker.join();
            }
        }
        void Clipboard_ManagerImpl::run() {
            BackGroundWorker = std::thread([&] {
                
            });
        }
        void Clipboard_ManagerImpl::copy(const std::string& text) {
            [[NSPasteboard generalPasteboard] declareTypes: [NSArray arrayWithObject: NSStringPboardType] owner:nil];
            [[NSPasteboard generalPasteboard] setString:[NSString stringWithUTF8String:text.c_str()] forType: NSStringPboardType];
            
        }
        void Clipboard_ManagerImpl::copy(const Image& img){
            
        }
    };
}
