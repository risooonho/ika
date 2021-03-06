
#include "imagearraypanel.h"
#include "imagebank.h"
#include "common/listener.h"

namespace iked {

    BEGIN_EVENT_TABLE(ImageArrayPanel, GraphicsFrame)
        EVT_PAINT(ImageArrayPanel::onPaint)
        EVT_ERASE_BACKGROUND(ImageArrayPanel::onEraseBackground)
        EVT_LEFT_UP(ImageArrayPanel::onLeftClick)
        EVT_RIGHT_UP(ImageArrayPanel::onRightClick)
        EVT_LEFT_DCLICK(ImageArrayPanel::onDoubleClick)

        EVT_SCROLLWIN(ImageArrayPanel::onScroll)
        EVT_SIZE(ImageArrayPanel::onSize)
    END_EVENT_TABLE()

    ImageArrayPanel::ImageArrayPanel(wxWindow* parent, ImageArrayDocument* doc)
        : GraphicsFrame(parent)
        , document(doc)
        , selectedImage(0)
        , scrollPos(0)
        , imagePadSize(0)
    {
        document->destroyed.add(bind(this, &ImageArrayPanel::onDocumentDestroyed));
        Refresh();
    }

    int ImageArrayPanel::getImageAtPos(int x, int y) const {
        y += scrollPos;

        int xstep = document->getWidth() + imagePadSize;
        int ystep = document->getHeight() + imagePadSize;
        int cols = max(LogicalWidth() / xstep, 1);

        x /= xstep;
        y /= ystep;

        int index = y * cols + x;

        if (0 <= index && index < document->getCount()) {
            return index;
        } else {
            return -1;
        }
    }

    void ImageArrayPanel::getImagePos(int index, int* x, int* y) const {
        if (index > document->getCount()) {
            *x = *y = -1;
        } else {
            uint frameWidth  = document->getWidth()  + (imagePadSize ? 1 : 0);
            uint frameHeight = document->getHeight() + (imagePadSize ? 1 : 0);
            uint framesPerRow = getNumCols();

            *y = (index / framesPerRow) * frameHeight - scrollPos;
            *x = (index % framesPerRow) * frameWidth;
        }
    }

    int ImageArrayPanel::getSelectedImage() const {
        return selectedImage;
    }

    void ImageArrayPanel::onSize(wxSizeEvent& event) {
        updateScrollbar();
        GraphicsFrame::OnSize(event);
        Refresh();
    }

    void ImageArrayPanel::onScroll(wxScrollWinEvent& event) {
        if (event.m_eventType == wxEVT_SCROLLWIN_TOP) {
            scrollPos = 0;
        } else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM) {
            // guaranteed to be too big.  Will be corrected in updateScrollbar
            scrollPos = document->getCount();
        } else if (event.m_eventType == wxEVT_SCROLLWIN_LINEUP) {
            scrollPos--;
        } else if (event.m_eventType == wxEVT_SCROLLWIN_LINEDOWN) {
            scrollPos++;
        } else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEUP) {
            scrollPos -= GetScrollThumb(wxVERTICAL);
        } else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEDOWN) {
            scrollPos += GetScrollThumb(wxVERTICAL);
        } else {
            scrollPos = event.GetPosition();
        }

        updateScrollbar();
        Refresh();
    }

    void ImageArrayPanel::onLeftClick(wxMouseEvent& event) {
        GraphicsFrame::OnMouseEvent(event);
        wxPoint p = event.GetPosition();

        int index = getImageAtPos(p.x, p.y);
        if (index != -1 && index != selectedImage) {
            selectedImage = index;
            leftClickImage.fire(index);
            Refresh();
        }
    }

    void ImageArrayPanel::onRightClick(wxMouseEvent& event) {
        GraphicsFrame::OnMouseEvent(event);
        wxPoint p = event.GetPosition();

        int index = getImageAtPos(p.x, p.y);
        if (index != -1) {
            selectedImage = index;
            Refresh();
            rightClickImage.fire(index);
        }
    }

    void ImageArrayPanel::onDoubleClick(wxMouseEvent& event) {
        GraphicsFrame::OnMouseEvent(event);
        wxPoint p = event.GetPosition();

        int index = getImageAtPos(p.x, p.y);
        if (index != -1) {
            selectedImage = index;
            Refresh();
            doubleClickImage.fire(index);
        }
    }
    
    void ImageArrayPanel::onPaint(wxPaintEvent& event){
        wxPaintDC paintDC(this);

        if (!document) {
            // This sometimes happens during the constructor
            return;
        }

        int width = LogicalWidth();

        int framex = document->getWidth();
        int framey = document->getHeight();

        int xstep = framex + imagePadSize;
        int ystep = framey + imagePadSize;

        int yadjust = scrollPos % ystep;

        int cols = max(1, width / xstep);

        SetCurrent();
        Clear();

        int x = 0;
        int y = 0;
        int index = (scrollPos / ystep) * cols;
        while (index < document->getCount()) {
            Blit(document->getImage(index), x * xstep, y * ystep - yadjust, true);

            index++;
            x++;
            if (x >= cols) {
                x = 0;
                y++;
            }
        }

        int x2, y2;
        framex = document->getWidth();
        framey = document->getHeight();
        getImagePos(selectedImage, &x2, &y2);

        Rect(x2, y2, framex + 1, framey + 1, RGBA(255, 255, 255));

        ShowPage();
    }

    void ImageArrayPanel::onDocumentDestroyed(Document* doc) {
        if (doc == document) {
            document = 0;
        }
    }

    void ImageArrayPanel::updateScrollbar() {
        SetScrollbar(wxVERTICAL, scrollPos, LogicalHeight(), getNumRows() * document->getHeight());
    }

    int ImageArrayPanel::getNumCols() const {
        return max(1, LogicalWidth() / (document->getWidth() + imagePadSize));
    }

    int ImageArrayPanel::getNumRows() const {
        return max(1, (document->getCount() + getNumCols() / 2) / getNumCols());
    }

}
