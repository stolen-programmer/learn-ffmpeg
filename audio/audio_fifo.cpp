//
// Created by juvenile on 2023-09-10.
//

#include <cassert>
#include <common.h>

extern "C" {
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
}

int main() {
    char errbuf[1024] = {0};
    FILE *fp = nullptr; // 输出文件
    AVPacket *pkt = nullptr; // 采集的数据包
    AVAudioFifo *resampleFifo = nullptr; // 重采样队列

    av_log_set_level(AV_LOG_TRACE);

    // 注册设备
    avdevice_register_all();

    // 查找输入格式
    auto inputFormat = av_find_input_format("dshow");
    if (inputFormat == nullptr) {
        av_log(nullptr, AV_LOG_ERROR, "dshow not found\n");
        return -1;
    }

    // 准备参数
    // https://ffmpeg.org/ffmpeg-devices.html#dshow
    AVDictionary *options = nullptr;
    // 采样率 每秒采样点 dshow一般默认44100
    av_dict_set(&options, "sample_rate", "44100", 0);
    // 采样大小 16位 默认值 通常s16le
    av_dict_set(&options, "sample_size", "16", 0);
    // 通道数  一般默认为2
    av_dict_set(&options, "channels", "2", 0);
    // 缓冲区大小 毫秒单位  44100(采样率)*16(采样大小)*2(通道) = 1411200 bits / 8 = 176400 bytes
    // 176400 / 2(1000ms / 500ms) = 88200 bytes
    av_dict_set(&options, "audio_buffer_size", "500", 0);
    // 测试不可用选项
    av_dict_set(&options, "test", "test", 0);
    av_dict_set(&options, "test", "test", 0);

    // open_device 打开设备
    AVFormatContext *inputFormatContext = nullptr;
    auto ret =
            avformat_open_input(&inputFormatContext, "audio=耳机 (Q10 Hands-Free AG Audio)", inputFormat, &options);
    check_ret(ret);

    // 有不可用选项
    if (options != nullptr) {
        AVDictionaryEntry *entry = nullptr;
        while (entry = av_dict_get(options, "", entry, AV_DICT_IGNORE_SUFFIX)) {
            av_log(nullptr, AV_LOG_DEBUG, "不可用 %s: %s\n", entry->key, entry->value);
        }

        av_dict_free(&options);
    }

    // 重采样的前后的数据播放时间必须相同
    // 44100(采样率)/16(采样格式)/2(立体声) -> 48000/32/2
    // 88200 / 2 / 2 = 22050 采样点
    // 22050 是44100的一半  所以是500ms
    // 48000 / 2 = 24000 采样点 目标采样点是24000
    // 分配数据缓冲区
    SwrContext *swrContext = nullptr;
    AVChannelLayout outAvChannelLayout = AV_CHANNEL_LAYOUT_STEREO;// 输出的声道布局 立体声(双声道)
    AVChannelLayout inAvChannelLayout = AV_CHANNEL_LAYOUT_STEREO; // 输入的声道布局 立体声

    uint8_t **src_audio_data = nullptr;
    int src_linesize = 0;
    av_samples_alloc_array_and_samples(&src_audio_data, // 音频数据存放的空间
                                       &src_linesize,  // 空间大小
                                       inAvChannelLayout.nb_channels, // 声道
                                       22050, // 采样点  88200(数据包数据大小) / 2(采样大小 字节) / 2(声道) = 22050 个采样点
                                       AV_SAMPLE_FMT_S16, 1); // 按1字节对齐

    assert(src_audio_data != nullptr);

    // fltp临时缓冲
    // 由于planar  需要很大的空间
    uint8_t **tmp_audio = nullptr;
    int tmp_linesize = 0;
    av_samples_alloc_array_and_samples(&tmp_audio,
                                       &tmp_linesize,
                                       2,
                                       48000,
                                       AV_SAMPLE_FMT_FLTP, 1);

    uint8_t **dst_audio_data = nullptr;
    int dst_linesize = 0;
    av_samples_alloc_array_and_samples(&dst_audio_data, // 音频数据存放的空间
                                       &dst_linesize,  // 空间大小
                                       inAvChannelLayout.nb_channels, // 声道
                                       24000, // 采样点  48000 / 2 = 24000
                                       // 后续编码aac 默认的aac编码器使用FLTP格式
                                       AV_SAMPLE_FMT_FLTP, 1); // 按1字节对齐

    assert(dst_audio_data != nullptr); // 断言不为空  理论上内存充足不会为空  为空则崩溃

    // 创建重采样上下文
    ret = swr_alloc_set_opts2(&swrContext,
                              &outAvChannelLayout, AV_SAMPLE_FMT_FLTP, 48000,// 输出 声道, 采样格式, 采样率
                              &inAvChannelLayout, AV_SAMPLE_FMT_S16, 44100,  // 输入 声道, 采样格式, 采样率
                              0, nullptr);

    check_label(ret, _SWR_INIT);

    ret = swr_init(swrContext);

    check_label(ret, _SWR_QUIT);

    fp = fopen("./test.pcm", "wb");

    // 音频队列按采样点为单位
    // 重采样音频队列
    resampleFifo = av_audio_fifo_alloc(AV_SAMPLE_FMT_FLTP, 2, 24000);

    pkt = av_packet_alloc();

    for (auto i = 0; i < 10; i++) {
        // 读取一个音频包
        ret = av_read_frame(inputFormatContext, pkt);
        check_label(ret, _QUIT);

        av_log(nullptr, AV_LOG_INFO, "pkt size: %d\n", pkt->size);
        memcpy(src_audio_data[0], pkt->data, pkt->size);

        // 输出的采样点个数
        auto nb_samples = swr_convert(swrContext, dst_audio_data, 24000, (const uint8_t**) src_audio_data, 22050);

        av_log(nullptr, AV_LOG_INFO, "resample: %d\n", nb_samples);
        // 默认的aac编码器每帧最多1024采样点
        // 但是这里明显多了  所以使用队列
        // 重采样后的数据写入队列
        av_audio_fifo_write(resampleFifo, (void**) dst_audio_data, nb_samples);

        // 重采样队列中有48000个采样点
        while (av_audio_fifo_size(resampleFifo) >= 48000) {
            // TODO aac编码 由于默认的aac编码器每帧最大1024采样点 所以采用FIFO
            // 这边先写入pcm文件
            av_audio_fifo_read(resampleFifo, (void**) tmp_audio, 48000);

            fwrite(tmp_audio[0], 1, tmp_linesize, fp);
            fwrite(tmp_audio[1], 1, tmp_linesize, fp);

            av_log(nullptr, AV_LOG_INFO, "write pcm file: %d sample\n", 48000);

        }

        av_packet_unref(pkt);
    }
    {
        auto size = av_audio_fifo_size(resampleFifo);
        av_log(nullptr, AV_LOG_INFO, "buffer has %d sample\n", size);
    }


_QUIT:;
    av_audio_fifo_free(resampleFifo);

    fclose(fp);
    av_packet_free(&pkt);

_SWR_QUIT:; // 初始化成功
    swr_free(&swrContext);
_SWR_INIT:; // 初始化失败
    av_freep(&src_audio_data[0]);
    av_freep(&src_audio_data);
    av_freep(&dst_audio_data[0]);
    av_freep(&dst_audio_data);
    av_freep(&tmp_audio[0]);
    av_freep(&tmp_audio);
    // 关闭输入
    avformat_close_input(&inputFormatContext);
}