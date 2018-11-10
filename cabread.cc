
#include "cabextract.hh"

#include <iostream>
#include <algorithm>
#include <string.h>
#include <fstream>

int list(const char *cab)
{
    CabExtract ce;

    if (ce.MountCab(cab) < 0) {
        std::cout << "failed to open " << cab << "\n";
        return 1;
    }

    FindCabData cabdata;
    while (ce.FindNextFileInCab(&cabdata) > 0) {
        std::cout << cabdata.name << " size " << cabdata.length << "\n";
    }

    return 0;
}

static ssize_t cabfilesize(CabExtract &ce, const char *cabfile)
{
    FindCabData cabdata;
    while (ce.FindNextFileInCab(&cabdata) > 0) {
        if (cabdata.name == cabfile)
            return cabdata.length;
    }
    return -1; // not found..
}

int extract(const char *cab, const char *cabfile, const char *dest)
{
    CabExtract ce;
    if (ce.MountCab(cab) < 0) {
        std::cout << "failed to open " << cab << "\n";
        return 1;
    }

    ssize_t size = cabfilesize(ce, cabfile);
    if (size < 0) {
        std::cout << "failed to find cabfile " << cabfile << " in " << cab << "\n";
        return 1;
    }

    HANDLE cf;
    if ( ce.OpenFileInCab(cabfile, &cf) < 0) {
        std::cout << "failed to open cabfile " << cabfile << " in " << cab << "\n";
        return 1;
    }

    std::ofstream outfile(dest, std::ofstream::binary);

    std::vector<uint8_t> buf(1024);
    off_t offset = 0;
    while (offset < size) {
        DWORD bytesread = std::min(DWORD(size-offset), DWORD(buf.size()));
        if (ce.ReadFromFileInCab(cf, offset, buf.data(), &bytesread) < 0) {
            std::cout << "faild to read " << cabfile << " in " << cab << "\n";
            return 1;
        }
        outfile.write((char*)buf.data(), bytesread);
        offset += bytesread;
    }
    ce.CloseFileInCab(cf);

    outfile.close();

    return 0;
}
int usage(const char *progname)
{
    std::cout << "Usage:\n";
    std::cout << progname << " -l <cab>                        list files in the cabinet\n";
    std::cout << progname << " -e <cab> <cabfile> <destfile>   extract a file from cabinet\n";
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1],"-l") == 0)
        return list(argv[2]);

    else if (argc == 5 && strcmp(argv[1],"-e") == 0)
        return extract(argv[2], argv[3], argv[4]);

    return usage(argv[0]);
}