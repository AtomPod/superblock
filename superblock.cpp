#include "superblock.h"
#include "needle.h"
#include <fstream>

namespace hfs {

SuperBlock::SuperBlock(SuperBlock::Options o) :
    m_backendStorage(o.backendStorage),
    m_needleMap(o.needleMap)
{
    Check();
}

SuperBlock::~SuperBlock()
{

}

ByteBuffer SuperBlock::Get(NeedleID id)
{
    NeedleValue index = m_needleMap->Get(id);
    if (!index) {
        return ByteBuffer();
    }

    Needle::Ptr needle = Needle::CreateEmpty();
    if (!needle->Read(m_backendStorage.ptr(), index.offset(), index.size())) {
        return ByteBuffer();
    }

    if (needle->flags()) {
        return ByteBuffer();
    }

    return needle->data();
}

NeedleID SuperBlock::Put(ByteBuffer data)
{
    int64_t  offset    = 0;
    uint32_t size      = 0;
    Needle::Ptr needle = Needle::Create(data);

    if (!needle->Append(m_backendStorage.ptr(), offset, size)) {
        return NeedleID(0);
    }

    if (!m_needleMap->Set(needle->id(), offset, size)) {
        return NeedleID(0);
    }

    return needle->id();
}

bool SuperBlock::Delete(NeedleID id)
{
    NeedleValue value = m_needleMap->Get(id);
    if (!value) {
        return false;
    }

    Needle::Ptr needle = Needle::CreateEmpty();
    bool result = needle->UpdateFlags(m_backendStorage.ptr(), value.offset(), 1);
    if (!result) {
        return false;
    }

    if (!m_needleMap->Delete(id)) {
        return false;
    }

    return true;
}

bool SuperBlock::Check()
{
    NeedleValue lastValue;
    lastValue.setOffset(0);
    lastValue.setSize(0);

    m_needleMap->AscendingVisit([&lastValue](NeedleValue value) {
        if (lastValue.offset() < value.offset())
          lastValue = value;
        return true;
    });


    int64_t checkSize = lastValue.offset() + lastValue.size();
    // 如果文件大小大于当前储存的位置，那么说明有数据未被添加到索引中
    // 则扫描储存文件，接下来的数据，否应该建立索引
    if (m_backendStorage->Size() > checkSize) {
        int64_t off = checkSize;
        NeedleValue value;
        bool skip;
        while (ParseNextPart(off, value, skip)) {
            off = value.offset() + value.size();

            if (!skip) {
                m_needleMap->Set(value.id(), value.offset(), value.size());
            }
        }
    }

    return true;
}

bool SuperBlock::ParseNextPart(int64_t off, NeedleValue &value, bool &skip)
{
    skip = false;
    Needle::Ptr needle = Needle::CreateEmpty();
    if (!needle->ReadHeader(m_backendStorage.ptr(), off, 0)) {
        return false;
    }

    if (needle->flags()) {
        skip = true;
    }

    value.setId(needle->id());
    value.setSize(needle->actualSize());
    value.setOffset(off);
    return true;
}


}
