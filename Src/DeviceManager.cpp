#include "DeviceManager.hpp"
#include <iostream>

Device::Device(Display *dpy, XDeviceInfo info) :
    dpy(dpy), name(info.name), id(info.id), info(info)
{
    // Open the Xinput device
    dev = XOpenDevice(dpy, id);
}

XValuatorInfoPtr Device::GetClass(XID c_class)
{
    XValuatorInfoPtr v = (XValuatorInfoPtr)info.inputclassinfo;
    for (int i = 0; i < info.num_classes; i++)
    {
        if (v->c_class == ValuatorClass)
            return v;
        v = (XValuatorInfoPtr)((char*)v + v->length);
    }

    return nullptr;
}

int Device::GetMode()
{
    XValuatorInfoPtr v = GetClass(ValuatorClass);
    if (v != nullptr)
        return v->mode ? 0 : 1;

    return 0;
}

void Device::SetMode(int mode)
{
    XSetDeviceMode(dpy, dev, mode ? 0 : 1);
    XFlush(dpy);
}

Device::~Device()
{
    // Close the device when deleted
    XCloseDevice(dpy, dev);
}

DeviceManager::DeviceManager()
{
    // Open display and get tablet type id
    dpy = XOpenDisplay(nullptr);
    Atom styles_type = XInternAtom(dpy, "STYLUS", True);

    // Fetch the xinput devices
    int	ndevices;
    XDeviceInfo *info = XListInputDevices(dpy, &ndevices);
    devices.reserve(ndevices);

    // Load the tablet devices
    for (int i = 0; i < ndevices; i++)
    {
        if (info[i].type != 0L)
        {
            printf("%s (%s)\n", info[i].name, XGetAtomName(dpy, info[i].type));
            if (info[i].type == styles_type)
            {
                Device* dev = new Device(dpy, info[i]);
                devices.emplace_back(dev);
            }
        }
    }
    
    // Clean up device list
    XFreeDeviceList(info);
}

// Returns the device with that name
Device *DeviceManager::DeviceByName(string name) const
{
    for (Device *dev : devices)
        if (dev->GetName() == name)
            return dev;
    return nullptr;
}

// Return a list of device names
vector<string> DeviceManager::DeviceNames() const
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
