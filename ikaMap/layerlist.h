
#ifndef LAYERLIST_H
#define LAYERLIST_H

#include <vector>

#include "common/utility.h"
#include "common/listener.h"
#include "events.h"
#include "wxinc.h"

struct Map;
struct Executor;
class wxWindow;

struct LayerText : public wxStaticText {
    LayerText(wxWindow* parent);

    void OnMouseDown(wxMouseEvent& event);

    DECLARE_EVENT_TABLE();
};


/**
 * Displays information about a single map layer.
 * Draws pretty icons next to the name of the layer.
 */
struct LayerBox : public wxWindow {
    LayerBox(wxWindow* parent, wxPoint = wxDefaultPosition, wxSize = wxDefaultSize); // also temp

    void SetVisibilityIcon(wxIcon& bmp);
    void SetActiveIcon(wxIcon& icon);

    void SetLabel(const std::string& label);

    void DoToggleVisibility(wxCommandEvent& event);
    void DoActivateLayer(wxMouseEvent& event);
    void DoContextMenu(wxContextMenuEvent& event);

    DECLARE_EVENT_TABLE();

private:
    wxBitmapButton* _visibilityIcon;
    wxBitmapButton* _activeIcon;
    wxStaticText*   _label;
};

/**
 * Contains a list of LayerBoxes.  Also scrolls and stuff.
 */
struct LayerList : public wxScrolledWindow {
    LayerList(Executor* executor, wxWindow* parent, wxPoint position = wxDefaultPosition, wxSize size = wxDefaultSize);

    // These happen in response to events from executor.
    void OnMapLayersChanged(const MapEvent& event);
    void OnVisibilityChanged(const MapEvent& event);
    void OnLayerActivated(uint index);

    // These we recieve from the LayerBoxes.
    void OnToggleVisibility(wxCommandEvent& event);
    void OnActivateLayer(wxMouseEvent& event);
    void OnShowContextMenu(wxContextMenuEvent& event);

    // Menu event handlers
    void OnEditLayerProperties(wxCommandEvent&);
    void OnShowOnly(wxCommandEvent&);
    void OnShowAll(wxCommandEvent&);
    void OnCloneLayer(wxCommandEvent&);
    void OnDeleteLayer(wxCommandEvent&);

    void Update(Map* map);
    void UpdateIcons();

    DECLARE_EVENT_TABLE();

private:

    std::vector<LayerBox*> _boxes;  // little UI widget box thingies.
    Executor* _executor;            // bleh. -_-;

    wxBoxSizer* _sizer;

    ScopedPtr<wxMenu> _contextMenu;
    int _contextMenuIndex;          // needed to determine which layer is the subject of a context menu command.  -1 if not needed at the moment.

    wxIcon _visibleIcon;
    wxIcon _activeIcon;
    wxIcon _blankIcon;
};

#endif
