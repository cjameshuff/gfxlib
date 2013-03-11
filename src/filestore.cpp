
#include "filestore.h"

#include <stdexcept>

#include <iostream>
#include <boost/format.hpp>
using boost::format;

#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

using namespace std;

namespace filestore {

MappedFile::MappedFile(const std::string & fpath, size_t fsize, size_t msize):
    filePath(fpath),
    fd(0),
    baseAddr(nullptr)
{
    fd = open(filePath.c_str(), O_RDWR | O_CREAT, (mode_t)0600);
    if(fd < 0)
        throw std::runtime_error((format("Could not open file \"%s\": %s")% filePath % strerror(errno)).str());
    Remap(fsize, msize);
}

void MappedFile::Remap(size_t fsize, size_t msize)
{
    // cerr << format(">>MappedFile::Remap(%u, %u)\n")% fsize % msize;
    
    // Get file size, and expand if needed
    uint64_t s = lseek(fd, 0, SEEK_END);
    fileSize = (fsize == 0)? s : fsize;
    if(s < fileSize) {
        if(lseek(fd, fileSize - 1, SEEK_SET) == -1)
            throw std::runtime_error((format("Could not seek to end of file \"%s\": %s")% filePath % strerror(errno)).str());
        
        if(write(fd, "", 1) != 1)
            throw std::runtime_error((format("Could not write to file \"%s\": %s")% filePath % strerror(errno)).str());
    }
    // cerr << format("File size: %u\n")% fileSize;
    
    // Can't grow mapping size without invalidating old address
    if(msize > mapSize)
        baseAddr = nullptr;
    
    mapSize = msize;
    // cerr << format("region(0x%x)\n")% ((size_t)baseAddr);
    // MAP_SHARED or MAP_FILE?
    if(baseAddr)
        baseAddr = mmap(baseAddr, mapSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    else
        baseAddr = mmap(0, mapSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(baseAddr == MAP_FAILED)
        throw std::runtime_error((format("Could not map file \"%s\": %s")% filePath % strerror(errno)).str());
    
    // cerr << "<<MappedFile::Remap()\n";
}

// Broken Boost based implementation
// void MappedFile::Remap(size_t fsize, size_t msize)
// {
//     cerr << format(">>MappedFile::Remap(%u, %u)\n")% fsize % msize;
//     
//     std::filebuf fbuf;
//     fbuf.open(filePath, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
//     uint64_t s = fbuf.pubseekoff(0, std::ios_base::end);
//     fileSize = (fsize == 0)? s : fsize;
//     if(s < fileSize) {
//         fbuf.pubseekoff(fileSize - 1, std::ios_base::beg);
//         fbuf.sputc(0);
//     }
//     fbuf.close();
//     
//     if(msize < mapSize)
//         baseAddr = nullptr;
//     
//     mapSize = msize;
//     cerr << "mapping\n";
//     if(fd < 0)
//         throw std::runtime_error(format("Could not open file \"%s\": %s")% fname % strerror(errno));
//     
//     mappedFile = boost::interprocess::file_mapping(filePath.c_str(), boost::interprocess::read_write);
//     cerr << format("region(0x%x)\n")% ((size_t)baseAddr);
//     // if(baseAddr)
//     region = boost::interprocess::mapped_region(mappedFile, boost::interprocess::read_write, 0, mapSize, baseAddr);
//     baseAddr = region.get_address();
//     cerr << "<<MappedFile::Remap()\n";
// }

void MappedFile::Flush() {
    msync(baseAddr, mapSize, MS_SYNC);
    // region.flush();
}



// fibonacci series x8
// Good number of block sizes that efficiently contain powers of 2, all aligned on 8-byte boundaries
const size_t kAllocSizes[64] = {
    8,// 0: 2^3 B
    16,// 1: 2^4 B
    24,
    40,
    64,// 4: 2^6 B
    104,
    168,
    272,// 7: 256 + 16 B
    440,// 8
    712,
    1152,// 10: 1024 + 128 B
    1864,
    3016,
    4880,// 13: 4096 + 784 B
    7896,
    12776,
    20672,// 16
    33448,// 17: 2^15 + 680 B
    54120,
    87568,// 19: 2^16 + 22032 B
    141688,// 20: 2^17 + 10616 B
    229256,
    370944,
    600200,
    971144,// 24
    1571344,// 25: 1 MB + 522768 B
    2542488,// 26: 2 MB + 445336 B
    4113832,// 27: 3 MB + 968104 B
    6656320,
    10770152,
    17426472,// 30: 16 MB + 649256 B
    28196624,
    45623096,// 32: 43.5 MB
    73819720,
    119442816,// 34: 113.9 MB
    193262536,// 35: 184.3 MB
    312705352,// 36: 298.2 MB
    505967888,// 37: 482.5 MB
    818673240,// 38: 780.748 MB
    1324641128,// 39: 1.234 GB
    2143314368,// 40: 1.996 GB
    3467955496,
    5611269864,
    9079225360,
    14690495224,
    23769720584,
    38460215808,
    62229936392,
    100690152200,// 48: 93.775 GB
    162920088592,
    263610240792,
    426530329384,
    690140570176,
    1116670899560,// 53: 1.015 TB
    1806811469736,
    2923482369296,
    4730293839032,// 56: 4.405 TB
    7653776208328,
    12384070047360,
    20037846255688,
    32421916303048,
    52459762558736,
    84881678861784,
    137341441420520,
    // 64
};

    
std::string SizeToS(size_t size) {
    double s = size;
    if(s < 1024)
        return (format("%u B")% s).str();
    s /= 1024;
    if(s < 1024)
        return (format("%u kB")% s).str();
    s /= 1024;
    if(s < 1024)
        return (format("%u MB")% s).str();
    s /= 1024;
    if(s < 1024)
        return (format("%u GB")% s).str();
    s /= 1024;
    return (format("%u TB")% s).str();
}

std::string LocToS(loc_t loc) {
    return (format("%s:%u")% SizeToS(BlockBytes(loc)) % FileOffset(loc)).str();
}

uint64_t BlockBytes(loc_t loc) {
    return kAllocSizes[BlockSize(loc)];
}

// Compute IDs for sub-blocks of block.
// The block is split into unequal-sized sub-blocks, of sizes s-1 and s-2.
// The lower block is the larger one, s-1, with the size of the upper block being s-2.
// Not valid for minimum-size blocks.
void FileStore::Split(loc_t loc, loc_t & low, loc_t & high)
{
    uint64_t s = BlockSize(loc);
    loc = FileOffset(loc);
    if(s == 1) {
        // split into two equal-sized blocks
        low = MakeLoc(loc, s - 1);
        high = MakeLoc(loc + kAllocSizes[s - 1], s - 1);
    }
    else {
        // low block size is s-1, high block size is s-2.
        low = MakeLoc(loc, s - 1);
        high = MakeLoc(loc + kAllocSizes[s - 1], s - 2);
    }
    // cerr << format("Split block %u:%u\n")% kAllocSizes[s] % FileOffset(loc);
    // cerr << format("\tlow: %u:%u\n")% BlockBytes(low) % FileOffset(low);
    // cerr << format("\thigh: %u:%u\n")% BlockBytes(high) % FileOffset(high);
}


FileStore::FileStore():
    index(nullptr),
    mapSize(1116670899560) // 1.015 TB default
{
}

FileStore::~FileStore() {
    Log();
    
    indexFile->Flush();
    delete indexFile;
    
    dataFile->Flush();
    delete dataFile;
}


void FileStore::Create(const std::string & pfx)
{
    prefix = pfx;
    string fname = (format("%sindex")% prefix).str();
    indexFile = new MappedFile(fname, 4096, 1024*1024*1024);
    fname = (format("%sdata")% prefix).str();
    dataFile = new MappedFile(fname, kAllocSizes[2], mapSize);
    Reset();
}

void FileStore::Load(const std::string & pfx)
{
    prefix = pfx;
    string fname = (format("%sindex")% prefix).str();
    indexFile = new MappedFile(fname, 0, 1024*1024*1024);
    fname = (format("%sdata")% prefix).str();
    dataFile = new MappedFile(fname, kAllocSizes[2], mapSize);
    
    if(indexFile->FileSize() == 0)
    {
        Reset();
    }
    else
    {
        index = static_cast<index_t *>(indexFile->baseAddr);
        objectLocs = (loc_t *)((uint8_t*)(indexFile->baseAddr) + 2048);
        
        // Open data file
        fname = (format("%sdata")% prefix).str();
        dataFile->Remap(kAllocSizes[index->dataFileSize], mapSize);
        
        // Gather unused IDs
        lastID = indexFile->FileSize() - 2048 - 1;
        for(int j = 1; j <= lastID; ++j)
            if(objectLocs[j] == 0)
                freeIDs.push(j);
    }
}

void FileStore::Reset()
{
    cerr << format("Resetting file store\n");
    indexFile->Remap(4096, 1024*1024*1024);
    memset(indexFile->baseAddr, 0, 4096);
    
    index = static_cast<index_t *>(indexFile->baseAddr);
    index->filestoreVersion = 1;
    index->numObjects = 0;
    index->dataFileSize = 2;
    
    // Freelists and object locations are already initialized
    // Object locations start 2048 bytes into the file
    objectLocs = (loc_t *)((uint8_t*)(indexFile->baseAddr) + 2048);
    
    cerr << format("Remapping data file\n");
    dataFile->Remap(kAllocSizes[index->dataFileSize], mapSize);
    PushToFreelist(MakeLoc(0, index->dataFileSize));
    
    // Start out with a few thousand IDs, all available.
    lastID = indexFile->FileSize() - 2048 - 1;
    for(int j = 1; j <= lastID; ++j)
        freeIDs.push(j);
    
    Flush();
    cerr << format("Reset complete\n");
}


void FileStore::Flush()
{
    indexFile->Flush();
    dataFile->Flush();
}

void FileStore::ZeroFreeMem()
{
    cerr << format("Zeroing free memory\n");
    // Can't zero 8-byte blocks, as all 8 bytes are required for linking
    for(int s = 1; s < kNumAllocSizes; ++s)
    {
        loc_t loc = index->freeLists[s];
        while(loc != 0) {
            memset(Get<uint8_t>(loc) + 8, 0, kAllocSizes[s] - 8);
            loc = *Get<loc_t>(loc);
        }
    }
}

size_t FileStore::CountFreeBytes() const
{
    size_t totalUnused = 0;
    for(int s = 0; s < kNumAllocSizes; ++s)
    {
        int n = 0;
        loc_t loc = index->freeLists[s];
        while(loc != 0) {
            loc = *Get<loc_t>(loc);
            ++n;
        }
        totalUnused += kAllocSizes[s]*n;
    }
    return totalUnused;
}


void FileStore::Log() const
{
    cerr << "================================\n";
    cerr << format("Number of objects: %d\n")% index->numObjects;
    size_t totalSize = kAllocSizes[index->dataFileSize];
    size_t totalUnused = 0;
    for(int s = 0; s < kNumAllocSizes; ++s)
    {
        int n = 0;
        loc_t loc = index->freeLists[s];
        while(loc != 0) {
            // cerr << format("-> %s\n")% LocToS(loc);
            loc = *Get<loc_t>(loc);
            ++n;
        }
        cerr << format("free list %d: %d entries (%s each)\n")% s % n % SizeToS(kAllocSizes[s]);
        totalUnused += kAllocSizes[s]*n;
    }
    cerr << format("Total size: %d\n")% SizeToS(totalSize);
    cerr << format("Total unused: %d (%d %%)\n")% SizeToS(totalUnused) % (totalUnused*100.0/totalSize);
}

void FileStore::LogObject(id_t objID) const
{
    const loc_t & objRec = objectLocs[objID];
    cerr << format("object %d\n")% objID;
    cerr << format("FileOffset(objRec): %d\n")% FileOffset(objRec);
    cerr << format("BlockSize(objRec): %d\n")% BlockSize(objRec);
}


void FileStore::PushToFreelist(loc_t loc)
{
    // Save old head at start of block and set new head to block location.
    uint64_t s = BlockSize(loc);
    // cerr << format("Pushing to free list %d: %u:%u\n")% BlockSize(loc) % kAllocSizes[BlockSize(loc)] % FileOffset(loc);
    // cerr << format("\told head: %u:%u\n")% kAllocSizes[BlockSize(index->freeLists[s])] % FileOffset(index->freeLists[s]);
    *Get<loc_t>(loc) = index->freeLists[s];
    index->freeLists[s] = loc;
}

loc_t FileStore::PopFromFreelist(uint64_t s)
{
    // If freelist head is not null:
    // Set freelist to loc_t stored in block pointed to by freelist head
    // return old head
    loc_t loc = index->freeLists[s];
    if(loc)
        index->freeLists[s] = *Get<loc_t>(index->freeLists[s]);
    
    // cerr << format("Popped from free list %d: %u:%u\n")% s % BlockSize(loc) % FileOffset(loc);
    // cerr << format("\tnew head: %u:%u\n")% kAllocSizes[BlockSize(index->freeLists[s])] % FileOffset(index->freeLists[s]);
    return loc;
}


void FileStore::Free(loc_t loc)
{
    // Simply push to free list for now. This may actually be the best implementation for many uses.
    // Block consolidation may be best done as a periodic maintenance task.
    PushToFreelist(loc);
}

// Given a block location already popped from free list:
// recursively split until a minimal block is reached, adding unused fragments to
// the appropriate free lists.
loc_t FileStore::AllocFrom(loc_t loc, size_t allocSize)
{
    // cerr << format("FileStore::AllocFrom(%u:%u, %u)\n")% BlockBytes(loc) % FileOffset(loc)% allocSize;
    int sizeIdx = BlockSize(loc);
    if(sizeIdx == 1)
    {
        // only one smaller list
        if(kAllocSizes[0] >= allocSize)
        {
            // Split, take low half
            loc_t lowHalf, highHalf;
            Split(loc, lowHalf, highHalf);
            PushToFreelist(highHalf);
            return lowHalf;
        }
    }
    else if(sizeIdx > 1)
    {
        if(kAllocSizes[sizeIdx - 2] >= allocSize)
        {
            // Split, take small sub-block
            loc_t lowHalf, highHalf;
            Split(loc, lowHalf, highHalf);
            PushToFreelist(lowHalf);
            return AllocFrom(highHalf, allocSize);
        }
        else if(kAllocSizes[sizeIdx - 1] >= allocSize)
        {
            // Split, take large sub-block
            loc_t lowHalf, highHalf;
            Split(loc, lowHalf, highHalf);
            PushToFreelist(highHalf);
            return AllocFrom(lowHalf, allocSize);
        }
    }
    // Couldn't split.
    return loc;
}

loc_t FileStore::Alloc(size_t allocSize)
{
    // cerr << format("FileStore::Alloc(%u)\n")% allocSize;
    // Look for a freelist with sufficiently sized free blocks
    int s = 0;
    while(s < kNumAllocSizes && kAllocSizes[s] < allocSize)
        ++s;
    
    // Take first list with available blocks
    while(s < kNumAllocSizes && index->freeLists[s] == 0)
        ++s;
    
    // cerr << format("Free list %u\n")% s;
    loc_t allocation = 0;
    if(s < kNumAllocSizes)
        allocation = AllocFrom(PopFromFreelist(s), allocSize);
    else
    {
        // Grow data file
        while(allocation == 0)
        {
            // Given sequential sizes A, B, C, C = A + B.
            // size[n+1] = size[n-1] + size[n]
            size_t oldSize = kAllocSizes[index->dataFileSize];
            ++(index->dataFileSize);
            size_t newSize = kAllocSizes[index->dataFileSize];
            
            cerr << format("Growing data file from %u B to %u B\n")% oldSize % newSize;
            dataFile->Remap(newSize, mapSize);
            
            // If allocated block is large enough, allocate directly from block.
            // Otherwise add to free list and grow again.
            // Offset is previous file size, block size is file size - 2
            if(allocSize <= kAllocSizes[index->dataFileSize - 2])
                allocation = AllocFrom(MakeLoc(oldSize, index->dataFileSize - 2), allocSize);
            else
                PushToFreelist(MakeLoc(oldSize, index->dataFileSize - 2));
        }
    }
    // FIXME: check for allocations too big to handle
    // cerr << format("Allocated block of size %u at %u\n")% kAllocSizes[BlockSize(allocation)] % FileOffset(allocation);
    return allocation;
}

id_t FileStore::New(size_t allocSize)
{
    // Allocate object ID
    id_t objID;
    if(freeIDs.empty())
    {
        // TODO: Need to expand index to add new object IDs
        // objID = ++lastID;
        // objectLocs.push_back(0);
        throw std::runtime_error("Allocation failed: out of object IDs");
    }
    else
    {
        objID = freeIDs.top();
        freeIDs.pop();
    }
    ++(index->numObjects);
    // cerr << format("\nfs::New(): %d\n")% objID;
    
    loc_t objMem = Alloc(allocSize);
    if(objMem == 0)
        throw std::runtime_error("Allocation failed");
    
    objectLocs[objID] = objMem;
    return objID;
}


void FileStore::Free(id_t objID)
{
    // cerr << format("\nfs::Free(): %d\n")% objID;
    objectLocs[objID] = 0;
    freeIDs.push(objID);
    --(index->numObjects);
}


} // namespace filestore
