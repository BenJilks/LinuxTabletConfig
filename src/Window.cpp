#include "Window.hpp"
#include "gtkmm/button.h"

LTCWindow::LTCWindow(const DeviceManager &dm, Config &config) 
    : layout(Gtk::Orientation::ORIENTATION_VERTICAL)
    , device_layout(Gtk::Orientation::ORIENTATION_HORIZONTAL)
    , device_label("Device: ")
    , settings(dm, config)
    , dm(dm)
    , config(config)
{
    set_title("Linux Tablet Config");
    set_events(
          Gdk::POINTER_MOTION_MASK 
        | Gdk::BUTTON_PRESS_MASK 
        | Gdk::BUTTON_RELEASE_MASK);

    const int margin = 6;
    layout.set_margin_top(margin);
    layout.set_margin_bottom(margin);
    layout.set_margin_left(margin);
    layout.set_margin_right(margin);

    // Build device select
    device_select.set_hexpand(true);
    device_select.signal_changed().connect(
        sigc::mem_fun(this, &LTCWindow::DeviceSelected));

    if (dm.HasDevices())
    {
        for (auto name : dm.DeviceNames())
            device_select.append(name);
        device_select.set_active_text(dm.DeviceNames()[0]);
    }
    device_layout.add(device_label);
    device_layout.add(device_select);

    layout.add(device_layout);
    layout.add(settings);
    add(layout);
    show_all_children();
}

void LTCWindow::DeviceSelected()
{
    auto device_name = device_select.get_active_text();
    auto device = dm.DeviceByName(device_name);
    settings.SelectDevice(device);
}
