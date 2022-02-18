#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include "backendstorage.h"
#include "needlemap.h"
#include "objectview.h"
#include <thread>

namespace hfs {

class SuperBlock
{
public:
    struct Options {
        ObjectView<BackendStorage> backendStorage;
        ObjectView<NeedleMap>      needleMap;
    };
public:
    SuperBlock(Options o);
    ~SuperBlock();
public:
    ByteBuffer Get(NeedleID id);
    NeedleID   Put(ByteBuffer data);
    bool       Delete(NeedleID id);
protected:
    bool       Check();
    bool       ParseNextPart(int64_t off, NeedleValue &value, bool &skip);
private:
    ObjectView<BackendStorage>  m_backendStorage;
    ObjectView<NeedleMap>       m_needleMap;
};

}


#endif // SUPERBLOCK_H
