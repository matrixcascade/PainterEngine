
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "platform/modules/px_udp.h"

// ------------------------------------
int PX_UDPInitialize(PX_UDP* udp, PX_UDP_IP_TYPE type) {
    int err;
    int nZero = 0;
    int nRecvBuf = 1024 * 1024 * 2;
    int nSendBuf = 1024 * 1024 * 2;
    int optval = 1;
    int imode = 1, rev;
    udp->type = type;

    if ((udp->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        err = errno;
        return 0;
    }

    rev = ioctl(udp->socket, FIONBIO, (u_long*)&imode);
    setsockopt(udp->socket, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
    setsockopt(udp->socket, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));

    if (rev == -1) {
        close(udp->socket);
        return 0;
    }

    if (setsockopt(udp->socket, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == -1) {
        close(udp->socket);
        return 0;
    }
    return 1;
}

// ------------------------------------
int PX_UDPSend(PX_UDP* udp, PX_UDP_ADDR addr, void* buffer, int size) {
    int length;
    switch (udp->type) {
        case PX_UDP_IP_TYPE_IPV4: {
            struct sockaddr_in to;
            to.sin_family = AF_INET;
            to.sin_addr.s_addr = addr.ipv4;
            to.sin_port = (addr.port);
            while (size > 0) {
                if ((length = (int)sendto(udp->socket, (const char*)buffer, size, 0, (struct sockaddr*)&to, sizeof(struct sockaddr))) == -1) {
                    return 0;
                }
                size -= length;
            }
            return 1;
        } break;
        case PX_UDP_IP_TYPE_IPV6: {
            //
        } break;
    }
    return 0;
}

// ------------------------------------
int PX_UDPReceived(PX_UDP* udp, PX_UDP_ADDR* from_addr, void* buffer, int buffersize, int* readsize) {
    size_t ReturnSize;
    switch (udp->type) {
        case PX_UDP_IP_TYPE_IPV4: {
            struct sockaddr_in in;
            socklen_t SockAddrSize = sizeof(struct sockaddr);
            if ((ReturnSize = recvfrom(udp->socket, (char*)buffer, buffersize, 0, (struct sockaddr*)&in, &SockAddrSize)) != -1) {
                from_addr->ipv4 = in.sin_addr.s_addr;
                from_addr->port = in.sin_port;
                *readsize = (int)ReturnSize;
                return 1;
            } else
                return 0;
        } break;
        case PX_UDP_IP_TYPE_IPV6: {
            //
        } break;
    }
    return 0;
}

// ------------------------------------
void PX_UDPFree(PX_UDP* udp) { close(udp->socket); }

// ------------------------------------
int PX_UDPListen(PX_UDP* udp, unsigned short listen_Port) {
    struct sockaddr_in addrin;
    addrin.sin_family = AF_INET;
    addrin.sin_addr.s_addr = INADDR_ANY;
    addrin.sin_port = (listen_Port);

    if (bind(udp->socket, (struct sockaddr*)&addrin, sizeof(struct sockaddr)) == -1) {
        close(udp->socket);
        return 0;
    }
    return 1;
}

// ------------------------------------
PX_UDP_ADDR PX_UDP_ADDR_IPV4(unsigned int ipv4, unsigned short port) {
    PX_UDP_ADDR addr;
    addr.ipv4 = ipv4;
    addr.port = port;
    return addr;
}

// ------------------------------------
unsigned int PX_UDPGetHostByName(const char* host, unsigned int dns_addr) {
    PX_UDP udp;
    if (PX_UDPInitialize(&udp, PX_UDP_IP_TYPE_IPV4)) {
        int strl, size, srcindex = 0, ptrindex = 12;
        int try = 8;
        PX_UDP_ADDR target;
        unsigned char content[1024] = {0x01, 0xEC, 0x01, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
        while (host[srcindex]) {
            unsigned char* plen = &content[ptrindex];
            *plen = 0;
            ptrindex++;
            while (1) {
                if (host[srcindex] == 0) {
                    break;
                } else if (host[srcindex] == '.') {
                    srcindex++;
                    break;
                } else {
                    content[ptrindex] = host[srcindex];
                    (*plen)++;
                }
                ptrindex++;
                srcindex++;
            }
            if (host[srcindex] == 0) {
                break;
            }
        }
        strl = (int)strlen(host);
        content[13 + strl + 1] = 0;
        content[13 + strl + 2] = 1;
        content[13 + strl + 3] = 0;
        content[13 + strl + 4] = 1;
        target.ipv4 = dns_addr;
        target.port = 53 << 8;

        while (try--) {
            PX_UDPSend(&udp, target, content, 12 + 1 + strl + 1 + 4);
            usleep(200000);
            if (PX_UDPReceived(&udp, &target, content, sizeof(content), &size)) {
                return (content[size - 1] << 24) + (content[size - 2] << 16) + (content[size - 3] << 8) + (content[size - 4]);
            }
        }
    }
    return 0;
}
