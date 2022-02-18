#ifndef CRC_H
#define CRC_H
#include <cstdint>
#include <string>
#include <memory>
#include <array>
#include "bytebuffer.h"

namespace hfs {

class CRC
{
public:
    class Table
    {
    public:
        typedef std::shared_ptr<std::array<uint32_t, 256>> DataTable;
    public:
        Table(DataTable data);
        ~Table();
    public:
        uint32_t  at(uint64_t index) const;
        uint32_t &at(uint64_t index);
    protected:
        void generateData();
    private:
        DataTable m_data;
    };
public:
    static Table DefaultTable();
    static const int DATA_SIZE = sizeof(uint32_t);
public:
    CRC(uint32_t crc = 0, Table table = DefaultTable());
public:
    static CRC fromData(ByteBuffer data);
    static CRC fromBytes(ByteBuffer bytes);

    void update(ByteBuffer data);
    ByteBuffer toBytes() const;
public:
    bool operator==(const CRC& other) const { return m_crc == other.m_crc; }
    bool operator!=(const CRC& other) const { return !(*this == other); }
    operator uint32_t() const { return m_crc; }
private:
    uint32_t m_crc;
    Table    m_table;
};

}

#endif // CRC_H
