#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavcodec/packet.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#define YUV_PATH "big_buck_bunny_360x240_i420.yuv"
#define OUTPUT_PATH "big_buck_bunny_360x240_h264enc.h264";
#define VIDEO_IN_BUFF_SIZE 20480
#define VIDEO_REFILL_THRESH 4096

static char s_errbuf[246] = {0};
static char *av_get_err(int err)
{
    av_strerror(err, s_errbuf, sizeof(s_errbuf));
    return s_errbuf;
}

int64_t get_time()
{
    return av_gettime_relative() / 1000;  // 换算成毫秒
}

static void print_video_format(const AVFrame *frame)
{
    printf("print_video_format:---\n");
    printf("width:%u\n", frame->width);
    printf("height:%u\n", frame->height);
    printf("format:%u, AV_PIX_FMT_YUV420P=%d\n", frame->format, AV_PIX_FMT_YUV420P);
    printf("video frame data = %f \n", (frame->width) * (frame->height) * 1.5);
    printf("frame->line[0] = %d \n", frame->linesize[0]);
    printf("frame->line[1] = %d \n", frame->linesize[1]);
    printf("frame->line[2] = %d \n", frame->linesize[2]);
}

using Byte = unsigned char;
using Bytes = std::vector<Byte>;
static std::vector<Bytes> binarySplit(const Bytes &bytes,
                                      const Bytes &delimiter)
{
    std::vector<Bytes> result;
    size_t startIndex = 0;
    size_t endIndex = 0;
    for (size_t i = 0; i < bytes.size() - delimiter.size() + 1; ++i)
    {
        if (0 == memcmp(bytes.data() + i, delimiter.data(), delimiter.size()))
        {
            endIndex = i;
            if (startIndex < endIndex)
            {
                result.push_back(Bytes(bytes.data() + startIndex, bytes.data() + endIndex));
            }
            startIndex = endIndex + delimiter.size();
        }
    }
    endIndex = bytes.size();
    if (startIndex < endIndex)
    {
        result.push_back(Bytes(bytes.data() + startIndex, bytes.data() + endIndex));
    }
    return result;
}
static std::vector<Bytes> nalus;
static std::vector<std::vector<Bytes>> samples;

static int encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile)
{
    int ret;

    /* send the frame to the encoder */
    if (frame)
    {
        printf("Send frame %3lld\n", frame->pts);
    }
    /* 通过查阅代码，使用x264进行编码时，具体缓存帧是在x264源码进行， 不会增加avframe对应buffer的reference*/
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0)
    {
        fprintf(stderr, "Error sending a frame for encoding\n");
        return -1;
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return 0;
        }
        else if (ret < 0)
        {
            fprintf(stderr, "Error encoding audio frame\n");
            return -1;
        }

        if (pkt->flags & AV_PKT_FLAG_KEY)
        {
            printf("Write packet flags:%d pts:%3lld dts:%3lld (size:%5d)\n",
                   pkt->flags, pkt->pts, pkt->dts, pkt->size);
        }
        if (!pkt->flags)
        {
            printf("Write packet flags:%d pts:%3lld dts:%3lld (size:%5d)\n",
                   pkt->flags, pkt->pts, pkt->dts, pkt->size);
        }
        static int count = 0;
        count++;
        printf("Nalu header %d:0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
               count, pkt->data[0], pkt->data[1], pkt->data[2], pkt->data[3], pkt->data[4]);
        fwrite(pkt->data, 1, pkt->size, outfile);

        static Bytes shortdelimiter = {0x00, 0x00, 0x01};
        static Bytes longdelimiter = {0x00, 0x00, 0x00, 0x01};
        auto long_split = binarySplit(Bytes((Byte *)pkt->data, (Byte *)pkt->data + pkt->size),
                                      longdelimiter);
        for (auto &&bytes: long_split)
        {
            auto splits = binarySplit(bytes, shortdelimiter);
            nalus.insert(nalus.end(), splits.begin(), splits.end());
        }
        std::vector<Bytes> sample = {};
        for (auto &&bytes: nalus)
        {
            sample.push_back(bytes);
            uint8_t unitType = (uint8_t)bytes[0] & 0x1F;
            if (unitType >= 1 && unitType <= 5 && !sample.empty())
            {
                samples.push_back(sample);
                sample = {};
            }
        }
        nalus = sample;
    }
    return 0;
}

// extract yuv: ffmpeg -i source.mp4 -t 5 -r 25 -pix_fmt yuv420p target.yuv
// play: ffplay target.h264
int main()
{
    fprintf(stderr, "ffmpeg version:%s\n", av_version_info());

    if (0)
    {
        uint8_t bytes[] = {0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x06, 0x07, 0x08, 0x01};
        auto list = binarySplit(Bytes((Byte *)bytes, (Byte *)(bytes + sizeof(bytes))),
                                {0x01, 0x02});
        auto list2 = binarySplit(Bytes((Byte *)bytes, (Byte *)(bytes + sizeof(bytes))),
                                 {0x01});

        return 0;
    }

    AVPacket *pkt = NULL;
    AVFrame *frame = NULL;
    const AVCodec *codec = NULL;
    AVCodecContext *codec_ctx = NULL;
    const char *out_file_name = OUTPUT_PATH;

    int len = 0;
    int ret = 0;
    FILE *in_file = NULL;
    FILE *out_file = NULL;
    uint8_t *data = NULL;
    size_t data_size = 0;
    uint8_t in_buff[VIDEO_IN_BUFF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE] = {0};

    pkt = av_packet_alloc();
    if (!pkt)
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame)
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    int frame_bytes = 0;
    uint8_t *yuv_buf = NULL;

    int64_t begin_time = get_time();
    int64_t end_time = begin_time;
    int64_t all_begin_time = get_time();
    int64_t all_end_time = all_begin_time;
    int64_t pts = 0;
    std::string avCodecName;

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        fprintf(stderr, "Codec not found\n");
        goto failed;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx)
    {
        fprintf(stderr, "Could not allocated audio codec context\n");
        goto failed;
    }
    codec_ctx->width = 360;
    codec_ctx->height = 240;
    /* Set the time base, pay attention to the correct setting of the time base, which will affect the output of the
     * bitrate. That is, the time base of AVFrame's pts needs to be consistent with codec_ctx->time_mase*/
    codec_ctx->time_base = {1, 1000};
    codec_ctx->time_base.den = 1000;
    codec_ctx->framerate = {25, 1};
    codec_ctx->gop_size = 25;
    codec_ctx->max_b_frames = 0;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    avCodecName = codec->name;
    if ("libx264" == avCodecName)
    {
        // 相关的参数可以参考libx264.c的 AVOption options
        // ultrafast all encode time:2270ms
        // medium all encode time:5815ms
        // veryslow all encode time:19836ms
        ret = av_opt_set(codec_ctx->priv_data, "preset", "medium", 0);
        if (ret != 0)
        {
            printf("av_opt_set preset failed\n");
        }
        ret = av_opt_set(codec_ctx->priv_data, "profile", "high", 0); // 默认是high, main
        if (ret != 0)
        {
            printf("av_opt_set profile failed\n");
        }
        // ret = av_opt_set(codec_ctx->priv_data, "tune", "zerolatency", 0); // 直播是才使用该设置
        ret = av_opt_set(codec_ctx->priv_data, "tune", "film", 0); //  画质film
        if (ret != 0)
        {
            printf("av_opt_set tune failed\n");
        }
    }

    /* 设置编码器参数, 设置bitrate */
    codec_ctx->bit_rate = 300000;
//    codec_ctx->rc_max_rate = 3000000;
//    codec_ctx->rc_min_rate = 3000000;
//    codec_ctx->rc_buffer_size = 2000000;
//    codec_ctx->thread_count = 4;  // 开了多线程后也会导致帧输出延迟, 需要缓存thread_count帧后再编程。
//    codec_ctx->thread_type = FF_THREAD_FRAME; // 并 设置为FF_THREAD_FRAME
    /* 对于H264 AV_CODEC_FLAG_GLOBAL_HEADER  设置则只包含I帧，此时sps pps需要从codec_ctx->extradata读取
     *  不设置则每个I帧都带 sps pps sei
     */
//    codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; // 存本地文件时不要去设置

    /* 将codec_ctx和codec进行绑定 */
    ret = avcodec_open2(codec_ctx, codec, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "Could not open codec: %s\n", av_get_err(ret));
        exit(1);
    }
    printf("thread_count: %d, thread_type:%d\n", codec_ctx->thread_count, codec_ctx->thread_type);

    in_file = fopen(YUV_PATH, "rb");
    if (!in_file)
    {
        fprintf(stderr, "Could not open %s\n", YUV_PATH);
        goto failed;
    }

    out_file = fopen(out_file_name, "wb");
    if (!out_file)
    {
        fprintf(stderr, "Could not open %s\n", out_file_name);
        goto failed;
    }

    // 为frame分配buffer
    frame->format = codec_ctx->pix_fmt;
    frame->width = codec_ctx->width;
    frame->height = codec_ctx->height;
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0)
    {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
    // 计算出每一帧的数据 像素格式 * 宽 * 高   1382400
    frame_bytes = av_image_get_buffer_size((AVPixelFormat)frame->format, frame->width, frame->height, 1);
    printf("frame_bytes %d\n", frame_bytes);
    yuv_buf = (uint8_t *)malloc(frame_bytes);
    if (!yuv_buf)
    {
        printf("yuv_buf malloc failed\n");
        return 1;
    }
    printf("start enode\n");

    while (1)
    {
        memset(yuv_buf, 0, frame_bytes);
        size_t read_bytes = fread(yuv_buf, 1, frame_bytes, in_file);
        if (read_bytes <= 0)
        {
            printf("read file finish\n");
            break;
        }
        /* 确保该frame可写, 如果编码器内部保持了内存参考计数，则需要重新拷贝一个备份 目的是新写入的数据和编码器保存的数据不能产生冲突 */
        int frame_is_writable = 1;
        if (av_frame_is_writable(frame) == 0)
        {
            // 这里只是用来测试
            printf("the frame can't write, buf:%p\n", frame->buf[0]);
            if (frame->buf && frame->buf[0])
            {
                // 打印referenc-counted，必须保证传入的是有效指针
                printf("ref_count1(frame) = %d\n", av_buffer_get_ref_count(frame->buf[0]));
            }
            frame_is_writable = 0;
        }
        ret = av_frame_make_writable(frame);
        if (frame_is_writable == 0)
        {
            // 这里只是用来测试
            printf("av_frame_make_writable, buf:%p\n", frame->buf[0]);
            if (frame->buf && frame->buf[0])
            {
                // 打印referenc-counted，必须保证传入的是有效指针
                printf("ref_count2(frame) = %d\n", av_buffer_get_ref_count(frame->buf[0]));
            }
        }
        if (ret != 0)
        {
            printf("av_frame_make_writable failed, ret = %d\n", ret);
            break;
        }
        int need_size = av_image_fill_arrays(frame->data, frame->linesize, yuv_buf,
                                             (AVPixelFormat)frame->format,
                                             frame->width, frame->height, 1);
        if (need_size != frame_bytes)
        {
            printf("av_image_fill_arrays failed, need_size:%d, frame_bytes:%d\n",
                   need_size, frame_bytes);
            break;
        }
        pts += 40;
        // 设置pts
        frame->pts = pts;       // 使用采样率作为pts的单位，具体换算成秒 pts*1/采样率
        begin_time = get_time();
        ret = encode(codec_ctx, frame, pkt, out_file);
        end_time = get_time();
        printf("encode time:%lldms\n", end_time - begin_time);
        if (ret < 0)
        {
            printf("encode failed\n");
            break;
        }
    }

    /* 冲刷编码器 */
    encode(codec_ctx, NULL, pkt, out_file);
    all_end_time = get_time();
    printf("all encode time:%lldms\n", all_end_time - all_begin_time);
    printf("h264dec done!\n");

    for (int i = 0; i < samples.size(); ++i)
    {
        auto &sample = samples[i];
        std::string fileName = "samples/h264_enc/sample-" + std::to_string((int)i) + ".h264";
        auto out_file = fopen(fileName.c_str(), "wb");
        for (int j = 0; j < sample.size(); ++j)
        {
            uint32_t length = sample[j].size();
            length = htonl(length);
            Bytes bytes((Byte *)&length, (Byte *)&length + sizeof(uint32_t));
            bytes.insert(bytes.end(), sample[j].begin(), sample[j].end());
            sample[j].swap(bytes);
            fwrite(sample[j].data(), 1, sample[j].size(), out_file);
        }
        if (out_file)
        {
            fclose(out_file);
        }
    }

failed:
    if (out_file)
    {
        fclose(out_file);
    }
    if (in_file)
    {
        fclose(in_file);
    }
    if (codec_ctx)
    {
        avcodec_free_context(&codec_ctx);
    }
    if (frame)
    {
        av_frame_free(&frame);
    }
    if (pkt)
    {
        av_packet_free(&pkt);
    }
    if (yuv_buf)
    {
        free(yuv_buf);
    }
    getchar();
    return 0;
}