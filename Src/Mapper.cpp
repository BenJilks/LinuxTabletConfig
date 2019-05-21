#include "Mapper.hpp"
#include <X11/Xlib.h>
#include <iostream>
#include <cmath>

#define VERT_BOARDER 10
#define CORNER_SIZE 7
#define CIRCLES True

Mapper::Mapper(const DeviceManager& dm) :
    dm(dm), current_monitor(0)
{
    start_x = 0;
    start_y = 0;
    end_x = 1;
    end_y = 1;

    corner_selected = -1;
    body_selected = false;
    dragging = false;
}

void Mapper::DrawCorners(cairo_t *cr)
{
    // Draw each corner at each offset
    for (auto corner : corners)
    {
        int offset_x = corner.first;
        int offset_y = corner.second;
#if CIRCLES
        cairo_arc(cr, 
            map.x + offset_x, map.y + offset_y, 
            CORNER_SIZE * 1.2f, 0, 2 * G_PI);
#else
        cairo_rectangle(cr, 
            map.x - CORNER_SIZE + offset_x, 
            map.y - CORNER_SIZE + offset_y, 
            CORNER_SIZE*2, CORNER_SIZE*2);
#endif
        cairo_fill(cr);
    }
}

void Mapper::DrawMonitor(cairo_t *cr, string name)
{
    // Draw screen box
    cairo_rectangle(cr, map.x, map.y, map.width, map.height);
    cairo_set_source_rgb(cr, 0.63, 0.24, 0.62);
    cairo_fill(cr);

    // Draw screen boarder
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 4);
    cairo_rectangle(cr, map.x, map.y, map.width, map.height);
    cairo_stroke(cr);

    // Draw corners
    DrawCorners(cr);

    // Draw screen name
    cairo_move_to(cr, map.x + 10, map.y + 20);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_show_text(cr, name.c_str());
}

void Mapper::Draw(cairo_t *cr, int width, int height)
{
    this->width = width;
    this->height = height;

    const Monitor& monitor = dm.GetMonitor(current_monitor);
    float asp = (float)height / (float)width * monitor.GetAspectRatio();
    map.x = start_x * width;
    map.y = start_y * height;
    map.width = end_x * width - map.x;
    map.height = end_y * height - map.y;

    corners = {
        std::make_pair(0, 0),
        std::make_pair(map.width, 0),  
        std::make_pair(map.width, map.height),
        std::make_pair(0, map.height) 
    };
    DrawMonitor(cr, monitor.DisplayInfo());
}

void Mapper::MouseMoved(int x, int y)
{
    if (dragging)
    {
        // Calculate the mouse movement in map space
        float delta_x = (x - last_mouse_pos.first) / (float)width;
        float delta_y = (y - last_mouse_pos.second) / (float)height;
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
    }
    else
    {
        // Test of mouse is over a corner
        corner_selected = -1;
        for (int i = 0; i < corners.size(); i++)
        {
            auto corner = corners[i];
            int a = map.x + corner.first - x;
            int b = map.y + corner.second - y;
            if (sqrtf(a*a + b*b) <= CORNER_SIZE * 2)
                corner_selected = i;
        }

        // Test if mouse is over the body
        if (x >= map.x && x <= map.x + map.width && 
            y >= map.y && y <= map.y + map.height)
            body_selected = true;
        else
            body_selected = false;
    }

    last_mouse_pos = std::make_pair(x, y);
}

void Mapper::MouseDown()
{
    if (corner_selected != -1 || body_selected)
        dragging = true;
}

void Mapper::MouseUp()
{
    dragging = false;
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
