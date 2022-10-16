#include "SocketClient.h"

#include <iostream>
#include <string>
#include <vector>

#include <WinSock2.h>

int __cdecl main(int argc, char** argv)
{
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    std::string argv1 = argv[1];

    std::vector<std::thread> threadVector;

    for (int i = 0; i < 400; ++i)
    {
        threadVector.push_back(std::thread([&argv1]() {
            std::string recvBuf;
            recvBuf.reserve(128);
            std::string sendBuf = "Contrary to popular belief, Lorem Ipsum is not simply random text. It has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old. Richard McClintock, a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of \"de Finibus Bonorum et Malorum\" (The Extremes of Good and Evil) by Cicero, written in 45 BC. This book is a treatise on the theory of ethics, very popular during the Renaissance. The first line of Lorem Ipsum, \"Lorem ipsum dolor sit amet..\", comes from a line in section 1.10.32.";

            SocketClient client;
            SOCKET sock = client.Connect(argv1);
            client.Receive(sock, recvBuf, 128);
            client.Send(sock, sendBuf, sendBuf.size());
            client.Shutdown(sock);
        }));
    }

    for (auto& th : threadVector)
    {
        th.join();
    }

    return 0;
}
