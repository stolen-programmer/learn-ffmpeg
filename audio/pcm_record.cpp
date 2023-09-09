//
// Created by juvenile on 2023-09-08.
//

#include <cassert>
#include <common.h>
#include <iostream>

extern "C" {
#include <libavdevice/avdevice.h>
}

int main(int argc, const char *argv[]) {
    av_log_set_level(AV_LOG_TRACE);

    char errbuf[1024] = {0};
    // 注册设备
    avdevice_register_all();

    // 查找输入格式
    auto inputFormat = av_find_input_format("dshow");
    if (inputFormat == nullptr) {
        std::cout << "dshow not found" << '\n';
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

    auto fp = fopen("./test.pcm", "wb");

    auto pkt = av_packet_alloc();

    for (auto i = 0; i < 10; i++) {
        // 读取一个音频包
        ret = av_read_frame(inputFormatContext, pkt);
        check_label(ret, _QUIT);

        av_log(nullptr, AV_LOG_DEBUG, "pkt size: %d\n", pkt->size);
        fwrite(pkt->data, 1, pkt->size, fp);
        av_packet_unref(pkt);
    }

_QUIT:;
    fclose(fp);
    av_packet_free(&pkt);
    // 关闭输入
    avformat_close_input(&inputFormatContext);
}
