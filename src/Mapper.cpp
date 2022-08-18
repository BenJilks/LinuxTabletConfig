/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Mapper.hpp"
#include <QLayout>
#include <QBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <X11/Xlib.h>
#include <iostream>
#include <cmath>
#include <qbrush.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <utility>

#define CIRCLES         True
#define VERT_BOARDER    10
#define CORNER_SIZE     7

Mapper::Mapper(const DeviceManager& dm, QWidget *parent) 
    : QWidget(parent, {})
    , dm(dm)
{
    setContentsMargins(5, 5, 5, 5);
    setMinimumSize(100, 100);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    // Connect signals
    /*
    signal_draw().connect( 
        sigc::mem_fun(this, &Mapper::Draw));

    signal_motion_notify_event().connect( 
        sigc::mem_fun(this, &Mapper::MouseMoved));

    signal_button_press_event().connect( 
        sigc::mem_fun(this, &Mapper::MouseDown));

    signal_button_release_event().connect( 
        sigc::mem_fun(this, &Mapper::MouseUp));
    */
}

void Mapper::draw_corners()
{
    // Draw each corner at each offset
    for (auto corner : corners)
    {
        int offset_x = corner.first;
        int offset_y = corner.second;
        QPainter painter(this);
#if CIRCLES
        painter.setPen(QColor(0, 0, 0));
        painter.setBrush(QColor(0, 0, 0));
        painter.drawEllipse(
            map.x - CORNER_SIZE + offset_x, 
            map.y - CORNER_SIZE + offset_y, 
            CORNER_SIZE*2, CORNER_SIZE*2);
#else
        painter.fillRect(
            map.x - CORNER_SIZE + offset_x, 
            map.y - CORNER_SIZE + offset_y, 
            CORNER_SIZE*2, CORNER_SIZE*2,
            QColor(0, 0, 0));
#endif
        painter.end();
    }
}

void Mapper::draw_monitor(string name)
{
    // Draw screen box
    QRect rect(map.x, map.y, map.width, map.height);
    {
        QPainter painter(this);
        painter.fillRect(rect, QColor(161, 61, 158));
        painter.end();
    }

    // Draw screen boarder
    {
        QPainter painter(this);
        painter.setPen(QPen(QColor(0, 0, 0), 4));
        painter.drawRect(rect);
        painter.end();
    }

    // Draw corners
    draw_corners();

    // Draw screen name
    {
        QPainter painter(this);
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(map.x + 10, map.y + 20, name.c_str());
        painter.end();
    }
}

void Mapper::paintEvent(QPaintEvent*)
{
    const Monitor& monitor = dm.GetMonitor(current_monitor);
    float asp = (float)height() / (float)width() * monitor.GetAspectRatio();
    map.x = start_x * width();
    map.y = start_y * height();
    map.width = end_x * width() - map.x;
    map.height = end_y * height() - map.y;

    corners = {
        std::make_pair(0, 0),
        std::make_pair(map.width, 0),  
        std::make_pair(map.width, map.height),
        std::make_pair(0, map.height),
    };

    draw_monitor(monitor.DisplayInfo());
}

void Mapper::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->localPos().x();
    int y = event->localPos().y();

    // Calculate the mouse movement in map space
    float delta_x = (x - last_mouse_pos.first) / (float)width();
    float delta_y = (y - last_mouse_pos.second) / (float)height();

    if (body_selected && corner_selected == -1)
    {
        // Move whole box
        start_x += delta_x; end_x += delta_x;
        start_y += delta_y; end_y += delta_y;
    }
    else if (corner_selected != -1)
    {
        // Only move selected corner
        auto corner = corners[corner_selected];
        start_x += delta_x * (corner.first > 0 ? 0 : 1);
        start_y += delta_y * (corner.second > 0 ? 0 : 1);
        end_x += delta_x * (corner.first > 0 ? 1 : 0);
        end_y += delta_y * (corner.second > 0 ? 1 : 0);

        // Force aspect ratio if enabled
        if (keep_aspect)
        {
            float aspect_ratio = monitor_aspect_ratio / device_aspect_ratio;
            if (corner.first == 0)
                start_x = end_x - ((end_y - start_y) * aspect_ratio);  
            else
                end_x = start_x + ((end_y - start_y) * aspect_ratio);
        }
    }

    // Make sure the box does not invert
    start_x = fminf(start_x, end_x - 0.01f);
    start_y = fminf(start_y, end_y - 0.01f);
    repaint();

    last_mouse_pos = std::make_pair(x, y);
}

void Mapper::Fill()
{
    start_x = 0;
    start_y = 0;
    end_x = 1;
    end_y = 1;

    if (keep_aspect)
    {
        float aspect_ratio = device_aspect_ratio / monitor_aspect_ratio;
        if (aspect_ratio < 1.0)
        {
            start_y = (1.0 - aspect_ratio) / 2.0;
            end_y = start_y + aspect_ratio;
        }
        else
        {
            aspect_ratio = 1.0 / aspect_ratio;
            start_x = (1.0 - aspect_ratio) / 2.0;
            end_x = start_x + aspect_ratio;
        }
    }

    repaint();
}

void Mapper::mousePressEvent(QMouseEvent *event)
{
    int x = event->localPos().x();
    int y = event->localPos().y();

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

    last_mouse_pos = std::make_pair(x, y);
}

void Mapper::mouseReleaseEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);

    if (on_changed)
        on_changed();
}

void Mapper::SetDevice(Device *device)
{
    device_aspect_ratio = device->GetAspectRatio();
}

void Mapper::SetMonitor(int id)
{
    monitor_aspect_ratio = dm.GetMonitor(id).GetAspectRatio();
    current_monitor = id;
}

void Mapper::SetMap(std::array<float, 4> map)
{
    start_x = map[0]; 
    start_y = map[1]; 
    end_x = map[2]; 
    end_y = map[3];
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
