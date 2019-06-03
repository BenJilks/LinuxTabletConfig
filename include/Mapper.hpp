#pragma once
#include "DeviceManager.hpp"
#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <array>
using std::vector;
using std::string;
using std::array;
using std::pair;

class Mapper
{
public:
    Mapper(const DeviceManager& dm);
    void Draw(cairo_t *cr, int width, int height);
    void MouseMoved(int x, int y);
    void MouseDown();
    void MouseUp();

    void SetMonitor(int id);
    inline int GetMonitor() const { return current_monitor; }

    void MapTo(Device *device) const;
    void SetMapping(float start_x, float end_x, 
        float start_y, float end_y);
    string GetAreaString() const;

private:
    void DrawMonitor(cairo_t *cr, string name);
    void DrawCorners(cairo_t *cr);

    GdkRectangle map;
    array<pair<int, int>, 4> corners;
    int corner_selected;
    bool dragging, body_selected;
    pair<int, int> last_mouse_pos;

    float start_x, start_y;
    float end_x, end_y;
    int width, height;
    int current_monitor;
    const DeviceManager& dm;

};
