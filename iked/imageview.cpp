
#include "imageview.h"
#include "graph.h"

BEGIN_EVENT_TABLE(CImageView,IDocView)
    EVT_PAINT(CImageView::OnPaint)
END_EVENT_TABLE()

CImageView::CImageView(CMainWnd* parent,CPixelMatrix* img)
    : IDocView(parent,"image"), pData(img)
{
    //SetSize(-1,-1,340,340);

    pGraph=new CGraphFrame(this);
    pGraph->SetSize(GetClientSize());

    pImage=new CImage(*img);
}

CImageView::~CImageView()
{
    delete pImage;
}

void CImageView::OnPaint()
{
    wxPaintDC bleh(this);

    pGraph->SetCurrent();
    pGraph->Clear();
    pGraph->ShowPage();
}

void CImageView::OnSave(wxCommandEvent&)
{
}