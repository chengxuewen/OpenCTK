#include <iostream>

extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
#include <libavcodec/packet.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#define H264_PATH "big_buck_bunny_360x240.h264"
#define OUTPUT_PATH "big_buck_bunny_360x240_h264dec.yuv";
#define VIDEO_IN_BUFF_SIZE 20480
#define VIDEO_REFILL_THRESH 4096

static char s_errbuf[246] = {0};
static char *av_get_err(int err)
{
    av_strerror(err, s_errbuf, sizeof(s_errbuf));
    return s_errbuf;
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

static void decode(AVCodecContext *codec_ctx, AVPacket *pkt, AVFrame *frame, FILE *out_file)
{
    /* send the packet with the compressed data to the decoder */
    int ret = avcodec_send_packet(codec_ctx, pkt);
    if (AVERROR(EAGAIN) == ret)
    {
        fprintf(stderr, "Receive frame and send packet both returned EAGAIN, which is an API violation\n");
    }
    else if (ret < 0)
    {
        fprintf(stderr, "Error submiting the packet to the decoder, err:%s, pkt_size:%d\n", av_get_err(ret), pkt->size);
        return;
    }

    /* read all the output frames (in_file general there may be any number of them) */
    while (ret >= 0)
    {
        ret = avcodec_receive_frame(codec_ctx, frame);
        if (AVERROR(EAGAIN) == ret || AVERROR_EOF == ret)
        {
            return;
        }
        else if (ret < 0)
        {
            fprintf(stderr, " Error during decoding\n");
            exit(1);
        }
        print_video_format(frame);

        // 一般H264默认为 AV_PIX_FMT_YUV420P
        // frame->linesize[1]  因为有字节对齐的问题。
        // 音频的处理方式，在交错模式的时候，使用的 声道数*每个声道有多少个音频样本 * 每个样本占用多少个字节，这是因为音频上没有字节对齐的问题
        // 字节对齐问题的根本是因为 ，对于一张 322 * 356 的图片来说 ，322并不是16的整倍数，322/16 = 20......2 也就是说一行会有2个字节的剩余
        // 那么这个剩余的2个字节，怎么办呢？会多给14个字节和剩余的2个字节 结合起来。
        // 因此如果我们用和音频类似的写法： fwrite(frame->data[0], 1, frame->width * frame->height,  outfile) 去写，就会有问题，
        // 因为要保证这里 width是16的整倍数, 这时候就要用到 ffmpeg 的AVFrame给我们提供的 linesize[x]了，

        // uint8_t *data[AV_NUM_DATA_POINTERS];
        // 指向实际的帧数据的指针数组。
        // 对于视频帧, 这通常是图像平面（如YUV中的Y、U、V平面）。
        // 对于音频帧，这通常是音频通道的数据指针。

        // int linesize[AV_NUM_DATA_POINTERS]：
        // 每一行（视频）或每一个音频通道（音频）的大小。
        // 对于视频，这通常是图像宽度的字节数。如果图像的宽度 除以 16 有余数，则这个值会凑成16的倍数。
        // 对于音频，这通常是这个通道的字节数大小。 在交错模式下： 理论上等于   声道数 * 每个声道有多少个音频样本 * 每个样本占用多少个字节
        // 但是，测试发现，在第一个AVFrame包和最后一个 AVframe的时候，linesize[0]的值 比 声道数 * 每个声道有多少个音频样本 * 每个样本占
        // 用多少个字节 大于64.

        // 了解了linesize[]的意义，对于一个avframe，就是包含了一帧，就是一张图片，
        // YUV420P的存储方式是这样的  YYYYYYYYUUVV
        // 那么对于 一张 YUV420P （322 * 120）的图片来看，有多少个Y 呢？多少个U，多少个V呢？Y的个数为：有 120行，一行一行的存储，
        // 每一行的实际大小为322, 但是存储322个Y后，就结束了吗？没有 ，因为有字节对齐问题，因此每次存储完322后，还要跳过14个字节，
        // 也就是实际大小为linesize[0], 我们先将Y全部存储完毕。
        // 再存储U，U的个数是多少呢？这里要回头看一下YUV420P存储结构图，这里只是结论：宽高均是Y的一半，因此这里要注意存储U的写法
        // V的存储和U是一样的。
        // 正确写法  linesize[]代表每行的字节数量，所以每行的偏移是linesize[]，但是真正存储的值 Y 是宽度，
        for (int j = 0; j < frame->height; j++)
        {
            fwrite(frame->data[0] + j * frame->linesize[0], 1, frame->width, out_file);
        }
        for (int j = 0; j < frame->height / 2; j++)
        {
            fwrite(frame->data[1] + j * frame->linesize[1], 1, frame->width / 2, out_file);
        }
        for (int j = 0; j < frame->height / 2; j++)
        {
            fwrite(frame->data[2] + j * frame->linesize[2], 1, frame->width / 2, out_file);
        }
    }
}

// extract h264: ffmpeg -i source.mp4 -an -f h264 target.h264
// extract mpeg2: ffmpeg -i source.mp4 -an -f mpeg2video target.mpeg2
// play: ffplay -pixel_format yuv420p -video_size 320x240 -framerate 15 target.yuv
int main()
{
    fprintf(stderr, "ffmpeg version:%s\n", av_version_info());

    AVPacket *pkt = NULL;
    AVFrame *frame = NULL;
    const AVCodec *codec = NULL;
    AVCodecContext *codec_ctx = NULL;
    AVCodecParserContext *parser = NULL;
    const char *out_file_name = OUTPUT_PATH;

    int len = 0;
    int ret = 0;
    FILE *in_file = NULL;
    FILE *out_file = NULL;
    uint8_t *data = NULL;
    size_t data_size = 0;
    uint8_t in_buff[VIDEO_IN_BUFF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE] = {0};

    pkt = av_packet_alloc();
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    codec = avcodec_find_decoder_by_name("libopenh264");
    fprintf(stderr, "Codec name:%s\n", codec->name);
    if (!codec)
    {
        fprintf(stderr, "Codec not found\n");
        goto failed;
    }

    parser = av_parser_init(codec->id);
    if (!parser)
    {
        fprintf(stderr, "Parser not found\n");
        goto failed;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx)
    {
        fprintf(stderr, "Could not allocated audio codec context\n");
        goto failed;
    }

    if (avcodec_open2(codec_ctx, codec, NULL) < 0)
    {
        fprintf(stderr, "Could not open codec\n");
        goto failed;
    }

    in_file = fopen(H264_PATH, "rb");
    if (!in_file)
    {
        fprintf(stderr, "Could not open %s\n", H264_PATH);
        goto failed;
    }

    out_file = fopen(out_file_name, "wb");
    if (!out_file)
    {
        fprintf(stderr, "Could not open %s\n", out_file_name);
        goto failed;
    }

    data = in_buff;
    data_size = fread(in_buff, 1, VIDEO_IN_BUFF_SIZE, in_file);

    while (data_size > 0)
    {
        if (!frame)
        {
            frame = av_frame_alloc();
            if (!frame)
            {
                fprintf(stderr, "Could not allocated video frame\n");
                goto failed;
            }
        }

        ret = av_parser_parse2(parser, codec_ctx, &pkt->data, &pkt->size,
                               data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0)
        {
            fprintf(stderr, "Error while parsing\n");
            goto failed;
        }
        data += ret;
        data_size -= ret;

        if (pkt->size)
        {
            decode(codec_ctx, pkt, frame, out_file);
        }
        if (data_size < VIDEO_REFILL_THRESH)
        {
            memmove(in_buff, data, data_size);
            data = in_buff;
            len = fread(data + data_size, 1, VIDEO_IN_BUFF_SIZE - data_size, in_file);
            if (len > 0)
            {
                data_size += len;
            }
        }
    }

    if (frame)
    {
        pkt->data = NULL;
        pkt->size = 0;
        decode(codec_ctx, pkt, frame, out_file);
    }
    printf("h264dec done!\n");

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
    if (parser)
    {
        av_parser_close(parser);
    }
    if (frame)
    {
        av_frame_free(&frame);
    }
    if (pkt)
    {
        av_packet_free(&pkt);
    }
    getchar();
    return 0;
}