//#include <stdlib.h>
#include <memory>
#include "winpoll.h"


//#include <WinSock2.h>
//#include <Mswsock.h>
#include <Winternl.h>


typedef struct _AFD_POLL_HANDLE_INFO_
{
  SOCKET Handle;
  ULONG Events;
  NTSTATUS Status;
} AFD_POLL_HANDLE_INFO, *PAFD_POLL_HANDLE_INFO;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4200)
#endif

typedef struct _AFD_POLL_INFO
{
  LARGE_INTEGER Timeout;
  ULONG HandleCount;
  ULONG Exclusive;
  AFD_POLL_HANDLE_INFO Handles[];
} AFD_POLL_INFO, *PAFD_POLL_INFO;

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Taken directly from ReactOS sources
/* AFD event bits */
#define AFD_EVENT_RECEIVE_BIT                   0
#define AFD_EVENT_OOB_RECEIVE_BIT               1
#define AFD_EVENT_SEND_BIT                      2
#define AFD_EVENT_DISCONNECT_BIT                3
#define AFD_EVENT_ABORT_BIT                     4
#define AFD_EVENT_CLOSE_BIT                     5
#define AFD_EVENT_CONNECT_BIT                   6
#define AFD_EVENT_ACCEPT_BIT                    7
#define AFD_EVENT_CONNECT_FAIL_BIT              8
#define AFD_EVENT_QOS_BIT                       9
#define AFD_EVENT_GROUP_QOS_BIT                 10
#define AFD_EVENT_ROUTING_INTERFACE_CHANGE_BIT  11
#define AFD_EVENT_ADDRESS_LIST_CHANGE_BIT       12
#define AFD_MAX_EVENT                           13
#define AFD_ALL_EVENTS                          ((1 << AFD_MAX_EVENT) - 1)

/* AFD Event Flags */
#define AFD_EVENT_RECEIVE                   (1 << AFD_EVENT_RECEIVE_BIT)
#define AFD_EVENT_OOB_RECEIVE               (1 << AFD_EVENT_OOB_RECEIVE_BIT)
#define AFD_EVENT_SEND                      (1 << AFD_EVENT_SEND_BIT)
#define AFD_EVENT_DISCONNECT                (1 << AFD_EVENT_DISCONNECT_BIT)
#define AFD_EVENT_ABORT                     (1 << AFD_EVENT_ABORT_BIT)
#define AFD_EVENT_CLOSE                     (1 << AFD_EVENT_CLOSE_BIT)
#define AFD_EVENT_CONNECT                   (1 << AFD_EVENT_CONNECT_BIT)
#define AFD_EVENT_ACCEPT                    (1 << AFD_EVENT_ACCEPT_BIT)
#define AFD_EVENT_CONNECT_FAIL              (1 << AFD_EVENT_CONNECT_FAIL_BIT)
#define AFD_EVENT_QOS                       (1 << AFD_EVENT_QOS_BIT)
#define AFD_EVENT_GROUP_QOS                 (1 << AFD_EVENT_GROUP_QOS_BIT)
#define AFD_EVENT_ROUTING_INTERFACE_CHANGE  (1 << AFD_EVENT_ROUTING_INTERFACE_CHANGE_BIT)
#define AFD_EVENT_ADDRESS_LIST_CHANGE       (1 << AFD_EVENT_ADDRESS_LIST_CHANGE_BIT)

/* IOCTL Generation */
#define FSCTL_AFD_BASE                      FILE_DEVICE_NETWORK
#define AFD_CONTROL_CODE(Operation,Method) ((FSCTL_AFD_BASE)<<12 | (Operation<<2) | Method)

/* AFD Commands */
#define AFD_SELECT			9

#define IOCTL_AFD_SELECT AFD_CONTROL_CODE(AFD_SELECT, METHOD_BUFFERED )

//#define QT_BEGIN_NAMESPACE namespace winpolla {

enum ntstatus:int
{
  success = 0,
  pending = 0x00000103L,
  invalid_handle = static_cast<int>(0xC0000008L),
  object_type_mismatch = static_cast<int>(0xC0000024L),
  insufficient_resources = static_cast<int>(0xC000009AL),
  pagefile_quota = static_cast<int>(0xC0000007L),
  commitment_limit = static_cast<int>(0xC000012DL),
  working_set_quota = static_cast<int>(0xC00000A1L),
  no_memory = static_cast<int>(0xC0000017L),
  quota_exceeded = static_cast<int>(0xC0000044L),
  too_many_paging_files = static_cast<int>(0xC0000097L),
  remote_resources = static_cast<int>(0xC000013DL),
  too_many_addresses = static_cast<int>(0xC0000209L),
  sharing_violation = static_cast<int>(0xC0000043L),
  address_already_exists = static_cast<int>(0xC000020AL),
  link_timeout = static_cast<int>(0xC000013FL),
  io_timeout = static_cast<int>(0xC00000B5L),
  timeout = 0x00000102L,
  graceful_disconnect = static_cast<int>(0xC0000237L),
  remote_disconnect = static_cast<int>(0xC000013CL),
  connection_reset = static_cast<int>(0xC000020DL),
  link_failed = static_cast<int>(0xC000013EL),
  connection_disconnected = static_cast<int>(0xC000020CL),
  port_unreachable = static_cast<int>(0xC000023FL),
  hoplimit_exceeded = static_cast<int>(0xC000A012L),
  local_disconnect = static_cast<int>(0xC000013BL),
  transaction_aborted = static_cast<int>(0xC000020FL),
  connection_aborted = static_cast<int>(0xC0000241L),
  bad_network_path = static_cast<int>(0xC00000BEL),
  network_unreachable = static_cast<int>(0xC000023CL),
  protocol_unreachable = static_cast<int>(0xC000023EL),
  host_unreachable = static_cast<int>(0xC000023DL),
  cancelled = static_cast<int>(0xC0000120L),
  request_aborted = static_cast<int>(0xC0000240L),
  buffer_overflow = static_cast<int>(0x80000005L),
  invalid_buffer_size = static_cast<int>(0xC0000206L),
  buffer_too_small = static_cast<int>(0xC0000023L),
  access_violation = static_cast<int>(0xC0000005L),
  device_not_ready = static_cast<int>(0xC00000A3L),
  request_not_accepted = static_cast<int>(0xC00000D0L),
  invalid_network_response = static_cast<int>(0xC00000C3L),
  network_busy = static_cast<int>(0xC00000BFL),
  no_such_device = static_cast<int>(0xC000000EL),
  no_such_file = static_cast<int>(0xC000000FL),
  object_path_not_found = static_cast<int>(0xC000003AL),
  object_name_not_found = static_cast<int>(0xC0000034L),
  unexpected_network_error = static_cast<int>(0xC00000C4L),
  invalid_connection = static_cast<int>(0xC0000140L),
  remote_not_listening = static_cast<int>(0xC00000BCL),
  connection_refused = static_cast<int>(0xC0000236L),
  pipe_disconnected = static_cast<int>(0xC00000B0L),
  conflicting_address = static_cast<int>(0xC0000018L),
  invalid_address = static_cast<int>(0xC0000141L),
  invalid_address_component = static_cast<int>(0xC0000207L),
  not_supported = static_cast<int>(0xC00000BBL),
  not_implemented = static_cast<int>(0xC0000002L),
  access_denied = static_cast<int>(0xC0000022L)
};

int with_socket_error(int error)
{
  WSASetLastError(error);
  return SOCKET_ERROR;
}

BOOL wait_for_status(HANDLE event)
{
  // Initial 0.5 seconds wait
  DWORD status = WaitForSingleObjectEx(event, 500, TRUE);
  if (status == WAIT_OBJECT_0)
    return TRUE;

  do
  {
    status = WaitForSingleObjectEx(event, INFINITE, TRUE);
  } while (status == STATUS_USER_APC);

  return status == STATUS_WAIT_0 ? TRUE : FALSE;
}

int ntstatus_to_socket_error(ntstatus status)
{
  switch (status)
  {
  case success:
    return ERROR_SUCCESS;

  case pending:
    return ERROR_IO_PENDING;

  case invalid_handle:
  case object_type_mismatch:
    return WSAENOTSOCK;

  case insufficient_resources:
  case pagefile_quota:
  case commitment_limit:
  case working_set_quota:
  case no_memory:
  case quota_exceeded:
  case too_many_paging_files:
  case remote_resources:
    return WSAENOBUFS;

  case too_many_addresses:
  case sharing_violation:
  case address_already_exists:
    return WSAEADDRINUSE;

  case link_timeout:
  case io_timeout:
  case timeout:
    return WSAETIMEDOUT;

  case graceful_disconnect:
    return WSAEDISCON;

  case remote_disconnect:
  case connection_reset:
  case link_failed:
  case connection_disconnected:
  case port_unreachable:
  case hoplimit_exceeded:
    return WSAECONNRESET;

  case local_disconnect:
  case transaction_aborted:
  case connection_aborted:
    return WSAECONNABORTED;

  case bad_network_path:
  case network_unreachable:
  case protocol_unreachable:
    return WSAENETUNREACH;

  case host_unreachable:
    return WSAEHOSTUNREACH;

  case cancelled:
  case request_aborted:
    return WSAEINTR;

  case buffer_overflow:
  case invalid_buffer_size:
    return WSAEMSGSIZE;

  case buffer_too_small:
  case access_violation:
    return WSAEFAULT;

  case device_not_ready:
  case request_not_accepted:
    return WSAEWOULDBLOCK;

  case invalid_network_response:
  case network_busy:
  case no_such_device:
  case no_such_file:
  case object_path_not_found:
  case object_name_not_found:
  case unexpected_network_error:
    return WSAENETDOWN;

  case invalid_connection:
    return WSAENOTCONN;

  case remote_not_listening:
  case connection_refused:
    return WSAECONNREFUSED;

  case pipe_disconnected:
    return WSAESHUTDOWN;

  case conflicting_address:
  case invalid_address:
  case invalid_address_component:
    return WSAEADDRNOTAVAIL;

  case not_supported:
  case not_implemented:
    return WSAEOPNOTSUPP;

  case access_denied:
    return WSAEACCES;

  default:
    enum
    {
      facility_ntwin32 = 0x7
    };

    if ((status & (facility_ntwin32 << 16)) == (facility_ntwin32 << 16) &&
        (status & (ERROR_SEVERITY_ERROR | ERROR_SEVERITY_WARNING)))
    {
      /* It's a windows error that has been previously mapped to an */
      /* ntstatus code. */
      return (DWORD)(status & 0xffff);
    }
    else
    {
      return WSAEINVAL;
    }
  }
}


int poll_impl(struct pollfd* fds, const unsigned int nfds, int timeout)
{
  if (fds == nullptr || nfds == 0)
    return with_socket_error(WSAEINVAL);

  enum { invalid_event_mask = POLLPRI | POLLWRBAND };

  // Count real number of sockets for which status is requested
  unsigned real_nfds = 0;
  for (unsigned long i = 0U; i < nfds; ++i)
  {
    if (fds[i].fd != INVALID_SOCKET)
    {
      if (fds[i].events & invalid_event_mask)
        return with_socket_error(WSAEINVAL);
      ++real_nfds;
    }
  }

  // No real socket was given
  if (real_nfds == 0)
    with_socket_error(WSAEINVAL);

  thread_local static struct WSAPOLL_TLS
  {
    ~WSAPOLL_TLS()
    {
      if (event != INVALID_HANDLE_VALUE)
        ::CloseHandle(event);
    }

    std::unique_ptr<char[]> mem{nullptr};
    ULONG mem_size{0};
    HANDLE event{::CreateEventA(nullptr, FALSE, FALSE, nullptr)};
  } tls;

  // Allocate (only if necessary) for poll info buffer
  const ULONG mem_size =
      sizeof(AFD_POLL_INFO) + real_nfds * sizeof(AFD_POLL_HANDLE_INFO);
  if (tls.mem_size < mem_size)
  {
    auto mem = new (std::nothrow) char[mem_size];
    if (!mem)
      return with_socket_error(WSAENOBUFS);

    tls.mem.reset(mem);
    tls.mem_size = mem_size;
  }

  // Prepare poll info
  AFD_POLL_INFO* const poll_info =
      reinterpret_cast<AFD_POLL_INFO*>(tls.mem.get());
  poll_info->Timeout.QuadPart =
      timeout < 0 ? INT64_MAX : timeout * (-10 * 1000LL);
  poll_info->HandleCount = real_nfds;
  poll_info->Exclusive = FALSE;

  AFD_POLL_HANDLE_INFO* handle_info = poll_info->Handles;

  for (unsigned long i = 0U; i < nfds; ++i)
  {
    if (fds[i].fd == INVALID_SOCKET)
      continue;

    DWORD bytes;
    SOCKET base_socket = fds[i].fd;
    const auto ret =
        WSAIoctl(fds[i].fd, SIO_BASE_HANDLE, nullptr, 0U, &base_socket,
                 sizeof(base_socket), &bytes, nullptr, nullptr);

    handle_info->Handle = !ret ? base_socket : fds[i].fd;
    handle_info->Status = 0;

    // Error events are always received which is is different than what we'd got
    // with select() call
    handle_info->Events =
        AFD_EVENT_ABORT | AFD_EVENT_DISCONNECT | AFD_EVENT_CONNECT_FAIL;

    if (fds[i].events & POLLRDNORM)
      handle_info->Events |= AFD_EVENT_RECEIVE | AFD_EVENT_ACCEPT;
    if (fds[i].events & POLLRDBAND)
      handle_info->Events |= AFD_EVENT_OOB_RECEIVE;
    if (fds[i].events & POLLWRNORM)
      handle_info->Events |= AFD_EVENT_SEND | AFD_EVENT_CONNECT;

    ++handle_info;
  }

  IO_STATUS_BLOCK iosb;
  auto device_handle = reinterpret_cast<HANDLE>(poll_info->Handles[0].Handle);
  auto status = static_cast<ntstatus>(NtDeviceIoControlFile(
      device_handle, tls.event, nullptr, nullptr, &iosb, IOCTL_AFD_SELECT,
      poll_info, mem_size, poll_info, mem_size));

  // poll() is blocking, so we must wait for the event to be set if timeout was
  // something else than 0
  if (status == pending)
  {
    BOOL res = wait_for_status(tls.event);
    if (!res)
      return with_socket_error(GetLastError());
    status = static_cast<ntstatus>(iosb.Status);
  }

  if (status == success && poll_info->HandleCount > 0)
  {
    handle_info = poll_info->Handles;

    for (unsigned long i = 0U; i < nfds; ++i)
    {
      if (fds[i].fd == INVALID_SOCKET)
      {
        fds[i].revents = POLLNVAL;
        continue;
      }

      fds[i].revents = 0;

      if (handle_info->Events & (AFD_EVENT_RECEIVE | AFD_EVENT_ACCEPT))
        fds[i].revents |= POLLRDNORM;
      if (handle_info->Events & (AFD_EVENT_OOB_RECEIVE))
        fds[i].revents |= POLLRDBAND;
      if (handle_info->Events & (AFD_EVENT_SEND | AFD_EVENT_CONNECT))
        fds[i].revents |= POLLWRNORM;

      if ((handle_info->Events & AFD_EVENT_DISCONNECT) &&
          (handle_info->Events & AFD_EVENT_ABORT))
        fds[i].revents |= POLLHUP | POLLRDNORM | POLLRDBAND;
      if (handle_info->Events & (AFD_EVENT_ABORT))
        fds[i].revents |= POLLHUP | POLLERR;
      if (handle_info->Events & (AFD_EVENT_CONNECT_FAIL))
        fds[i].revents |= POLLHUP | POLLERR;

      // We get this even if we didn't ask for it in case other thread close
      // the socket in the middle of polling
      if (handle_info->Events & (AFD_EVENT_CLOSE))
        fds[i].revents |= POLLNVAL;

      ++handle_info;
    }
  }

  WSASetLastError(ntstatus_to_socket_error(static_cast<ntstatus>(status)));
  return status >= 0 ? static_cast<int>(poll_info->HandleCount) : SOCKET_ERROR;
 }
//# define QT_END_NAMESPACE
//}
// namespace anonymous

extern "C" int poll(struct pollfd *pollfds , int fdcnt, int timeout)
{
    return poll_impl(pollfds, (const unsigned int)fdcnt, timeout);
}

