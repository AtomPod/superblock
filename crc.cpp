#include "crc.h"
#include "utility.h"
#include <iostream>

namespace hfs {

CRC::Table CRC::DefaultTable() {
    static Table defaultTable(nullptr);
    return defaultTable;
}

CRC::CRC(uint32_t crc, Table table) : m_crc(crc), m_table(table)
{

}


CRC CRC::fromData(ByteBuffer data)
{
    CRC crc;
    crc.update(data);
    return crc;
}

CRC CRC::fromBytes(ByteBuffer bytes)
{
    return CRC(BytesToUInt32(bytes));
}

void CRC::update(ByteBuffer data)
{
    m_crc ^= 0xFFFFFFFF;
    for (size_t i = 0; i != data.size(); ++i) {
        m_crc = m_table.at((m_crc ^ static_cast<uint8_t>(data[i])) & 0xff) ^ (m_crc >> 8);
    }
    m_crc ^= 0xFFFFFFFF;
}

ByteBuffer CRC::toBytes() const
{
    return UInt32ToBytes(m_crc);
}

CRC::Table::Table(DataTable data) : m_data(data) {
    if (data == nullptr) {

        generateData();
    }
}

CRC::Table::~Table() {

}

uint32_t CRC::Table::at(uint64_t index) const {
    return m_data->at(index);
}

uint32_t &CRC::Table::at(uint64_t index) {
    return  m_data->at(index);
}

void CRC::Table::generateData()
{
    m_data = std::make_shared< std::array<uint32_t, 256> >();
    uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t c = i;
        for (size_t j = 0; j < 8; j++)
        {
            if (c & 1) {
                c = polynomial ^ (c >> 1);
            }
            else {
                c >>= 1;
            }
        }
        m_data->at(i) = c;
    }
}

}
