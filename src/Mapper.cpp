#include "Mapper.hpp"
#include <X11/Xlib.h>
#include <iostream>
#include <cmath>

#define CIRCLES         True
#define VERT_BOARDER    10
#define CORNER_SIZE     7

Mapper::Mapper(const DeviceManager& dm) 
    : Gtk::DrawingArea()
    , current_monitor(0)
    , dm(dm)
{
    const int margin = 5;
    set_margin_top(margin);
    set_margin_bottom(margin);
    set_margin_left(margin);
    set_margin_right(margin);

    set_size_request(100, 100);
    set_hexpand(true);
    set_vexpand(true);
    set_focus_on_click(true);
    set_events(
          Gdk::POINTER_MOTION_MASK 
        | Gdk::BUTTON_PRESS_MASK 
        | Gdk::BUTTON_RELEASE_MASK);

    start_x = 0;
    start_y = 0;
    end_x = 1;
    end_y = 1;

    corner_selected = -1;
    body_selected = false;
    dragging = false;

    // Connect signals
    signal_draw().connect( 
        sigc::mem_fun(this, &Mapper::Draw));

    signal_motion_notify_event().connect( 
        sigc::mem_fun(this, &Mapper::MouseMoved));

    signal_button_press_event().connect( 
        sigc::mem_fun(this, &Mapper::MouseDown));

    signal_button_release_event().connect( 
        sigc::mem_fun(this, &Mapper::MouseUp));
}

void Mapper::DrawCorners(const Cairo::RefPtr<Cairo::Context>& cr)
{
    // Draw each corner at each offset
    for (auto corner : corners)
    {
        int offset_x = corner.first;
        int offset_y = corner.second;
#if CIRCLES
        cr->arc(
            map.get_x() + offset_x, map.get_y() + offset_y, 
            CORNER_SIZE * 1.2f, 0, 2 * G_PI);
#else
        cr->rectangle(
            map.x - CORNER_SIZE + offset_x, 
            map.y - CORNER_SIZE + offset_y, 
            CORNER_SIZE*2, CORNER_SIZE*2);
#endif

        cr->fill();
    }
}

void Mapper::DrawMonitor(const Cairo::RefPtr<Cairo::Context>& cr, string name)
{
    // Draw screen box
    cr->rectangle(map.get_x(), map.get_y(), map.get_width(), map.get_height());
    cr->set_source_rgb(0.63, 0.24, 0.62);
    cr->fill();

    // Draw screen boarder
    cr->set_source_rgb(0, 0, 0);
    cr->set_line_width(4);
    cr->rectangle(map.get_x(), map.get_y(), map.get_width(), map.get_height());
    cr->stroke();

    // Draw corners
    DrawCorners(cr);

    // Draw screen name
    cr->move_to(map.get_x() + 10, map.get_y() + 20);
    cr->set_source_rgb(1, 1, 1);
    cr->show_text(name);
}

bool Mapper::Draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    const Monitor& monitor = dm.GetMonitor(current_monitor);
    float asp = (float)get_height() / (float)get_width() * monitor.GetAspectRatio();
    map.set_x(start_x * get_width());
    map.set_y(start_y * get_height());
    map.set_width(end_x * get_width() - map.get_x());
    map.set_height(end_y * get_height() - map.get_y());

    corners = {
        std::make_pair(0, 0),
        std::make_pair(map.get_width(), 0),  
        std::make_pair(map.get_width(), map.get_height()),
        std::make_pair(0, map.get_height()) 
    };
    DrawMonitor(cr, monitor.DisplayInfo());

    return true;
}

bool Mapper::MouseMoved(GdkEventMotion* motion_event)
{
    int x = motion_event->x;
    int y = motion_event->y;

    if (dragging)
    {
        // Calculate the mouse movement in map space
        float delta_x = (x - last_mouse_pos.first) / (float)get_width();
        float delta_y = (y - last_mouse_pos.second) / (float)get_height();
        if (corner_selected == -1)
        {
            // Move whole box
            start_x += delta_x; end_x += delta_x;
            start_y += delta_y; end_y += delta_y;
        }
        else
        {
            // Only move selected corner
            auto corner = corners[corner_selected];
            start_x += delta_x * (corner.first > 0 ? 0 : 1);
            start_y += delta_y * (corner.second > 0 ? 0 : 1);
            end_x += delta_x * (corner.first > 0 ? 1 : 0);
            end_y += delta_y * (corner.second > 0 ? 1 : 0);
        }

        // Make sure the box does not invert
        start_x = fminf(start_x, end_x - 0.01f);
        start_y = fminf(start_y, end_y - 0.01f);
        queue_draw();
    }
    else
    {
        // Test of mouse is over a corner
        corner_selected = -1;
        for (int i = 0; i < corners.size(); i++)
        {
            auto corner = corners[i];
            int a = map.get_x() + corner.first - x;
            int b = map.get_y() + corner.second - y;
            if (sqrtf(a*a + b*b) <= CORNER_SIZE * 2)
                corner_selected = i;
        }

        // Test if mouse is over the body
        if (x >= map.get_x() && x <= map.get_x() + map.get_width() && 
            y >= map.get_y() && y <= map.get_y() + map.get_height())
            body_selected = true;
        else
            body_selected = false;
    }

    last_mouse_pos = std::make_pair(x, y);
    return true;
}

bool Mapper::MouseDown(GdkEventButton*)
{
    if (corner_selected != -1 || body_selected)
        dragging = true;
    
    return true;
}

bool Mapper::MouseUp(GdkEventButton*)
{
    dragging = false;

    if (on_changed)
        on_changed();
    return true;
}

void Mapper::SetMonitor(int id)
{
    current_monitor = id;
}

void Mapper::MapTo(Device *device) const
{
    const Monitor &monitor = dm.GetMonitor(current_monitor);
    int w = monitor.GetWidth() / (end_x - start_x);
    int h = monitor.GetHeight() / (end_y - start_y);
    int x = monitor.GetX() - (start_x * monitor.GetWidth()) / (end_x - start_x);
    int y = monitor.GetY() - (start_y * monitor.GetHeight()) / (end_y - start_y);
    device->SetMap(x, y, w, h);
}

void Mapper::SetMapping(float start_x, float end_x, 
    float start_y, float end_y)
{
    this->start_x = start_x;
    this->end_x = end_x;
    this->start_y = start_y;
    this->end_y = end_y;
}

string Mapper::GetAreaString() const
{
    return std::to_string(start_x) + "," + std::to_string(end_x) +
        "," + std::to_string(start_y) + "," + std::to_string(end_y);
}
