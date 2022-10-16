#include "FileIO.hpp"
#include "library.hpp"

#include <cstdlib>

#ifndef COPYLIBRARY

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("USAGE: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    Uplinkzero::FileIO sourceFile(argv[1]);
    Uplinkzero::FileIO destinationFile(argv[2], true, false);

    size_t srcSz = sourceFile.GetFileSize();
    size_t readSz = 0;
    size_t nextReadSz = 0;
    size_t dstSz = 0;
    size_t currentBufSz = 0;

    if (srcSz > MAX_BUFFER_SIZE)
        nextReadSz = MAX_BUFFER_SIZE;
    else
        nextReadSz = srcSz;

    bool done = false;
    BYTE* buffer = reinterpret_cast<BYTE*>(malloc(nextReadSz));
    if (buffer == nullptr)
    {
        printf("No memory!\n");
        return 1;
    }
    currentBufSz = nextReadSz;

    do
    {

        if (nextReadSz != currentBufSz)
        {
            free(buffer);
            buffer = reinterpret_cast<BYTE*>(malloc(nextReadSz));
            if (buffer == nullptr)
            {
                printf("No memory!\n");
                return 1;
            }
        }

        sourceFile.ReadBlock(nextReadSz, buffer);
        dstSz += destinationFile.WriteBlock(nextReadSz, buffer);

        if (srcSz == dstSz)
        {
            done = true;
            break;
        }

        if (nextReadSz > srcSz - dstSz)
            nextReadSz = srcSz - dstSz;

        if (dstSz < srcSz)
        {
            continue;
        }
        if (dstSz > srcSz)
        {
            done = true;
            break;
        }
    } while (!done);

    free(buffer);

    if (srcSz != dstSz)
    {
        printf("Copying failed!\n");
        printf("Source file size = %zu    Destination file size = %zu\n", srcSz, dstSz);
        return 1;
    }
    else
    {
        printf("Copied %zu bytes from %s to %s\n", dstSz, sourceFile.GetFilePath(), destinationFile.GetFilePath());
        return 0;
    }
}

#endif // COPYLIBRARY
