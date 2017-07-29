
#import <Foundation/Foundation.h>
#import <appkit/appkit.h>

#include "Clipboard.h"
#include "Clipboard_Lite.h"
#include <string>
#include <functional>
#include <memory>
#include <iostream>
#include <assert.h>
#include <chrono>

namespace SL {
    namespace Clipboard_Lite {
        Clipboard_ManagerImpl::Clipboard_ManagerImpl() {
            Copying = false;
        }
        Clipboard_ManagerImpl::~Clipboard_ManagerImpl() {
            KeepRunning = false;
            if (BackGroundWorker.joinable()) {
                BackGroundWorker.join();
            }
        }
        std::string	convertNsString( NSString *str )
        {
            return std::string( [str UTF8String] );
        }
        void Clipboard_ManagerImpl::run() {
            KeepRunning = true;
            BackGroundWorker = std::thread([&] {
                //mac platform we must poll to retrive clipboard events
                while(KeepRunning){
                    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
                    if(pasteboard.changeCount != ChangeCount){
                        ChangeCount = pasteboard.changeCount;
                      
                        if(onImage){
                            NSBitmapImageRep *rep = (NSBitmapImageRep*)[NSBitmapImageRep imageRepWithPasteboard:pasteboard];
                            if( rep && rep.CGImage){
                                Clipboard_Lite::Image img;
                                img.Width =CGImageGetWidth(rep.CGImage);
                                img.Height =CGImageGetHeight(rep.CGImage);
                                
                                auto prov = CGImageGetDataProvider(rep.CGImage);
                                if(!prov){
                                    continue;
                                }
                                auto bytesperrow = CGImageGetBytesPerRow(rep.CGImage);
                                auto rawdatas= CGDataProviderCopyData(prov);
                                auto buf = CFDataGetBytePtr(rawdatas);
                                img.PixelStride =4;
                                auto datalen = img.Width*img.Height *img.PixelStride ;
                                img.Data = std::shared_ptr<unsigned char>(new unsigned char[datalen], [](unsigned char* p){ delete [] p;});
                                if(bytesperrow == img.PixelStride *img.Width){
                                    //most efficent, can be done in a single memcpy
                                    memcpy(img.Data.get(),buf, datalen);
                                } else {
                                    //for loop needed to copy each row
                                    auto dst =img.Data.get();
                                    auto src =buf;
                                    for (auto h =0; h<img.Height;h++) {
                                        memcpy(dst,src, img.PixelStride*img.Width);
                                        dst +=img.PixelStride*img.Width;
                                        src +=bytesperrow;
                                    }
                                }
                                onImage(img);
                                CFRelease(rawdatas);
                                continue;
                            }
                        }
                        if(onText){
                            NSArray *classArray = [NSArray arrayWithObject:[NSString class]];
                            NSDictionary *options = [NSDictionary dictionary];
                            
                            if( [pasteboard canReadObjectForClasses:classArray options:options] ) {
                                NSArray *objectsToPaste = [pasteboard readObjectsForClasses:classArray options:options];
                                NSString *text = [objectsToPaste firstObject];
                                if(text){
                                    onText(convertNsString(text));
                                }
                            }
                        }
                    }
                   
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
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
