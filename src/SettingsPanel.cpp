#include "SettingsPanel.hpp"

SettingsPanel::SettingsPanel(const DeviceManager &dm, Config &config)
    : device(nullptr)
    , mode_label("Mode: ")
    , monitor_label("Monitor: ")
    , mapper(dm)
    , mapper_settings(Gtk::Orientation::ORIENTATION_VERTICAL)
    , dm(dm)
    , config(config)
{
    const int margin = 5;
    set_margin_top(margin);
    set_margin_bottom(margin);
    set_margin_left(margin);
    set_margin_right(margin);
    set_hexpand(true);

    mode_select.set_hexpand(true);
    mode_select.append("Abaslute");
    mode_select.append("Relative");
    mode_select.signal_changed().connect(
        sigc::mem_fun(this, &SettingsPanel::ModeChanged));
    attach(mode_label, 0, 0);
    attach(mode_select, 1, 0);

    monitor_select.set_hexpand(true);
    monitor_select.signal_changed().connect(
        sigc::mem_fun(this, &SettingsPanel::MonitorChanged));

    for (int i = 0; i < dm.GetMonitorCount(); i++)
        monitor_select.append(dm.GetMonitor(i).GetName());
    attach(monitor_label, 0, 1);
    attach(monitor_select, 1, 1);

    mapper.SetMap(device_config.GetMap());
    mapper.SignalChanged([this]() { MappingChanged(); });
    attach(mapper, 0, 2, 2);
    mapper.queue_draw();
}

void SettingsPanel::SelectDevice(Device *device)
{
    this->device = device;
    
    device_config = config.GetDevice(device->GetName());
    mode_select.set_active(device->GetMode());
    monitor_select.set_active_text(device_config.GetMonitor());

    mapper.SetMap(device_config.GetMap());
    mapper.queue_draw();
}

void SettingsPanel::ModeChanged()
{
    int mode = mode_select.get_active_row_number();
    device->SetMode(mode);

    printf("Mode: %i\n", mode);
}

void SettingsPanel::MonitorChanged()
{
    int monitor = monitor_select.get_active_row_number();
    string monitor_name = monitor_select.get_active_text();
    device_config.SetMonitor(monitor_name);
    config.SaveDevice(device_config);

    mapper.SetMonitor(monitor);
    mapper.MapTo(device);
    mapper.queue_draw();
    printf("Monitor: %i(%s)\n", monitor, monitor_name.c_str());
}

void SettingsPanel::MappingChanged()
{
    device_config.SetMap(mapper.GetMap());
    config.SaveDevice(device_config);

    mapper.MapTo(device);
    printf("Mapping changed\n");
}
