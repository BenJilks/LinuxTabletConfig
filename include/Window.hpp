#pragma once

#include <gtkmm.h>
#include <gtkmm/window.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include "SettingsPanel.hpp"
#include "DeviceManager.hpp"
#include "Config.hpp"

class LTCWindow : public Gtk::Window
{
public:
    LTCWindow(const DeviceManager &dm, Config &config);

private:
    void DeviceSelected();

    const DeviceManager &dm;
    Config &config;

    Gtk::Box layout;
    Gtk::Box device_layout;
    Gtk::Label device_label;
    Gtk::ComboBoxText device_select;
    SettingsPanel settings;

};
