
#ifndef PIXELTYPE_H
#define PIXELTYPE_H

#include <cfloat>
#include "math3d/vmath.h"

namespace bigimage {
// *****************************************************************************
// Pixel types
// *****************************************************************************

struct PixelTypeU32 {
    typedef uint32_t pixel_val_t;
    typedef uint32_t pixel_comp_t;
    const int kChannels = 1;
    const pixel_comp_t kMaxValue = 0xFFFFFFFF;
    const pixel_comp_t kMinValue = 0x00000000;
    const pixel_comp_t kWhiteValue = 0xFFFFFFFF;
    const pixel_comp_t kBlackValue = 0x00000000;
};

struct PixelTypeRGBA32 {
    typedef uint32_t pixel_val_t;
    typedef uint8_t pixel_comp_t;
    const int kChannels = 4;
    const pixel_comp_t kMaxValue = 0xFF;
    const pixel_comp_t kMinValue = 0x00;
    const pixel_comp_t kWhiteValue = 0xFF;
    const pixel_comp_t kBlackValue = 0x00;
};

// struct PixelTypeRGB24 {
//     typedef uint32_t pixel_val_t;
//     typedef uint8_t pixel_comp_t;
//     const int k = 3;
//     const int kChannels = 3;
//     const pixel_comp_t kMaxValue = 0xFF;
//     const pixel_comp_t kMinValue = 0x00;
//     const pixel_comp_t kWhiteValue = 0xFF;
//     const pixel_comp_t kBlackValue = 0x00;
// };

struct PixelTypeRGBAf {
    typedef float4 pixel_val_t;
    typedef float pixel_comp_t;
    const int kChannels = 4;
    const pixel_comp_t kMaxValue = FLT_MAX;
    const pixel_comp_t kMinValue = FLT_MIN;
    const pixel_comp_t kWhiteValue = 1.0f;
    const pixel_comp_t kBlackValue = 0.0f;
};


template<typename dpixT, typename spixT>
void CopyPixels(typename dpixT::pixel_val_t * dst, const typename spixT::pixel_val_t * src, size_t len);

template<typename dpixT, typename spixT>
void CopyPixel(typename dpixT::pixel_val_t & dst, const typename spixT::pixel_val_t & src);


template<>
void CopyPixels<PixelTypeRGBA32, PixelTypeRGBA32>(uint32_t * dst, const uint32_t * src, size_t len)
{
    memcpy(dst, src, len*sizeof(uint32_t));
}


template<>
void CopyPixel<PixelTypeRGBA32, PixelTypeRGBAf>(uint32_t & dst, const float4 & src) {
    dst = (((uint32_t)(src[0]*255) << 0) & 0x000000FF) |
          (((uint32_t)(src[1]*255) << 8) & 0x0000FF00) |
          (((uint32_t)(src[2]*255) << 16) & 0x00FF0000) |
          (((uint32_t)(src[3]*255) << 24) & 0xFF000000);
}

template<>
void CopyPixels<PixelTypeRGBA32, PixelTypeRGBAf>(uint32_t * dst, const float4 * src, size_t len) {
    for(int32_t p = 0; p < len; ++p)
        CopyPixel<PixelTypeRGBA32, PixelTypeRGBAf>(*dst++, *src++);
}


} // namespace bigimage
#endif // PIXELTYPE_H
