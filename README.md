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
hfs::NeedleID id = superBlock->Put(val.data());
```

### 3. 获取数据
```C++
hfs::ByteBuffer buffer = superBlock->Get(id);
std::cout << "get:" << buffer << '\n'; 
  //(注：需要自己实现std::ostream &operator<<(std::ostream& os, const hfs::ByteBuffer &buf))
```

### 4. 删除数据
```C++
std::string val = "hello super block";
hfs::NeedleID id = superBlock->Put(val.data());
bool success = superBlock->Delete(id);
std::cout << "delete: " << success << '\n';
```

## 功能支持
- [x] 文件储存
- [x] 索引文件丢失后自动创建修复
- [ ] 多线程支持
- [ ] 网络支持
- [ ] 自动整理储存文件

## Benchmark
- ### 测试库
	- #### google branchmark
- ### Put操作
```
Run on (4 X 3955.76 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 3072 KiB (x1)
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
BM_put/1            21739 ns        20927 ns        37333
BM_put/10          214844 ns       209961 ns         3200
BM_put/100        2146679 ns      2099609 ns          320
BM_put/1000      21530862 ns     21599265 ns           34
BM_put/10000    216231033 ns    218750000 ns            3
BM_put/100000  2190777800 ns   2187500000 ns            1
BM_put/1000000 22808579100 ns 22265625000 ns            1
```

- ### Get操作
```
Run on (4 X 3923.69 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 3072 KiB (x1)
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
BM_get/1             7103 ns         6975 ns        89600
BM_get/10           69641 ns        68011 ns         8960
BM_get/100         584434 ns       578125 ns         1000
BM_get/1000       5844033 ns      5555556 ns           90
BM_get/10000     58278182 ns     56818182 ns           11
BM_get/100000   585089000 ns    593750000 ns            1
BM_get/1000000 5865646200 ns   5812500000 ns            1
```

## 代码实现参考
- seaweedfs https://github.com/chrislusf/seaweedfs