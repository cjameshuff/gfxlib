
#ifndef IMAGEPROC_H
#define IMAGEPROC_H

namespace bigimage {

// *****************************************************************************
// ImageProcJob
// *****************************************************************************

// Workflow:
// Setup:
// For each output tile, job increments reference counter in all required input tiles.
// Execution:
// Jobs write output to temporary files and register these in a map.
// On completion of execution for a tile, job decrements reference counters. When a
// reference count reaches zero, the corresponding result tile is looked up and moved
// into its location.
// 
// Notes:
// With block-allocated tiles, the final move is an actual memcpy() of pixel data.
// With a more flexible allocator that doesn't enforce locality, it could be a copy
// of a pointer.

template<typename imageT>
class ImageProcJob {
  protected:
    void Setup(imageT & image);
    
  public:
    ImageProcJob();
    ~ImageProcJob();
    
    void Execute(imageT & image);
};

template<typename imageT>
ImageProcJob<imageT>::ImageProcJob() {}

template<typename imageT>
ImageProcJob<imageT>::~ImageProcJob() {}

template<typename imageT>
auto ImageProcJob<imageT>::Setup(imageT & image) -> void {
    // Mark all tiles required by job
}

template<typename imageT>
auto ImageProcJob<imageT>::Execute(imageT & image) -> void {
    Setup(image);
}

} // namespace bigimage
#endif // IMAGEPROC_H
