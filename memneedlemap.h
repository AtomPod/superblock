#ifndef MEMNEEDLEMAP_H
#define MEMNEEDLEMAP_H
#include "needlemap.h"
#include <deque>
#include <map>
#include <fstream>

namespace hfs {

class MemNeedleMap : public NeedleMap
{
  public:
    MemNeedleMap(const std::string &indexFile);
    ~MemNeedleMap();
  public:
    virtual bool Set(NeedleID id, int64_t off, uint32_t size);
    virtual NeedleValue Get(NeedleID id);
    virtual bool Delete(NeedleID id);
    virtual bool AscendingVisit(VisitCallback callback);
    virtual void Close();
    virtual bool WriteTo(std::ostream &os);
    virtual bool ReadFrom(std::istream &is);
  public:
    bool SaveIndexToFile(const std::string &filepath);
    bool LoadIndexFromFile(const std::string &filepath);

  protected:
    bool writePutActionToFile(const NeedleValue &value);
    bool writeDeleteActionToFile(const NeedleValue &value);

  private:
    std::map<NeedleID, uint64_t> m_needleIndexs;
    std::deque<NeedleValue> m_needleDatas;
    std::fstream m_indexFile;
};

}


#endif // MEMNEEDLEMAP_H
