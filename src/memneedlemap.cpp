#include "superblock/memneedlemap.h"
#include <fstream>
#include <chrono>
#include <filesystem>

namespace hfs {

MemNeedleMap::MemNeedleMap(const std::string &indexFile)
{
  Initialize(indexFile);
  MemNeedleMap::ReadFrom(m_indexFile);
}

MemNeedleMap::~MemNeedleMap()
{

}

bool MemNeedleMap::Set(NeedleID id, int64_t off, uint32_t size)
{
  return WritePutActionToFile(SetRaw(id, off, size));
}

NeedleValue MemNeedleMap::Get(NeedleID id)
{
  auto found = m_needleIndexs.find(id);
  if (found == m_needleIndexs.end()) {
    return NeedleValue();
  }

  return found->second;
}

bool MemNeedleMap::Delete(NeedleID id)
{
  NeedleValue value;
  if (!FindAndRemoveIndex(id, value)) {
    return false;
  }
  return WriteDeleteActionToFile(value);
}

bool MemNeedleMap::AscendingVisit(NeedleMap::VisitCallback callback)
{
  if (callback == nullptr) {
    return false;
  }

  for (const auto &value : m_needleIndexs) {
    if (!callback(value.second)) {
      return false;
    }
  }
  return true;
}

void MemNeedleMap::Close()
{
  if (m_indexFile.is_open()) {
    m_indexFile.flush();
    m_indexFile.close();
  }
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

    if (value.size() != DELETED_FILE_SIZE) {
      SetRaw(value.id(), value.offset(), value.size());
    } else {
      DeleteRaw(value.id());
    }
  }
  is.clear();
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

void MemNeedleMap::Initialize(const std::string &filepath)
{
  m_indexFile.open(filepath.c_str(), std::ios_base::in | std::ios_base::binary);
  if(!m_indexFile.is_open()) {
      m_indexFile.open(filepath.c_str(), std::ios_base::out | std::ios_base::in | std::ios_base::binary | std::ios_base::trunc);
  } else {
      m_indexFile.close();
      m_indexFile.open(filepath.c_str(), std::ios_base::out | std::ios_base::in | std::ios_base::binary);
  }
  m_indexFilePath = filepath;
}

NeedleValue MemNeedleMap::SetRaw(NeedleID id, int64_t off, uint32_t size)
{
  NeedleValue value(id, off, size);
  m_needleIndexs.insert(std::make_pair(id, value));
  return value;
}

void MemNeedleMap::DeleteRaw(NeedleID id)
{
  m_needleIndexs.erase(id);
}

bool MemNeedleMap::WritePutActionToFile(const NeedleValue &value)
{
  ByteBuffer buffer = value.ToBytes();
  if (!m_indexFile.seekp(0, std::ios_base::end)) {
    return false;
  }

  if (!m_indexFile.write(buffer.data(), buffer.size())) {
    return false;
  }
  return true;
}

bool MemNeedleMap::FindAndRemoveIndex(const NeedleID &id, NeedleValue &value)
{
  auto found = m_needleIndexs.find(id);
  if (found == m_needleIndexs.end()) {
    return false;
  }

  value = found->second;
  m_needleIndexs.erase(found);
  return true;
}

bool MemNeedleMap::WriteDeleteActionToFile(NeedleValue &value)
{
  value.setSize(DELETED_FILE_SIZE);
  ByteBuffer buffer = value.ToBytes();

  m_indexFile.seekp(0, std::ios_base::end);
  if (!m_indexFile.write(buffer.data(), buffer.size())) {
    return false;
  }

  return true;
}

}
