#include "DeviceManager.hpp"
#include <iostream>

Device::Device(Display *dpy, string name, long unsigned int id) :
    dpy(dpy), name(name), id(id)
{
    dev = XOpenDevice(dpy, id);
}

Device::~Device()
{
    XCloseDevice(dpy, dev);
}

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
    {
        if (info[i].type == tablet_type)
        {
            Device* dev = new Device(dpy, info[i].name, info[i].id);
            devices.emplace_back(dev);
        }
    }
    
    // Clean up device list
    XFreeDeviceList(info);
}

// Returns the device with that name
Device *DeviceManager::DeviceByName(string name)
{
    for (Device *dev : devices)
        if (dev->GetName() == name)
            return dev;
    return nullptr;
}

// Return a list of device names
vector<string> DeviceManager::DeviceNames()
{
    vector<string> names;
    names.reserve(devices.size());
    for (Device *dev : devices)
        names.push_back(dev->GetName());
    return names;
}

DeviceManager::~DeviceManager()
{
    // Close and delete all devices
    for (Device *dev : devices)
        delete dev;
    devices.clear();

    XCloseDisplay(dpy);
}
