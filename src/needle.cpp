#include "superblock/needle.h"
#include "superblock/utility.h"
#include <iostream>

namespace hfs {

Needle::Ptr Needle::Create(ByteBuffer data)
{
    Ptr ptr(new Needle());
    ptr->m_id = NewNeedleID();
    ptr->m_data = data;
    ptr->m_size = static_cast<uint32_t>(data.size());
    ptr->m_crc  = CRC::fromData(data);
    return ptr;
}

Needle::Ptr Needle::CreateEmpty()
{
    return Ptr(new Needle());
}

Needle::Needle() : m_id(0), m_flags(0), m_size(0), m_padding(nullptr)
{

}

Needle::~Needle()
{

}

bool Needle::UpdateFlags(BackendStorage *storage, int64_t off, uint8_t flags)
{
    if (storage == nullptr) {
        return false;
    }

    m_flags = flags;

    off += NEEDLE_OFFSET_TO_FLASG;

    if (storage->WriteAt(ByteBuffer(reinterpret_cast<char*>(&flags), 1), off) != 1) {
        return  false;
    }
    return true;
}

bool Needle::Append(BackendStorage *storage, int64_t &off, uint32_t &size)
{
    if (storage == nullptr) {
        return false;
    }

    ByteBuffer buffer = PackToBytes();
    int64_t    offset = storage->Size();

    off               = offset;
    size              = static_cast<uint32_t>(buffer.size());
    if (storage->WriteAt(buffer, offset) == -1) {
        return false;
    }

    return true;
}

bool Needle::Read(BackendStorage *storage, int64_t off, uint32_t size)
{
    if (storage == nullptr || off < 0 || size == 0) {
        return false;
    }

    ByteBuffer bytes(size);
    if (storage->ReadAt(bytes, off) != size) {
        return false;
    }

    return ReadFromBytes(bytes, size , off);
}

bool Needle::ReadHeader(BackendStorage *storage, int64_t off, uint32_t size)
{
    (void)size;

    ByteBuffer buffer(NEEDLE_HEAD_SIZE);
    if (storage->ReadAt(buffer, off) != NEEDLE_HEAD_SIZE) {
        return false;
    }

    return ParseNeedleHeader(buffer);
}

bool Needle::ReadBody(BackendStorage *storage, int64_t off, uint32_t size)
{
    uint32_t bodySize = size - NEEDLE_HEAD_SIZE;
    ByteBuffer bytes(bodySize);
    if (storage->ReadAt(bytes, off + NEEDLE_HEAD_SIZE) != bodySize) {
        return false;
    }

    return ParseNeedleBody(bytes);
}

bool Needle::ReadFromBytes(ByteBuffer bytes, uint32_t size, int64_t off)
{
    (void)off;

    if (bytes.size() < size) {
        return false;
    }

    if (!ParseNeedleHeader(bytes(0, NEEDLE_HEAD_SIZE)) || m_size + NEEDLE_HEAD_SIZE + CRC::DATA_SIZE > size) {
        return false;
    }

    uint64_t NEEDLE_HEAD_AND_DATA_SIZE = NEEDLE_HEAD_SIZE + m_size;
    m_data  = bytes(NEEDLE_HEAD_SIZE, NEEDLE_HEAD_AND_DATA_SIZE);
    m_crc   = CRC::fromBytes(bytes(NEEDLE_HEAD_AND_DATA_SIZE, NEEDLE_HEAD_AND_DATA_SIZE + CRC::DATA_SIZE));
    CRC crc = CRC::fromData(m_data);

    if (crc != m_crc) {
        return false;
    }

    return true;
}

bool Needle::ParseNeedleHeader(ByteBuffer bytes)
{
    if (bytes.size() < NEEDLE_HEAD_SIZE) {
        return false;
    }

    uint64_t offset = 0;
    m_id = BytesToNeedleID(bytes(0, NEEDLE_ID_SIZE));
    offset += NEEDLE_ID_SIZE;

    m_actualSize = BytesToUInt32(bytes(offset, offset + NEEDLE_ACTUAL_SIZE_SIZE));
    offset += NEEDLE_ACTUAL_SIZE_SIZE;

    m_flags = static_cast<uint8_t>(bytes[offset]);
    offset += NEEDLE_FLAGS_SIZE;

    m_size  = BytesToUInt32(bytes(offset, offset + NEEDLE_SIZE_SIZE));
    return true;
}

bool Needle::ParseNeedleBody(ByteBuffer bytes)
{
    if (bytes.size() < m_size + CRC::DATA_SIZE) {
         return false;
    }

    m_data  = bytes(0, m_size);
    m_crc   = CRC::fromBytes(bytes(m_size, m_size + CRC::DATA_SIZE));
    CRC crc = CRC::fromData(m_data);

    if (crc != m_crc) {
        return false;
    }
    return true;
}

ByteBuffer Needle::PackToBytes() const
{
    uint64_t byteSize = NEEDLE_HEAD_SIZE + m_data.size() + CRC::DATA_SIZE;
    byteSize     = PaddingLength(byteSize, PADDING_SIZE);
    m_actualSize = static_cast<uint32_t>(byteSize);

    ByteBuffer bytes(byteSize);

    uint64_t offset = 0;
    bytes(offset, offset + NEEDLE_ID_SIZE).assign(NeedleIDToBytes(m_id)); offset += NEEDLE_ID_SIZE;
    bytes(offset, offset + NEEDLE_ACTUAL_SIZE_SIZE).assign(UInt32ToBytes(m_actualSize)); offset += NEEDLE_ACTUAL_SIZE_SIZE;
    bytes(offset, offset + NEEDLE_FLAGS_SIZE).assign(reinterpret_cast<const char*>(&m_flags), NEEDLE_FLAGS_SIZE); offset += NEEDLE_FLAGS_SIZE;
    bytes(offset, offset + NEEDLE_SIZE_SIZE).assign(UInt32ToBytes(m_size)); offset += NEEDLE_SIZE_SIZE;
    bytes(offset, offset + m_data.size()).assign(m_data); offset += m_data.size();
    bytes(offset, offset + CRC::DATA_SIZE).assign(m_crc.toBytes());

    return bytes;
}

ByteBuffer Needle::data() const
{
    return m_data;
}

NeedleID Needle::id() const
{
    return m_id;
}

CRC Needle::crc() const
{
    return m_crc;
}

uint32_t Needle::size() const
{
    return m_size;
}

uint32_t Needle::actualSize() const
{
    return m_actualSize;
}

uint8_t Needle::flags() const
{
    return m_flags;
}

}
