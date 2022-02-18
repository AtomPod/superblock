#ifndef NEEDLEVALUE_H
#define NEEDLEVALUE_H
#include "bytebuffer.h"
#include "types.h"
#include "utility.h"

namespace hfs {

class NeedleValue
{
public:
    NeedleValue(NeedleID id = NeedleID(), int64_t off = 0, uint32_t size = 0);
    ~NeedleValue();
public:
    NeedleID id() const;
    void setId(const NeedleID &id);

    int64_t offset() const;
    void setOffset(const int64_t &offset);

    uint32_t size() const;
    void setSize(const uint32_t &size);
public:
    ByteBuffer ToBytes() const;
    bool       ReadBytes(ByteBuffer buffer);
public:
    operator bool() const { return m_id != 0 && m_size != 0; }
private:
    NeedleID m_id;
    int64_t  m_offset;
    uint32_t m_size;
};

}

#endif // NEEDLEVALUE_H
