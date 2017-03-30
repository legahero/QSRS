# QSRS
本项目是在ossrs/srs 2.0版本的上进行了基于QT 5.6 ( windows minGW)的兼容性修改形成的，本项目对State Threads Library-1.9进行了修改，使st能够兼容windows 32的环境。
在QT下进行调试的时候要在“项目”-》“运行”的运行参数中加上："-c XXXpath\conf\srs.conf"，
srs.conf必须是全路径，否则找不到.

命令行运行：QSRS.exe -c srs.conf   ，conf文件与执行文件在同一个目录，必须qt动态库文件必须有。
推流例子：ffmpeg -re -i test.flv -vcodec copy -acodec copy -f flv -y rtmp://127.0.0.1:1935/live/livestream
拉流例子：ffplay rtmp://127.0.0.1:1935/live/livestream
