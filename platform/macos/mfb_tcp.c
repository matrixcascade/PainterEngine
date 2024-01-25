
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "platform/modules/px_tcp.h"

// ------------------------------------
int PX_TCPInitialize(PX_TCP* tcp, PX_TCP_IP_TYPE type) {
    int err;
    int nRecvBuf = 1024 * 1024; // Size of the receive buffer
    int nSendBuf = 1024 * 1024; // Size of the send buffer
    int optval = 1;
    tcp->type = type;

    // Create a socket
    if ((tcp->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        err = errno;
        return 0;
    }

    // Set the size of the receive buffer
    if (setsockopt(tcp->socket, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int)) == -1) {
        close(tcp->socket);
        return 0;
    }

    // Set the size of the send buffer
    if (setsockopt(tcp->socket, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int)) == -1) {
        close(tcp->socket);
        return 0;
    }

    // Other socket configurations

    return 1;
}

// ------------------------------------
int PX_TCPConnect(PX_TCP* tcp, PX_TCP_ADDR addr) {
    struct sockaddr_in to;
    int ret;
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = addr.ipv4;
    to.sin_port = (addr.port);

    ret = connect(tcp->socket, (const struct sockaddr*)&to, sizeof(to));
    if (ret == 0) {
        tcp->connectAddr = addr;
        return 1;
    }
    return 0;
}

// ------------------------------------
int PX_TCPSend(PX_TCP* tcp, void* buffer, int size) {
    int length;
    switch (tcp->type) {
        case PX_TCP_IP_TYPE_IPV4: {
            while (size > 0) {
                if ((length = (int)send(tcp->socket, (const char*)buffer, size, 0)) == -1) {
                    return 0;
                }
                size -= length;
            }
            return 1;
        } break;
        case PX_TCP_IP_TYPE_IPV6: {
            return 0;
        } break;
    }
    return 0;
}

// ------------------------------------
int PX_TCPSocketSend(unsigned int socket, void* buffer, int size) {
    char* sendBuffer = (char*)buffer;
    int length;
    int sendsize = size;
    do {
        if ((length = (int)send(socket, (const char*)sendBuffer, size, 0)) == -1) {
            return 0;
        }
        sendBuffer += length;
        size -= length;
    } while (size > 0);
    return sendsize;
}

// ------------------------------------
int PX_TCPReceived(PX_TCP* tcp, void* buffer, int buffersize, int timeout) {
    size_t ReturnSize;
    struct timeval stimeout = {0, timeout * 1000};
    setsockopt(tcp->socket, SOL_SOCKET, SO_SNDTIMEO, (int*)&stimeout, (socklen_t)sizeof(struct timeval));

    switch (tcp->type) {
        case PX_TCP_IP_TYPE_IPV4: {
            int SockAddrSize = sizeof(struct sockaddr_in);
            return (int)recv(tcp->socket, (char*)buffer, buffersize, 0);
        } break;
        case PX_TCP_IP_TYPE_IPV6: {
            return 0;
        } break;
    }
    return 0;
}

// ------------------------------------
int PX_TCPSocketReceived(unsigned int socket, void* buffer, int buffersize, int timeout) {
    int SockAddrSize = sizeof(struct sockaddr_in);
    size_t ReturnSize;
    struct timeval stimeout = {0, timeout * 1000};
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (int*)&stimeout, sizeof(struct timeval));

    return (int)recv(socket, (char*)buffer, buffersize, 0);
}

// ------------------------------------
int PX_TCPAccept(PX_TCP* tcp, unsigned int* socket, PX_TCP_ADDR* fromAddr) {
    struct sockaddr sockaddr;
    int len;
    *socket = accept(tcp->socket, (struct sockaddr*)&sockaddr, (socklen_t*)&len);
    return *socket != -1;
}

// ------------------------------------
void PX_TCPFree(PX_TCP* tcp) { close(tcp->socket); }

// ------------------------------------
int PX_TCPListen(PX_TCP* tcp, unsigned short listen_Port) {
    struct sockaddr_in addrin;
    addrin.sin_family = AF_INET;
    addrin.sin_addr.s_addr = INADDR_ANY;
    addrin.sin_port = (listen_Port);

    if (bind(tcp->socket, (struct sockaddr*)&addrin, sizeof(struct sockaddr)) == -1) {
        close(tcp->socket);
        return 0;
    }
    return 1;
}

// ------------------------------------
PX_TCP_ADDR PX_TCP_ADDR_IPV4(unsigned int ipv4, unsigned short port) {
    PX_TCP_ADDR addr;
    addr.ipv4 = ipv4;
    addr.port = port;
    return addr;
}

// ------------------------------------
void PX_TCPSocketFree(unsigned int socket) { close(socket); }
