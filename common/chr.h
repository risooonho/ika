
#ifndef CHR_H
#define CHR_H

#include "types.h"
#include "Canvas.h"

/**
 *  Hardware independant representation of a sprite.
 *
 *  Note that the data structure allows for each frame to have its own dimensions.
 *  The file format does too, however we won't be actually implementing this yet.
 */
class CCHRfile
{
    vector<Canvas>    frame;                    ///< frame data

    int        nWidth, nHeight;
    int        nHotx, nHoty;                    ///< hotspot position
    int        nHotw, nHoth;                    ///< hotspot width / height

    void PackData(u8* data, int& size);
    void UnpackData(u8* data, int size);

public:
    std::vector<std::string>  sMovescript;      ///< movement scripts
    std::string     sDescription;               ///< authoring information?  I dunno, but it's here.
    
    CCHRfile(int width = 16, int height = 16);

    Canvas& GetFrame(int frame) const;
    void UpdateFrame(const Canvas& newdata, int nFrame);
    inline int Width(int nFrame = 0)  const   {   return nWidth;  }
    inline int Height(int nFrame = 0) const   {   return nHeight; }
    inline int NumFrames()            const   {   return frame.size();            }
    
    inline int& HotX(int frame = 0)           {   return nHotx;   }       
    inline int& HotY(int frame = 0)           {   return nHoty;   }     ///< Hotspot position
    inline int& HotW(int frame = 0)           {   return nHotw;   }
    inline int& HotH(int frame = 0)           {   return nHoth;   }     ///< Hotspot size

    void AppendFrame();                                                 ///< Adds a new, empty frame.
    void InsertFrame(int i);                                            ///< Inserts a new, empty frame at the specified position.
    void InsertFrame(int i, Canvas& p);                                 ///< Inserts the image as a new frame at the specified position.
    void DeleteFrame(int i);                                            ///< Removes the specified frame.

    void Resize(int width, int height);                                 ///< Resize all the frames.

    void New(int framex, int framey);                                   ///< Creates a new sprite of the specified dimensions.
    bool Load(const char* fname);                                       ///< Loads sprite data from a file
    void Save(const char* fname);                                       ///< Writes the sprite data to a file.
    void SaveOld(const char* fname);                                    ///< Writes the sprite data to a file, in VERGE's CHR format.

private:
    bool Loadv2CHR(class File& f);
    bool Loadv4CHR(class File& f);
};

#endif
