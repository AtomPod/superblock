#ifndef BACKENDSTORAGE_H
#define BACKENDSTORAGE_H
#include "bytebuffer.h"

namespace hfs {

class BackendStorage
{
public:
    BackendStorage();
    virtual ~BackendStorage();
public:
    virtual int64_t ReadAt(ByteBuffer buf, int64_t off) = 0;
    virtual int64_t WriteAt(ByteBuffer buf, int64_t off) = 0;
    virtual int64_t Append(ByteBuffer buf) = 0;
    virtual int64_t Size() = 0;
    virtual void    Truncate(int64_t size) = 0;
    virtual void    Close() = 0;
};

}


#endif // BACKENDSTORAGE_H
