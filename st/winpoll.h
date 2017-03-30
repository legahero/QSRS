#ifndef _WINPOLL_H
#define _WINPOLL_H


#include <Mswsock.h>
#include <winsock2.h>
#include <sys/types.h>



#if (_WIN32_WINNT < 0x0600)
//#define POLLIN 1
//#define POLLPRI 2
//#define POLLOUT 4
//#define POLLERR 8
//#define POLLNVAL 20

#define POLLRDNORM 0x0100
#define POLLRDBAND 0x0200
#define POLLIN    (POLLRDNORM | POLLRDBAND)
#define POLLPRI    0x0400

#define POLLWRNORM 0x0010
#define POLLOUT   (POLLWRNORM)
#define POLLWRBAND 0x0020

#define POLLERR    0x0001
#define POLLHUP    0x0002
#define POLLNVAL   0x0004

#define SIO_BASE_HANDLE _WSAIOR(IOC_WS2,34)

struct pollfd {
               int fd;
               short events;
               short revents;
               };

#endif


/* same as WSABUF */
struct iovec {
    u_long iov_len;
    char *iov_base;
};

#ifndef PROCESSOR_POWER_INFORMATION
typedef struct _PROCESSOR_POWER_INFORMATION {
  ULONG  Number;
  ULONG  MaxMhz;
  ULONG  CurrentMhz;
  ULONG  MhzLimit;
  ULONG  MaxIdleState;
  ULONG  CurrentIdleState;
} PROCESSOR_POWER_INFORMATION , *PPROCESSOR_POWER_INFORMATION;
#endif

// see http://msdn.microsoft.com/en-us/library/cc704588(v=prot.10).aspx
#ifndef STATUS_SUCCESS
enum {
      STATUS_UNKNOWN = -1,
      STATUS_SUCCESS =              0x00000000,
      STATUS_BUFFER_OVERFLOW =      0x80000005,
      STATUS_INVALID_DEVICE_REQUEST = 0xC0000010,
      STATUS_END_OF_FILE =          0xC0000011,
      STATUS_CONFLICTING_ADDRESSES = 0xC0000018,
      STATUS_NO_MATCH =             0xC000001E,
      STATUS_ACCESS_DENIED =        0xC0000022,
      STATUS_BUFFER_TOO_SMALL =     0xC0000023,
      STATUS_OBJECT_TYPE_MISMATCH = 0xC0000024,
      STATUS_FILE_NOT_FOUND =       0xC0000028,
      STATUS_OBJECT_NAME_INVALID =  0xC0000033,
      STATUS_OBJECT_NAME_NOT_FOUND = 0xC0000034,
      STATUS_INVALID_PARAMETER_1 =  0xC00000EF,
      STATUS_IO_DEVICE_ERROR =      0xC0000185
 } ;
#endif

#define inline __inline

static inline int writev(int sock, struct iovec *iov, int nvecs)
{
    DWORD ret;
    if (WSASend(sock, (LPWSABUF)iov, nvecs, &ret, 0, NULL, NULL) == 0) {
        return ret;
    }
    return -1;
}

//WSARecv(SOCKET s,LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
static inline int readv(int sock, struct iovec *iov, int nvecs)
{
    DWORD ret;
    if (WSARecv(sock, (LPWSABUF)iov, nvecs, &ret, 0, NULL, NULL) == 0) {
        return ret;
    }
    return -1;
}

/*
struct msghdr  {
    void  * msg_name ;   //  消息的协议地址  协议地址和套接口信息，在非连接的UDP中，发送者要指定对方地址端口，接受方用于的到数据来源，如果不需要的话可以设置为NULL（在TCP或者连接的UDP中，一般设置为NULL）
    socklen_t msg_namelen ;   //  地址的长度
    struct iovec  * msg_iov ;   //  多io缓冲区的地址
     int  msg_iovlen ;   //  缓冲区的个数
    void  * msg_control ;   //  辅助数据的地址
    socklen_t msg_controllen ;   //  辅助数据的长度
     int  msg_flags ;   //  接收消息的标识
} ;
*/

#if (_WIN32_WINNT < 0x0600)
#ifdef __cplusplus
extern "C" {
#endif

int poll(struct pollfd *pollfds , int fdcnt, int timeout);
pid_t fork(void);
int kill(pid_t dPID,int signo);

const char *  inet_ntop(int af, const void *src, char *dst, size_t size);

int GetCpuHz();

#ifdef __cplusplus
}
#endif

#else
//int WSAAPI WSAPoll(WSAPOLLFD fdarray[],ULONG nfds,INT timeout);
static inline int poll(struct pollfd *pollfds , int fdcnt, int timeout)
{
    DWORD ret=WSAPoll(pollfds, fdcnt,timeout);
    if (ret == 0) {
        return ret;
    }
    return -1;
}
#endif

#endif
