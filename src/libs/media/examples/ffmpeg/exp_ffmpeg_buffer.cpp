#include <iostream>

extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/avutil.h>
#include <libavcodec/packet.h>
}

#define MEM_ITEM_SIZE (1024 * 20 * 102)

// test av_packet_alloc and av_packet_free
void av_packet_test1()
{
    AVPacket *pkt = av_packet_alloc();
    int ret = av_new_packet(pkt, MEM_ITEM_SIZE);
    memccpy(pkt->data, (void *)&av_packet_test1, 1, MEM_ITEM_SIZE);
    av_packet_unref(pkt);
    av_packet_free(&pkt); // called av_packet_unref
}

// test av_init_packet cause memory leak
void av_packet_test2()
{
    AVPacket *pkt = av_packet_alloc();
    int ret = av_new_packet(pkt, MEM_ITEM_SIZE);
    memccpy(pkt->data, (void *)&av_packet_test2, 1, MEM_ITEM_SIZE);
    av_packet_free(&pkt);
}

// test av_packet_move_ref
void av_packet_test3()
{
    AVPacket *pkt1 = av_packet_alloc();
    av_new_packet(pkt1, MEM_ITEM_SIZE);
    memccpy(pkt1->data, (void *)&av_packet_test3, 1, MEM_ITEM_SIZE);

    AVPacket *pkt2 = av_packet_alloc();
    av_packet_move_ref(pkt2, pkt1);

    av_packet_free(&pkt1);
    av_packet_free(&pkt2);
}

// test av_packet_clone
void av_packet_test4()
{
    AVPacket *pkt1 = av_packet_alloc();
    av_new_packet(pkt1, MEM_ITEM_SIZE);
    memccpy(pkt1->data, (void *)&av_packet_test4, 1, MEM_ITEM_SIZE);

    AVPacket *pkt2 = av_packet_clone(pkt1);

    av_packet_free(&pkt1);
    av_packet_free(&pkt2);
}

// test av_packet_ref and av_packet_unref
void av_packet_test5()
{
    AVPacket *pkt1 = av_packet_alloc();
    if (pkt1->buf)
    {
        printf("%s(%d) ref_count(pkt) = %d\n", __FUNCTION__, __LINE__, av_buffer_get_ref_count(pkt1->buf));
    }
    av_new_packet(pkt1, MEM_ITEM_SIZE);
    if (pkt1->buf)
    {
        printf("%s(%d) ref_count(pkt) = %d\n", __FUNCTION__, __LINE__, av_buffer_get_ref_count(pkt1->buf));
    }
    memccpy(pkt1->data, (void *)&av_packet_test5, 1, MEM_ITEM_SIZE);

    AVPacket *pkt2 = av_packet_alloc();
    av_packet_move_ref(pkt2, pkt1);

    av_packet_ref(pkt1, pkt2);
    av_packet_ref(pkt1, pkt2);
    if (pkt1->buf)
    {
        printf("%s(%d) ref_count(pkt) = %d\n", __FUNCTION__, __LINE__, av_buffer_get_ref_count(pkt1->buf));
    }
    if (pkt2->buf)
    {
        printf("%s(%d) ref_count(pkt) = %d\n", __FUNCTION__, __LINE__, av_buffer_get_ref_count(pkt2->buf));
    }
    av_packet_unref(pkt1); // ref_count 2
    av_packet_unref(pkt1);
    if (pkt1->buf)
    {
        printf("pkt1->buf not set NULL\n");
    }
    else
    {
        printf("pkt1->buf has set NULL\n");
    }
    if (pkt2->buf)
    {
        printf("%s(%d) ref_count(pkt) = %d\n", __FUNCTION__, __LINE__, av_buffer_get_ref_count(pkt2->buf));
    }
    av_packet_unref(pkt2);

    av_packet_free(&pkt1);
    av_packet_free(&pkt2);
}

// test AVPacket copy
void av_packet_test6()
{
    AVPacket *pkt1 = av_packet_alloc();
    av_new_packet(pkt1, MEM_ITEM_SIZE);
    memccpy(pkt1->data, (void *)&av_packet_test4, 1, MEM_ITEM_SIZE);

    AVPacket *pkt2 = av_packet_alloc();
    *pkt2 = *pkt1;
    av_init_packet(pkt1); // reset packet

    av_packet_free(&pkt1);
    av_packet_free(&pkt2);
}

void av_frame_test1()
{
    AVFrame *frame = av_frame_alloc();
    frame->nb_samples = 1024;
    frame->format = AV_SAMPLE_FMT_S16;
    frame->ch_layout = AV_CHANNEL_LAYOUT_MONO;
    av_frame_get_buffer(frame, 0);
    if (frame->buf && frame->buf[0])
    {
        printf("1 frame->buf[0]->Size = %d\n", frame->buf[0]->size);
        printf("ref_count1(frame) = %d\n", av_buffer_get_ref_count(frame->buf[0]));
    }
    int ret = av_frame_make_writable(frame);
    printf("av_frame_make_writable ret = %d\n", ret);
    if (frame->buf && frame->buf[0])
    {
        printf("ref_count2(frame) = %d\n", av_buffer_get_ref_count(frame->buf[0]));
    }

    av_frame_unref(frame);
    if (frame->buf && frame->buf[0])
    {
        printf("ref_count3(frame) = %d\n", av_buffer_get_ref_count(frame->buf[0]));
    }

    av_frame_free(&frame);
}

int main()
{
    fprintf(stderr, "ffmpeg version:%s\n", av_version_info());
    av_packet_test1();
    av_packet_test2();
    av_packet_test3();
    av_packet_test4();
    av_packet_test5();
    av_packet_test6();
    av_frame_test1();
    return 0;
}