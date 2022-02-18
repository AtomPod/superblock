#ifndef BACKENDSTORAGEFILE_H
#define BACKENDSTORAGEFILE_H
#include "backendstorage.h"
#include <fstream>

namespace hfs {

class BackendStorageFile : public BackendStorage
{
public:
    BackendStorageFile();
    ~BackendStorageFile();
public:
    bool Open(const std::string filepath);
public:
    virtual int64_t ReadAt(ByteBuffer buf, int64_t off);
    virtual int64_t WriteAt(ByteBuffer buf, int64_t off);
    virtual int64_t Append(ByteBuffer buf);
    virtual int64_t Size();
    virtual void    Truncate(int64_t size);
    virtual void    Close();
private:
    std::fstream m_file;
    std::string  m_filepath;
};

}

#endif // BACKENDSTORAGEFILE_H
