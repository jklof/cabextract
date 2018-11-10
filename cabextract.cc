#include "cabextract.hh"

#include <mspack.h>
#include <unistd.h>

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <algorithm>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>


// this will make a new unique id, works only in linux
// but we're targetting only linux for now..
static std::string make_uuid()
{
    std::ifstream t("/proc/sys/kernel/random/uuid");
    std::string uuid((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    return uuid;
}

CabExtract::CabExtract()
{
}
CabExtract::~CabExtract()
{
    // close open temp files, since they are already
    // unlinked closing will also delete them
    for (auto && fd : openfiles)
        close((int)fd);

    if (cab)
        cabd->close(cabd, cab);

    if (cabd)
        mspack_destroy_cab_decompressor(cabd);
}

int CabExtract::MountCab(const char *file_path)
{
    if (!cabd)
        cabd = mspack_create_cab_decompressor(NULL);

    if (!cabd)
        return -1;

    // if mounting again, close the previous mount..
    if (cab)
        cabd->close(cabd, cab);

    // open cab
    cab = cabd->open(cabd, file_path);

    // failed to open..
    if (!cab)
        return -1;

    // point file 'iterator' to beginning
    file = cab->files;

    return 0;
}

int CabExtract::FindFirstFileInCab(struct FindCabData *pdata)
{
    file = cab->files;
    if (!pdata)
        return 0;
    return FindNextFileInCab(pdata);
}

int CabExtract::FindNextFileInCab(struct FindCabData *pdata)
{
    if (!pdata)
        return -1;

    if (file == nullptr)
        return 0;  // zero means no more entries..

    pdata->name   = file->filename;
    pdata->length = file->length;
    file = file->next;
    return 1;
}

int CabExtract::OpenFileInCab(const char *file_name_in_cab, HANDLE *phFileInCab )
{
    if (!phFileInCab || !file_name_in_cab)
        return -1;

    // find the requested file in the cab
    struct mscabd_file *cf = cab->files;
    while (cf) {
        if (strcmp(cf->filename, file_name_in_cab) == 0)
            break;
        cf = cf->next;
    }

    if (!cf)
        return -1; // file not found

    std::string tmpfile = "/tmp/CabExtract-" + make_uuid();

    // extract the file to tmpfile
    if (cabd->extract(cabd, cf, tmpfile.c_str()) != MSPACK_ERR_OK) {
        unlink(tmpfile.c_str());
        return -1;
    }

    // open tmpfile
    int fd = open(tmpfile.c_str(), O_RDONLY);

    // unlink tmpfile
    unlink(tmpfile.c_str());

    if (fd == -1)
        return -1; // file open failed..

    *phFileInCab = fd;

    // keep track of the fd so we can close still open files
    // when CabExtract is destructed
    openfiles.push_back(*phFileInCab);

    return 0;
}

int CabExtract::ReadFromFileInCab(HANDLE hFileInCab, int FromPos, void *destBuffer, DWORD *BytesReturned)
{
    if (!BytesReturned || !destBuffer)
        return -1;

    ssize_t ret = pread(hFileInCab, destBuffer, *BytesReturned, FromPos);
    if (ret == -1)
        return -1;
    *BytesReturned = ret;
    return 0;
}

int CabExtract::CloseFileInCab(HANDLE hFileInCab)
{
    // stop keeping track of this file descriptor
    openfiles.erase(std::remove(openfiles.begin(), openfiles.end(), hFileInCab), openfiles.end());

    // close file descriptor, will also delete the temp file
    close((int)hFileInCab);
    return 0;
}

