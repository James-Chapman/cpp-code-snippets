#include "library.hpp"

#include "FileIO.hpp"

#include <cstdlib>

#ifdef COPYLIBRARY

using namespace Uplinkzero;

#ifdef __cplusplus
extern "C"
{
#endif

    FILEIO_ERR CopyFile(const char* _src, const char* _dest)
    {
        FileIO sourceFile(_src);
        FileIO destinationFile(_dest, true, false);

        size_t srcSz = sourceFile.GetFileSize();
        size_t readSz = 0;
        size_t nextReadSz = 0;
        size_t dstSz = 0;

        if (srcSz == 0)
            return ERR_UNKNOWN;

        if (srcSz > MAX_BUFFER_SIZE)
            nextReadSz = MAX_BUFFER_SIZE;
        else
            nextReadSz = srcSz;

        bool done = false;

        do
        {
            BYTE* buffer = reinterpret_cast<BYTE*>(malloc(nextReadSz));
            if (buffer == nullptr)
                return ERR_NOMEM;

            sourceFile.ReadBlock(nextReadSz, buffer);
            dstSz += destinationFile.WriteBlock(nextReadSz, buffer);
            free(buffer);
            if (srcSz == dstSz)
            {
                done = true;
                break;
            }

            if (dstSz < srcSz)
                continue;
            if (dstSz > srcSz)
                break;
            if (nextReadSz > srcSz - dstSz)
                nextReadSz = srcSz - dstSz;
        } while (!done);

        if (srcSz != dstSz)
            return ERR_NODISK;

        return ERR_OK;
    }

#ifdef __cplusplus
}
#endif

#endif // COPYLIBRARY
