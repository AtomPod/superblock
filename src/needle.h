#ifndef NEEDLE_H
#define NEEDLE_H

#include "types.h"
#include "crc.h"
#include "backendstorage.h"

namespace hfs {

class Needle
{
public:
    typedef std::unique_ptr<Needle> Ptr;
public:
    static Ptr Create(ByteBuffer data);
    static Ptr CreateEmpty();
public:
    Needle();
    ~Needle();

    bool UpdateFlags(BackendStorage *storage, int64_t off, uint8_t flags);
    bool Append(BackendStorage *storage, int64_t &off, uint32_t &size);
    bool Read(BackendStorage *storage, int64_t off, uint32_t size);

    bool ReadHeader(BackendStorage *storage, int64_t off, uint32_t size);
    bool ReadBody(BackendStorage *storage, int64_t off, uint32_t size);

    bool ReadFromBytes(ByteBuffer bytes, uint32_t size, int64_t off);
    bool ParseNeedleHeader(ByteBuffer bytes);
    bool ParseNeedleBody(ByteBuffer bytes);
    ByteBuffer PackToBytes() const;

    ByteBuffer data() const;
    NeedleID id() const;
    CRC crc() const;
    uint32_t size() const;
    uint32_t actualSize() const;
    uint8_t flags() const;

private:
    NeedleID    m_id;
    uint8_t     m_flags;
    mutable     uint32_t    m_actualSize;
    uint32_t    m_size;
    ByteBuffer  m_data;
    CRC         m_crc;
    char       *m_padding;
};

}


#endif // NEEDLE_H
