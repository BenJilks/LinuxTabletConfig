#include "DeviceManager.hpp"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>
#include <X11/XKBlib.h>

DeviceManager::DeviceManager()
{
    // Open display and get tablet type id
    dpy = XOpenDisplay(nullptr);
    Atom tablet_type = XInternAtom(dpy, XI_TABLET, True);

    // Fetch the xinput devices
    int	ndevices;
    XDeviceInfo *info = XListInputDevices(dpy, &ndevices);
    devices.reserve(ndevices);

    // Load the tablet devices
    for (int i = 0; i < ndevices; i++)
        if (info[i].type == tablet_type)
            devices.emplace_back(info[i].name, info[i].id);
    
    // Clean up device list
    XFreeDeviceList(info);
}

// Return a list of device names
vector<string> DeviceManager::DeviceNames()
{
    vector<string> names(devices.size());
    for (Device device : devices)
        names.push_back(device.GetName());
    return names;
}

DeviceManager::~DeviceManager()
{
    XCloseDisplay(dpy);
}
