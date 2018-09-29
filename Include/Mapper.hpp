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

private:
    float start_x, start_y;
    float end_x, end_y;
    const DeviceManager& dm;

};
