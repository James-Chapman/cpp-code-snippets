#pragma once

#define WIN32_LEAN_AND_MEAN

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"

class SocketServer
{
public:
    SocketServer()
        : m_listenSocket{INVALID_SOCKET}, m_resultAddrInfo{nullptr}, m_result{0}, m_sendResult{0}, m_shutdown{false}
    {
        // Initialize Winsock
        m_result = ::WSAStartup(MAKEWORD(2, 2), &m_wsaData);
        if (m_result != 0)
        {
            printf("WSAStartup failed with error: %d\n", m_result);
            return;
        }

        ZeroMemory(&m_hintsAddrInfo, sizeof(m_hintsAddrInfo));
        m_hintsAddrInfo.ai_family = AF_INET;
        m_hintsAddrInfo.ai_socktype = SOCK_STREAM;
        m_hintsAddrInfo.ai_protocol = IPPROTO_TCP;
        m_hintsAddrInfo.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        m_result = ::getaddrinfo(NULL, DEFAULT_PORT, &m_hintsAddrInfo, &m_resultAddrInfo);
        if (m_result != 0)
        {
            printf("getaddrinfo failed with error: %d\n", m_result);
        }
    }

    virtual ~SocketServer()
    {
        if (m_resultAddrInfo)
        {
            ::freeaddrinfo(m_resultAddrInfo);
        }
        ::closesocket(m_listenSocket);
        ::WSACleanup();
    }

    void Receive(SOCKET& socket, std::string& recvBuf, size_t bufLen)
    {
        int recvBytes{0};
        int sentBytes{0};

        std::string sendBuf = "Hello client.";

        // Send an initial buffer
        sentBytes = ::send(socket, sendBuf.c_str(), sendBuf.size(), 0);
        if (socket == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", ::WSAGetLastError());
            ::closesocket(socket);
            return;
        }

        if (sentBytes > 0)
        {
            printf("Bytes sent: %d\n", sentBytes);
        }
        else
        {
            printf("send failed with error: %d\n", ::WSAGetLastError());
        }

        do
        {
            recvBytes = ::recv(socket, const_cast<char*>(recvBuf.data()), bufLen, 0);
            if (recvBytes > 0)
            {
                printf("Bytes received: %d\n", recvBytes);
            }
            else if (recvBytes == 0)
            {
                printf("\n");
            }
            else
            {
                printf("recv failed with error: %d\n", ::WSAGetLastError());
                ::closesocket(socket);
            }

        } while (recvBytes > 0);
    }

    void Listen(int threads)
    {
        // Create a SOCKET for the server to listen for client connections.
        m_listenSocket =
            ::socket(m_resultAddrInfo->ai_family, m_resultAddrInfo->ai_socktype, m_resultAddrInfo->ai_protocol);
        if (m_listenSocket == INVALID_SOCKET)
        {
            std::stringstream ss;
            ss << "socket failed with error: " << ::WSAGetLastError();
            ::freeaddrinfo(m_resultAddrInfo);
            ::WSACleanup();
            throw std::runtime_error{ss.str()};
        }

        // Setup the TCP listening socket
        m_result = ::bind(m_listenSocket, m_resultAddrInfo->ai_addr, (int)m_resultAddrInfo->ai_addrlen);
        if (m_result == SOCKET_ERROR)
        {
            std::stringstream ss;
            ss << "bind failed with error: " << ::WSAGetLastError();
            ::freeaddrinfo(m_resultAddrInfo);
            ::closesocket(m_listenSocket);
            ::WSACleanup();
            throw std::runtime_error{ss.str()};
        }

        ::freeaddrinfo(m_resultAddrInfo);

        // sping up 10 listening threads
        for (int i = 0; i < threads; ++i)
        {
            clientConnectionThreads.push_back(std::thread([this]() {
                m_result = ::listen(m_listenSocket, SOMAXCONN);
                if (m_result == SOCKET_ERROR)
                {
                    std::stringstream ss;
                    ss << "listen failed with error: " << ::WSAGetLastError();
                    ::closesocket(m_listenSocket);
                    ::WSACleanup();
                    throw std::runtime_error{ss.str()};
                }

                while (!m_shutdown)
                {
                    struct sockaddr_in clientAddrInfo
                    {
                        0
                    };
                    socklen_t sockLen = sizeof(clientAddrInfo);
                    auto sock = ::accept(m_listenSocket, reinterpret_cast<sockaddr*>(&clientAddrInfo), &sockLen);

                    char da[32];
                    auto ip = inet_ntop(clientAddrInfo.sin_family, &clientAddrInfo.sin_addr, da, 32);
                    printf("Accepted Connection from: %s\n", ip);

                    try
                    {
                        std::string recvBuf;
                        recvBuf.reserve(DEFAULT_BUFLEN);
                        // Accept a client socket

                        if (sock == INVALID_SOCKET)
                        {
                            std::stringstream ss;
                            ss << "accept failed with error: " << ::WSAGetLastError();
                            ::closesocket(m_listenSocket);
                            ::WSACleanup();
                            throw std::runtime_error{ss.str()};
                        }

                        this->Receive(sock, recvBuf, DEFAULT_BUFLEN);

                        // cleanup
                        ::closesocket(sock);
                    }

                    catch (const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';

                        // shutdown the connection since we're done
                        m_result = ::shutdown(sock, SD_SEND);
                        if (m_result == SOCKET_ERROR)
                        {
                            printf("shutdown failed with error: %d\n", ::WSAGetLastError());
                            ::closesocket(sock);
                            ::WSACleanup();
                        }
                    }

                    ::closesocket(sock);
                }
            }));
        }
    }

    void Shutdown()
    {
        printf("Calling shutdown\n");
        // No longer need server socket
        m_shutdown = true;
        ::closesocket(m_listenSocket);
        ::WSACleanup();

        for (auto& t : clientConnectionThreads)
        {
            t.join();
        }
    }

private:
    WSADATA m_wsaData;
    SOCKET m_listenSocket;
    std::vector<std::thread> clientConnectionThreads;

    struct addrinfo* m_resultAddrInfo;
    struct addrinfo m_hintsAddrInfo;

    int m_result;
    int m_sendResult;
    bool m_shutdown;
};
