#include "needlevalue.h"

namespace hfs {

NeedleValue::NeedleValue(NeedleID id, int64_t off, uint32_t size) : m_id(id), m_offset(off), m_size(size)
{

}

NeedleValue::~NeedleValue()
{

}

NeedleID NeedleValue::id() const
{
    return m_id;
}

void NeedleValue::setId(const NeedleID &id)
{
    m_id = id;
}

int64_t NeedleValue::offset() const
{
    return m_offset;
}

void NeedleValue::setOffset(const int64_t &offset)
{
    m_offset = offset;
}

uint32_t NeedleValue::size() const
{
    return m_size;
}

void NeedleValue::setSize(const uint32_t &size)
{
    m_size = size;
}

ByteBuffer NeedleValue::ToBytes() const
{
    ByteBuffer buffer(NEEDLE_ID_SIZE + NEEDLE_OFFSET_SIZE + NEEDLE_SIZE_SIZE);
    buffer(0, NEEDLE_ID_SIZE).assign(NeedleIDToBytes(m_id));
    buffer(NEEDLE_ID_SIZE, NEEDLE_ID_SIZE + NEEDLE_OFFSET_SIZE).assign(Int64ToBytes(m_offset));
    buffer(NEEDLE_ID_SIZE + NEEDLE_OFFSET_SIZE, NEEDLE_ID_SIZE + NEEDLE_OFFSET_SIZE + NEEDLE_SIZE_SIZE).assign(UInt32ToBytes(m_size));
    return buffer;
}

bool NeedleValue::ReadBytes(ByteBuffer buffer)
{
    size_t minSize = NEEDLE_ID_SIZE + NEEDLE_OFFSET_SIZE + NEEDLE_SIZE_SIZE;
    if (minSize > buffer.size()) {
        return false;
    }
    m_id = BytesToNeedleID(buffer(0, NEEDLE_ID_SIZE));
    m_offset = BytesToInt64(buffer(NEEDLE_ID_SIZE, NEEDLE_ID_SIZE + NEEDLE_OFFSET_SIZE));
    m_size = BytesToUInt32(buffer(NEEDLE_ID_SIZE + NEEDLE_OFFSET_SIZE, NEEDLE_ID_SIZE + NEEDLE_OFFSET_SIZE + NEEDLE_SIZE_SIZE));
    return true;
}

}
