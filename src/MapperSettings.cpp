#include "MapperSettings.hpp"

MapperSettings::MapperSettings(const DeviceManager &dm)
    : Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL)
    , fill("Fill")
    , start("Start X", "Start Y")
    , end("End X", "End Y")
    , mapper(dm)
{
    keep_aspect.add_label("Keep Aspect Ratio");
    keep_aspect.signal_clicked().connect(
        sigc::mem_fun(this, &MapperSettings::OnKeepAspect));
    
    fill.signal_clicked().connect(
        sigc::mem_fun(this, &MapperSettings::OnFill));

    settings.set_margin_top(10);
    settings.attach(fill, 0, 0, 4);
    start.AttachGrid(settings, 1);
    end.AttachGrid(settings, 2);
    settings.attach(keep_aspect, 0, 3, 4);
    add(mapper);
    add(settings);

    start.SignalChanged([this]() { OnChanged(); });
    end.SignalChanged([this]() { OnChanged(); });
    mapper.SignalChanged([this]() 
    {
        Update();

        if (on_changed)
            on_changed();
    });
}

void MapperSettings::Update()
{
    auto map = mapper.GetMap();
    start.Update(map[0], map[1]);
    end.Update(map[2], map[3]);
    mapper.queue_draw();
}

void MapperSettings::OnChanged()
{
    array<float, 4> map;
    array<float, 2> start_values = start.GetValues();
    array<float, 2> end_values = end.GetValues();
    map[0] = start_values[0];
    map[1] = start_values[1];
    map[2] = end_values[0];
    map[3] = end_values[1];

    mapper.SetMap(map);
    mapper.queue_draw();
    if (on_changed)
        on_changed();
}

void MapperSettings::OnKeepAspect()
{
    mapper.SetKeepAspect(keep_aspect.get_active());
}

void MapperSettings::OnFill()
{
    mapper.Fill();
    Update();

    if (on_changed)
        on_changed();
}
