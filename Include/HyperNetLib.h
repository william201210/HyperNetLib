#ifndef _HIGH_PERFORMANCE_NET_LIB_H_
#define _HIGH_PERFORMANCE_NET_LIB_H_

#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_

#include <stdint.h> /* int64_t */
#include <errno.h>

#ifdef WIN32
#include <windows.h>
typedef HINSTANCE lib_handle;
#define LoadLib LoadLibraryA
#define GetAPI GetProcAddress
#define FreeLib FreeLibrary
#else //WIN32
#include <dlfcn.h>
typedef void *lib_handle;
#define LoadLib(x) dlopen(x, RTLD_LAZY)
#define GetAPI dlsym
#define FreeLib dlclose
#endif //WIN32

#ifndef __cplusplus
typedef enum {false = 0, true = 1} bool;
#endif //__cplusplus

#ifdef WIN32
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else //WIN32
typedef long long int64;
typedef unsigned long long uint64;
#endif //WIN32

#if !defined(_SSIZE_T_) && !defined(_SSIZE_T_DEFINED)
typedef intptr_t ssize_t;
# define _SSIZE_T_
# define _SSIZE_T_DEFINED
#endif

#include <stddef.h>
#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

#endif //_TYPE_DEF_H_

#ifndef _WIN32
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
#endif

typedef void *net_handle;
typedef void *tcp_handle;
typedef void *udp_handle;
typedef void *stream_handle;
typedef void *mutex_handle;
typedef void *com_handle;
typedef void *rwlock_handle;
typedef void *sem_handle;
typedef void *shutdown_handle;
typedef void *write_handle;
typedef void *pipe_handle;
typedef void *connect_handle;
typedef void *send_handle;
typedef void *timer_handle;
typedef void *cpu_handle;
typedef void *address_handle;
typedef void *work_handle;
typedef void *thread_handle;
typedef void *addr_handle;
typedef void *once_handle;
typedef void *loop_handle;

typedef char net_tcp_t[1024];
typedef char net_shutdown_t[1024];
typedef char net_write_t[1024];

typedef enum {
  NET_MAX_ERRORS
} net_err_code;

struct net_err_s {
  /* read-only */
  net_err_code code;
  /* private */
  int sys_errno_;
};

typedef struct net_err_s net_err_t;

typedef enum {
  NET_UNKNOWN_HANDLE = 0,
  NET_FILE,
  NET_HANDLE_TYPE_MAX
} net_handle_type;

typedef enum {
  NET_UNKNOWN_REQ = 0,
  NET_REQ_TYPE_MAX
} net_req_type;

#if defined(__unix__) || defined(__POSIX__) || \
    defined(__APPLE__) || defined(_AIX)
#include <sys/types.h> /* size_t */
typedef struct {
  char* base;
  size_t len;
} net_buf_t;
typedef int net_os_sock_t;
#else
#include <Windows.h>
typedef struct net_buf_t {
  ULONG len;
  char* base;
} net_buf_t;
typedef SOCKET net_os_sock_t;
#endif

typedef enum {
  NET_LEAVE_GROUP = 0,
  NET_JOIN_GROUP
} net_membership;

typedef void (*net_shutdown_cb)(shutdown_handle req, int status);
typedef void (*net_close_cb)(com_handle handle);
typedef void (*net_connection_cb)(stream_handle server, int status);
typedef net_buf_t (*net_alloc_cb)(com_handle handle, size_t suggested_size);
typedef void (*net_read_cb)(stream_handle stream, ssize_t nread, net_buf_t buf);
typedef void (*net_read2_cb)(pipe_handle pipe, ssize_t nread, net_buf_t buf,
    net_handle_type pending);
typedef void (*net_write_cb)(write_handle req, int status);
typedef void (*net_connect_cb)(connect_handle req, int status);
typedef void (*net_timer_cb)(timer_handle handle, int status);
typedef void (*net_getaddrinfo_cb)(addr_handle req,
                                  int status,
                                  struct addrinfo* res);
typedef void (*net_udp_send_cb)(send_handle req, int status);
typedef void (*net_udp_recv_cb)(udp_handle handle, ssize_t nread, net_buf_t buf,
    struct sockaddr* addr, unsigned flags);
typedef void (*net_work_cb)(work_handle req);
typedef void (*net_after_work_cb)(work_handle req);

#ifdef __cplusplus
extern "C" {
#endif

typedef net_handle (*_loop_new)(void);
typedef void (*_loop_delete)(net_handle);
typedef net_handle (*_default_loop)(void);
typedef int (*_run)(net_handle);
typedef int (*_run_once)(net_handle);
typedef void (*_ref)(com_handle);
typedef void (*_unref)(com_handle);
typedef void (*_update_time)(net_handle);
typedef int64_t (*_now)(net_handle);
typedef net_err_t (*_last_error)(net_handle);
typedef const char* (*__strerror)(net_err_t err);
typedef const char* (*_err_name)(net_err_t err);
typedef int (*__shutdown)(shutdown_handle req, stream_handle handle,
    net_shutdown_cb cb);
typedef size_t (*_handle_size)(net_handle_type type);
typedef size_t (*_req_size)(net_req_type type);
typedef int (*_is_active)(const com_handle handle);
typedef void (*_close)(com_handle handle, net_close_cb close_cb);
typedef net_buf_t (*_buf_init)(char* base, unsigned int len);
typedef int (*__listen)(stream_handle stream, int backlog, net_connection_cb cb);
typedef int (*__accept)(stream_handle server, stream_handle client);
typedef int (*_read_start)(stream_handle, net_alloc_cb alloc_cb,
    net_read_cb read_cb);
typedef int (*_read_stop)(stream_handle);
typedef int (*_read2_start)(stream_handle, net_alloc_cb alloc_cb,
    net_read2_cb read_cb);
typedef int (*_write)(write_handle req, stream_handle handle,
    net_buf_t bufs[], int bufcnt, net_write_cb cb);
typedef int (*_write2)(write_handle req, stream_handle handle, net_buf_t bufs[],
    int bufcnt, stream_handle send_handle, net_write_cb cb);
typedef int (*_is_readable)(const stream_handle handle);
typedef int (*_is_writable)(const stream_handle handle);
typedef int (*_is_closing)(const com_handle handle);
typedef int (*_tcp_init)(net_handle, tcp_handle handle);
typedef int (*_tcp_open)(tcp_handle handle, net_os_sock_t sock);
typedef int (*_tcp_nodelay)(tcp_handle handle, int enable);
typedef int (*_tcp_keepalive)(tcp_handle handle, int enable,
    unsigned int delay);
typedef int (*_tcp_simultaneous_accepts)(tcp_handle handle, int enable);
typedef int (*_tcp_bind)(tcp_handle handle, struct sockaddr_in);
typedef int (*_tcp_bind6)(tcp_handle handle, struct sockaddr_in6);
typedef int (*_tcp_getsockname)(tcp_handle handle, struct sockaddr* name,
    int* namelen);
typedef int (*_tcp_getpeername)(tcp_handle handle, struct sockaddr* name,
    int* namelen);
typedef int (*_tcp_connect)(connect_handle req, tcp_handle handle,
    struct sockaddr_in address, net_connect_cb cb);
typedef int (*_tcp_connect6)(connect_handle req, tcp_handle handle,
    struct sockaddr_in6 address, net_connect_cb cb);
typedef int (*_udp_init)(net_handle, udp_handle handle);
typedef int (*_udp_open)(udp_handle handle, net_os_sock_t sock);
typedef int (*_udp_bind)(udp_handle handle, struct sockaddr_in addr,
    unsigned flags);
typedef int (*_udp_bind6)(udp_handle handle, struct sockaddr_in6 addr,
    unsigned flags);
typedef int (*_udp_getsockname)(udp_handle handle, struct sockaddr* name,
    int* namelen);
typedef int (*_udp_set_membership)(udp_handle handle,
    const char* multicast_addr, const char* interface_addr,
    net_membership membership);
typedef int (*_udp_set_multicast_loop)(udp_handle handle, int on);
typedef int (*_udp_set_multicast_ttl)(udp_handle handle, int ttl);
typedef int (*_udp_set_broadcast)(udp_handle handle, int on);
typedef int (*_udp_set_ttl)(udp_handle handle, int ttl);
typedef int (*_udp_send)(send_handle req, udp_handle handle,
    net_buf_t bufs[], int bufcnt, struct sockaddr_in addr,
    net_udp_send_cb send_cb);
typedef int (*_udp_send6)(send_handle req, udp_handle handle,
    net_buf_t bufs[], int bufcnt, struct sockaddr_in6 addr,
    net_udp_send_cb send_cb);
typedef int (*_udp_recv_start)(udp_handle handle, net_alloc_cb alloc_cb,
    net_udp_recv_cb recv_cb);
typedef int (*_udp_recv_stop)(udp_handle handle);
typedef int (*_timer_init)(net_handle, timer_handle timer);
typedef int (*_timer_start)(timer_handle timer,
                             net_timer_cb cb,
                             int64_t timeout,
                             int64_t repeat);
typedef int (*_timer_stop)(timer_handle timer);
typedef int (*_timer_again)(timer_handle timer);
typedef void (*_timer_set_repeat)(timer_handle timer, int64_t repeat);
typedef int64_t (*_timer_get_repeat)(timer_handle timer);
typedef int (*__getaddrinfo)(net_handle loop,
                             addr_handle req,
                             net_getaddrinfo_cb getaddrinfo_cb,
                             const char* node,
                             const char* service,
                             const struct addrinfo* hints);
typedef void (*__freeaddrinfo)(struct addrinfo* ai);
typedef struct sockaddr_in (*_ip4_addr)(const char* ip, int port);
typedef struct sockaddr_in6 (*_ip6_addr)(const char* ip, int port);
typedef int (*_ip4_name)(struct sockaddr_in* src, char* dst, size_t size);
typedef int (*_ip6_name)(struct sockaddr_in6* src, char* dst, size_t size);
typedef net_err_t (*_inet_ntop)(int af, const void* src, char* dst, size_t size);
typedef net_err_t (*_inet_pton)(int af, const char* src, void* dst);
typedef int (*_exepath)(char* buffer, size_t* size);
typedef net_err_t (*_cwd)(char* buffer, size_t size);
typedef net_err_t (*_chdir)(const char* dir);
typedef uint64_t (*_get_free_memory)(void);
typedef uint64_t (*_get_total_memory)(void);
typedef uint64_t (*_hrtime)(void);
typedef int (*_mutex_init)(mutex_handle handle);
typedef void (*_mutex_destroy)(mutex_handle handle);
typedef void (*_mutex_lock)(mutex_handle handle);
typedef int (*_mutex_trylock)(mutex_handle handle);
typedef void (*_mutex_unlock)(mutex_handle handle);
typedef int (*_rwlock_init)(rwlock_handle rwlock);
typedef void (*_rwlock_destroy)(rwlock_handle rwlock);
typedef void (*_rwlock_rdlock)(rwlock_handle rwlock);
typedef int (*_rwlock_tryrdlock)(rwlock_handle rwlock);
typedef void (*_rwlock_rdunlock)(rwlock_handle rwlock);
typedef void (*_rwlock_wrlock)(rwlock_handle rwlock);
typedef int (*_rwlock_trywrlock)(rwlock_handle rwlock);
typedef void (*_rwlock_wrunlock)(rwlock_handle rwlock);
typedef int (*_sem_init)(sem_handle sem, unsigned int value);
typedef void (*_sem_destroy)(sem_handle sem);
typedef void (*_sem_post)(sem_handle sem);
typedef void (*_sem_wait)(sem_handle sem);
typedef int (*_sem_trywait)(sem_handle sem);
typedef void (*_once)(once_handle guard, void (*callback)(void));
typedef int (*_thread_create)(thread_handle tid,
    void (*entry)(void *arg), void *arg);
typedef unsigned long (*_thread_self)(void);
typedef int (*_thread_join)(thread_handle tid);
typedef int (*_queue_work)(net_handle loop, work_handle req,
    net_work_cb work_cb, net_after_work_cb after_work_cb);
typedef net_err_t (*_uptime)(double* uptime);
typedef net_err_t (*_cpu_info)(cpu_handle* cpu_infos, int* count);
typedef void (*_free_cpu_info)(cpu_handle cpu_infos, int count);
typedef net_err_t (*_interface_addresses)(address_handle* addresses,
  int* count);
typedef void (*_free_interface_addresses)(address_handle addresses,
  int count);

struct HyperNetLib
{
	net_handle handle;
#ifdef HYPER_NETLIB_API
#undef HYPER_NETLIB_API
#undef HYPER_NETLIB_API2
#endif
#define HYPER_NETLIB_API(x) _##x x
#define HYPER_NETLIB_API2(x) __##x _##x
	HYPER_NETLIB_API(loop_new);
	HYPER_NETLIB_API(loop_delete);
	HYPER_NETLIB_API(default_loop);
	HYPER_NETLIB_API(run);
	HYPER_NETLIB_API(run_once);
	HYPER_NETLIB_API(ref);
	HYPER_NETLIB_API(unref);
	HYPER_NETLIB_API(update_time);
	HYPER_NETLIB_API(now);
	HYPER_NETLIB_API(last_error);
	HYPER_NETLIB_API2(strerror);
	HYPER_NETLIB_API(err_name);
	HYPER_NETLIB_API2(shutdown);
	HYPER_NETLIB_API(handle_size);
	HYPER_NETLIB_API(req_size);
	HYPER_NETLIB_API(is_active);
	HYPER_NETLIB_API(close);
	HYPER_NETLIB_API(buf_init);
	HYPER_NETLIB_API2(listen);
	HYPER_NETLIB_API2(accept);
	HYPER_NETLIB_API(read_start);
	HYPER_NETLIB_API(read_stop);
	HYPER_NETLIB_API(read2_start);
	HYPER_NETLIB_API(write);
	HYPER_NETLIB_API(write2);
	HYPER_NETLIB_API(is_readable);
	HYPER_NETLIB_API(is_writable);
	HYPER_NETLIB_API(is_closing);
	HYPER_NETLIB_API(tcp_init);
	HYPER_NETLIB_API(tcp_open);
	HYPER_NETLIB_API(tcp_nodelay);
	HYPER_NETLIB_API(tcp_keepalive);
	HYPER_NETLIB_API(tcp_simultaneous_accepts);
	HYPER_NETLIB_API(tcp_bind);
	HYPER_NETLIB_API(tcp_bind6);
	HYPER_NETLIB_API(tcp_getsockname);
	HYPER_NETLIB_API(tcp_getpeername);
	HYPER_NETLIB_API(tcp_connect);
	HYPER_NETLIB_API(tcp_connect6);
	HYPER_NETLIB_API(udp_init);
	HYPER_NETLIB_API(udp_open);
	HYPER_NETLIB_API(udp_bind);
	HYPER_NETLIB_API(udp_bind6);
	HYPER_NETLIB_API(udp_getsockname);
	HYPER_NETLIB_API(udp_set_membership);
	HYPER_NETLIB_API(udp_set_multicast_loop);
	HYPER_NETLIB_API(udp_set_multicast_ttl);
	HYPER_NETLIB_API(udp_set_broadcast);
	HYPER_NETLIB_API(udp_set_ttl);
	HYPER_NETLIB_API(udp_send);
	HYPER_NETLIB_API(udp_send6);
	HYPER_NETLIB_API(udp_recv_start);
	HYPER_NETLIB_API(udp_recv_stop);
	HYPER_NETLIB_API(timer_init);
	HYPER_NETLIB_API(timer_start);
	HYPER_NETLIB_API(timer_stop);
	HYPER_NETLIB_API(timer_again);
	HYPER_NETLIB_API(timer_set_repeat);
	HYPER_NETLIB_API(timer_get_repeat);
	HYPER_NETLIB_API2(getaddrinfo);
	HYPER_NETLIB_API2(freeaddrinfo);
	HYPER_NETLIB_API(ip4_addr);
	HYPER_NETLIB_API(ip6_addr);
	HYPER_NETLIB_API(ip4_name);
	HYPER_NETLIB_API(ip6_name);
	HYPER_NETLIB_API(inet_ntop);
	HYPER_NETLIB_API(inet_pton);
	HYPER_NETLIB_API(exepath);
	HYPER_NETLIB_API(cwd);
	HYPER_NETLIB_API(chdir);
	HYPER_NETLIB_API(get_free_memory);
	HYPER_NETLIB_API(get_total_memory);
	HYPER_NETLIB_API(hrtime);
	HYPER_NETLIB_API(mutex_init);
	HYPER_NETLIB_API(mutex_destroy);
	HYPER_NETLIB_API(mutex_lock);
	HYPER_NETLIB_API(mutex_trylock);
	HYPER_NETLIB_API(mutex_unlock);
	HYPER_NETLIB_API(rwlock_init);
	HYPER_NETLIB_API(rwlock_destroy);
	HYPER_NETLIB_API(rwlock_rdlock);
	HYPER_NETLIB_API(rwlock_tryrdlock);
	HYPER_NETLIB_API(rwlock_rdunlock);
	HYPER_NETLIB_API(rwlock_wrlock);
	HYPER_NETLIB_API(rwlock_trywrlock);
	HYPER_NETLIB_API(rwlock_wrunlock);
	HYPER_NETLIB_API(sem_init);
	HYPER_NETLIB_API(sem_destroy);
	HYPER_NETLIB_API(sem_post);
	HYPER_NETLIB_API(sem_wait);
	HYPER_NETLIB_API(sem_trywait);
	HYPER_NETLIB_API(once);
	HYPER_NETLIB_API(thread_create);
	HYPER_NETLIB_API(thread_self);
	HYPER_NETLIB_API(thread_join);
	HYPER_NETLIB_API(queue_work);
	HYPER_NETLIB_API(uptime);
	HYPER_NETLIB_API(cpu_info);
	HYPER_NETLIB_API(free_cpu_info);
	HYPER_NETLIB_API(interface_addresses);
	HYPER_NETLIB_API(free_interface_addresses);
};

typedef bool (*_HyperNetLib_Initialize)(struct HyperNetLib *hyperNetLib);
bool Initialize(struct HyperNetLib *hyperNetLib);

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif //_HIGH_PERFORMANCE_NET_LIB_H_

