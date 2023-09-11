#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
//
// Created by juvenile on 2023-09-10.
//
int main(int argc, const char *argv[]) {

    // planar to packed
    // LLRR to LRLR

    // arg0 当前可执行文件名
    // arg1 48000 采样率
    // arg2 32 采样大小
    // arg3 2 声道
    // arg4 输入文件
    // arg5 输出文件
    if (argc < 6) {
        printf("Usage: convert.exe 44100 16 2 input.pcm output.pcm");
        exit(-1);
    }
    // 采样率
    auto sample_rate = atoi(argv[1]);
    auto sample_fmt = atoi(argv[2]) / 8;
    auto channel = atoi(argv[3]);
    auto total = sample_rate * sample_fmt * channel;

    auto input_file = fopen(argv[4], "rb");
    auto output_file = fopen(argv[5], "wb");

    auto **buf = (uint8_t **) malloc(sizeof(uint8_t *) * channel);
    memset(buf, 0, sizeof(uint8_t *) * channel);

    *buf = (uint8_t *) malloc(sizeof(uint8_t) * sample_rate * sample_fmt * channel);
    memset(*buf, 0, sample_rate * sample_fmt * channel);

    for (auto i = 1; i < channel; i++) {
        buf[i] = buf[i - 1] + sample_rate * sample_fmt;
    }
    size_t ret = 0;
    size_t seek = 0;
    int index = 0;
    // 读取48000 * 4 * 2
    // 384000 / 1024 = 375
    // 384000 - 300(300 % 384000) >= 1024 取1024
    // 384000 - 382976(382976 % 384000) >= 1024 取1024
    // 384000 - 384000(384000 % 384000) < 1024
    // 384000 / 384000 = 1
    while (true) {

        while (seek / total == index) {
            // 读取的数据不够了
            ret = fread(buf[0] + seek % total, 1, total - seek % total >= 1024 ? 1024 : total - seek % total, input_file);
            if (ret < 1024 && ret < (total - seek % total)) {
                // 不够一包
                if (seek % total || seek / total == index) {
                    goto QUIT;
                }
            }
            seek += ret;
        }

        index++;

        printf("read %d sample %d %lu", index, sample_rate, sample_rate * sample_fmt * channel * index);

        for (auto i = 0; i < sample_rate * sample_fmt; i += sample_fmt) {
            for (auto ch = 0; ch < channel; ch++) {
                fwrite(&buf[ch][i], 1, sample_fmt, output_file);
            }
        }
    }


QUIT:;
    free(buf[0]);
    free(buf);

    fclose(input_file);
    fclose(output_file);

    return 0;
}