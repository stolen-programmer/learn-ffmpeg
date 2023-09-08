
[ffmpeg-devices.html#dshow](https://ffmpeg.org/ffmpeg-devices.html#dshow)

```shell
ffmpeg.exe -hide_banner -list_devices true -f dshow -i dummy # 列出dshow可用设备
[dshow @ 0000029E49900B80] "screen-capture-recorder" (video)
[dshow @ 0000029E49900B80]   Alternative name "@device_sw_{860BB310-5D01-11D0-BD3B-00A0C911CE86}\{4EA69364-2C8A-4AE6-A561-56E4B5044439}"
[dshow @ 0000029E49900B80] "耳机 (Q10 Hands-Free AG Audio)" (audio)
[dshow @ 0000029E49900B80]   Alternative name "@device_cm_{33D9A762-90C8-11D0-BD43-00A0C911CE86}\wave_{AA48EF84-69D5-4A9B-B7FA-923DB5B0445B}"
[dshow @ 0000029E49900B80] "virtual-audio-capturer" (audio)
[dshow @ 0000029E49900B80]   Alternative name "@device_sw_{33D9A762-90C8-11D0-BD43-00A0C911CE86}\{8E146464-DB61-4309-AFA1-3578E927E935}"
```
