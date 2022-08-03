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

    void SetDevice(Device *device);
    void SetMonitor(int id);
    void SetMap(array<float, 4> map);
    void Fill();

    inline void SetKeepAspect(bool flag) { keep_aspect = flag; }

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
    pair<int, int> last_mouse_pos;
    std::function<void()> on_changed;
    float device_aspect_ratio;
    float monitor_aspect_ratio;

    int current_monitor;
    float start_x, start_y;
    float end_x, end_y;
    bool dragging, body_selected;
    bool keep_aspect;
    int corner_selected;
    const DeviceManager& dm;

};
