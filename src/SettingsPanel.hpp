#pragma once

#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include "DeviceManager.hpp"
#include "MapperSettings.hpp"
#include "Config.hpp"

class SettingsPanel : public Gtk::Grid
{
public:
    SettingsPanel(const DeviceManager &dm, Config &config);

    void SelectDevice(Device *device);

private:
    void ModeChanged();
    void MonitorChanged();
    void MappingChanged();

    const DeviceManager &dm;
    Config &config;

    Device *device;
    Config::Device device_config;

    Gtk::Label mode_label;
    Gtk::Label monitor_label;
    Gtk::ComboBoxText mode_select;
    Gtk::ComboBoxText monitor_select;

    MapperSettings mapper;
};
