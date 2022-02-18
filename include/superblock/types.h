#ifndef TYPES_H
#define TYPES_H
#include <cstdint>
#include <string>
#include "bytebuffer.h"

namespace hfs {

typedef uint64_t NeedleID;
ByteBuffer NeedleIDToReadable(NeedleID id);
ByteBuffer NeedleIDToBytes(NeedleID id);
NeedleID BytesToNeedleID(ByteBuffer id);
NeedleID NewNeedleID();


const int NEEDLE_OFFSET_SIZE = sizeof(uint64_t);
const int NEEDLE_ID_SIZE = sizeof(NeedleID);
const int NEEDLE_FLAGS_SIZE = sizeof(uint8_t);
const int NEEDLE_SIZE_SIZE = sizeof(uint32_t);
const int NEEDLE_ACTUAL_SIZE_SIZE = sizeof(uint32_t);
const int NEEDLE_HEAD_SIZE = NEEDLE_ACTUAL_SIZE_SIZE + NEEDLE_ID_SIZE + NEEDLE_FLAGS_SIZE + NEEDLE_SIZE_SIZE;
const int NEEDLE_OFFSET_TO_FLASG = NEEDLE_ID_SIZE + NEEDLE_ACTUAL_SIZE_SIZE;
}

#endif // TYPES_H
