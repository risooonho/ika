
#ifndef IMAGEBANK_H
#define IMAGEBANK_H

#include "types.h"
#include <set>

class Image;
class Canvas;

// abstract image bank class.
// Subclassed for VSPs, fonts and such, so they can have hardware - specific copies
// that stay sync'd.
class ImageBank
{
protected:
    std::vector<Image*> bitmaps;
    std::set<int> altered;

    virtual void SetImage(const Canvas& img, uint idx) = 0;
public:
    virtual ~ImageBank();

    void Sync();
    void SyncAll();
    void FreeBitmaps();

    virtual Canvas& Get(uint idx) = 0;
    void Set(const Canvas& img, uint idx);

    virtual uint Count() const = 0;

    Image& GetImage(uint idx);
};

#endif