#pragma once

#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include "Mapper.hpp"
#include "VectorWidget.hpp"

class MapperSettings : public Gtk::Box
{
public:
    MapperSettings(const DeviceManager &dm);

    // Mapper parse throughs
    inline void SetMap(array<float, 4> map) { mapper.SetMap(map); }
    inline array<float, 4> GetMap() const { return mapper.GetMap(); }
    inline void MapTo(Device *device) const { mapper.MapTo(device); }
    inline void SetDevice(Device *device) { mapper.SetDevice(device); }
    inline void SetMonitor(int id) { mapper.SetMonitor(id); }
    
    void SignalChanged(std::function<void()> func) { on_changed = func; }
    void Update();

private:
    void OnChanged();
    void OnKeepAspect();
    void OnFill();

    std::function<void()> on_changed;
    Mapper mapper;
    Gtk::Grid settings;
    Gtk::Button fill;
    VectorWidget<2> start, end;
    Gtk::CheckButton keep_aspect;

};
