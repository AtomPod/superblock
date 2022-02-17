#include "memneedlemap.h"
#include <fstream>

namespace hfs {

MemNeedleMap::MemNeedleMap()
{

}

MemNeedleMap::~MemNeedleMap()
{

}

bool MemNeedleMap::Set(NeedleID id, int64_t off, uint32_t size)
{
    m_needleDatas.emplace_back(id, off, size);
    m_needleIndexs.insert(std::make_pair(id, m_needleDatas.size() - 1));
    return true;
}

NeedleValue MemNeedleMap::Get(NeedleID id)
{
    auto found = m_needleIndexs.find(id);
    if (found == m_needleIndexs.end()) {
        return NeedleValue();
    }

    return m_needleDatas[found->second];
}

bool MemNeedleMap::Delete(NeedleID id)
{
    auto found = m_needleIndexs.find(id);
    if (found == m_needleIndexs.end()) {
        return false;
    }
    m_needleDatas.erase(m_needleDatas.begin() + static_cast<int64_t>(found->second));
    m_needleIndexs.erase(found);
    return true;
}

bool MemNeedleMap::AscendingVisit(NeedleMap::VisitCallback callback)
{
    if (callback == nullptr) {
        return false;
    }

    for (const auto &value : m_needleDatas) {
        if (!callback(value)) {
            return false;
        }
    }
    return true;
}

void MemNeedleMap::Close()
{

}

bool MemNeedleMap::WriteTo(std::ostream &os)
{
    if (!os) {
        return false;
    }

    return AscendingVisit([&](NeedleValue value) {
        ByteBuffer buffer = value.ToBytes();
        if (os.write(buffer.data(), static_cast<std::streamsize>(buffer.size()))) {
            return true;
        }
        return false;
    });
}

bool MemNeedleMap::ReadFrom(std::istream &is)
{
    if (!is) {
        return false;
    }

    ByteBuffer buffer(NEEDLE_ID_SIZE + NEEDLE_SIZE_SIZE + NEEDLE_OFFSET_SIZE);
    while (is.read(buffer.data(), static_cast<std::streamsize>(buffer.size()))) {
        NeedleValue value;
        value.ReadBytes(buffer);
        Set(value.id(), value.offset(), value.size());
    }

    return true;
}

bool MemNeedleMap::SaveIndexToFile(const std::string &filepath)
{
    std::ofstream ofs(filepath.c_str(), std::ios_base::out  | std::ios_base::binary);
    if (!ofs) {
        return false;
    }

    return WriteTo(ofs);
}

bool MemNeedleMap::LoadIndexFromFile(const std::string &filepath)
{
    std::ifstream ifs(filepath.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!ifs) {
        return false;
    }

    return ReadFrom(ifs);
}

}
