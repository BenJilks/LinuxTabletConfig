/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QWidget>
#include <functional>
#include <vector>
#include <string>
#include <array>
#include "DeviceManager.hpp"
#include "VectorWidget.hpp"

class Mapper : public QWidget
{
    Q_OBJECT

public:
    Mapper(const DeviceManager& dm, QWidget *parent = nullptr);
    virtual ~Mapper() = default;

    void SetDevice(Device *device);
    void SetMonitor(int id);
    void SetMap(std::array<float, 4> map);
    void Fill();

    inline void SetKeepAspect(bool flag) { keep_aspect = flag; }

    inline std::array<float, 4> GetMap() const 
        { return { start_x, start_y, end_x, end_y }; }
    
    inline int GetMonitor() const { return current_monitor; }

    void SignalChanged(std::function<void()> func) { on_changed = func; }

    void MapTo(Device *device) const;
    void SetMapping(float start_x, float end_x, 
        float start_y, float end_y);
    string GetAreaString() const;

protected:
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    void draw_monitor(string name);
    void draw_corners();

    struct Rectangle
    {
        int x { 0 };
        int y { 0 };
        int width { 0 };
        int height { 0 };
    };

    Rectangle map;
    std::array<std::pair<int, int>, 4> corners;
    std::pair<int, int> last_mouse_pos;
    std::function<void()> on_changed;
    float device_aspect_ratio { 1 };
    float monitor_aspect_ratio { 1 };

    int current_monitor { 0 };
    float start_x { 0 };
    float start_y { 0 };
    float end_x { 1 };
    float end_y { 1 };
    bool body_selected { false };
    bool keep_aspect { false };
    int corner_selected { -1 };
    const DeviceManager& dm;
};
