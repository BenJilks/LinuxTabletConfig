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
    const DeviceManager& dm;

};
