#ifndef CABEXTRACT_HH
#define CABEXTRACT_HH

#include <vector>
#include <string>
#include <sys/types.h>

struct FindCabData
{
    // this is probably meant to have file name +
    // possible other file attributes in it
    // something like stat()
    std::string name;
    size_t      length;
    bool operator==(const FindCabData &other) const {
        return (name == other.name) && (length == other.length);
    }
};

// windows isms
typedef int HANDLE;
typedef int32_t DWORD;

class CabExtract {
    struct mscab_decompressor *cabd = nullptr;
    struct mscabd_cabinet *cab = nullptr;
    struct mscabd_file *file = nullptr;

    std::vector<HANDLE> openfiles;

public:
    CabExtract();
    ~CabExtract();

    // Opens and loads CAB data into the library and prepares further processing. Returns error status.
    int MountCab(const char *file_path /*in*/);

    // these two are meant to iterate through files in the cab...
    int FindFirstFileInCab(struct FindCabData *pdata); // reset + findnextfilecab
    int FindNextFileInCab(struct FindCabData *pdata);

    // Accessing data of files stored in CAB
    int OpenFileInCab(const char *file_name_in_cab, HANDLE *phFileInCab ); // open file in cab
    int ReadFromFileInCab(HANDLE hFileInCab, int FromPos, void *destBuffer, DWORD *BytesRetuned); // read from file
    int CloseFileInCab(HANDLE hFileInCab);

};

#endif // CABEXTRACT_HH

