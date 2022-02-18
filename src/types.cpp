#include "superblock/types.h"
#include "superblock/utility.h"
#include <random>
#include <time.h>

namespace hfs {

static std::mt19937_64 defaultRandomEngine(static_cast<uint64_t>(time(nullptr)));
static std::uniform_int_distribution<uint64_t> defaultRandom;

ByteBuffer NeedleIDToBytes(NeedleID id)
{
    char* idBytes = reinterpret_cast<char*>(&id);
    return ByteBuffer(idBytes, sizeof(id));
}

NeedleID BytesToNeedleID(ByteBuffer id)
{
    const char* idBytes = id.data();
    const NeedleID* needleID = reinterpret_cast<const NeedleID*>(idBytes);
    if (needleID == nullptr) {
        return NeedleID();
    }
    return *needleID;
}

NeedleID NewNeedleID()
{
    return NeedleID(defaultRandom(defaultRandomEngine));
}

ByteBuffer NeedleIDToReadable(NeedleID id)
{
    return BytesToBase64(NeedleIDToBytes(id));
}

}
