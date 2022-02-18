#include <iostream>
#include <chrono>

#include "superblock/needle.h"
#include "superblock/bytebuffer.h"
#include "superblock/backendstoragefile.h"
#include "superblock/memneedlemap.h"
#include "superblock/superblock.h"

using namespace std;

std::ostream &operator<<(std::ostream& os, const hfs::ByteBuffer &buf) {
    os << std::string(buf.data(), buf.size());
    return os;
}

void putData(hfs::SuperBlock *superBlock) {
  for (int i = 0; i < 10; ++i) {
    std::string val = "a12312sdasjjslkdajskldjalksjdklasjkldjaskldjklsajdklsajkldas" + std::to_string(i);
    hfs::NeedleID id = superBlock->Put(val.data());
    std::cout << "put: id = " << id << '\n';
  }
}

void getData(hfs::SuperBlock *superBlock) {
  std::string val = "hello super block";
  hfs::NeedleID id = superBlock->Put(val.data());
  hfs::ByteBuffer buffer = superBlock->Get(id);
  std::cout << "get:" << buffer << '\n';
}

void deleteData(hfs::SuperBlock *superBlock) {
  std::string val = "hello super block";
  hfs::NeedleID id = superBlock->Put(val.data());
  bool success = superBlock->Delete(id);
  std::cout << "delete: " << success << '\n';
}

int main()
{
    hfs::BackendStorageFile *bsf = new hfs::BackendStorageFile();
    if (!bsf->Open("backend.fs")) {
      std::cout << "failed to open backend\n";
      return 0;
    }

    hfs::MemNeedleMap *mnm = new hfs::MemNeedleMap("hfs.index");

    hfs::SuperBlock::Options opts = {};
    opts.needleMap            = mnm;
    opts.backendStorage       = bsf;

    hfs::SuperBlock superBlock(opts);

    putData(&superBlock);
    getData(&superBlock);
    deleteData(&superBlock);

    return 0;
}
