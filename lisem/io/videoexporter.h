#ifndef VIDEOEXPORTER_H
#define VIDEOEXPORTER_H

#include "defines.h"
#include "QObject"
#include "QString"
#include "QImage"
#include "QMediaRecorder"
#include "iostream"
#include "error.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/avfft.h>

#include <libavdevice/avdevice.h>

#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include <libavformat/avformat.h>
#include <libavformat/avio.h>

    // libav resample

#include <libavutil/opt.h>
#include <libavutil/common.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/file.h>


    // hwaccel
#include "libavcodec/vdpau.h"
#include "libavutil/hwcontext.h"
#include "libavutil/hwcontext_vdpau.h"


}

extern "C" {
    // lib swresample
#include "libswscale/swscale.h"
}


inline void Log(std::string str) {


    std::cout << str << std::endl;

    LISEMS_ERROR(QString(str.c_str()));

}

typedef void(*FuncPtr)(const char *);
extern FuncPtr ExtDebug;
extern char errbuf[32];

inline void VideoExportDebug(std::string str, int err) {
   Log(str + " " + std::to_string(err));
   if (err < 0) {
       av_strerror(err, errbuf, sizeof(errbuf));
       str +=  errbuf;
   }
   Log(str);
   ExtDebug(str.c_str());
}

inline void avlog_cb(void *, int level, const char * fmt, va_list vargs) {

}


class LISEM_API VideoCapture : public QObject
{

    Q_OBJECT


public:

        VideoCapture() {
            oformat = NULL;
            ofctx = NULL;
            videoStream = NULL;
            videoFrame = NULL;
            swsCtx = NULL;
            frameCounter = 0;

            // Initialize libavcodec
            av_register_all();
            av_log_set_callback(avlog_cb);
        }

        ~VideoCapture() {
            Free();
        }

        void Init(int width, int height, int fpsrate, int bitrate, QString infilename);

        void AddFrame(uint8_t *data);

        void Finish(bool save);

    private:

        AVOutputFormat *oformat;
        AVFormatContext *ofctx;

        AVStream *videoStream;
        AVFrame *videoFrame;

        AVCodec *codec;
        AVCodecContext *cctx;

        SwsContext *swsCtx;

        QString filename;
        QString filename_temp;

        int frameCounter;

        int fps;

        void Free();

        void Remux();

};

namespace VideoExport
{
inline VideoCapture* Init(int width, int height, int fps, int bitrate, QString filename) {
    std::cout << "init video capture"<< std::endl;
    VideoCapture *vc = new VideoCapture();
    vc->Init(width, height, fps, bitrate, filename);
    return vc;
};

inline void AddFrame(uint8_t *data, VideoCapture *vc) {
    std::cout << "add frame"<< std::endl;

    vc->AddFrame(data);
}

inline void Finish(VideoCapture *vc, bool save) {
    std::cout << "finish"<< std::endl;

    vc->Finish(save);
}

inline void SetDebug(FuncPtr fp) {
    ExtDebug = fp;
};

}




#endif // VIDEOEXPORTER_H
