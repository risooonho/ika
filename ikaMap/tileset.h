
#ifndef TILESET_H
#define TILESET_H

#include "types.h"
#include <set>
#include "imagebank.h"
//#include <wx\wx.h>


class VSP;

class TileSet : public ImageBank
{
private:
    VSP* pVsp;

    virtual void SetImage(const Canvas& img, uint idx);

public:

    TileSet();
    ~TileSet();

    bool Load(const std::string& fileName);
    bool Save(const std::string& fileName);
    void New(int width, int height);

    virtual Canvas&   Get(uint idx);                // returns the pixel data for the tile
    virtual uint   Count() const;
    int  Width() const;
    int  Height() const;

    void AppendTile();
    void AppendTile(Canvas& c);
    void InsertTile(uint pos);
    void InsertTile(uint pos, Canvas& c);
    void DeleteTile(uint pos);

    inline const VSP& GetVSP() const { return *pVsp; }
};

#endif