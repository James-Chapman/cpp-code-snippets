#pragma once

typedef enum _FILEIO_ERR
{
    ERR_OK,
    ERR_NOMEM,
    ERR_NODISK,
    ERR_UNKNOWN
} FILEIO_ERR;

#define MAX_BUFFER_SIZE 0x1000 // 4096

#ifdef COPYLIBRARY

#ifdef __cplusplus
extern "C"
{
#endif

    FILEIO_ERR CopyFile(const char* _src, const char* _dest);

#ifdef __cplusplus
}
#endif

#endif