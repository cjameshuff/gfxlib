

#ifndef TARGATOOL_H
#define TARGATOOL_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <stdint.h>


struct SimpleImage {
    uint8_t * imagedata;
    uint8_t ** imagerows;
    size_t width, height;
    size_t pixelBytes;
    
    SimpleImage(): imagedata(NULL), imagerows(NULL), width(0), height(0), pixelBytes(0) {}
    
    SimpleImage(size_t w, size_t h, size_t pb):
         imagedata(NULL), imagerows(NULL), width(0), height(0), pixelBytes(0)
    {Realloc(w, h, pb);}
    
    ~SimpleImage() {delete[] imagedata; delete[] imagerows;}
    
    void Realloc(size_t w, size_t h, size_t pb) {
        width = w; height = h; pixelBytes = pb;
        if(imagedata) delete[] imagedata;
        if(imagerows) delete[] imagerows;
        imagedata = new uint8_t[width*height*pixelBytes];
        imagerows = new uint8_t *[height];
        for(size_t y = 0; y < height; ++y)
            imagerows[y] = imagedata + width*y*pixelBytes;
    }
    
    void Clear() {memset(imagedata, 0, width*height*pixelBytes);}
    
    void Clear(uint8_t r, uint8_t g, uint8_t b) {
        for(size_t p = 0, n = width*height*pixelBytes; p < n; p += pixelBytes) {
            imagedata[p] = r;
            imagedata[p+1] = g;
            imagedata[p+2] = b;
        }
    }
    void Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        for(size_t p = 0, n = width*height*pixelBytes; p < n; p += pixelBytes) {
            imagedata[p] = r;
            imagedata[p+1] = g;
            imagedata[p+2] = b;
            imagedata[p+3] = a;
        }
    }
    
    //Make sure image depth supports the number of components you specify...no checking done!
    void SetPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b) {
        imagerows[y][x*pixelBytes] = r;
        imagerows[y][x*pixelBytes+1] = g;
        imagerows[y][x*pixelBytes+2] = b;
    }
    void SetPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        imagerows[y][x*pixelBytes] = r;
        imagerows[y][x*pixelBytes+1] = g;
        imagerows[y][x*pixelBytes+2] = b;
        imagerows[y][x*pixelBytes+3] = a;
    }
    
    uint8_t * GetPixel(size_t x, size_t y) {return imagerows[y] + x*pixelBytes;}
    const uint8_t * GetPixel(size_t x, size_t y) const {return imagerows[y] + x*pixelBytes;}
};

// Implements true-color type TGA format.
//    Header:
//    byte: ID length
//    byte: cmap type
//    byte: image type
//        2: uncompressed true color
//        3: uncompressed black-white
//        10: RLE compressed true color
//        11: RLE compressed black-white
//    5 bytes: cmap spec
//    10 bytes: image spec
//        2 bytes: x origin (not much use without a Targa video card)
//        2 bytes: y origin
//        2 bytes: width
//        2 bytes: height
//      byte: pixel depth(bits per pixel: 8, 16, 24, 32)
//      byte: image descriptor
//    var: image ID
//    var: cmap data
//    var: image data
//    *var: dev fields
//    *var: dev directory
//
//    Footer:
//    long: extension area offset
//    long: developer directory offset
//    ASCII: signature:
//        TRUEVISION-XFILE
//          "."
//        binary 0 string terminator

enum {
    kTargaType_Truecolor = 2,
    kTargaType_BW = 3,
    kTargaType_RLE_Truecolor = 10,
    kTargaType_RLE_BW = 11
};

class TargaFileInfo {
    uint8_t cmapType;
    uint8_t cmapSpec[5];
    uint8_t imageSpec[10];
    uint8_t idLen;
    char * imageID;
    uint8_t descriptor;
    
    uint8_t * tgaImageData;
    
//    bool Write(std::ofstream & fin);
    
  public:
    size_t width, height;
    uint8_t type;
    uint8_t pixelBytes;
    uint8_t depth;
    
  public:
    TargaFileInfo():
        imageID(NULL),    
        tgaImageData(NULL),
        width(0), height(0), type(0), pixelBytes(3), depth(24)
    {}
    TargaFileInfo(size_t w, size_t h, int d = 24):
        imageID(NULL),    
        tgaImageData(NULL),
        width(w), height(h), type(0), pixelBytes(d/8), depth(d)
    {}
    ~TargaFileInfo() {
        if(tgaImageData) delete[] tgaImageData;
        if(imageID) delete[] imageID;
    }
    
    // Read as 1D pixels array, starting at lower left corner and going right and up,
    // and as rows array, starting at bottom row and going up.
    bool Read(const std::string & filename, SimpleImage & image);
    
    // Read header data
    bool ReadHeader(std::ifstream & fin);
    // Read image data into already-allocated buffer (use ReadHeader() to determine size)
    bool ReadImage(std::ifstream & fin, uint8_t * imagedata);
    
    static bool Write(const std::string & filename, const SimpleImage & image);
    bool Write(const std::string & filename, const uint8_t * imagedata);
};

// TODO: error handling...
inline bool TargaFileInfo::ReadHeader(std::ifstream & fin)
{
    fin.read((char *)&idLen, 1);//image ID length
    fin.read((char *)&cmapSpec, 1);//cmap type
    fin.read((char *)&type, 1);//image type
    fin.read((char *)cmapSpec, 5);//cmap specification
    fin.read((char *)imageSpec, 10);//image specification
    
    if(imageID) delete[] imageID;
    imageID = new char[idLen];
    fin.read((char *)imageID, idLen);//image ID
    
    width = (imageSpec[5] << 8) | imageSpec[4];
    height = (imageSpec[7] << 8) | imageSpec[6];
    depth = imageSpec[8];
    descriptor = imageSpec[9];
    
    // Only handling true color images, so no color map data to read
    if(type == kTargaType_Truecolor)
        pixelBytes = depth/8;
    
    return true;
}

inline bool TargaFileInfo::ReadImage(std::ifstream & fin, uint8_t * tgaImageData)
{
    fin.read((char *)tgaImageData, width*height*pixelBytes);
    if(depth == 24) {
        // write BGR data as RGB
        for(int p = 0, n = width*height*3; p < n; p += 3)
            std::swap(tgaImageData[p+2], tgaImageData[p]);
    }
    else if(depth == 32) {
        // read BGRA data as RGBA
        for(int p = 0, n = width*height*4; p < n; p += 4)
            std::swap(tgaImageData[p+2], tgaImageData[p]);
    }
    
    return true;
}

inline bool TargaFileInfo::Read(const std::string & filename, SimpleImage & image)
{
    std::ifstream fin(filename.c_str(), std::ios::in | std::ios::binary);
    ReadHeader(fin);
    if(type == kTargaType_Truecolor) {
        if(tgaImageData) delete[] tgaImageData;
        tgaImageData = new uint8_t[width*height*pixelBytes];
    }
    ReadImage(fin, tgaImageData);
    fin.close();
    if(!tgaImageData)
        return false;
    
    // Translate from file pixel layout into standard (origin at lower left) layout
    // and RGBA byte order
    pixelBytes = depth/8;
    switch(descriptor & 0x30) {
      case 0x00:// lower left origin
        //Take a shortcut here, avoid reallocating entire image
//        cout << "origin at lower left" << endl;
        image.width = width;
        image.height = height;
        image.pixelBytes = pixelBytes;
        if(image.imagedata) delete[] image.imagedata;
        if(image.imagerows) delete[] image.imagerows;
        image.imagedata = tgaImageData;
        image.imagerows = new uint8_t *[height];
        for(size_t y = 0; y < height; ++y)
            image.imagerows[y] = image.imagedata + width*y*pixelBytes;
        tgaImageData = NULL;
      break;
      
      case 0x10:// lower right origin
//        cout << "origin at lower right" << endl;
        image.Realloc(width, height, pixelBytes);
        
        for(size_t x = 0; x < width; ++x)
        for(size_t y = 0; y < height; ++y) {
            size_t srcIdx = (width*y + (width - 1 - x))*pixelBytes;
            size_t dstIdx = (width*y + x)*pixelBytes;
            memcpy(image.imagedata + dstIdx, tgaImageData + srcIdx, pixelBytes);
        }
      break;
      
      case 0x20:// upper left origin
//        cout << "origin at upper left" << endl;
        image.Realloc(width, height, pixelBytes);
        
        //Rows are in same order, just the order of rows is different. Copy a row at a time.
        for(size_t y = 0; y < height; ++y) {
            size_t srcIdx = width*(height - 1 - y)*pixelBytes;
            size_t dstIdx = width*y*pixelBytes;
            memcpy(image.imagedata + dstIdx, tgaImageData + srcIdx, pixelBytes*width);
        }
      break;
      
      case 0x30:// upper right origin
//        cout << "origin at upper right" << endl;
        image.Realloc(width, height, pixelBytes);
        
        for(size_t x = 0; x < width; ++x)
        for(size_t y = 0; y < height; ++y) {
            size_t srcIdx = (width*(height - 1 - y) + (width - 1 - x))*pixelBytes;
            size_t dstIdx = (width*y + x)*pixelBytes;
            memcpy(image.imagedata + dstIdx, tgaImageData + srcIdx, pixelBytes);
        }
      break;
    }
    
    if(tgaImageData) delete[] tgaImageData;
    tgaImageData = NULL;
    
    return true;
}

inline bool TargaFileInfo::Write(const std::string & filename, const SimpleImage & image)
{
    TargaFileInfo writer;
    writer.pixelBytes = image.pixelBytes;
    writer.depth = image.pixelBytes*8;
    writer.width = image.width;
    writer.height = image.height;
    return writer.Write(filename, image.imagedata);
}

inline bool TargaFileInfo::Write(const std::string & filename, const uint8_t * imagedata)
{
    std::ofstream fout(filename.c_str(), std::ios::out | std::ios::binary);
    
    type = kTargaType_Truecolor;
    
//    imageID = NULL;
    if(imageID == NULL)
        idLen = 0;
    else
        idLen = strlen(imageID);
    cmapType = 0;//no color-map data
    
    for(size_t j = 0; j < 5; ++j)
        cmapSpec[j] = 0;
    
    // Write header
    fout.write((char *)&idLen, 1);
    fout.write((char *)&cmapType, 1);
    fout.write((char *)&type, 1);
    fout.write((char *)cmapSpec, 5);
    
    // Write image specification:
    uint8_t imagespec[10] = {
        0, 0,// x origin
        0, 0,// y origin
        (uint8_t)(width & 0xFF), (uint8_t)((width >> 8) & 0xFF),// width
        (uint8_t)(height & 0xFF), (uint8_t)((height >> 8) & 0xFF),// height
        depth,//Need I explain?
        0// Image descriptor: no attribute bits, lower-left origin
    };
    fout.write((char *)imagespec, 10);
    
    if(idLen > 0)
        fout.write(imageID, sizeof(char)*idLen);
//    fout.write(***, sizeof(char));//color-map data (not used)
    
    // Write image data
    if(depth == 24) {
        // write RGB data as BGR
        for(int p = 0, n = width*height*3; p < n; p += 3) {
            fout.put(imagedata[p+2]);
            fout.put(imagedata[p+1]);
            fout.put(imagedata[p]);
        }
    }
    else if(depth == 32) {
        // write RGBA data as BGRA
        for(int p = 0, n = width*height*4; p < n; p += 4) {
            fout.put(imagedata[p+2]);
            fout.put(imagedata[p+1]);
            fout.put(imagedata[p]);
            fout.put(imagedata[p+3]);
        }
    }
    else fout.write((char *)imagedata, width*height*pixelBytes);
    
    uint8_t extArea[4] = {0};
    uint8_t devDir[4] = {0};
    char signature[] = "TRUEVISION-XFILE.";
    fout.write((char *)extArea, sizeof(uint8_t)*4);
    fout.write((char *)devDir, sizeof(uint8_t)*4);
    fout.write((char *)signature, sizeof(char)*(strlen(signature)+1));
    
    fout.close();
    return true;
}

#endif // targatool.h
