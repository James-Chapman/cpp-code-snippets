/*******************************************************************************
 * Copyright (c) 2014 James Chapman
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ********************************************************************************/

#pragma once
#ifndef SINGLELOG_HPP
#define SINGLELOG_HPP

#include <atomic>
#include <codecvt>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#else
#include <mutex>
#endif

namespace FourtyTwo
{
/**
 * string <--> wstring converter
 * C++11
 */
std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

/**
 * Levels of logging available
 */
typedef enum _LogLevel
{
    L_TRACE = 100,
    L_DEBUG = 200,
    L_INFO = 300,
    L_NOTICE = 400,
    L_WARNING = 500,
    L_ERROR = 600,
    L_CRITICAL = 700,
    L_OFF = 1000
} LogLevel;

/**
 * Get current date/time, format is YYYY-MM-DD HH:mm:ss
 * ref: http://en.cppreference.com/w/cpp/chrono/c/wcsftime
 */
inline static std::string currentDateTime()
{
    time_t now = time(0);
    struct tm tstruct = {0};
    char buf[30] = {0};
    localtime_s(&tstruct, &now);
    asctime_s(buf, &tstruct);
    strftime(buf, sizeof(buf), "%F %T", &tstruct); // equivalent to "%Y-%m-%d %H:%M:%S"
    return buf;
}

/**
 * Get current date/time, format is YYYY-MM-DD HH:mm:ss
 * ref: http://en.cppreference.com/w/cpp/chrono/c/wcsftime
 */
inline static std::wstring currentDateTimeW()
{
    time_t now = time(0);
    struct tm tstruct = {0};
    wchar_t buf[30] = {0};
    localtime_s(&tstruct, &now);
    _wasctime_s(buf, &tstruct);
    wcsftime(buf, sizeof(buf), L"%F %T", &tstruct); // equivalent to "%Y-%m-%d %H:%M:%S"
    return buf;
}

/**
 * Wrapper class to handle locking on various platforms because
 * Critical Sections are preferable on Windows
 */
class ScopedLogLock
{
  public:
#ifdef _WIN32
    explicit ScopedLogLock(CRITICAL_SECTION* _cs)
    {
        m_lock = _cs;
        EnterCriticalSection(m_lock);
    }
    ~ScopedLogLock()
    {
        LeaveCriticalSection(m_lock);
        m_lock = nullptr;
    }
#else
    ScopedLogLock(std::mutex* _mtx)
    {
        m_lock = _mtx;
        m_lock->lock();
    }
    ~ScopedLogLock()
    {
        m_lock->unlock();
        m_lock = nullptr;
    }
#endif
  private:
#ifdef _WIN32
    CRITICAL_SECTION* m_lock;
#else
    std::mutex* m_lock;
#endif
};

/**
 * Logger class
 */
class SingletonLogger
{
  public:
    /**
     * Return a reference to the instance of this class
     * C++11 handles thread safety and removes the need for manual locking
     * http://stackoverflow.com/questions/8102125/is-local-static-variable-initialization-thread-safe-in-c11
     * http://stackoverflow.com/questions/33114896/reentrancy-in-static-method-with-static-variable
     */
    static SingletonLogger* getInstance()
    {
        static SingletonLogger instance;
        return &instance;
    }

    /**
     * Destructor
     */
    ~SingletonLogger()
    {
        if (!m_exit.load())
        {
            m_exit.store(true);
            m_consoleWriter.join();
            m_fstreamWriter.join();
        }
        if (m_fileOut.is_open())
        {
            m_fileOut << "\n\n";
            m_fileOut.close();
        }
#ifdef _WIN32
        DeleteCriticalSection(&m_consoleLogDequeLock);
        DeleteCriticalSection(&m_fstreamLogDequeLock);
        DeleteCriticalSection(&m_fstreamLock);
#endif
    }

    /**
     * Set the minimum log level for the console
     * L_TRACE, L_DEBUG, L_INFO, L_NOTICE, L_WARNING, ERROR, L_CRITICAL, L_OFF
     */
    void setConsoleLogLevel(LogLevel _logLevel)
    {
        m_consoleLogLevel = _logLevel;
    }

    /**
     * Set the minimum log level for the log file
     * L_TRACE, L_DEBUG, L_INFO, L_NOTICE, L_WARNING, ERROR, L_CRITICAL, L_OFF
     */
    void setFileLogLevel(LogLevel _logLevel)
    {
        m_fileLogLevel = _logLevel;
    }

    /**
     * Set the path to the log file
     */
    void setLogFilePath(const std::string& _filePath)
    {
        m_filePath = _filePath;
        m_fileOut.open(m_filePath, std::ios_base::out);
    }

    /**
     * Set the path to the log file
     */
    void setLogFilePath(const std::wstring& _filePath)
    {
        setLogFilePath(converter.to_bytes(_filePath));
    }

    /**
     * Log TRACE level messages
     */
#ifdef _DEBUG
    void trace(const std::string& _mod, const std::string& _msg)
    {
        std::string level = "TRACE";
        std::string line = makeLogLine(level, _mod, _msg);
        if (m_consoleLogLevel <= L_TRACE)
        {
            consoleLog(line);
        }
        if ((m_fileLogLevel <= L_TRACE) && (m_fileOut.is_open()))
        {
            fileLog(line);
        }
    }
#else
    void trace(std::string _mod, std::string _msg)
    {
    }
#endif

    /**
     * Log TRACE level messages
     */
    void trace(const std::wstring& _mod, const std::wstring& _msg)
    {
        trace(converter.to_bytes(_mod), converter.to_bytes(_msg));
    }

    /**
     * Log DEBUG level messages
     */
    void debug(const std::string& _mod, const std::string& _msg)
    {
        std::string level = "DEBUG";
        std::string line = makeLogLine(level, _mod, _msg);
        if (m_consoleLogLevel <= L_DEBUG)
        {
            consoleLog(line);
        }
        if ((m_fileLogLevel <= L_DEBUG) && (m_fileOut.is_open()))
        {
            fileLog(line);
        }
    }

    /**
     * Log DEBUG level messages
     */
    void debug(const std::wstring& _mod, const std::wstring& _msg)
    {
        debug(converter.to_bytes(_mod), converter.to_bytes(_msg));
    }

    /**
     * Log INFO level messages
     */
    void info(const std::string& _mod, const std::string& _msg)
    {
        std::string level = "INFO";
        std::string line = makeLogLine(level, _mod, _msg);
        if (m_consoleLogLevel <= L_INFO)
        {
            consoleLog(line);
        }
        if ((m_fileLogLevel <= L_INFO) && (m_fileOut.is_open()))
        {
            fileLog(line);
        }
    }

    /**
     * Log INFO level messages
     */
    void info(const std::wstring& _mod, const std::wstring& _msg)
    {
        info(converter.to_bytes(_mod), converter.to_bytes(_msg));
    }

    /**
     * Log NOTICE level messages
     */
    void notice(const std::string& _mod, const std::string& _msg)
    {
        std::string level = "NOTICE";
        std::string line = makeLogLine(level, _mod, _msg);
        if (m_consoleLogLevel <= L_NOTICE)
        {
            consoleLog(line);
        }
        if ((m_fileLogLevel <= L_NOTICE) && (m_fileOut.is_open()))
        {
            fileLog(line);
        }
    }

    /**
     * Log NOTICE level messages
     */
    void notice(const std::wstring& _mod, const std::wstring& _msg)
    {
        notice(converter.to_bytes(_mod), converter.to_bytes(_msg));
    }

    /**
     * Log WARNING level messages
     */
    void warning(const std::string& _mod, const std::string& _msg)
    {
        std::string level = "WARNING";
        std::string line = makeLogLine(level, _mod, _msg);
        if (m_consoleLogLevel <= L_WARNING)
        {
            consoleLog(line);
        }
        if ((m_fileLogLevel <= L_WARNING) && (m_fileOut.is_open()))
        {
            fileLog(line);
        }
    }

    /**
     * Log WARNING level messages
     */
    void warning(const std::wstring& _mod, const std::wstring& _msg)
    {
        warning(converter.to_bytes(_mod), converter.to_bytes(_msg));
    }

    /**
     * Log ERROR level messages
     */
    void error(const std::string& _mod, const std::string& _msg)
    {
        std::string level = "ERROR";
        std::string line = makeLogLine(level, _mod, _msg);
        if (m_consoleLogLevel <= L_ERROR)
        {
            consoleLog(line);
        }
        if ((m_fileLogLevel <= L_ERROR) && (m_fileOut.is_open()))
        {
            fileLog(line);
        }
    }

    /**
     * Log ERROR level messages
     */
    void error(const std::wstring& _mod, const std::wstring& _msg)
    {
        error(converter.to_bytes(_mod), converter.to_bytes(_msg));
    }

    /**
     * Log CRITICAL level messages
     */
    void critical(const std::string& _mod, const std::string& _msg)
    {
        std::string level = "CRITICAL";
        std::string line = makeLogLine(level, _mod, _msg);
        if (m_consoleLogLevel <= L_CRITICAL)
        {
            consoleLog(line);
        }
        if ((m_fileLogLevel <= L_CRITICAL) && (m_fileOut.is_open()))
        {
            fileLog(line);
        }
    }

    /**
     * Log CRITICAL level messages
     */
    void critical(const std::wstring& _mod, const std::wstring& _msg)
    {
        critical(converter.to_bytes(_mod), converter.to_bytes(_msg));
    }

  private:
    /**
     * Private Constructor
     */
    SingletonLogger()
    {
#ifdef _WIN32
        InitializeCriticalSection(&m_consoleLogDequeLock);
        InitializeCriticalSection(&m_fstreamLogDequeLock);
        InitializeCriticalSection(&m_fstreamLock);
#endif
        m_consoleLogLevel = L_TRACE;
        m_fileLogLevel = L_TRACE;
        m_filePath = "";
        m_exit = false;
        m_consoleWriter = std::thread(&SingletonLogger::consoleWriter, this);
        m_fstreamWriter = std::thread(&SingletonLogger::fstreamWriter, this);
    }

    /**
     * Copy constructor, we don't want it since this is a Singleton
     */
    SingletonLogger(SingletonLogger const& copy) = delete;
    SingletonLogger& operator=(SingletonLogger const& copy) = delete;

    /**
     * Create a common format log line
     * Note: There might be a better way to produce UTF8 from ANSI text? This is "expensive".
     */
    static inline std::string makeLogLine(const std::string& _level, const std::string& _module,
                                          const std::string& _message)
    {
        std::stringstream ss;
        ss << "" << currentDateTime() << "  <" << _level << ">  " + _module + ":  " << _message << "\n";
        std::string ansi_s = ss.str();
        std::wstring utf16_s = converter.from_bytes(ansi_s);
        return converter.to_bytes(utf16_s); // return UTF-8
    }

    /**
     * Log message to console deque
     */
    inline void consoleLog(std::string _s)
    {
        ScopedLogLock lock(&m_consoleLogDequeLock);
        m_consoleLogDeque.push_back(_s);
    }

    /**
     * Log message to file deque
     */
    inline void fileLog(std::string _s)
    {
        ScopedLogLock lock(&m_fstreamLogDequeLock);
        m_fstreamLogDeque.push_back(_s);
    }

    /**
     * Write messages to the console.
     */
    void consoleWriter()
    {
        while (1)
        {
            bool consoleLogEmpty = m_consoleLogDeque.empty();
            if (!consoleLogEmpty)
            {
                ScopedLogLock lock(&m_consoleLogDequeLock);
                std::string s = m_consoleLogDeque.front();
                m_consoleLogDeque.pop_front();
                std::cout << s;
            }

            if ((m_exit.load()) && (consoleLogEmpty))
            {
                break;
            }

            // Sleep, otherwise this loop just eats CPU cycles for breakfast
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    /**
     * Write messages to the log file.
     */
    void fstreamWriter()
    {
        while (1)
        {
            bool fstreamLogEmpty = m_fstreamLogDeque.empty();
            if (!fstreamLogEmpty)
            {
                std::string s;
                {
                    ScopedLogLock lock(&m_fstreamLogDequeLock);
                    s = m_fstreamLogDeque.front();
                    m_fstreamLogDeque.pop_front();
                }
                {
                    ScopedLogLock lock(&m_fstreamLock);
                    m_fileOut.flush();
                    m_fileOut << s;
                }
            }

            if ((m_exit.load()) && (fstreamLogEmpty))
            {
                break;
            }

            // Sleep, otherwise this loop just eats CPU cycles for breakfast
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    LogLevel m_consoleLogLevel; // Min level for console logs
    LogLevel m_fileLogLevel;    // Min level for file logs
    std::ofstream m_fileOut;    // File output stream
    std::string m_filePath;     // Log file path

#ifdef _WIN32
    CRITICAL_SECTION m_consoleLogDequeLock;
    CRITICAL_SECTION m_fstreamLogDequeLock;
    CRITICAL_SECTION m_fstreamLock;
#else
    std::mutex m_consoleLogDequeLock;
    std::mutex m_fstreamLogDequeLock;
    std::mutex m_fstreamLock;
#endif

    std::deque<std::string> m_consoleLogDeque;
    std::deque<std::string> m_fstreamLogDeque;

    std::atomic_bool m_exit;

    std::thread m_consoleWriter;
    std::thread m_fstreamWriter;
};
}; // namespace FourtyTwo

#endif // SINGLELOG_HPP
