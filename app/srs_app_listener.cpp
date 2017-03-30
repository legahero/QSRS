/*
The MIT License (MIT)

Copyright (c) 2013-2015 SRS(ossrs)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <srs_app_listener.hpp>

#include <sys/types.h>

#ifndef _WIN32
 #include <sys/socket.h>
 #include <arpa/inet.h>
#else
    #include <winsock2.h>
    #pragma comment(lib,"ws2_32.lib")
    #include <QDebug>
#endif

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#include <srs_kernel_log.hpp>
#include <srs_kernel_error.hpp>
#include <srs_app_server.hpp>
#include <srs_app_utility.hpp>

// set the max packet size.
#define SRS_UDP_MAX_PACKET_SIZE 65535

// sleep in ms for udp recv packet.
#define SRS_UDP_PACKET_RECV_CYCLE_INTERVAL_MS 0

// nginx also set to 512
#define SERVER_LISTEN_BACKLOG 512

ISrsUdpHandler::ISrsUdpHandler()
{
}

ISrsUdpHandler::~ISrsUdpHandler()
{
}

int ISrsUdpHandler::on_stfd_change(st_netfd_t /*fd*/)
{
    return ERROR_SUCCESS;
}

ISrsTcpHandler::ISrsTcpHandler()
{
}

ISrsTcpHandler::~ISrsTcpHandler()
{
}

SrsUdpListener::SrsUdpListener(ISrsUdpHandler* h, string i, int p)
{
    handler = h;
    ip = i;
    port = p;

    _fd = -1;
    _stfd = NULL;

    nb_buf = SRS_UDP_MAX_PACKET_SIZE;
    buf = new char[nb_buf];

    pthread = new SrsReusableThread("udp", this);
}

SrsUdpListener::~SrsUdpListener()
{
    // close the stfd to trigger thread to interrupted.
    srs_close_stfd(_stfd);

    pthread->stop();
    srs_freep(pthread);
    
    // st does not close it sometimes, 
    // close it manually.
    close(_fd);

    srs_freepa(buf);
}

int SrsUdpListener::fd()
{
    return _fd;
}

st_netfd_t SrsUdpListener::stfd()
{
    return _stfd;
}

int SrsUdpListener::listen()
{
    int ret = ERROR_SUCCESS;
    
//legahero 也许不是必须的
#ifdef WIN32
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsadata;
    if(WSAStartup(sockVersion,&wsadata)!=0)
    {
        srs_error("ERROR: WSAStartup");
    }
#endif
    
    if ((_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        ret = ERROR_SOCKET_CREATE;
        srs_error("create linux socket error. ip=%s, port=%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    srs_verbose("create linux socket success. ip=%s, port=%d, fd=%d", ip.c_str(), port, _fd);

	 int reuse_socket = 1;
#ifndef _WIN32    
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR,&reuse_socket, sizeof(int)) == -1) {
#else
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse_socket, sizeof(int)) == -1) {
#endif
        ret = ERROR_SOCKET_SETREUSE;
        srs_error("setsockopt reuse-addr error. ip=%s, port=%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    srs_verbose("setsockopt reuse-addr success. ip=%s, port=%d, fd=%d", ip.c_str(), port, _fd);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (bind(_fd, (const sockaddr*)&addr, sizeof(sockaddr_in)) == -1) {
        ret = ERROR_SOCKET_BIND;
        srs_error("bind socket error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    srs_verbose("bind socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    if ((_stfd = st_netfd_open_socket(_fd)) == NULL){
        ret = ERROR_ST_OPEN_SOCKET;
        srs_error("st_netfd_open_socket open socket failed. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    srs_verbose("st open socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    if ((ret = pthread->start()) != ERROR_SUCCESS) {
        srs_error("st_thread_create listen thread error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    srs_verbose("create st listen thread success, ep=%s:%d", ip.c_str(), port);

    return ret;
}

int SrsUdpListener::cycle()
{
    int ret = ERROR_SUCCESS;

    // TODO: FIXME: support ipv6, @see man 7 ipv6
    sockaddr_in from;
    int nb_from = sizeof(sockaddr_in);
    int nread = 0;

    if ((nread = st_recvfrom(_stfd, buf, nb_buf, (sockaddr*)&from, &nb_from, ST_UTIME_NO_TIMEOUT)) <= 0) {
        srs_warn("ignore recv udp packet failed, nread=%d", nread);
        return ret;
    }
    
    if ((ret = handler->on_udp_packet(&from, buf, nread)) != ERROR_SUCCESS) {
        srs_warn("handle udp packet failed. ret=%d", ret);
        return ret;
    }

    if (SRS_UDP_PACKET_RECV_CYCLE_INTERVAL_MS > 0) {
        st_usleep(SRS_UDP_PACKET_RECV_CYCLE_INTERVAL_MS * 1000);
    }

    return ret;
}

SrsTcpListener::SrsTcpListener(ISrsTcpHandler* h, string i, int p)
{
    handler = h;
    ip = i;
    port = p;

    _fd = -1;
    _stfd = NULL;

    pthread = new SrsReusableThread("tcp", this);
}

SrsTcpListener::~SrsTcpListener()
{
    pthread->stop();
    srs_freep(pthread);
    
    srs_close_stfd(_stfd);
}

int SrsTcpListener::fd()
{
    return _fd;
}

int SrsTcpListener::listen()
{
    int ret = ERROR_SUCCESS;

#ifdef WIN32
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsadata;
    if(WSAStartup(sockVersion,&wsadata)!=0)
    {
        srs_error("ERROR: WSAStartup");
    }
#endif
    
    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ret = ERROR_SOCKET_CREATE;
        srs_error("create linux socket error. port=%d, ret=%d", port, ret);
        return ret;
    }
    srs_verbose("create linux socket success. port=%d, fd=%d", port, _fd);
    
    int reuse_socket = 1;
#ifndef _WIN32    
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket, sizeof(int)) == -1) {
#else    
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse_socket, sizeof(int)) == -1) {
#endif
        ret = ERROR_SOCKET_SETREUSE;
        srs_error("setsockopt reuse-addr error. port=%d, ret=%d", port, ret);
        return ret;
    }
    srs_verbose("setsockopt reuse-addr success. port=%d, fd=%d", port, _fd);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (bind(_fd, (const sockaddr*)&addr, sizeof(sockaddr_in)) == -1) {
        ret = ERROR_SOCKET_BIND;
        srs_error("bind socket error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    srs_verbose("bind socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);

#ifdef WIN32
    int iRet=::listen(_fd, SERVER_LISTEN_BACKLOG);
    qDebug() <<"listen sock:" << _fd<<" ip:"<<ip.c_str()<<" port:"<<port<<" ret:"<<iRet<<"\n";
    if (iRet !=0)
    {
        ret = ERROR_SOCKET_LISTEN;
        srs_error("listen socket error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);        
        return ret;
    }    
#else    
    if (::listen(_fd, SERVER_LISTEN_BACKLOG) == -1) {
        ret = ERROR_SOCKET_LISTEN;
        srs_error("listen socket error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
#endif

    srs_verbose("listen socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    // Create file descriptor object from OS socket 
    if ((_stfd = st_netfd_open_socket(_fd)) == NULL){
        ret = ERROR_ST_OPEN_SOCKET;
        srs_error("st_netfd_open_socket open socket failed. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    srs_verbose("st open socket success. ep=%s:%d, fd=%d", ip.c_str(), port, _fd);
    
    if ((ret = pthread->start()) != ERROR_SUCCESS) {
        srs_error("st_thread_create listen thread error. ep=%s:%d, ret=%d", ip.c_str(), port, ret);
        return ret;
    }
    srs_verbose("create st listen thread success, ep=%s:%d", ip.c_str(), port);
    
    return ret;
}

int SrsTcpListener::cycle()
{
    int ret = ERROR_SUCCESS;
#ifdef WIN32
    struct sockaddr_in from;
    int fromlen;
    st_netfd_t client_stfd = st_accept(_stfd, (struct sockaddr *)&from, &fromlen, ST_UTIME_NO_TIMEOUT);
#else
    st_netfd_t client_stfd = st_accept(_stfd, NULL, NULL, ST_UTIME_NO_TIMEOUT);
#endif

    if(client_stfd == NULL){
        // ignore error.
        if (errno != EINTR) {
            srs_error("ignore accept thread stoppped for accept client error:%d",errno);
        }
        //EINTR是linux中函数的返回状态，在不同的函数中意义不同。表示某种阻塞的操作，被接收到的信号中断，造成的一种错误返回值。
        return ret;
    }
#ifdef WIN32
    // Save peer address, so we can retrieve it later   这个没有调用为什么？legahero
    st_netfd_setspecific(client_stfd, &from.sin_addr, NULL);
#endif
    srs_verbose("get a client. fd=%d", st_netfd_fileno(client_stfd));
    qDebug()<<"srv_stfd=" <<_stfd<<" client socket stfd=" << client_stfd;
    
    if ((ret = handler->on_tcp_client(client_stfd)) != ERROR_SUCCESS) {
        srs_warn("accept client error. ret=%d", ret);
        return ret;
    }
    
    return ret;
}

