#pragma once
#include <vector>
#include <string>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>
#include <X11/XKBlib.h>
using std::vector;
using std::string;

#define MODE_ABSOLUTE 0
#define MODE_RELATIVE 1

class Device
{
public:
    Device(Display *dpy, XDeviceInfo info);
    int GetMode();
    void SetMode(int mode);
    void SetMap(int x, int y, int width, int height);

    inline string GetName() const { return name; }
    inline int GetId() const { return id; }
    ~Device();

private:
    XValuatorInfoPtr GetClass(XID c_class);

    string name;
    long unsigned int id;
    XDeviceInfo info;
    XDevice *dev;
    Display *dpy;
};

class Monitor
{
public:
    Monitor(string name, int x, int y, int w, int h) :
        name(name), x(x), y(y), width(w), height(h), asp((float)w / (float)h) {}
    string DisplayInfo() const;
    void MapTo(Device *device) const;
    
    inline string GetName() const { return name; }
    inline int GetX() const { return x; }
    inline int GetY() const { return y; }
    inline int GetWidth() const { return width; }
    inline int GetHeight() const { return height; }
    inline float GetAspectRatio() const { return asp; }

private:
    string name;
    int x, y, width, height;
    float asp;

};

class DeviceManager
{
public:
    DeviceManager();
    vector<string> DeviceNames() const;
    Device *DeviceByName(string name) const;
    const Monitor& GetMonitorByName(string name) const;

    inline bool HasDevices() const { return devices.size() > 0; }
    inline Device* DefaultDevice() const { return devices[0]; }
    inline const Monitor& GetMonitor(int m) const { return monitors[m]; }
    inline int GetMonitorCount() const { return monitors.size(); }

    ~DeviceManager();

private:
    vector<Device*> devices;
    vector<Monitor> monitors;
    XDeviceInfo *info;
    Display *dpy;

};
