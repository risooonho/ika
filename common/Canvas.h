#ifndef PIXEL_MATRIX_H
#define PIXEL_MATRIX_H

#include "types.h"

/*!
    Canvases are purely software representations of images.  Nothing more.
    Any time one is manipulating graphics for more than just user-feedback (editing image files, and such)
    these are used.

    TODO: Make all this stuff more robust and optimized, so that gfx_soft uses these directly.
          Less code doing more work. ^_~
*/
class Canvas
{
private:
    RGBA* _pixels;                                              ///< Pointer to raw pixel data
    int   _width, _height;                                      ///< Dimensions
    
    Rect _cliprect;                                             ///< Operations are restricted to this region of the image.
    
public:
    // con/destructors
    Canvas();
    Canvas(int width,int height);
    Canvas(u8* pData,int nWidth,int nHeight,u8* pal);
    Canvas(RGBA* pData,int nWidth,int nHeight);
    Canvas(const Canvas& src);
    Canvas(const char* fname);
    virtual ~Canvas(); // why did I make this virtual? O_o

    void Save(const char* fname);
    
    // The basics
    void CopyPixelData(u8* data,int width,int height,u8* pal);  ///< Copies raw, palettized pixel data into the image
    void CopyPixelData(RGBA* data,int width,int height);        ///< Copies raw RGBA pixel data into the image
    Canvas& operator = (const Canvas& rhs);         ///< Copies one image into another.
    bool operator == (const Canvas& rhs);                 ///< Returns true if the images have the same dimensions, and contain the same data. (SLOW!)
    
    // Accessors
    inline const int& Width()   const { return _width;  }
    inline const int& Height()  const { return _height; }
    inline RGBA* GetPixels()    const { return _pixels; }
    RGBA GetPixel(int x,int y);
    void SetPixel(int x,int y,RGBA c);

    // Misc junk
    void Clear(RGBA colour);
    void Rotate();
    void Flip();
    void Mirror();
    void Resize(int x,int y);
    
    const Rect& GetClipRect() const { return _cliprect; }
    void SetClipRect(const Rect& r);
};

#include "CanvasBlitter.h"

#endif