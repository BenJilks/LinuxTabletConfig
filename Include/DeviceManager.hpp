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

class DeviceManager
{
public:
    DeviceManager();
    vector<string> DeviceNames() const;
    Device *DeviceByName(string name) const;

    inline bool HasDevices() const { return devices.size() > 0; }
    inline Device* DefaultDevice() const { return devices[0]; }

    ~DeviceManager();

private:
    vector<Device*> devices;
    Display *dpy;

};
