
// TODO: Make a second implementation of this stuff, and make these three classes into interfaces, with their existing implementation
// moved to GLGraphFactory, GLGraphFrame, and GLImage, respectively.

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include "types.h"
#include "pixel_matrix.h"
#include <wx\wx.h>
#include <wx\glcanvas.h>

class CGraphFrame;
class CImage;

class CGraphFactory
{
public:
    CGraphFrame* CreateFrame(wxWindow* parent);
    CImage* CreateImage(const CPixelMatrix& srcdata);
};

class CGraphFrame : public wxGLCanvas
{
public:

    CGraphFrame(wxWindow* parent);
    ~CGraphFrame();

    void Rect(int x,int y,int w,int h,RGBA colour);
    void RectFill(int x,int y,int w,int h,RGBA colour);

    void Blit(CImage& src,int x,int y,bool trans);
    void ScaleBlit(CImage& src,int x,int y,int w,int h,bool trans);

    void ShowPage();

    void OnErase(wxEraseEvent&) {}

    DECLARE_EVENT_TABLE()
};

class CImage
{
    friend CGraphFrame; // :x
protected:
    GLuint hTex;
    int nWidth,nHeight;

public:
    CImage(const CPixelMatrix& src);
    ~CImage();
};

#endif