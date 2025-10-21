#include <iostream>

extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
#include <libavcodec/packet.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#define AAC_PATH "big_buck_bunny_360x240.aac"
#define AUDIO_IN_BUFF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

static char s_errbuf[246] = {0};
static char *av_get_err(int err)
{
    av_strerror(err, s_errbuf, sizeof(s_errbuf));
    return s_errbuf;
}

static void print_audio_format(const AVFrame *frame)
{
    printf("print_video_format:---\n");
    printf("nb_samples:%u\n", frame->nb_samples);
    printf("sample_rate:%u\n", frame->sample_rate);
    printf("nb_channels:%u\n", frame->ch_layout.nb_channels);
    printf("format:%u, AV_SAMPLE_FMT_FLTP:%d\n", frame->format, AV_SAMPLE_FMT_FLTP);
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
    int data_size= 0;
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
        data_size = av_get_bytes_per_sample(codec_ctx->sample_fmt);
        if (data_size < 0)
        {
            /* this should not occur, checking just for paranoia */
            fprintf(stderr, "Failed to alculate data size\n");
            exit(1);
        }
        print_audio_format(frame);

        // 音频的处理方式，在交错模式的时候，使用的 声道数*每个声道有多少个音频样本 * 每个样本占用多少个字节，这是因为音频上没有字节对齐的问题
        // fwrite(frame->data[0], 1, frame->width * frame->height,  outfile) 去写，就会有问题，
        // 对于音频，这通常是这个通道的字节数大小。 在交错模式下： 理论上等于   声道数 * 每个声道有多少个音频样本 * 每个样本占用多少个字节
        /*
            Plannar mode:
                LLLLLLRRRRRRLLLLLLRRRRRRLLLLLLRRRRRR...
                LLLLLLRRRRRR as a frame
            Interleave mode:
                LRLRLRLRLRLRLRLRLRLR...
                LR as a audio sample
        */
        for (size_t i = 0; i < frame->nb_samples; i++)
        {
            for (size_t ch = 0; ch < codec_ctx->ch_layout.nb_channels; ch++) // Interleave mode write file, float format
            {
                fwrite(frame->data[ch] + data_size * i, 1, data_size, out_file);
            }
        }
    }
}

// ffplay -ar 48000 -ch_layout 5.1 -f f32le target.pcm
int main()
{
    fprintf(stderr, "ffmpeg version:%s\n", av_version_info());

    AVPacket *pkt = NULL;
    AVFrame *frame = NULL;
    const AVCodec *codec = NULL;
    AVCodecContext *codec_ctx = NULL;
    AVCodecParserContext *parser = NULL;
    const char *out_file_name = "big_buck_bunny_360x240_aacdec.pcm";

    int len = 0;
    int ret = 0;
    FILE *in_file = NULL;
    FILE *out_file = NULL;
    uint8_t *data = NULL;
    size_t data_size = 0;
    uint8_t in_buff[AUDIO_IN_BUFF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE] = {0};

    pkt = av_packet_alloc();
    codec = avcodec_find_decoder(AV_CODEC_ID_AAC);
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

    in_file = fopen(AAC_PATH, "rb");
    if (!in_file)
    {
        fprintf(stderr, "Could not open %s\n", AAC_PATH);
        goto failed;
    }

    out_file = fopen(out_file_name, "wb");
    if (!out_file)
    {
        fprintf(stderr, "Could not open %s\n", out_file_name);
        goto failed;
    }

    data = in_buff;
    data_size = fread(in_buff, 1, AUDIO_IN_BUFF_SIZE, in_file);

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
        if (data_size < AUDIO_REFILL_THRESH)
        {
            memmove(in_buff, data, data_size);
            data = in_buff;
            len = fread(data + data_size, 1, AUDIO_IN_BUFF_SIZE - data_size, in_file);
            if (len > 0)
            {
                data_size += len;
            }
        }
    }

    pkt->data = NULL;
    pkt->size = 0;
    decode(codec_ctx, pkt, frame, out_file);
    printf("aacdec done!\n");

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