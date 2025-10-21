#include <iostream>

extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
}

#define MP4_PATH "big_buck_bunny_360x240.mp4"

#define ADTS_HEADER_LEN 7;

const int sampling_frequencies[] = {
    96000, // 0x0
    88200, // 0x1
    64000, // 0x2
    48000, // 0x3
    44100, // 0x4
    32000, // 0x5
    24000, // 0x6
    22050, // 0x7
    16000, // 0x8
    12000, // 0x9
    11025, // 0xa
    8000   // 0xb
           // 0xc 0xd 0xe 0xf is reserved
};

int adts_header(char *const p_adts_header, const int data_length,
                const int profile, const int samplerate,
                const int channels)
{
    int sampling_frequency_index = 3; // default 48000hz
    int adts_len = data_length + 7;

    int frequencies_size = sizeof(sampling_frequencies) / sizeof(sampling_frequencies[0]);
    int i = 0;
    for (i = 0; i < frequencies_size; i++)
    {
        if (sampling_frequencies[i] == samplerate)
        {
            sampling_frequency_index = i;
            break;
        }
    }
    if (i >= frequencies_size)
    {
        printf("unsupport samplerate:%d\n", samplerate);
        return -1;
    }

    p_adts_header[0] = 0xff;      // syncword:0xfff high 8bits
    p_adts_header[1] = 0xf0;      // syncword:0xfff low 4bits
    p_adts_header[1] |= (0 << 3); // MPEG Version:0:MPEG-4,1:MPEG-2 1bit
    p_adts_header[1] |= (0 << 1); // Layer:0 2bits
    p_adts_header[1] |= 1;        // protection absent:1 1bit

    p_adts_header[2] = (profile) << 6;                          // profile:profile 2bits
    p_adts_header[2] |= (sampling_frequency_index & 0x0f) << 2; // sampling frequency index:sampling_frequency_index  4bits
    p_adts_header[2] |= (0 << 1);                               // private bit:0 1bit
    p_adts_header[2] |= (channels & 0x04) >> 2;                 // channel configuration:channels high 1bit

    p_adts_header[3] = (channels & 0x03) << 6;       // channel configuration:channels low 2bits
    p_adts_header[3] |= (0 << 5);                    // original:0 1bit
    p_adts_header[3] |= (0 << 4);                    // home:0 1bit
    p_adts_header[3] |= (0 << 3);                    // copyright id bit:0 1bit
    p_adts_header[3] |= (0 << 2);                    // copyright id start:0 1bit
    p_adts_header[3] |= ((adts_len & 0x1800) >> 11); // frame length:value high 2bits

    p_adts_header[4] = (uint8_t)((adts_len & 0x7f8) >> 3); // frame length:value middle 8bits
    p_adts_header[5] = (uint8_t)((adts_len & 0x7) << 5);   // frame length:value low 3bits
    p_adts_header[5] |= 0x1f;                              // buffer fullness:0x7ff high 5bits
    p_adts_header[6] = 0xfc;                               // 11111100 // buffer fullness:0x7ff low 6bits
    // number_of_raw_data_blocks_in_frame：
    // There are numb_of_raw_data-blocks_in_frame+1 AAC original frame in the ADTS frame.
    return 0;
}

int main()
{
    fprintf(stderr, "ffmpeg version:%s\n", av_version_info());

    int len = 0;
    int audio_index = -1;
    AVPacket *pkt = NULL;
    AVFormatContext *fmt_ctx = NULL;

    const char *aac_file = "big_buck_bunny_360x240.aac";
    FILE *aac_fd = fopen(aac_file, "wb");
    if (!aac_fd)
    {
        av_log(NULL, AV_LOG_DEBUG, "Could not open destination file %s\n", aac_file);
        return -1;
    }

    int ret = avformat_open_input(&fmt_ctx, MP4_PATH, NULL, NULL);
    if (ret < 0)
    {
        char errbuf[1024] = {0};
        av_strerror(ret, errbuf, sizeof(errbuf) - 1);
        av_log(NULL, AV_LOG_DEBUG, "Could not open source file: %s, %d( %s)\n", MP4_PATH, ret, errbuf);
        goto failed;
    }

    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0)
    {
        char errbuf[1024] = {0};
        av_strerror(ret, errbuf, sizeof(errbuf) - 1);
        av_log(NULL, AV_LOG_DEBUG, "Failed to find stream information: %s, %d( %s)\n", MP4_PATH, ret, errbuf);
        goto failed;
    }

    printf("\n=== av_dump_format file:%s ===\n", MP4_PATH);
    av_dump_format(fmt_ctx, 0, MP4_PATH, 0);
    printf("\n=== av_dump_format finish ===\n\n");
    printf("media name:%s\n", fmt_ctx->url);
    printf("stream number:%d\n", fmt_ctx->nb_streams);
    printf("media average ratio:%lldkbps\n", (int64_t)fmt_ctx->bit_rate / 1024);

    // fmt_ctx->duration is usecs（microsecond）
    int total_seconds, hour, minute, second;
    total_seconds = (fmt_ctx->duration) / AV_TIME_BASE;
    hour = total_seconds / 3600;
    minute = (total_seconds & 3600) / 60;
    second = (total_seconds % 60);
    printf("total duration: %02d:%02d:%02d\n\n", hour, minute, second);

    pkt = av_packet_alloc();

    audio_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_index < 0)
    {
        av_log(NULL, AV_LOG_DEBUG, "Could not find %s stream in input file %s\n",
               av_get_media_type_string(AVMEDIA_TYPE_AUDIO), MP4_PATH);
        goto failed;
    }

    av_log(NULL, AV_LOG_INFO, "audio profile :%d\n", fmt_ctx->streams[audio_index]->codecpar->profile);
    if (AV_CODEC_ID_AAC != fmt_ctx->streams[audio_index]->codecpar->codec_id)
    {
        av_log(NULL, AV_LOG_DEBUG, "the media file no contain AAC stream, it's codec_id is %d\n",
               fmt_ctx->streams[audio_index]->codecpar->codec_id);
        goto failed;
    }
    while (av_read_frame(fmt_ctx, pkt) >= 0)
    {
        if (pkt->stream_index == audio_index)
        {
            char adts_header_buf[7] = {0};
            adts_header(adts_header_buf, pkt->size,
                        fmt_ctx->streams[audio_index]->codecpar->profile,
                        fmt_ctx->streams[audio_index]->codecpar->sample_rate,
                        fmt_ctx->streams[audio_index]->codecpar->ch_layout.nb_channels);
            fwrite(adts_header_buf, 1, 7, aac_fd);

            len = fwrite(pkt->data, 1, pkt->size, aac_fd);
            if (len != pkt->size)
            {
                av_log(NULL, AV_LOG_DEBUG, "warning, length of writed data isn't equal pkt->size(%d, %d)\n",
                       len, pkt->size);
            }
        }
        av_packet_unref(pkt);
    }

failed:
    if (aac_fd)
    {
        fclose(aac_fd);
    }
    if (pkt)
    {
        av_packet_free(&pkt);
    }
    if (fmt_ctx)
    {
        avformat_free_context(fmt_ctx);
    }
    getchar();
    return 0;
}