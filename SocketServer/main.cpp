#include "SocketServer.h"

namespace
{
bool isRunning{true};

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        printf("Ctrl-C event\n\n");
        isRunning = false;
        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        printf("Ctrl-Close event\n\n");
        return TRUE;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        printf("Ctrl-Break event\n\n");
        return FALSE;

    case CTRL_LOGOFF_EVENT:
        printf("Ctrl-Logoff event\n\n");
        return FALSE;

    case CTRL_SHUTDOWN_EVENT:
        printf("Ctrl-Shutdown event\n\n");
        return FALSE;

    default:
        return FALSE;
    }
}
} // namespace

int __cdecl main(void)
{
    SocketServer s;

    if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        s.Listen(24);
        std::cout << "Listening on 24 threads...\n";
        while (isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    else
    {
        return 1;
    }

    s.Shutdown();
    return 0;
}
