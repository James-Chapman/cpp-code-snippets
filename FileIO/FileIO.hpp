#pragma once

#pragma warning(disable : 4996)

#include <cstdio>

#define BYTE unsigned char

namespace Uplinkzero
{
class FileIO
{
  public:
    FileIO(const char* path, bool write = false, bool read = true);
    virtual ~FileIO();

    size_t GetFileSize();
    size_t ReadBlock(size_t size, BYTE* pBuffer);
    size_t WriteBlock(size_t size, BYTE* pBuffer);

    void operator<<(const char* string); // Dangerous! Assumes NUL termination. Should only be used with strings.

    const char* GetFileName();
    const char* GetFilePath();

  private:
    FILE* m_handle;
    size_t m_fileSize;
    size_t m_cursor;
    bool m_error;
    bool m_openRead;
    bool m_openWrite;
    const char* m_filePath;
};

} // namespace Uplinkzero
