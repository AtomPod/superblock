# superblock
一个参考Haystack储存思路实现的简化版单线程C++储存库，支持添加、删除、获取，支持自定义储存方式。

## 需求
- C++11 或更高

## 示例

### 1. 创建储存
```C++11
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
```

### 2. 添加数据
```C++
std::string val = "hello super block";
hfs::NeedleID id = superBlock.Put(val.data());
```

### 3. 获取数据
```C++
  hfs::ByteBuffer buffer = superBlock->Get(id);
  std::cout << "get:" << buffer << '\n'; 
  //(注：需要自己实现std::ostream &operator<<(std::ostream& os, const hfs::ByteBuffer &buf))
```

### 4. 删除数据
```C++
void deleteData(hfs::SuperBlock *superBlock) {
  std::string val = "hello super block";
  hfs::NeedleID id = superBlock->Put(val.data());
  bool success = superBlock->Delete(id);
  std::cout << "delete: " << success << '\n';
}
```

## 功能支持
- [x] 文件储存
- [x] 索引文件丢失后自动创建修复
- [ ] 多线程支持
- [ ] 网络支持
- [ ] 自动整理储存文件