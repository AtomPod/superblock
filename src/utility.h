#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <cstdint>
#include "bytebuffer.h"

namespace hfs {

const uint64_t PADDING_SIZE = sizeof(intptr_t);

int64_t     BytesToInt64(ByteBuffer buffer);
ByteBuffer  Int64ToBytes(int64_t val);
ByteBuffer  UInt32ToBytes(uint32_t val);
uint32_t    BytesToUInt32(ByteBuffer buffer);
uint64_t    PaddingLength(uint64_t size, uint64_t pad);
ByteBuffer  BytesToBase64(ByteBuffer buffer);
ByteBuffer  Base64ToBytes(ByteBuffer buffer);

}

#endif // UTILITY_H
