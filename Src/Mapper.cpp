#include "Mapper.hpp"
#include <X11/Xlib.h>
#include <iostream>

Mapper::Mapper(const DeviceManager& dm) :
    dm(dm)
{
    start_x = 0;
    start_y = 0;
    end_x = 1;
    end_y = 1;
}

void Mapper::Draw(cairo_t *cr, int width, int height)
{
    const Monitor& monitor = dm.GetMonitor(0);
    float asp = (float)height / (float)width * monitor.GetAspectRatio();
    int w = (end_x * width * asp) - (start_x * width);
    int x = width / 2 - w / 2;
    int y = start_y * height;
    int h = (end_y * height) - y;

    cairo_rectangle(cr, x, y, w, h);
    cairo_set_source_rgb(cr, 0.63, 0.24, 0.62);
    cairo_fill(cr);

    cairo_move_to(cr, x + 10, y + 20);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_show_text(cr, (monitor.GetName() + " (" + std::to_string(monitor.GetWidth()) + "x" + std::to_string(monitor.GetHeight()) + ")").c_str());
}
