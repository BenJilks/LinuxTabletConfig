#include "Mapper.hpp"
#include <X11/Xlib.h>
#include <iostream>

#define VERT_BOARDER 10
#define CORNER_SIZE 7
#define CIRCLES True

Mapper::Mapper(const DeviceManager& dm) :
    dm(dm)
{
    start_x = 0;
    start_y = 0;
    end_x = 1;
    end_y = 1;
}

void Mapper::DrawCorners(cairo_t *cr, GdkRectangle rect)
{
    // Define corner offsets
    int corner_offsets[] = 
    { 
        0, 0,  
        rect.width, 0,  
        rect.width, rect.height,  
        0, rect.height 
    };

    // Draw each corner at each offset
    for (int i = 0; i < 4; i++)
    {
        int offset_x = corner_offsets[i * 2];
        int offset_y = corner_offsets[i * 2 + 1];
#if CIRCLES
        cairo_arc(cr, 
            rect.x + offset_x, rect.y + offset_y, 
            CORNER_SIZE * 1.2f, 0, 2 * G_PI);
#else
        cairo_rectangle(cr, 
            rect.x - CORNER_SIZE + offset_x, 
            rect.y - CORNER_SIZE + offset_y, 
            CORNER_SIZE*2, CORNER_SIZE*2);
#endif
        cairo_fill(cr);
    }
}

void Mapper::DrawMonitor(cairo_t *cr, string name, GdkRectangle rect)
{
    // Draw screen box
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_set_source_rgb(cr, 0.63, 0.24, 0.62);
    cairo_fill(cr);

    // Draw screen boarder
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 4);
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_stroke(cr);

    // Draw corners
    DrawCorners(cr, rect);

    // Draw screen name
    cairo_move_to(cr, rect.x + 10, rect.y + 20);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_show_text(cr, name.c_str());
}

void Mapper::Draw(cairo_t *cr, int width, int height)
{
    const Monitor& monitor = dm.GetMonitor(0);
    float asp = (float)height / (float)width * monitor.GetAspectRatio();
    
    GdkRectangle rect;
    rect.width = (end_x * width * asp) - (start_x * width);
    rect.x = width / 2 - rect.width / 2;
    rect.y = start_y * height + VERT_BOARDER;
    rect.height = (end_y * height) - rect.y - VERT_BOARDER*2;
    DrawMonitor(cr, monitor.DisplayInfo(), rect);
}

void Mapper::MouseMoved(int x, int y)
{

}

void Mapper::MouseDown()
{

}

void Mapper::MouseUp()
{

}
