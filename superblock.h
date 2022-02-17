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
        std::string                indexFilePath;
        uint32_t                   saveInterval;
    };
public:
    SuperBlock(Options o);
    ~SuperBlock();
public:
    ByteBuffer Get(NeedleID id);
    NeedleID   Put(ByteBuffer data);
    bool       Delete(NeedleID id);
protected:
    bool       WriteToFile();
    bool       ReadFromFile();
    bool       Check();
    bool       ParseNextPart(int64_t off, NeedleValue &value, bool &skip);
private:
    ObjectView<BackendStorage>  m_backendStorage;
    ObjectView<NeedleMap>       m_needleMap;
    std::string                 m_indexFilepath;
    bool                        m_backendStorageIsOwner;
    bool                        m_needleMapIsOwner;
    std::thread                 m_indexSaveThread;
    uint32_t                    m_saveInterval;
};

}


#endif // SUPERBLOCK_H
