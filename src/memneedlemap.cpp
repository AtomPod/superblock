#include "superblock/memneedlemap.h"
#include <fstream>
#include <chrono>

namespace hfs {

MemNeedleMap::MemNeedleMap(const std::string &indexFile)
{
  initialize(indexFile);
  MemNeedleMap::ReadFrom(m_indexFile);
}

MemNeedleMap::~MemNeedleMap()
{

}

bool MemNeedleMap::Set(NeedleID id, int64_t off, uint32_t size)
{
  setRaw(id, off, size);
  return writePutActionToFile(m_needleDatas.back());
}

NeedleValue MemNeedleMap::Get(NeedleID id)
{
  auto found = m_needleIndexs.find(id);
  if (found == m_needleIndexs.end()) {
    return NeedleValue();
  }

  return m_needleDatas[found->second];
}

bool MemNeedleMap::Delete(NeedleID id)
{
  uint64_t from = 0;
  uint64_t to = 0;

  auto result = swapNeedIdWithLast(id, from, to);

  if (result != ActionResult_Ok)
    return false;

  m_needleDatas.pop_back();
  m_needleIndexs.erase(id);

  return writeDeleteActionToFile(from ,to);
}

bool MemNeedleMap::AscendingVisit(NeedleMap::VisitCallback callback)
{
  if (callback == nullptr) {
    return false;
  }

  for (const auto &value : m_needleDatas) {
    if (!callback(value)) {
      return false;
    }
  }
  return true;
}

void MemNeedleMap::Close()
{

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
    setRaw(value.id(), value.offset(), value.size());
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

void MemNeedleMap::initialize(const std::string &filepath)
{
  m_indexFile.open(filepath.c_str(), std::ios_base::in | std::ios_base::binary);
  if(!m_indexFile.is_open()) {
      m_indexFile.open(filepath.c_str(), std::ios_base::out | std::ios_base::in | std::ios_base::binary | std::ios_base::trunc);
  } else {
      m_indexFile.close();
      m_indexFile.open(filepath.c_str(), std::ios_base::out | std::ios_base::in | std::ios_base::binary);
  }

}

bool MemNeedleMap::setRaw(NeedleID id, int64_t off, uint32_t size)
{
  m_needleDatas.emplace_back(id, off, size);
  m_needleIndexs.insert(std::make_pair(id, m_needleDatas.size() - 1));
  return true;
}

bool MemNeedleMap::writePutActionToFile(const NeedleValue &value)
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

bool MemNeedleMap::writeDeleteActionToFile(const uint64_t &from, const uint64_t &to)
{
  const NeedleValue &value = m_needleDatas[from];
  ByteBuffer buffer = value.ToBytes();
  uint64_t off = from * buffer.size();

  m_indexFile.seekp(off, std::ios_base::beg);
  if (!m_indexFile.write(buffer.data(), buffer.size())) {
    return false;
  }

  return true;
}

MemNeedleMap::ActionResult MemNeedleMap::swapNeedIdWithLast(const NeedleID &id, uint64_t &from, uint64_t &to)
{
  auto found = m_needleIndexs.find(id);
  if (found == m_needleIndexs.end()) {
    return ActionResult_NotFound;
  }

  if (m_needleDatas.size() <= 1) {
    return ActionResult_Ok;
  }

  auto dataIterator = m_needleDatas.begin() + static_cast<int64_t>(found->second);
  if (dataIterator >= m_needleDatas.end())
    return ActionResult_NotFound;

  auto& needValue = *dataIterator;

  auto& lastNeedValue = m_needleDatas.back();
  std::swap(needValue, lastNeedValue);

  auto lastNeedleIndex = m_needleIndexs.find(needValue.id());
  if (lastNeedleIndex == m_needleIndexs.end())
    return ActionResult_DestinationNotFound;

  from = found->second;
  to = lastNeedleIndex->second;
  std::swap(lastNeedleIndex->second, found->second);
  return ActionResult_Ok;
}

}
