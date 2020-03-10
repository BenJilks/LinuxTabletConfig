#pragma once
#include <functional>
#include <vector>
#include <string>
#include <array>
#include <gtkmm/drawingarea.h>
#include "DeviceManager.hpp"
#include "VectorWidget.hpp"
using std::vector;
using std::string;
using std::array;
using std::pair;

class Mapper : public Gtk::DrawingArea
{
public:
    Mapper(const DeviceManager& dm);

    void SetMonitor(int id);

    void SetMap(array<float, 4> map) 
        { start_x = map[0]; start_y = map[1]; 
          end_x = map[2]; end_y = map[3]; }

    inline array<float, 4> GetMap() const 
        { return { start_x, start_y, end_x, end_y }; }
    
    inline int GetMonitor() const { return current_monitor; }

    void SignalChanged(std::function<void()> func) { on_changed = func; }

    void MapTo(Device *device) const;
    void SetMapping(float start_x, float end_x, 
        float start_y, float end_y);
    string GetAreaString() const;

private:
    bool Draw(const Cairo::RefPtr<Cairo::Context>& cr);
    void DrawMonitor(const Cairo::RefPtr<Cairo::Context>& cr, string name);
    void DrawCorners(const Cairo::RefPtr<Cairo::Context>& cr);

    bool MouseMoved(GdkEventMotion* motion_event);
    bool MouseDown(GdkEventButton*);
    bool MouseUp(GdkEventButton*);

    Gdk::Rectangle map;
    array<pair<int, int>, 4> corners;
    int corner_selected;
    bool dragging, body_selected;
    pair<int, int> last_mouse_pos;
    std::function<void()> on_changed;

    float start_x, start_y;
    float end_x, end_y;
    int current_monitor;
    const DeviceManager& dm;

};
