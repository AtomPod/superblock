#include "backendstoragefile.h"
#include <iostream>

namespace hfs {

BackendStorageFile::BackendStorageFile()
{

}

BackendStorageFile::~BackendStorageFile()
{

}

bool BackendStorageFile::Open(const std::string filepath)
{
    m_file.open(filepath.c_str(), std::ios_base::in | std::ios_base::binary);
    if(!m_file.is_open()) {
        m_file.open(filepath.c_str(), std::ios_base::out | std::ios_base::in | std::ios_base::binary | std::ios_base::trunc);
    } else {
        m_file.close();
        m_file.open(filepath.c_str(), std::ios_base::out | std::ios_base::in | std::ios_base::binary);
    }
    m_filepath = filepath;
    return m_file.is_open();
}

int64_t BackendStorageFile::ReadAt(ByteBuffer buf, int64_t off)
{
    if (!m_file.is_open()) {
        return -1;
    }

    if (!m_file.good()) {
        m_file.clear();
    }

    m_file.seekg(off, std::ios_base::beg);
    m_file.read(buf.data(), static_cast<std::streamsize>(buf.size()));
    return m_file.gcount();
}

int64_t BackendStorageFile::WriteAt(ByteBuffer buf, int64_t off)
{
    if (!m_file.is_open()) {
        return -1;
    }

    if (!m_file.good()) {
        m_file.clear();
    }

    if (!m_file.seekp(off, std::ios_base::beg)) {
        return -1;
    }

    m_file.write(buf.data(), static_cast<std::streamsize>(buf.size()));
    return buf.size();
}

int64_t BackendStorageFile::Append(ByteBuffer buf)
{
    if (!m_file.is_open()) {
        return -1;
    }

    if (!m_file.good()) {
        m_file.clear();
    }

    if (!m_file.seekp(0, std::ios_base::end)) {
        return -1;
    }

    m_file.write(buf.data(), static_cast<std::streamsize>(buf.size()));
    return buf.size();
}

int64_t BackendStorageFile::Size()
{
    if (!m_file.is_open()) {
        return -1;
    }

    m_file.clear();
    m_file.seekg(0, std::ios_base::end);
    int64_t size = m_file.tellg();
    return size;
}

void BackendStorageFile::Truncate(int64_t size)
{
    std::string newFilepath = m_filepath + ".tmp";
    BackendStorageFile newStorageFile;
    if (!newStorageFile.Open(newFilepath)) {
        return;
    }

    ByteBuffer bytesBuffer(4096);
    int64_t readSize = 0;
    int64_t off = 0;

    while ((readSize = ReadAt(bytesBuffer, off)) != -1 && size > off) {
        if (off + readSize > size) {
            readSize = size - off;
        }

        newStorageFile.Append(bytesBuffer(0, static_cast<uint64_t>(readSize)));
        off += readSize;
    }

    Close();
    newStorageFile.Close();

    std::remove(m_filepath.c_str());
    std::rename(newFilepath.c_str(), m_filepath.c_str());

    Open(m_filepath);
}

void BackendStorageFile::Close()
{
    if (!m_file.is_open()) {
        return;
    }
    m_file.close();
}


}
