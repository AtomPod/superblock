#ifndef NEEDLEMAP_H
#define NEEDLEMAP_H

#include "needlevalue.h"
#include <functional>
#include <iostream>

namespace hfs {

class NeedleMap
{
public:
    typedef std::function<bool(NeedleValue)> VisitCallback;
public:
    NeedleMap();
    virtual ~NeedleMap();
public:
    virtual bool Set(NeedleID id, int64_t off, uint32_t size) = 0;
    virtual NeedleValue Get(NeedleID id) = 0;
    virtual bool Delete(NeedleID id) = 0;
    virtual bool AscendingVisit(VisitCallback callback) = 0;
    virtual void Close() = 0;
    virtual bool WriteTo(std::ostream &os) = 0;
    virtual bool ReadFrom(std::istream &is) = 0;
};

}

#endif // NEEDLEMAP_H
