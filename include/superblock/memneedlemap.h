#ifndef MEMNEEDLEMAP_H
#define MEMNEEDLEMAP_H
#include "needlemap.h"
#include <deque>
#include <map>
#include <fstream>

namespace hfs {

class MemNeedleMap : public NeedleMap
{
    static const uint32_t DELETED_FILE_SIZE = (uint32_t)(-1);
  public:
    enum ActionResult {
      ActionResult_Ok,
      ActionResult_NotFound,
      ActionResult_DestinationNotFound
    };

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
    void Initialize(const std::string &indexFile);
    NeedleValue SetRaw(NeedleID id, int64_t off, uint32_t size);
    void DeleteRaw(NeedleID id);
    bool WritePutActionToFile(const NeedleValue &value);
    bool WriteDeleteActionToFile(NeedleValue &value);
    bool FindAndRemoveIndex(const NeedleID &id, NeedleValue &value);

  private:
    std::map<NeedleID, NeedleValue> m_needleIndexs;
    std::fstream m_indexFile;
    std::string m_indexFilePath;
};

}


#endif // MEMNEEDLEMAP_H
