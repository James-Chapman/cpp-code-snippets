#pragma once

#define WIN32_LEAN_AND_MEAN

#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"

class SocketClient
{
  public:
    SocketClient() : m_result{0}
    {
        // Initialize Winsock
        m_result = ::WSAStartup(MAKEWORD(2, 2), &m_wsaData);
        if (m_result != 0)
        {
            printf("WSAStartup failed with error: %d\n", m_result);
            return;
        }

        ZeroMemory(&m_hintsAddrInfo, sizeof(m_hintsAddrInfo));
        m_hintsAddrInfo.ai_family = AF_UNSPEC;
        m_hintsAddrInfo.ai_socktype = SOCK_STREAM;
        m_hintsAddrInfo.ai_protocol = IPPROTO_TCP;
    }

    virtual ~SocketClient()
    {
        ::freeaddrinfo(m_resultAddrInfo);
        ::WSACleanup();
    }

    SOCKET Connect(std::string server)
    {
        // Resolve the server address and port
        m_result = ::getaddrinfo(server.c_str(), DEFAULT_PORT, &m_hintsAddrInfo, &m_resultAddrInfo);
        if (m_result != 0)
        {
            printf("getaddrinfo failed with error: %d\n", m_result);
            ::WSACleanup();
        }

        SOCKET sock;
        // Attempt to connect to an address until one succeeds
        for (struct addrinfo* ptr = m_resultAddrInfo; ptr != NULL; ptr = ptr->ai_next)
        {
            // Create a SOCKET for connecting to server
            sock = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (sock == INVALID_SOCKET)
            {
                printf("socket failed with error: %ld\n", ::WSAGetLastError());
                ::WSACleanup();
                return 1;
            }

            // Connect to server.
            m_result = ::connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (m_result == SOCKET_ERROR)
            {
                closesocket(sock);
                sock = INVALID_SOCKET;
                continue;
            }
            break;
        }

        if (sock == INVALID_SOCKET)
        {
            printf("Unable to connect to server!\n");
            ::WSACleanup();
            return 1;
        }

        return sock;
    }

    void Send(SOCKET sock, std::string& buf, size_t bufLen)
    {
        // Send an initial buffer
        m_result = ::send(sock, buf.c_str(), bufLen, 0);
        if (m_result == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", ::WSAGetLastError());
            ::closesocket(sock);
            ::WSACleanup();
        }

        printf("Bytes Sent: %ld\n", m_result);
    }

    void Receive(SOCKET sock, std::string& buf, size_t bufLen)
    {
        // Receive a hello message.
        m_result = ::recv(sock, const_cast<char*>(buf.data()), bufLen, 0);
        if (m_result > 0)
        {
            printf("Bytes received: %d\n", m_result);
        }
        else
        {
            printf("recv failed with error: %d\n", ::WSAGetLastError());
        }
    }

    void Shutdown(SOCKET sock)
    {
        // shutdown the connection since no more data will be sent
        m_result = ::shutdown(sock, SD_SEND);
        if (m_result == SOCKET_ERROR)
        {
            printf("shutdown failed with error: %d\n", ::WSAGetLastError());
            ::closesocket(sock);
            ::WSACleanup();
        }
    }

  private:
    WSADATA m_wsaData;
    struct addrinfo* m_resultAddrInfo;
    struct addrinfo m_hintsAddrInfo;
    int m_result;
};
