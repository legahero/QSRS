#-------------------------------------------------
#
# Project created by QtCreator 2017-02-07T14:42:17
#
#-------------------------------------------------

QT       += core network
QT -= gui

LIBS += -lpthread libwsock32 libws2_32 libntdll libpowrprof

CONFIG += c++11

TARGET = QSRS
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH +="./app"
INCLUDEPATH +="./core"
INCLUDEPATH +="./kernel"
INCLUDEPATH +="./libs"
INCLUDEPATH +="./protocol"
INCLUDEPATH +="./main"
INCLUDEPATH +="./utest"
INCLUDEPATH +="./auto"
INCLUDEPATH +="./st"

SOURCES += main.cpp\
    app/srs_app_async_call.cpp \
    app/srs_app_bandwidth.cpp \
    app/srs_app_caster_flv.cpp \
    app/srs_app_config.cpp \
    app/srs_app_conn.cpp \
    app/srs_app_dvr.cpp \
    app/srs_app_edge.cpp \
    app/srs_app_empty.cpp \
    app/srs_app_encoder.cpp \
    app/srs_app_ffmpeg.cpp \
    app/srs_app_forward.cpp \
    app/srs_app_hds.cpp \
    app/srs_app_heartbeat.cpp \
    app/srs_app_hls.cpp \
    app/srs_app_http_api.cpp \
    app/srs_app_http_client.cpp \
    app/srs_app_http_conn.cpp \
    app/srs_app_http_hooks.cpp \
    app/srs_app_http_static.cpp \
    app/srs_app_http_stream.cpp \
    app/srs_app_ingest.cpp \
    app/srs_app_listener.cpp \
    app/srs_app_log.cpp \
    app/srs_app_mpegts_udp.cpp \
    app/srs_app_pithy_print.cpp \
    app/srs_app_recv_thread.cpp \
    app/srs_app_refer.cpp \
    app/srs_app_reload.cpp \
    app/srs_app_rtmp_conn.cpp \
    app/srs_app_rtsp.cpp \
    app/srs_app_security.cpp \
    app/srs_app_server.cpp \
    app/srs_app_source.cpp \
    app/srs_app_st.cpp \
    app/srs_app_statistic.cpp \
    app/srs_app_thread.cpp \
    app/srs_app_utility.cpp \
    core/srs_core.cpp \
    core/srs_core_autofree.cpp \
    core/srs_core_mem_watch.cpp \
    core/srs_core_performance.cpp \
    kernel/srs_kernel_aac.cpp \
    kernel/srs_kernel_buffer.cpp \
    kernel/srs_kernel_codec.cpp \
    kernel/srs_kernel_consts.cpp \
    kernel/srs_kernel_error.cpp \
    kernel/srs_kernel_file.cpp \
    kernel/srs_kernel_flv.cpp \
    kernel/srs_kernel_log.cpp \
    kernel/srs_kernel_mp3.cpp \
    kernel/srs_kernel_stream.cpp \
    kernel/srs_kernel_ts.cpp \
    kernel/srs_kernel_utility.cpp \
    main/srs_main_server.cpp \
    protocol/srs_http_stack.cpp \
    protocol/srs_protocol_buffer.cpp \
    protocol/srs_protocol_json.cpp \
    protocol/srs_protocol_kbps.cpp \
    protocol/srs_raw_avc.cpp \
    protocol/srs_rtmp_amf0.cpp \
    protocol/srs_rtmp_handshake.cpp \
    protocol/srs_rtmp_io.cpp \
    protocol/srs_rtmp_msg_array.cpp \
    protocol/srs_rtmp_stack.cpp \
    protocol/srs_rtmp_utility.cpp \
    protocol/srs_rtsp_stack.cpp \    
    st/io.c \
    st/key.c \
    st/sched.c \    
    st/stk.c \
    st/sync.c \
    st/winpoll.cpp \
    st/event.c \
    st/fork.c

HEADERS  += \
    app/srs_app_async_call.hpp \
    app/srs_app_bandwidth.hpp \
    app/srs_app_caster_flv.hpp \
    app/srs_app_config.hpp \
    app/srs_app_conn.hpp \
    app/srs_app_dvr.hpp \
    app/srs_app_edge.hpp \
    app/srs_app_empty.hpp \
    app/srs_app_encoder.hpp \
    app/srs_app_ffmpeg.hpp \
    app/srs_app_forward.hpp \
    app/srs_app_hds.hpp \
    app/srs_app_heartbeat.hpp \
    app/srs_app_hls.hpp \
    app/srs_app_http_api.hpp \
    app/srs_app_http_client.hpp \
    app/srs_app_http_conn.hpp \
    app/srs_app_http_hooks.hpp \
    app/srs_app_http_static.hpp \
    app/srs_app_http_stream.hpp \
    app/srs_app_ingest.hpp \
    app/srs_app_listener.hpp \
    app/srs_app_log.hpp \
    app/srs_app_mpegts_udp.hpp \
    app/srs_app_pithy_print.hpp \
    app/srs_app_recv_thread.hpp \
    app/srs_app_refer.hpp \
    app/srs_app_reload.hpp \
    app/srs_app_rtmp_conn.hpp \
    app/srs_app_rtsp.hpp \
    app/srs_app_security.hpp \
    app/srs_app_server.hpp \
    app/srs_app_source.hpp \
    app/srs_app_st.hpp \
    app/srs_app_statistic.hpp \
    app/srs_app_thread.hpp \
    app/srs_app_utility.hpp \
    core/srs_core.hpp \
    core/srs_core_autofree.hpp \
    core/srs_core_mem_watch.hpp \
    core/srs_core_performance.hpp \
    kernel/srs_kernel_aac.hpp \
    kernel/srs_kernel_buffer.hpp \
    kernel/srs_kernel_codec.hpp \
    kernel/srs_kernel_consts.hpp \
    kernel/srs_kernel_error.hpp \
    kernel/srs_kernel_file.hpp \
    kernel/srs_kernel_flv.hpp \
    kernel/srs_kernel_log.hpp \
    kernel/srs_kernel_mp3.hpp \
    kernel/srs_kernel_stream.hpp \
    kernel/srs_kernel_ts.hpp \
    kernel/srs_kernel_utility.hpp \
    protocol/srs_http_stack.hpp \
    protocol/srs_protocol_buffer.hpp \
    protocol/srs_protocol_json.hpp \
    protocol/srs_protocol_kbps.hpp \
    protocol/srs_raw_avc.hpp \
    protocol/srs_rtmp_amf0.hpp \
    protocol/srs_rtmp_handshake.hpp \
    protocol/srs_rtmp_io.hpp \
    protocol/srs_rtmp_msg_array.hpp \
    protocol/srs_rtmp_stack.hpp \
    protocol/srs_rtmp_utility.hpp \
    protocol/srs_rtsp_stack.hpp \    
    st/common.h \
    st/md.h \    
    st/public.h \    
    st/winpoll.h \
    server_win.h


