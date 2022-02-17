#ifndef MEMNEEDLEMAP_H
#define MEMNEEDLEMAP_H
#include "needlemap.h"
#include <deque>
#include <map>

namespace hfs {

class MemNeedleMap : public NeedleMap
{
public:
    MemNeedleMap();
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
private:
    std::map<NeedleID, uint64_t> m_needleIndexs;
    std::deque<NeedleValue> m_needleDatas;
};

}


#endif // MEMNEEDLEMAP_H
