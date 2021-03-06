#include <limits>

#include "command.h"
#include "layerlist.h"
#include "mainwindow.h"
#include "common/map.h"
#include "common/log.h"

namespace {
    enum {
        id_showLayer = wxID_HIGHEST + 1,
        id_activateLayer,
        id_editLayerProperties,
        id_showOnly,
        id_showAll,
        id_cloneLayer,
        id_deleteLayer
    };
}


BEGIN_EVENT_TABLE(LayerText, wxStaticText)
    EVT_LEFT_DOWN(LayerText::OnMouseDown)
END_EVENT_TABLE()

LayerText::LayerText(wxWindow* parent)
    : wxStaticText(parent, -1, "-")
{
}

void LayerText::OnMouseDown(wxMouseEvent& event)
{
    Log::Write("Activate layer at layer text.");
    GetParent()->ProcessEvent(event);
    //event.Skip();
}


BEGIN_EVENT_TABLE(LayerBox, wxWindow)
    EVT_LEFT_DOWN(LayerBox::DoActivateLayer)
    EVT_BUTTON(id_showLayer, LayerBox::DoToggleVisibility)
    EVT_CONTEXT_MENU(LayerBox::DoContextMenu)
END_EVENT_TABLE()

LayerBox::LayerBox(wxWindow* parent, wxPoint position, wxSize size)
    : wxWindow(parent, -1, position, size, wxSUNKEN_BORDER)
{
    wxIcon blankIcon("blankicon", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);

    const long style = 0;

    _visibilityIcon = new wxBitmapButton(this, id_showLayer, blankIcon, wxDefaultPosition, wxSize(16, 16), style);
    _activeIcon     = new wxBitmapButton(this, id_activateLayer, blankIcon, wxDefaultPosition, wxSize(16, 16), style);
    _label = new LayerText(this);

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(_visibilityIcon, 0, wxBOTTOM, 4);
    sizer->Add(_activeIcon, 0, wxBOTTOM, 4);
    sizer->Add(_label, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);
    SetSizer(sizer);
    sizer->Fit(this);
}

void LayerBox::SetVisibilityIcon(wxIcon& icon) {
    _visibilityIcon->SetLabel(icon);
}

void LayerBox::SetActiveIcon(wxIcon& icon) {
    _activeIcon->SetLabel(icon);
}

void LayerBox::DoToggleVisibility(wxCommandEvent& event) {
    event.SetInt(GetId());
    event.Skip();
}

void LayerBox::DoActivateLayer(wxMouseEvent& event) {
    event.SetId(GetId());
    GetParent()->ProcessEvent(event);
    //event.Skip();
}

void LayerBox::DoContextMenu(wxContextMenuEvent& event) {
    // Write our layer index to the event, then skip, so the enclosing control can recieve it.
    event.SetInt(GetId());
    event.Skip();
}

void LayerBox::SetLabel(const std::string& label) {
    _label->SetLabel(label.c_str());
    Layout();
}

namespace {
    enum {
        id_properties
    };
}



BEGIN_EVENT_TABLE(LayerList, wxScrolledWindow)
    EVT_LEFT_DOWN(LayerList::OnActivateLayer)
    EVT_BUTTON(id_showLayer, LayerList::OnToggleVisibility)
    EVT_CONTEXT_MENU(LayerList::OnShowContextMenu)

    EVT_MENU(id_editLayerProperties, LayerList::OnEditLayerProperties)
    EVT_MENU(id_showOnly, LayerList::OnShowOnly)
    EVT_MENU(id_showAll, LayerList::OnShowAll)
    EVT_MENU(id_cloneLayer, LayerList::OnCloneLayer)
    EVT_MENU(id_deleteLayer, LayerList::OnDeleteLayer)
END_EVENT_TABLE()

LayerList::LayerList(Executor* executor, wxWindow* parent, wxPoint position, wxSize size)
    : wxScrolledWindow(parent, -1, position, size)
    , _executor(executor)
    , _visibleIcon("eyeicon", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16)
    , _activeIcon("brushicon", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16)
    , _blankIcon("blankicon", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16)
{
    _sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(_sizer);
    Layout();
    SetScrollRate(0, 1);

    // Create layer right-click menu here.
    _contextMenu = new wxMenu();
    _contextMenu->Append(id_showOnly, "Show &Only", "Hide all layers except this one.");
    _contextMenu->Append(id_showAll, "Show &All", "Unhide all layers.");
    _contextMenu->AppendSeparator();

    _contextMenu->Append(id_cloneLayer, "&Clone Layer", "Create an exact duplicate of this layer.");
    _contextMenu->Append(id_deleteLayer, "&Delete Layer", "Delete this layer.");
    _contextMenu->AppendSeparator();

    _contextMenu->Append(id_editLayerProperties, "&Properties", "Edit the properties of this layer.");
}

void LayerList::OnMapLayersChanged(const MapEvent& event) {
    // probably not a good idea.  Better to only do this when
    // layers have been moved around, destroyed, created, etc.
    // But what the fuck.
    Update(event.map);
}

void LayerList::OnVisibilityChanged(const MapEvent& event) {
    Freeze();
    UpdateIcons();
    Thaw();
}

void LayerList::OnLayerActivated(uint index) {
    Freeze();
    UpdateIcons();
    Thaw();
}

void LayerList::OnToggleVisibility(wxCommandEvent& event) {
    uint layerIndex = event.GetInt();

    wxASSERT(layerIndex < _executor->GetMap()->NumLayers());

    bool b = !_executor->IsLayerVisible(layerIndex);
    _executor->ShowLayer(layerIndex, b);
}

void LayerList::OnActivateLayer(wxMouseEvent& event) {
    Log::Write("Activate layer at layer list");

    uint layerIndex = event.GetId();

    if (layerIndex < _executor->GetMap()->NumLayers()) {
        _executor->SetCurrentLayer(layerIndex);
    }
}

void LayerList::OnShowContextMenu(wxContextMenuEvent& event) {
    _contextMenuIndex = event.GetId();
	wxASSERT(_boxes.size() <= (std::numeric_limits<signed int>::max)());
    if (_contextMenuIndex != -1 && _contextMenuIndex < _boxes.size()) {
        // open the menu iff we have a valid layer index.
        PopupMenu(_contextMenu.get(), ScreenToClient(::wxGetMousePosition()));
    } else {
        _contextMenuIndex = -1;
    }
}

void LayerList::OnEditLayerProperties(wxCommandEvent& event) {
    wxASSERT(_contextMenuIndex != -1);
    _executor->EditLayerProperties(_contextMenuIndex);
    _contextMenuIndex = -1;
}

void LayerList::OnShowOnly(wxCommandEvent&) {
    wxASSERT(_contextMenuIndex != -1);

    const unsigned int layerCount = _executor->GetMap()->NumLayers();
	wxASSERT(layerCount != (std::numeric_limits<unsigned int>::max)());

    _executor->SetCurrentLayer(_contextMenuIndex);

    for (unsigned int i = 0; i < layerCount; i++) {
        if (i != _contextMenuIndex) {
            _executor->ShowLayer(i, false);
        }
    }

    _contextMenuIndex = -1;
}

void LayerList::OnShowAll(wxCommandEvent&) {
    const uint layerCount = _executor->GetMap()->NumLayers();

    for (uint i = 0; i < layerCount; i++) {
        _executor->ShowLayer(i);
    }
}

void LayerList::OnCloneLayer(wxCommandEvent&) {
    wxASSERT(_contextMenuIndex != -1);

    _executor->HandleCommand(new CloneLayerCommand(_contextMenuIndex));
}

void LayerList::OnDeleteLayer(wxCommandEvent&) {
	wxASSERT(_contextMenuIndex != -1);

    if (wxMessageBox("Are you sure you wish to delete this layer?", "Notice", wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxYES) {
        _executor->HandleCommand(new DestroyLayerCommand(_contextMenuIndex));
    }
}

void LayerList::Update(Map* map) {
    // shorthand.
    const int n = map->NumLayers();

    Freeze();

    // Remove extra boxes that we don't want any more.
    for (int i = _boxes.size() - 1; i >= n; i--) {
        _sizer->Remove(i);

        LayerBox* box = _boxes[i];
        bool b = box->Destroy();
        wxASSERT(b);
        _boxes.pop_back();   // remove from our vector
    }

    // Add extra boxes.
    for (int i = _boxes.size(); i < n; i++) {
        LayerBox* box = new LayerBox(this);
        _boxes.push_back(box);
        _sizer->Add(box, 0, wxEXPAND);
    }

    // Now we're sure we have the right number of controls,
    // we can rename them, and set properties and junk.
    for (int i = 0; i < n; i++) {
        // Maybe icky.  Low id's are probably reserved by wxWindows.  Fix if it becomes an issue.
        // As it is, though, this is convenient, as each id corresponds to the index of the layer.
        _boxes[i]->SetId(i);
        _boxes[i]->SetLabel(map->GetLayer(i)->label);
    }

    UpdateIcons();

    _sizer->Layout();
    Thaw();
}

void LayerList::UpdateIcons() {
    const uint curLayer = _executor->GetCurrentLayer();

    for (uint i = 0; i < _boxes.size(); i++) {
        _boxes[i]->SetVisibilityIcon(
            _executor->IsLayerVisible(i)
                ? _visibleIcon
                : _blankIcon
            );
        _boxes[i]->SetActiveIcon(
            curLayer == i
                ? _activeIcon
                : _blankIcon
            );
    }
}
