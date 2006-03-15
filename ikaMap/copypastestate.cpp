
#include "copypastestate.h"
#include "command.h"
#include "mapview.h"
#include "common/map.h"
#include "tileset.h"

/*
 * Behaviour:
 *     shift+left-clicks anywhere
 *         Add state: allow the user to drag a rect, and add it to the current selection.
 *     ctrl+left-click anywhere
 *         Subtract state: drag a rect.  Remove that from the selection.
 *     left-click somewhere not selected
 *         Add state.
 *     left-click somewhere selected
 *         Switch to "move" state: allow the user to drag the selection around to move it.
 */

CopyPasteState::CopyPasteState(Executor* e)
    : EditState(e)
    , _state(NoneState)
    , _dragging(false)
    , _selX(0)
    , _selY(0)
    , _dragX(0)
    , _dragY(0)
    , _offsetX(0)
    , _offsetY(0)
{
    _tiles.tiles.Resize(0, 0);
}

void CopyPasteState::OnMouseDown(wxMouseEvent& event) {
    const Tileset* ts = GetTileset();
    const Map::Layer* curLayer = GetCurLayer();
    MapView* mv = GetMapView();

    int x = event.GetX();
    int y = event.GetY();
    mv->ScreenToTile(x, y);

#if 1
    if (event.LeftDown()) {
        _selection = Rect(x, y, x, y);
        if (event.ShiftDown()) {
            _state = AddState;
        } else if (event.ControlDown()) {
            _state = SubtractState;
        } else {
            return;
        }
    }
#else
    if (event.LeftDown()) {
        if (!_tiles.Empty()) {
            int x = _selX;
            int y = _selY;
            int x2 = x + _tiles.Width();
            int y2 = y + _tiles.Height();
            mv->TileToScreen(x, y);
            mv->TileToScreen(x2, y2);

            // See if we're dragging within or without the selection rectangle.
            if (clamp<int>(event.GetX(), x, x2) == event.GetX() &&
                clamp<int>(event.GetY(), y, y2) == event.GetY()
            ) {
                // If within, then we start moving it around.
                _offsetX = event.GetX();
                _offsetY = event.GetY();
            } else {
                // If without, then we dump the selection where it is, clear the selection rect, and let
                //   control flow to the second section, below. (thereby allowing the user to grab a new
                //   selection)
                Paste();
            }
        }

        // If there is no selection, or the mouse cursor is outside the old (now purged) tile buffer
        if (_tiles.Empty() && !_dragging) {
            // Begin the drag state.
            _selection.left = event.GetX();
            _selection.top = event.GetY();

            mv->ScreenToTile(_selection.left, _selection.top);
            _selection.right = _selection.left;
            _selection.bottom = _selection.top;

            _dragging = true;
        }

    } else if (event.RightDown() && _tiles.Empty()) {
        // Paste the clipboard, baby.
        _selX = event.GetX();
        _selY = event.GetY();
        mv->ScreenToTile(_selX, _selY);
        _tiles = _clipboard;
        _selection = Rect(_selX, _selY, _tiles.Width(), _tiles.Height());
        mv->Refresh();
    }
#endif
}

void CopyPasteState::OnMouseUp(wxMouseEvent& event) {
#if 1
    Map::Layer* lay = GetCurLayer();

    if (_state == AddState) {
        _state = NoneState;
        _selection.Normalize();
        
        // Figure out the minimum rect that can contain the union 
        // of the dragged rect and the existing selection.
        Rect newClip;
        if (_tiles.tiles.Empty()) {
            newClip = _selection;
        } else {
            newClip.left = min(_selection.left, _selX);
            newClip.right = max(_selection.right, _selX + _tiles.Width());
            newClip.top = min(_selection.top, _selY);
            newClip.bottom = max(_selection.bottom, _selY + _tiles.Height());
        }

        int ofsX = _selX - newClip.left;
        int ofsY = _selY - newClip.top;

        // FIXME: this is inefficient.

        Brush newBrush;
        newBrush.tiles.Resize(newClip.Width(), newClip.Height());

        // Copy the old bit over.
        for (uint y = 0; y < _tiles.Height(); y++) {
            for (uint x = 0; x < _tiles.Width(); x++) {
                Brush::Tile& t = newBrush.tiles(x + ofsX, y + ofsY);
                Brush::Tile& u = _tiles.tiles(x, y);
                newBrush.tiles(x + ofsX, y + ofsY) = _tiles.tiles(x, y);
            }
        }

        // Add the new stuff.
        for (uint y = _selection.top; y < _selection.bottom; y++) {
            for (uint x = _selection.left; x < _selection.right; x++) {
                Brush::Tile& t = newBrush.tiles(x - newClip.left, y - newClip.top);
                t.index = lay->tiles(x, y);
                t.mask = true;
            }
        }

        _tiles = newBrush;
        _selX = newClip.left;
        _selY = newClip.top;

        GetMapView()->Refresh();

    } else if (_state == SubtractState) {
        _state = NoneState;

        // Easier to remove than add.  Find the overlap, and flip a few mask bits.
        int xofs = _selection.left - _selX;
        int yofs = _selection.top - _selY;

        for (int y = 0; y < _selection.Height(); y++) {
            for (int x = 0; x < _selection.Width(); x++) {
                int sx = x + xofs;
                int sy = y + yofs;
                if (sx == clamp<int>(sx, 0, _tiles.Width()) &&
                    sy == clamp<int>(sy, 0, _tiles.Height())
                ) {
                    _tiles.tiles(sx, sy).mask = false;
                }
            }
        }

        GetMapView()->Refresh();
    }
#else
    if (!_tiles.Empty()) {
        // Update this so the dragging stuff knows where we're dragging from.
        _selection.left = _selX;
        _selection.right = _selX + _tiles.Width();
        _selection.top = _selY;
        _selection.bottom = _selY + _tiles.Height();

        // nothing, I think.  We move the selection around in OnMouseMove
        // Maybe send a command so that undo/redo will move the selection from wherever it was when
        // the mouse button was pushed, to where it is now that it's been released.
    } else if (_dragging) {
        Map::Layer* layer = GetCurLayer();

        _dragging = false;

        // Grab the tiles, put them in our clipboard and erase them from the map.
        // TODO: move this into a command

        _selection.Normalize();
        _selX = _selection.left;
        _selY = _selection.top;

        if (_selection.Width() > 1 || _selection.Height() > 1) {
            _tiles.Resize(_selection.Width(), _selection.Height());
            for (uint y = 0; y < _tiles.Height(); y++) {
                const uint sourceY = y + _selection.top;
                uint sourceX = _selection.left;

                for (uint x = 0; x < _tiles.Width(); x++) {
                    _tiles(x, y) = layer->tiles(sourceX, sourceY);
                    sourceX++;
                }
            }

            Matrix<uint> tempMat(_tiles.Width(), _tiles.Height());
            HandleCommand(new PasteTilesCommand(_selX, _selY, GetCurLayerIndex(), tempMat));

            _clipboard = _tiles;    // save this for duplication goodness
        }
        // If there's no selection rect, we just un-select.
        // An unfortunate side effect of the way things are set up is that you cannot copy/paste a single tile.
        // BOO FUCKING HOO.  Use the painter for that. ;)

        GetMapView()->Refresh();
    }
#endif
}

void CopyPasteState::OnMouseMove(wxMouseEvent& event) {
    //if (!event.LeftIsDown())    return; // nothing to do

    /*if (!_dragging) {
        int deltaX = (event.GetX() - _offsetX) / GetTileset()->Width();
        int deltaY = (event.GetY() - _offsetY) / GetTileset()->Height();

        // since _selection still holds the initial position of the selection, we can just use it.
        _selX = _selection.left + deltaX;
        _selY = _selection.top + deltaY;
        // Move the layer to wherever it's been dragged.

        GetMapView()->Refresh();
    } else */

    if (_state != NoneState) {
        _selection.right = event.GetX();
        _selection.bottom = event.GetY();

        GetMapView()->ScreenToTile(_selection.right, _selection.bottom);

        if (_selection.right >= _selection.left)    _selection.right++;
        if (_selection.bottom >= _selection.top)    _selection.bottom++;

        GetMapView()->Refresh();
        // Update the other corner of the selection rect.
    }
}

void CopyPasteState::OnRenderCurrentLayer() {
    const Tileset* ts = GetTileset();
    const Map::Layer* curLayer = GetCurLayer();
    MapView* mv = GetMapView();

    /*if (!_tiles.Empty()) {
        // Draw floating selection
        const int x = _selX * ts->Width()  + curLayer->x - mv->GetXWin();
        const int y = _selY * ts->Height() + curLayer->y - mv->GetYWin();

        mv->RenderBrush(_tiles, -x, -y);

        mv->GetVideo()->DrawRect(
            x, y,
            _tiles.Width() * ts->Width(),  _tiles.Height() * ts->Height(),
            RGBA(255, 255, 255)
        );

    } else if (_dragging) */ 
    
    if (!_tiles.tiles.Empty()) {
        int xofs = _selX;
        int yofs = _selY;
        mv->TileToScreen(xofs, yofs);
        mv->RenderBrush(_tiles, xofs, yofs);
    }

    if (_state != NoneState) {
        // Draw the current selection rectangle
        int x = _selection.left * ts->Width() - mv->GetXWin() + curLayer->x;
        int y = _selection.top * ts->Height() - mv->GetYWin() + curLayer->y;
        int w = _selection.Width() * ts->Width();
        int h = _selection.Height() * ts->Height();

        mv->GetVideo()->DrawRectFill(x, y, w, h, RGBA(255, 255, 255, 128));
        mv->GetVideo()->DrawRect    (x, y, w, h, RGBA(0, 255, 255));
    }
}

void CopyPasteState::OnEndState() {
    Paste();
}

void CopyPasteState::Paste() {
    if (_tiles.Empty()) return; // :P

    HandleCommand(new PasteBrushCommand(_selX, _selY, GetCurLayerIndex(), _tiles));
    _tiles.tiles.Resize(0, 0);
}