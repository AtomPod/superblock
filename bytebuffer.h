#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H
#include <memory>
#include <string>

namespace hfs {

class ByteBuffer
{
public:
    typedef std::string RawDataType;
    typedef std::shared_ptr< RawDataType > DataType;
public:
    ByteBuffer(uint64_t size = 0);
    ByteBuffer(const char *data);
    ByteBuffer(const char *data, size_t len);
    ~ByteBuffer();
public:
    char  operator[](const size_t &index) const;
    char &operator[](const size_t &index);
    ByteBuffer operator()(uint64_t sindex, uint64_t eindex);
public:
    char*       data();
    const char* data() const;
    size_t size() const;
    size_t append(char c);
    size_t append(const char *data);
    size_t append(const ByteBuffer &buffer);
    size_t append(const char *data, size_t size);
    size_t assign(const ByteBuffer &buffer);
    size_t assign(const char *data, size_t size);
    size_t assign(const char *data);
    bool   isOwner() const { return (m_data == nullptr || size() == m_data->size() ); }
protected:
    uint64_t startIndex() const;
    uint64_t endIndex() const;
protected:
    ByteBuffer(const ByteBuffer &other, uint64_t sindex, uint64_t eindex);
private:
    DataType m_data;
    uint64_t m_startIndex;
    uint64_t m_endIndex;
};

}

#endif // BYTEBUFFER_H
