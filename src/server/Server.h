#pragma once
#include <string>

namespace SL {
namespace RAT_Server {

    enum Server_Status { SERVER_RUNNING, SERVER_STOPPING, SERVER_STOPPED };

    class ServerImpl;
    class Server {
        ServerImpl *ServerImpl_;

      public:
        Server();
        ~Server();
        void ShareClipboard(bool share);
        bool ShareClipboard() const;
        void MaxConnections(int maxconnections);
        int MaxConnections() const;
        void FrameChangeInterval(int delay_in_ms);
        int FrameChangeInterval() const;
        void MouseChangeInterval(int delay_in_ms);
        int MouseChangeInterval() const;
        // imagecompression is [0, 100]    = [WORST, BEST]
        void ImageCompressionSetting(int compression);
        int ImageCompressionSetting() const;
        void EncodeImagesAsGrayScale(bool usegrayscale);
        bool EncodeImagesAsGrayScale() const;

        void Run(unsigned short port, std::string PasswordToPrivateKey, std::string PathTo_Private_Key, std::string PathTo_Public_Certficate);

#if __ANDROID__
        void OnImage(char *buf, int width, int height);
#endif
    };
} // namespace RAT_Server
} // namespace SL
