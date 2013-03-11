#ifndef FILESTORE_H
#define FILESTORE_H

// *****************************************************************************
// The file store provides a file-backed, expandable memory region with a simple
// allocator.
// A key-value associative structure is also implemented, allowing objects to
// be moved or reallocated without breaking references.
//
// At the lowest level, a fibonacci buddy allocator handles memory in fixed
// sized blocks. Each size is the sum of the two immediately smaller sizes,
// starting as: 8, 16, 24, 40, 64...
// Each block may be split into sub-blocks of the two smaller sizes. The lower
// block is always the larger one.
// 
// The data file grows to sizes from this series as needed by appending blocks
// of size s-1, where s is the current size of the file, advancing the size to
// s+1.
//
//
// -----------------------------------------------------------------------------
// There are 3 ways to refer to an object:
// 1: direct pointer to memory-mapped data. Direct pointers give the most direct
//    access, but can be invalidated by some FileStore operations. Translation
//    back to an id_t or loc_t is a fairly expensive operation that likely won't
//    be implemented.
// 
// 2: a loc_t encodes block information including block size, and byte offset
//    into the file. Allocated blocks are aligned to multiple of 8 bytes from
//    start of file. Locations may also refer to locations within allocated
//    blocks.
// 
// Allocation size field may be ignored, for locations in the interior of an
// allocated block.
// 
// 3: a id_t serves as an abstract handle, allowing objects to be moved around
//    in memory. Object IDs serve as indices into an array of loc_t's,
//    so lookup is cheap. Reverse lookup is expensive and unlikely to be
//    implemented.
// -----------------------------------------------------------------------------
//
// The index file stores number of files, loc_t's of free block lists, loc_t's
// of objects, and other such data. The object location table is at the end of
// the index file, allowing the list to grow freely.
// Operations that move memory blocks around must take care that they properly
// update the index.


#include <stack>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <fstream>

// #include <boost/interprocess/file_mapping.hpp>
// #include <boost/interprocess/mapped_region.hpp>

namespace filestore {


struct MappedFile {
    std::string filePath;
    int fd;
    // boost::interprocess::file_mapping mappedFile;
    // boost::interprocess::mapped_region region;
    void * baseAddr;
    size_t fileSize, mapSize;
    
    MappedFile(const std::string & fpath, size_t fsize, size_t msize);
    
    size_t FileSize() const {return fileSize;}
    size_t MemSize() const {return mapSize;}
    
    // Creates file if necessary, expands to given size if too small.
    // If file exists and is of at least given size, it is simply mapped as-is.
    // If given size is 0, full file is mapped.
    // If already mapped and given map size is <= previous one, maps to same address.
    void Remap(size_t fsize, size_t msize);
    
    void Flush();
};


// template<typename T>
// struct MappedArray {
//     uint64_t size;
//     uint64_t capacity;
//     uint64_t offset;
//     T * raw;
//     size_t set_offset(void * base, uint64_t o) {
//         offset = 0;
//         raw = (T *)((uint8_t*)base + offset);
//     }
// };

// Trivial memory manager
// template<typename T>
// struct PackedBlocks {
//     void * base;
//     struct block_t {
//         uint64_t start;
//         uint64_t size;
//     };
//     std::vector<block_t> records;
//   public:
//     PackedBlocks() {}
//     void resize(int idx, size_t size) {}
// };

typedef uint32_t id_t;
typedef uint64_t loc_t;

std::string SizeToS(size_t s);
std::string LocToS(loc_t loc);
uint64_t BlockBytes(loc_t loc);
static inline uint64_t BlockSize(loc_t loc) {return (loc >> 56) & 0xFF;}
static inline uint64_t FileOffset(loc_t loc) {return loc & ((~0ull) >> 8);}

const size_t kNumAllocSizes = 64;
class FileStore {
  public:
    
  protected:
    struct index_t {
        uint64_t filestoreVersion;
        loc_t freeLists[64];
        uint64_t numObjects;
        uint64_t dataFileSize;
    };
    
    std::string prefix;
    
    MappedFile * indexFile;
    MappedFile * dataFile;
    
    id_t lastID;
    std::stack<id_t> freeIDs;
    
    index_t * index;
    loc_t * objectLocs;
    
    size_t mapSize;
    
    // Buddy allocator
    // 16 bit file ID, 8 bit size, and a 40 bit offset
    static loc_t MakeLoc(loc_t loc, uint64_t sizeIdx) {return (sizeIdx << 56) | loc;}
    static void Split(loc_t loc, loc_t & low, loc_t & high);
    
    void PushToFreelist(loc_t loc);
    loc_t PopFromFreelist(uint64_t sizeIdx);
    
    // Allocate memory from free block, splitting if necessary and placing unused fragments in free lists.
    loc_t AllocFrom(loc_t loc, size_t allocSize);
    
    
  public:
    FileStore();
    ~FileStore();
    
    // Default data file size: 298.2 MB
    void Create(const std::string & pfx);
    void Load(const std::string & pfx);
    void Flush();
    
    void * Data() {return dataFile->baseAddr;}
    size_t DataSize() const {return dataFile->FileSize();}
    size_t CountFreeBytes() const;
    
    void ZeroFreeMem();
    
    // Destroys all contents of the file store.
    void Reset();
    
    // Expand file 0 to contain data in other files, and move data over.
    // Invalidates any pointers to memory-mapped data.
    // void Compact();
    
    void Log() const;
    void LogObject(id_t objID) const;
    
    // Allocate filestore-backed memory
    loc_t Alloc(size_t allocSize);
    // Free filestore-backed memory
    void Free(loc_t loc);
    
    // Get an ID for a new object. Allocates an ID and record, or reuses a previously
    // freed one.
    id_t New(size_t len);
    
    template<typename T>
    id_t New() {return New(sizeof(T));}
    
    // Release an ID.
    void Free(id_t objID);
    
    // Return portion of memory mapped to object.
    // Does not check for existence of object.
    template<typename T>
    T * GetObject(id_t objID) {return Get<T*>(objectLocs[objID]);}
    template<typename T>
    const T * GetObject(id_t objID) const {return Get<T*>(objectLocs[objID]);}
    
    template<typename T>
    T * Get(loc_t loc) {
        return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(dataFile->baseAddr) + FileOffset(loc));
    }
    template<typename T>
    const T * Get(loc_t loc) const {
        return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(dataFile->baseAddr) + FileOffset(loc));
    }
};

} // namespace fstore
#endif // FILESTORE_H
