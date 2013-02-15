
#include "filestore.h"

// #include <sys/stat.h>
// #include <sys/mman.h>
// #include <fcntl.h>

#include <stdexcept>

#include <iostream>
#include <boost/format.hpp>
using boost::format;

using namespace std;

namespace filestore {
// void StretchFile(int fd, size_t size)
// {
//     // Write dummy byte to create file of required size
//     if(lseek(fd, size - 1, SEEK_SET) == -1)
//         throw std::runtime_error(format("Could not seek to end of file \"%s\": %s")% fname % strerror(errno));
//     
//     if(write(fd), "", 1) != 1)
//         throw std::runtime_error(format("Could not write to file \"%s\": %s")% fname % strerror(errno));
// }
// 
// void OpenAndMap(const std::string & fname, size_t size)
// {
//     // | O_TRUNC
//     int fd = open(fname.c_str(), O_RDWR | O_CREAT, (mode_t)0600)
//     if(fd < 0)
//         throw std::runtime_error(format("Could not open file \"%s\": %s")% fname % strerror(errno));
//     
//     StretchFile(fd, size);
//     
//     uint8_t * mapped = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//     if(mapped == MAP_FAILED)
//         throw std::runtime_error(format("Could not map file \"%s\": %s")% fname % strerror(errno));
// }


FileStore::FileStore(const std::string & pfx):
    prefix(pfx)
{
    records.push_back(record_t());
    lastID = 0;// zeroth record is reserved for a null value
    numObjects = 0;
    
    writeList = nullptr;
    int i = 0;
    for(auto & f : files)
    {
        string fName = prefix + (char)('A' + i++);
        f.file = fopen(fName.c_str(), "wb");
        f.next = writeList;
        writeList = &f;
    }
    Log();
}

FileStore::~FileStore() {
    Log();
    for(auto & f : files)
        fclose(f.file);
}


void FileStore::Log() const
{
    cerr << "================================\n";
    cerr << format("Number of files: %d\n")% kFileStoreFiles;
    cerr << format("Number of objects: %d\n")% numObjects;
    size_t totalSize = 0, totalUnused = 0;
    for(auto & f : files) {
        totalSize += f.size;
        totalUnused += f.unused;
    }
    cerr << format("Total size: %d B\n")% totalSize;
    cerr << format("Total unused: %d B (%d %%)\n")% totalUnused % (totalUnused*100.0/totalSize);
    
    int fid = 0;
    for(auto & f : files) {
        cerr << format("File %d\n")% fid;
        cerr << format("  File: %u\n")% f.file;
        cerr << format("  Objects: %d\n")% f.objects;
        cerr << format("  Size: %d B\n")% f.size;
        cerr << format("  Unused: %d B (%d %%)\n")% f.unused % (f.unused*100.0/f.size);
        ++fid;
    }
}
void FileStore::LogObject(id_t objID) const
{
    const record_t & objRec = records[objID];
    cerr << format("object %d\n")% objID;
    cerr << format("objRec.FileID(): %d\n")% objRec.FileID();
    cerr << format("objRec.FileOffset(): %d\n")% objRec.FileOffset();
    cerr << format("objRec.Size(): %d\n")% objRec.Size();
}

// Index format:
// uint32_t: 0xABCDEF42 (Magic number/byte order check)
// uint32_t: format version
// uint32_t: numFiles
//   numFiles times:
//     uint64_t size
//     uint64_t available
//     uint64_t unused
//     uint64_t objects
// uint32_t: numObjects
//   numObjects times:
//     uint64_t location
//     uint64_t size
void FileStore::SaveIndex()
{
    // FILE * indexFile = fopen(prefix.c_str(), "wb");
    // fclose(indexFile);
}

void FileStore::LoadIndex()
{
    // Need to regenerate free ID list
    
}

void FileStore::Trim()
{
    for(auto & f : files)
        ftruncate(fileno(f.file), f.size);
}

void FileStore::Reset()
{
    records.clear();
    records.push_back(record_t());
    lastID = 0;// zeroth record is reserved for a null value
    while(!freeIDs.empty())// STL stupidity...
        freeIDs.pop();
    numObjects = 0;
    
    writeList = nullptr;
    int i = 0;
    for(auto & f : files)
    {
        f.size = 0;
        f.available = 0;
        f.unused = 0;
        f.objects = 0;
        f.next = writeList;
        writeList = &f;
    }
    Trim();
    Log();
}

id_t FileStore::New()
{
    id_t objID;
    if(freeIDs.empty())
    {
        objID = ++lastID;
        records.push_back(record_t());
    }
    else
    {
        objID = freeIDs.top();
        freeIDs.pop();
    }
    ++numObjects;
    // cerr << format("\nfs::New(): %d\n")% objID;
    return objID;
}


void FileStore::Free(id_t objID)
{
    // cerr << format("\nfs::Free(): %d\n")% objID;
    records[objID].SetLocation(0, 0);
    records[objID].SetSize(0);
    freeIDs.push(objID);
    --numObjects;
}


size_t FileStore::Get(id_t objID, uint8_t * value, size_t len)
{
    // cerr << format("\nfs::Get(): %d\n")% objID;
    // Read from file containing object
    record_t & objRec = records[objID];
    if(objRec.Size() == 0)
        return 0;
    if(objRec.Size() < len)
        len = objRec.Size();
    
    file_t & file = files[objRec.FileID()];
    fseek(file.file, objRec.FileOffset(), SEEK_SET);
    fread(value, len, 1, file.file);
    return len;
}

uint8_t * FileStore::Get(id_t objID)
{
    // cerr << format("\nfs::Get(): %d\n")% objID;
    // Read from file containing object
    record_t & objRec = records[objID];
    if(objRec.Size() == 0)
        return nullptr;
    
    file_t & file = files[objRec.FileID()];
    fseek(file.file, objRec.FileOffset(), SEEK_SET);
    uint8_t * value = new uint8_t[objRec.Size()];
    fread(value, objRec.Size(), 1, file.file);
    return value;
}

id_t FileStore::Set(id_t objID, const uint8_t * value, size_t len)
{
    // cerr << format("\nfs::Set(%d, %X, %u)\n")% objID % (uint64_t)value % len;
    LogObject(objID);
    
    if(objID == 0)
        objID = New();
    
    record_t & objRec = records[objID];
    
    if(len > objRec.Size())
    {
        // cerr << format("  rewriting %d\n")% objID;
        // Write to new location
        Write(objID, value, len);
    }
    else
    {
        // Rewrite value in place
        // cerr << format("  modifying %d\n")% objID;
        file_t & file = files[objRec.FileID()];
        fseek(file.file, objRec.FileOffset(), SEEK_SET);
        fwrite(value, len, 1, file.file);
        
        // Update record
        objRec.SetSize(len);
    }
    
    LogObject(objID);
    // cerr << format("<<fs::Set()\n");
    return objID;
}


void FileStore::Write(id_t objID, const uint8_t * value, size_t len)
{
    // cerr << format("\nfs::Write(%d, %X, %u)\n")% objID % (uint64_t)value % len;
    // Write value to end of destination file, head of writeList
    fseek(writeList->file, writeList->size, SEEK_SET);
    fwrite(value, len, 1, writeList->file);
    
    // Update record and file info
    record_t & objRec = records[objID];
    if(objRec.Size() > 0) {
        file_t & oldFile = files[objRec.FileID()];
        --(oldFile.objects);
        oldFile.unused += objRec.Size();
    }
    
    int fID = writeList - &files[0];
    // cerr << format("  fID: %d\n")% fID;
    objRec.SetLocation(fID, writeList->size);
    objRec.SetSize(len);
    writeList->size += len;
    ++(writeList->objects);
    
    if(writeList->size > writeList->next->size)
    {
        // Pop off the written file and insert it before the first file that's still larger.
        file_t * writtenFile = writeList;
        writeList = writeList->next;
        file_t * f = writeList;
        while(f->next && writtenFile->size > f->next->size)
            f = f->next;
        writtenFile->next = f->next;
        f->next = writtenFile;
    }
    
    // for(file_t * f = writeList; f; f = f->next)
    //     cerr << format(" : %d")% ((int)(f - &files[0]));
    // cerr << "\n";
    // cerr << format("<<fs::Write()\n");
}


void FileStore::GC()
{
    cerr << format("\nfs::GC()\n");
    // Find file with greatest amount of empty space, remove from writeList.
    // Move every object in that file to the other files and reset file.
    
    int deadFileID = 0;
    for(id_t fID = 1; fID < kFileStoreFiles; ++fID) {
        if(files[fID].unused > files[deadFileID].unused)
            deadFileID = fID;
    }
    
    // Remove from writeList
    file_t * deadFile = &files[deadFileID];
    file_t ** f = &writeList;
    while(*f != nullptr) {
        if(*f == deadFile)
            *f = (*f)->next;
        else
            f = &(*f)->next;
    }
    
    // Rewrite all objects in file to other files
    for(id_t objID = 1; objID < lastID; ++objID)
    {
        record_t & objRec = records[objID];
        if(objRec.FileID() != deadFileID || objRec.Size() == 0)
            continue;
        
        uint8_t * value = Get(objID);
        Write(objID, value, objRec.Size());
        delete[] value;
    }
    
    // Clear file and put at head of writeList
    deadFile->size = 0;
    deadFile->objects = 0;
    deadFile->unused = 0;
    deadFile->next = writeList;
    writeList = deadFile;
}

} // namespace filestore
