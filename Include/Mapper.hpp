#pragma once
#include "DeviceManager.hpp"
#include <gtk/gtk.h>
#include <vector>
#include <string>
using std::vector;
using std::string;

class Mapper
{
public:
    Mapper(const DeviceManager& dm);
    void Draw(cairo_t *cr, int width, int height);
    void MouseMoved(int x, int y);
    void MouseDown();
    void MouseUp();

private:
    void DrawMonitor(cairo_t *cr, string name, GdkRectangle rect);
    void DrawCorners(cairo_t *cr, GdkRectangle rect);

    float start_x, start_y;
    float end_x, end_y;
    const DeviceManager& dm;

};
