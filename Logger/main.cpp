/*******************************************************************************
* Author      : James Chapman
* License     : BSD
* Date        : 17 November 2014
* Description : Singlton Logger Usage
********************************************************************************/

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "singlelog.hpp"

void threadFunc(int c)
{
    FourtyTwo::SingletonLogger * THREAD_LOGGER = FourtyTwo::SingletonLogger::getInstance();
    std::stringstream ss;
    ss << "Thread " << c;
    std::string module = ss.str();
    THREAD_LOGGER->trace(module, "Message trace");
    THREAD_LOGGER->debug(module, "Message debug");
    THREAD_LOGGER->info(module, "Message info");
    THREAD_LOGGER->notice(module, "Message notice");
    THREAD_LOGGER->warning(module, "Message warning");
    THREAD_LOGGER->error(module, "Message error");
    THREAD_LOGGER->critical(module, "Message critical");
}

int main()
{
    FourtyTwo::SingletonLogger * MAIN_LOGGER = FourtyTwo::SingletonLogger::getInstance();
    MAIN_LOGGER->setConsoleLogLevel(FourtyTwo::LogLevel::L_NOTICE);
    MAIN_LOGGER->setLogFilePath("singlelog.log");

    // Create 10 threads that run threadFunc
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i)
    {
        threads.push_back(std::thread(threadFunc, i));
    }

    // Join and exit all 10 threads
    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        it->join();
    }

    MAIN_LOGGER->info("main", "finished logging");

    return 0;
}
