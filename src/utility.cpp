#include "superblock/utility.h"
#include <algorithm>
#include <iostream>

namespace hfs {

ByteBuffer UInt32ToBytes(uint32_t val)
{
    char *valBytes = reinterpret_cast<char*>(&val);
    return ByteBuffer(valBytes, sizeof(val));
}

uint32_t BytesToUInt32(ByteBuffer buffer)
{
    const uint32_t *valPtr = reinterpret_cast<const uint32_t *>(buffer.data());
    if (valPtr == nullptr) {
        return 0;
    }
    return *valPtr;
}

ByteBuffer Int64ToBytes(int64_t val)
{
    char *valBytes = reinterpret_cast<char*>(&val);
    return ByteBuffer(valBytes, sizeof(val));
}

int64_t BytesToInt64(ByteBuffer buffer)
{
    const int64_t *valPtr = reinterpret_cast<const int64_t *>(buffer.data());
    if (valPtr == nullptr) {
        return 0;
    }
    return *valPtr;
}

uint64_t PaddingLength(uint64_t size, uint64_t pad)
{
    uint64_t r = pad - (size % pad);
    if (r == pad) {
        return size;
    }

    return size + r;
}

ByteBuffer BytesToBase64(ByteBuffer buffer)
{
    const char *base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    ByteBuffer base64Buf;

    size_t bufferSize = buffer.size();
    size_t i = 0;
    for (i = 0; i + 3 <= bufferSize; i += 3) {
        uint32_t mask = uint32_t(buffer[i]) |
                        (uint32_t(buffer[i+1]) << 8) |
                        (uint32_t(buffer[i+2]) << 16);
        base64Buf.append(base64Table[(mask & 0x3f)]);
        base64Buf.append(base64Table[((mask >> 6) & 0x3f)]);
        base64Buf.append(base64Table[((mask >> 12) & 0x3f)]);
        base64Buf.append(base64Table[((mask >> 18) & 0x3f)]);
    }

    if (i < bufferSize) {
        uint64_t tail = bufferSize - i;
        if (tail == 1) {
            uint32_t mask = uint32_t(buffer[i]);
            base64Buf.append(base64Table[(mask & 0x3f)]);
            base64Buf.append(base64Table[((mask >> 6) & 0x3f)]);
            base64Buf.append("==", 2);
        } else {
            uint32_t mask = uint32_t(buffer[i]) |
                            (uint32_t(buffer[i+1]) << 8);
            base64Buf.append(base64Table[(mask & 0x3f)]);
            base64Buf.append(base64Table[((mask >> 6) & 0x3f)]);
            base64Buf.append(base64Table[((mask >> 12) & 0x3f)]);
            base64Buf.append('=');
        }
    }

    return base64Buf;
}

uint32_t AsciiToUInt32(char c) {
    if (c >= 'A' && c <= 'Z') {
        return static_cast<uint32_t>(c - 'A');
    } else if (c >= 'a' && c <= 'z') {
        return static_cast<uint32_t>(c - 'a' + 26);
    } else if (c >= '0' && c <= '9') {
        return static_cast<uint32_t>(c - '0' + 26 * 2);
    } else if (c == '+') {
        return 26 * 2 + 10;
    } else if (c == '/') {
        return 26 * 2 + 10 + 1;
    }
    return 0;
 }

ByteBuffer Base64ToBytes(ByteBuffer buffer)
{

    size_t size = buffer.size();
    if (size == 0 || size % 4 != 0) {
        return ByteBuffer();
    }

    ByteBuffer base64Buf;
    for (size_t index = 0; index + 4 < size; index += 4) {
        uint32_t mask = AsciiToUInt32(buffer[index]) |
                        (AsciiToUInt32(buffer[index + 1]) << 6) |
                        (AsciiToUInt32(buffer[index + 2]) << 12) |
                        (AsciiToUInt32(buffer[index + 3]) << 18);

        base64Buf.append(static_cast<char>(mask & 0xff));
        base64Buf.append(static_cast<char>((mask >> 8)  & 0xff));
        base64Buf.append(static_cast<char>((mask >> 16) & 0xff));
   }

   if (buffer[size - 1] == '=' && buffer[size - 2] == '=') {
       uint32_t mask = AsciiToUInt32(buffer[size - 4]) |
                       (AsciiToUInt32(buffer[size - 3]) << 6);
       base64Buf.append(static_cast<char>(mask & 0xff));
   } else if (buffer[size - 1] == '=') {
       uint32_t mask = AsciiToUInt32(buffer[size - 4]) |
                       (AsciiToUInt32(buffer[size - 3]) << 6) |
                       (AsciiToUInt32(buffer[size - 2]) << 12);

       base64Buf.append(static_cast<char>(mask & 0xff));
       base64Buf.append(static_cast<char>((mask >> 8)  & 0xff));
   } else {
       uint32_t mask = AsciiToUInt32(buffer[size - 4]) |
                       (AsciiToUInt32(buffer[size - 3]) << 6) |
                       (AsciiToUInt32(buffer[size - 2]) << 12) |
                       (AsciiToUInt32(buffer[size - 1]) << 18);

       base64Buf.append(static_cast<char>(mask & 0xff));
       base64Buf.append(static_cast<char>((mask >> 8)  & 0xff));
       base64Buf.append(static_cast<char>((mask >> 16) & 0xff));
   }

   return base64Buf;
}

}
