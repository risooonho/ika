
#include "scriptstate.h"
#include "script.h"
#include <cassert>

#include "mainwindow.h"
#include "mapview.h"

ScriptState::ScriptState(MainWindow* mainWnd)
    : EditState(mainWnd)
    , _script(0)
{
}

void ScriptState::SetScript(Script* script)
{
    // TODO: make sure that this isn't called while the script is still in use or something similarly bizzare.
    _script = script;
}

void ScriptState::OnMouseDown(wxMouseEvent& event)
{
    assert(_script);
    GetMapView()->ScreenToMap(event.m_x, event.m_y);
    try
    {
        _script->OnMouseDown(event.GetX(), event.GetY());
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }
}

void ScriptState::OnMouseUp(wxMouseEvent& event)
{
    assert(_script);
    GetMapView()->ScreenToMap(event.m_x, event.m_y);
    try
    {
        _script->OnMouseUp(event.GetX(), event.GetY());
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }
}

void ScriptState::OnMouseMove(wxMouseEvent& event)
{
    assert(_script);
    GetMapView()->ScreenToMap(event.m_x, event.m_y);

    try
    {
        _script->OnMouseMove(event.GetX(), event.GetY());
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }
}

void ScriptState::OnBeginState()
{
    assert(_script);

    try
    {
        _script->OnBeginState();
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }

}

void ScriptState::OnEndState()
{
    assert(_script);

    try
    {
        _script->OnEndState();
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }
}

void ScriptState::OnMouseWheel(wxMouseEvent& event)
{
    assert(_script);
    GetMapView()->ScreenToMap(event.m_x, event.m_y);
    try
    {
        _script->OnMouseWheel(event.GetX(), event.GetY(), event.GetWheelRotation() / event.GetWheelDelta());
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }

}

void ScriptState::OnRender()
{
    assert(_script);
    try
    {
        _script->OnRender();
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }
}

void ScriptState::OnRenderCurrentLayer()
{
    assert(_script);
    try
    {
        _script->OnRenderCurrentLayer();
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }
}

void ScriptState::OnSwitchLayers(uint oldLayer, uint newLayer)
{
    assert(_script);
    try
    {
        _script->OnSwitchLayers(oldLayer, newLayer);
    }
    catch (std::runtime_error err)
    {
        ::wxMessageBox(err.what(), "Script error", wxOK | wxCENTER, GetMainWindow());
    }
}