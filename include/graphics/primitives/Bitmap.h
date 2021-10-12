#ifndef BITMAP_H
#define BITMAP_H

#include <graphics/primitives/raster/ImgLoader.h>

#include <string>
#include <vector>

namespace rsp::graphics
{

/**
 * Class Bitmap
 *
 * The Bitmap is an object wrapper around raster images.
 * Various raster image formats can be implemented by descending specialized loaders
 * from the ImgLoader class and adding those to the GetRasterLoader method.
 */
class Bitmap
{
  public:
    /**
     * Load bitmap from given file.
     *
     * \param aImgName
     */
    Bitmap(std::string aImgName);

    /**
     * Create a bitmap of given pixel data
     *
     * \param apPixels
     * \param aHeight
     * \param aWidth
     * \param aBytesPerPixel
     */
    Bitmap(const uint32_t *apPixels, int aHeight, int aWidth, int aBytesPerPixel);

    /**
     * Create an empty in memory bitmap
     *
     * \param aHeight
     * \param aWidth
     * \param aBytesPerPixel
     */
    Bitmap(int aHeight, int aWidth, int aBytesPerPixel);

    /**
     * Why?
     */
    ~Bitmap();

    /**
     * Get the height of the bitmap.
     *
     * \return uint32_t
     */
    uint32_t GetHeight() const
    {
        return mHeight;
    }

    /**
     * Get the width of the bitmap.
     *
     * \return uint32_t
     */
    uint32_t GetWidth() const
    {
        return mWidth;
    }

    /**
     * Get a read only reference to the pixel data.
     *
     * \return const std::vector<uint32_t>&
     */
    const std::vector<uint32_t> &GetPixels() const
    {
        return mImagePixels;
    }

  protected:
    ImgLoader &GetRasterLoader(const std::string aFileExtension);

    uint32_t mHeight;
    uint32_t mWidth;
    uint16_t mBytesPerPixel;            //Unused
    std::vector<uint32_t> mImagePixels; //Pointer?
};

} // namespace rsp::graphics
#endif //BITMAP_H
