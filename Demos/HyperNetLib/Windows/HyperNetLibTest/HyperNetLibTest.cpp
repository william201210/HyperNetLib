#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define _DYNAMIC_LOAD_

#ifdef WIN32
#include "../../../../Include/HyperNetLib.h"
#define HYPER_NET_LIBPATH "..\\..\\..\\..\\Libs\\Windows\\HyperNetLib.dll"
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <HyperNetLib.h>
#define HYPER_NET_LIBPATH  "../../../Libs/Linux/HyperNetLib.so"
#endif //WIN32

struct HyperNetLib hyperNetLib;
net_tcp_t tcp_server;

typedef struct {
	net_tcp_t handle;
	net_shutdown_t shutdown_req;
} conn_rec;

static net_buf_t alloc_cb(com_handle handle, size_t suggested_size) {
	printf("client[%p] alloc buffer : [%d].\n", handle, suggested_size);
	static char buf[65536];
	return hyperNetLib.buf_init(buf, sizeof buf);
}

static void server_close_cb(com_handle handle) {
	printf("server[%p] shutdown.\n", handle);
}

static void connection_close_cb(com_handle handle) {
	conn_rec* conn = container_of(handle, conn_rec, handle);
	printf("client[%p] close, handle : %p .\n\n", conn, handle);
	free(conn);

    hyperNetLib.close((com_handle)&tcp_server, server_close_cb);
}

static void connection_shutdown_cb(shutdown_handle req, int status) {
	conn_rec* conn = container_of(req, conn_rec, shutdown_req);
	printf("client[%p] shutdown, req : %p .\n", conn, req);
	hyperNetLib.close((com_handle)&conn->handle, connection_close_cb);
}

static void read_cb(stream_handle stream, ssize_t nread, net_buf_t buf) {
	conn_rec* conn;
	conn = container_of(stream, conn_rec, handle);
	printf("receive %d bytes from client[%p], handle : %p.\n", nread, conn, stream);
	int r;

	if (nread >= 0)
	{
		printf("receive data from client:[%d]%s\n\n", nread, buf.base);
		return;
	}

	printf("client[%p] shutdown, handle : %p\n", conn, stream);

	r = hyperNetLib._shutdown(&conn->shutdown_req, stream, connection_shutdown_cb);
	assert(r == 0);
}

void server_connection_cb(stream_handle server, int status)
{
	printf("client connecting to server[%p].\n", server);

	assert(status == 0);
	assert(server == (stream_handle)&tcp_server);

	conn_rec* conn;
	conn = (conn_rec*)malloc(sizeof *conn);
	assert(conn != NULL);

	printf("client[%p] conncecting, handle : %p.\n", conn, &conn->handle);

	int r = 0;
	r = hyperNetLib.tcp_init(hyperNetLib.handle, &conn->handle);
	if(r != 0)
	{
		return;
	}

	r = hyperNetLib._accept(server, (stream_handle)&conn->handle);
	if(r != 0)
	{
		return;
	}

	r = hyperNetLib.read_start((stream_handle)&conn->handle, alloc_cb, read_cb);
	if(r != 0)
	{
		return;
	}

	printf("client[%p] connected to server[%p].\n\n", conn, server);
}

bool start_server()
{
	printf("starting server[%p].\n", &tcp_server);

	int r = hyperNetLib.tcp_init(hyperNetLib.handle, &tcp_server);
	if(r != 0)
	{
		return false;
	}

	struct sockaddr_in addr = hyperNetLib.ip4_addr("0.0.0.0", 8888);
	r = hyperNetLib.tcp_bind(&tcp_server, addr);
	if(r != 0)
	{
  	    hyperNetLib.close((com_handle)&tcp_server, server_close_cb);
		return false;
	}

	r = hyperNetLib._listen((stream_handle)&tcp_server, 128, server_connection_cb);
	if(r != 0)
	{
  	    hyperNetLib.close((com_handle)&tcp_server, server_close_cb);
		return false;
	}

	printf("started  server[%p] successfully.\n\n", &tcp_server);
	return true;
}

void HyperNetLib_Load()
{
#ifdef _DYNAMIC_LOAD_
	lib_handle handle = LoadLib(HYPER_NET_LIBPATH);
	if(handle == 0)
	{	
		printf("Load failed : %s.\n", HYPER_NET_LIBPATH);
		return;
	}
	 _HyperNetLib_Initialize HyperNetLib_Initialize = (_HyperNetLib_Initialize)GetAPI(handle, "Initialize");
	if(HyperNetLib_Initialize == 0)
	{
		printf("Get API error\n");
		return;
	}

	if(HyperNetLib_Initialize(&hyperNetLib))
	{
#else
	if(Initialize(&hyperNetLib))
	{
#endif //_DYNAMIC_LOAD_
		if(start_server())
		{
		}
		int result = hyperNetLib.run(hyperNetLib.handle);
	}

#ifdef _DYNAMIC_LOAD_
	FreeLib(handle);
#endif //_DYNAMIC_LOAD_
}

int main(int argc, char* argv[])
{
	printf("HyperNetLib testing start\r\n\n");

	HyperNetLib_Load();

	printf("\nPress Any key to Exit.");

	char c;
	scanf("%c", &c);
	return 0;
}