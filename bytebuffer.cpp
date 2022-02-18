#include "bytebuffer.h"
#include <string.h>

namespace hfs {

ByteBuffer::ByteBuffer(uint64_t size) : m_data(nullptr), m_startIndex(0), m_endIndex(0)
{
    if (size != 0) {
        m_data = std::make_shared< RawDataType >(size, '\0');
        m_endIndex = size;
    }
}

ByteBuffer::ByteBuffer(const char *data) : ByteBuffer(data, strlen(data))
{

}

ByteBuffer::ByteBuffer(const char *data, size_t len) : m_data(nullptr), m_startIndex(0), m_endIndex(0)
{
    if (data != nullptr && len > 0) {
        m_data = std::make_shared< RawDataType >(data, len);
        m_endIndex = len;
    }
}

ByteBuffer::ByteBuffer(const ByteBuffer &other, uint64_t sindex, uint64_t eindex) : m_data(nullptr), m_startIndex(0), m_endIndex(0)
{
    if (other.m_data != nullptr) {
        size_t rawSize = other.m_data->size();
        size_t sIndex = other.startIndex() + sindex;
        size_t eIndex = other.startIndex() + eindex;

        if (sIndex >= rawSize || eIndex > rawSize || sIndex >= eIndex) {
            throw  std::range_error("ByteBuffer: start index or end index over size");
        }

        m_data       = other.m_data;
        m_startIndex = sIndex;
        m_endIndex   = eIndex;
    }
}

ByteBuffer::~ByteBuffer()
{

}

char ByteBuffer::operator[](const size_t &index) const
{
    return m_data->at(index + startIndex());
}

char &ByteBuffer::operator[](const size_t &index)
{
    return m_data->at(index + startIndex());
}

ByteBuffer ByteBuffer::operator()(uint64_t sindex, uint64_t eindex) {
    if (eindex <= sindex) {
        throw std::range_error("ByteBuffer: start index >= end index");
    }
    return ByteBuffer(*this, sindex, eindex);
}

char *ByteBuffer::data()
{
    return &m_data->at(m_startIndex);
}

const char *hfs::ByteBuffer::data() const
{
    return m_data->c_str() + m_startIndex;
}

size_t ByteBuffer::size() const
{
    return m_endIndex - m_startIndex;
}

size_t ByteBuffer::append(char c)
{
    return append(&c, 1);
}

size_t ByteBuffer::append(const char *data)
{
    return append(data, strlen(data));
}

size_t ByteBuffer::append(const ByteBuffer &buffer)
{
    return append(buffer.data(), buffer.size());
}

size_t ByteBuffer::append(const char *data, size_t size)
{
    if (data == nullptr || size == 0 || !isOwner()) {
        return 0;
    }

    if (m_data == nullptr) {
        m_data = std::make_shared< RawDataType >();
    }

    m_data->append(data , size);
    m_endIndex += size;
    return size;
}

size_t ByteBuffer::assign(const ByteBuffer &buffer)
{
    return assign(buffer.data(), buffer.size());
}

size_t ByteBuffer::assign(const char *data, size_t size)
{
    if (size > this->size()) {
        size = this->size();
    }

    memcpy(this->data(), data, size);
    return size;
}

size_t ByteBuffer::assign(const char *data)
{
    return assign(data, strlen(data));
}

uint64_t ByteBuffer::startIndex() const
{
    return m_startIndex;
}

uint64_t ByteBuffer::endIndex() const
{
    return m_endIndex;
}



}
