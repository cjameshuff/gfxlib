#ifndef FILESTORE_H
#define FILESTORE_H

// The file store is a key-value associative structure that stores values in a set of
// files.
// If the new value for a key is larger than the old, it is written to the end of the
// file with the least used space and its location is updated.
// Over time, files accumulates unused space, which may be cleaned up by moving all
// values stored in a file to other files. The GC() function cleans the data file with
// the largest amount of unused space.
//
// The files are stored in an array which allows direct access by ID, and in a linked
// list which is kept sorted by used space, implementing a simple priority queue with
// the files with the least used space having priority.

// If no files have space, a new file is created. The file size is a power of 2 multiple
// of a fixed base size.

// Notes on memory mapping approaches:
// A file could be mapped to a fixed, large memory space, and remapped to that same space
// every time the file size is increased. It's unclear how this might interact with multiple
// threads/processes working in that memory space. Use of mmap() MAP_FIXED is "discouraged",
// but reasons for this aren't given in the manpage. Boost allows fixed mapping, but there is
// no documentation of any restrictions or caveats involved.
//
// Alternatively, new files can be created and mapped.

#include <stack>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
// #include <boost/interprocess/managed_mapped_file.hpp>

namespace filestore {


struct MappedFile {
    boost::interprocess::file_mapping mappedFile;
    boost::interprocess::mapped_region region;
    
    MappedFile(const std::string & filePath, size_t size)
    {
        std::filebuf fbuf;
        fbuf.open(filePath, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        fbuf.pubseekoff(size - 1, std::ios_base::beg);
        fbuf.sputc(0);
        fbuf.close();
        
        mappedFile = boost::interprocess::file_mapping(filePath.c_str(), boost::interprocess::read_write);
        region = boost::interprocess::mapped_region(mappedFile, boost::interprocess::read_write, 0, size);
    }
};


const int kFileStoreFiles = 4;
class FileStore {
  public:
    typedef uint32_t id_t;
    
  protected:
    class record_t {
        // Bit 63 of location is used to indicate which file the object is stored in.
        // It is the 1-bit index of the file in the files array.
        uint64_t location = 0;
        uint64_t size = 0;
      public:
        // Uppermost 8 bits of location are reserved for file ID.
        // 64 petabytes ought to be enough for anyone...
        uint64_t FileID() const {return location >> 55;}
        uint64_t FileOffset() const {return location & ((1UL << 55) - 1);}
        uint64_t Size() const {return size;}
        void SetLocation(uint64_t fid, uint64_t loc) {location = (fid << 55) | loc;}
        void SetSize(uint64_t s) {size = s;}
    };
    
    struct file_t {
        FILE * file = nullptr;
        uint64_t size = 0;
        uint64_t available = 0;
        uint64_t unused = 0;
        uint64_t objects = 0;
        file_t * next = nullptr;
    };
    
    std::string prefix;
    id_t lastID;
    std::stack<id_t> freeIDs;
    std::vector<record_t> records;
    
    uint32_t numObjects;
    
    std::array<file_t, kFileStoreFiles> files;
    file_t * writeList;
    
    void Write(id_t objID, const uint8_t * data, size_t len);
    
  public:
    FileStore(const std::string & pfx);
    ~FileStore();
    
    // Read/write the index associating IDs to objects, allowing the FileStore to be made persistent
    void SaveIndex();
    void LoadIndex();
    
    // Files are allowed to grow as needed, and after garbage collection may end with free space.
    // This truncates unused space at the end of the file to free up disk space.
    void Trim();
    
    // Destroys all contents of the file store and frees all IDs.
    void Reset();
    
    void Log() const;
    void LogObject(id_t objID) const;
    
    // Get an ID for a new object. Allocates an ID and record, or reuses a previously
    // freed one.
    id_t New();
    
    // Release an ID.
    void Free(id_t objID);
    
    // Return size of object.
    size_t GetSize(id_t objID) const {return records[objID].Size();}
    
    // Return true if object has a value
    // (ID must be allocated...does *NOT* check allocated status of ID.)
    bool Exists(id_t objID) const {return records[objID].Size() != 0;}
    
    // Get value in existing buffer
    size_t Get(id_t objID, uint8_t * data, size_t len);
    
    // Get value, allocated with new[]
    uint8_t * Get(id_t objID);
    
    // Set value. If objID is 0, allocate a new object record and return its ID.
    id_t Set(id_t objID, const uint8_t * data, size_t len);
    
    void GC();
};

} // namespace fstore
#endif // FILESTORE_H
