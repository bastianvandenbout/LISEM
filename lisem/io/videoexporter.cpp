#include "videoexporter.h"
#include "iostream"
#include "QApplication"
#include "site.h"

FuncPtr ExtDebug;
char errbuf[32];

using namespace std;

void VideoCapture::Init(int width, int height, int fpsrate, int bitrate, QString infilename) {

    std::cout << "opening video " << width << "  " << height << " " << fpsrate << " " << bitrate << std::endl;
    filename = infilename;
    fps = fpsrate;

    bool uhd = (width > 1920 || height > 1080)? true:false;

    filename_temp = uhd? QString(GetSite() + "/tmp.h265") : QString(GetSite() + "/tmp.h264");

    int err;

    if (!(oformat = av_guess_format(NULL, filename_temp.toStdString().c_str(), NULL))) {
        VideoExportDebug("Failed to define output format", 0);
        return;
    }

    if ((err = avformat_alloc_output_context2(&ofctx, oformat, NULL, filename_temp.toStdString().c_str()) < 0)) {
        VideoExportDebug("Failed to allocate output context", err);
        Free();
        return;
    }

    if (!(codec = avcodec_find_encoder(oformat->video_codec))) {
        VideoExportDebug("Failed to find encoder", 0);
        Free();
        return;
    }

    if (!(videoStream = avformat_new_stream(ofctx, codec))) {
        VideoExportDebug("Failed to create new stream", 0);
        Free();
        return;
    }

    if (!(cctx = avcodec_alloc_context3(codec))) {
        VideoExportDebug("Failed to allocate codec context", 0);
        Free();
        return;
    }

    videoStream->codecpar->codec_id = oformat->video_codec;
    videoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    videoStream->codecpar->width = width;
    videoStream->codecpar->height = height;
    videoStream->codecpar->format = AV_PIX_FMT_YUV420P;
    videoStream->codecpar->bit_rate = bitrate * 1000;
    videoStream->time_base = { 1, fps };

    avcodec_parameters_to_context(cctx, videoStream->codecpar);
    cctx->time_base = { 1, fps };
    cctx->max_b_frames = 2;
    cctx->gop_size = 12;
    if (videoStream->codecpar->codec_id == uhd?AV_CODEC_ID_H265 : AV_CODEC_ID_H264) {
        av_opt_set(cctx, "preset", "ultrafast", 0);
    }
    if (ofctx->oformat->flags & AVFMT_GLOBALHEADER) {
        cctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    avcodec_parameters_from_context(videoStream->codecpar, cctx);

    if ((err = avcodec_open2(cctx, codec, NULL)) < 0) {
        VideoExportDebug("Failed to open codec ", err);
        Free();
        return;
    }

    if (!(oformat->flags & AVFMT_NOFILE)) {
        if ((err = avio_open(&ofctx->pb, filename_temp.toStdString().c_str(), AVIO_FLAG_WRITE)) < 0) {
            VideoExportDebug("Failed to open file", err);
            Free();
            return;
        }
    }

    if ((err = avformat_write_header(ofctx, NULL)) < 0) {
        VideoExportDebug("Failed to write header", err);
        Free();
        return;
    }

    av_dump_format(ofctx, 0, filename_temp.toStdString().c_str(), 1);
}

void VideoCapture::AddFrame(uint8_t *data) {
    int err;
    if (!videoFrame) {

        videoFrame = av_frame_alloc();
        videoFrame->format = AV_PIX_FMT_YUV420P;
        videoFrame->width = cctx->width;
        videoFrame->height = cctx->height;

        if ((err = av_frame_get_buffer(videoFrame, 32)) < 0) {
            VideoExportDebug("Failed to allocate picture", err);
            return;
        }
    }

    if (!swsCtx) {
        swsCtx = sws_getContext(cctx->width, cctx->height, AV_PIX_FMT_RGB24, cctx->width, cctx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
    }

    int inLinesize[1] = { 3 * cctx->width };

    // From RGB to YUV
    sws_scale(swsCtx, (const uint8_t * const *)&data, inLinesize, 0, cctx->height, videoFrame->data, videoFrame->linesize);

    videoFrame->pts = frameCounter++;

    if ((err = avcodec_send_frame(cctx, videoFrame)) < 0) {
        VideoExportDebug("Failed to send frame", err);
        return;
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    if (avcodec_receive_packet(cctx, &pkt) == 0) {
        pkt.flags |= AV_PKT_FLAG_KEY;
        av_interleaved_write_frame(ofctx, &pkt);
        av_packet_unref(&pkt);
    }
}

void VideoCapture::Finish(bool save = true) {
    //DELAYED FRAMES
    //if(save)
    {
        std::cout << "save video file to " << filename.toStdString() << std::endl;
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;

        std::cout << 1 << std::endl;
        for (;;) {
            avcodec_send_frame(cctx, NULL);
            if (avcodec_receive_packet(cctx, &pkt) == 0) {
                av_interleaved_write_frame(ofctx, &pkt);
                av_packet_unref(&pkt);
            }
            else {
                break;
            }
        }

        std::cout << 2 << std::endl;

        av_write_trailer(ofctx);
        if (!(oformat->flags & AVFMT_NOFILE)) {
            int err = avio_close(ofctx->pb);
            if (err < 0) {
                VideoExportDebug("Failed to close file", err);
            }
        }
    }

    std::cout << 3 << std::endl;
    Free();

    Remux();
}

void VideoCapture::Free() {
    if (videoFrame) {
        av_frame_free(&videoFrame);
    }
    if (cctx) {
        avcodec_free_context(&cctx);
    }
    if (ofctx) {
        avformat_free_context(ofctx);
    }
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
}

void VideoCapture::Remux() {
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    int err;

    if ((err = avformat_open_input(&ifmt_ctx, filename_temp.toStdString().c_str(), 0, 0)) < 0) {
        VideoExportDebug("Failed to open input file for remuxing", err);
        goto end;
    }
    if ((err = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        VideoExportDebug("Failed to retrieve input stream information", err);
        goto end;
    }
    if ((err = avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL,filename.toStdString().c_str()))) {
        VideoExportDebug("Failed to allocate output context", err);
        goto end;
    }

    {
        AVStream *inVideoStream = ifmt_ctx->streams[0];
        AVStream *outVideoStream = avformat_new_stream(ofmt_ctx, NULL);
        if (!outVideoStream) {
            VideoExportDebug("Failed to allocate output video stream", 0);
            goto end;
        }
        outVideoStream->time_base = { 1, fps };
        avcodec_parameters_copy(outVideoStream->codecpar, inVideoStream->codecpar);
        outVideoStream->codecpar->codec_tag = 0;

        if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            if ((err = avio_open(&ofmt_ctx->pb, filename.toStdString().c_str(), AVIO_FLAG_WRITE)) < 0) {
                VideoExportDebug("Failed to open output file", err);
                goto end;
            }
        }

        if ((err = avformat_write_header(ofmt_ctx, 0)) < 0) {
            VideoExportDebug("Failed to write header to output file", err);
            goto end;
        }

        {
            AVPacket videoPkt;
            int ts = 0;
            while (true) {
                if ((err = av_read_frame(ifmt_ctx, &videoPkt)) < 0) {
                    break;
                }
                videoPkt.stream_index = outVideoStream->index;
                videoPkt.pts = ts;
                videoPkt.dts = ts;
                videoPkt.duration = av_rescale_q(videoPkt.duration, inVideoStream->time_base, outVideoStream->time_base);
                ts += videoPkt.duration;
                videoPkt.pos = -1;

                if ((err = av_interleaved_write_frame(ofmt_ctx, &videoPkt)) < 0) {
                    VideoExportDebug("Failed to mux packet", err);
                    av_packet_unref(&videoPkt);
                    break;
                }
                av_packet_unref(&videoPkt);
            }

            av_write_trailer(ofmt_ctx);
        }
    }



end:
    if (ifmt_ctx) {
        avformat_close_input(&ifmt_ctx);
    }
    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&ofmt_ctx->pb);
    }
    if (ofmt_ctx) {
        avformat_free_context(ofmt_ctx);
    }
}
