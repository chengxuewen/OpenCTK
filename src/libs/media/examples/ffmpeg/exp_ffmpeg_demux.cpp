#include <iostream>

extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
}

#define MP4_PATH "big_buck_bunny_360x240.mp4"

int main()
{
    fprintf(stderr, "ffmpeg version:%s\n", av_version_info());

    AVFormatContext *fmt_ctx = NULL;
    int video_index = -1;
    int audio_index = -1;
    AVPacket *pkt = NULL;
    int pkt_count = 0;
    int print_max_count = 10;

    int ret = avformat_open_input(&fmt_ctx, MP4_PATH, NULL, NULL);
    if (ret < 0)
    {
        char errbuf[1024] = {0};
        av_strerror(ret, errbuf, sizeof(errbuf) - 1);
        printf("Open %s failed:%s\n", MP4_PATH, errbuf);
        goto failed;
    }
    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0)
    {
        char errbuf[1024] = {0};
        av_strerror(ret, errbuf, sizeof(errbuf) - 1);
        printf("avformat_find_stream_info %s failed:%s\n", MP4_PATH, errbuf);
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

    for (uint32_t i = 0; i < fmt_ctx->nb_streams; i++)
    {
        AVStream *stream = fmt_ctx->streams[i];
        if (AVMEDIA_TYPE_AUDIO == stream->codecpar->codec_type)
        {
            printf("=== Audio info:\n");
            printf("index=% d\n", stream->index);
            printf("samplerate:%dHz\n", stream->codecpar->sample_rate);
            if (AV_SAMPLE_FMT_FLTP == stream->codecpar->format)
            {
                printf("sampleformat:AV_SAMPLE_FMT_FLTP\n");
            }
            else if (AV_SAMPLE_FMT_S16P == stream->codecpar->format)
            {
                printf("sampleformat:AV_SAMPLE_FMT_S16P\n");
            }
            printf("channel number:%d\n", stream->codecpar->ch_layout.nb_channels);
            if (AV_CODEC_ID_AAC == stream->codecpar->codec_id)
            {
                printf("audio codec:AV_CODEC_ID_AAC\n");
            }
            else if (AV_CODEC_ID_MP3 == stream->codecpar->codec_id)
            {
                printf("audio codec:AV_CODEC_ID_MP3\n");
            }
            else
            {
                printf("audio codec_id:%d\n", stream->codecpar->codec_id);
            }
            if (AV_NOPTS_VALUE != stream->duration)
            {
                int duration_audio = (stream->duration) * av_q2d(stream->time_base);
                printf("audio duration: %02d:%02d:%02d\n",
                       duration_audio / 3600, (duration_audio % 3600) / 60, duration_audio % 60);
            }
            else
            {
                printf("audio duration unknown");
            }
            printf("\n");
            audio_index = i;
        }
        else if (AVMEDIA_TYPE_VIDEO == stream->codecpar->codec_type)
        {
            printf("=== Video info:\n");
            printf("index=% d\n", stream->index);
            printf("fps:%lffps\n", av_q2d(stream->avg_frame_rate));
            if (AV_CODEC_ID_MPEG4 == stream->codecpar->codec_id)
            {
                printf("video codec:AV_CODEC_ID_MPEG4\n");
            }
            else if (AV_CODEC_ID_H264 == stream->codecpar->codec_id)
            {
                printf("video codec: H264\n");
            }
            else
            {
                printf("video codec_id:%d\n", stream->codecpar->codec_id);
            }
            printf("width:%d height:%d\n", stream->codecpar->width, stream->codecpar->height);
            if (AV_NOPTS_VALUE != stream->duration)
            {
                int duration_audio = (stream->duration) * av_q2d(stream->time_base);
                printf("video duration: %02d:%02d:%02d\n",
                       duration_audio / 3600, (duration_audio % 3600) / 60, duration_audio % 60);
            }
            else
            {
                printf("video duration unknown");
            }
            printf("\n");
            video_index = i;
        }
    }

    pkt = av_packet_alloc();
    printf("\n---av_read_frame start\n");
    while (1)
    {
        ret = av_read_frame(fmt_ctx, pkt);
        if (ret < 0)
        {
            printf("av_read_frame end\n");
            break;
        }
        if (pkt_count++ < print_max_count)
        {
            if (audio_index == pkt->stream_index)
            {
                printf("audio pts:%lld\n", pkt->pts);
                printf("audio dts:%lld\n", pkt->dts);
                printf("audio size:%d\n", pkt->size);
                printf("audio pos:%lld\n", pkt->pos);
                printf("audio duration:%lf\n\n", pkt->duration * av_q2d(fmt_ctx->streams[audio_index]->time_base));
            }
            else if (video_index == pkt->stream_index)
            {
                printf("video pts:%lld\n", pkt->pts);
                printf("video dts:%lld\n", pkt->dts);
                printf("video size:%d\n", pkt->size);
                printf("video pos:%lld\n", pkt->pos);
                printf("video duration:%lf\n\n", pkt->duration * av_q2d(fmt_ctx->streams[video_index]->time_base)); 
            }
            else
            {
            printf("unknown stream index:%d\n", pkt->stream_index);
            }
        }
        av_packet_unref(pkt);
    }
    if (pkt)
    {
        av_packet_free(&pkt);
    }
    
failed:
    if (fmt_ctx)
    {
        avformat_free_context(fmt_ctx);
    }
    getchar();
    return 0;
}