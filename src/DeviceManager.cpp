#include "DeviceManager.hpp"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>
#include <X11/XKBlib.h>
#include <iostream>

Device::Device(Display *dpy, XDeviceInfo info) :
    dpy(dpy), name(info.name), id(info.id), info(new XDeviceInfo(info))
{
    // Open the Xinput device
    dev = XOpenDevice(dpy, id);
}

XValuatorInfoPtr Device::GetClass(XID c_class)
{
    XValuatorInfoPtr v = (XValuatorInfoPtr)info->inputclassinfo;
    for (int i = 0; i < info->num_classes; i++)
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

float Device::GetAspectRatio()
{
    Atom type, prop = XInternAtom(dpy, "Wacom Tablet Area", True);
    unsigned long count, bytes_after;
	unsigned char* data;
    int format;

    data = nullptr;
    XGetDeviceProperty(dpy, (XDevice*)dev, prop, 
        0, 1000, False, AnyPropertyType, 
        &type, &format, &count, &bytes_after, &data);
    
    if (!data)
        return 1;
    
    long *ldata = (long*)data;
    long width = ldata[2];
    long height = ldata[3];
    return (float)width / (float)height;
}

void Device::SetMode(int mode)
{
    XSetDeviceMode(dpy, (XDevice*)dev, mode ? 0 : 1);
    XFlush(dpy);
}

void Device::SetMap(int offset_x, int offset_y, int output_width, int output_height)
{
    // Create matrix transform
	float width = DisplayWidth(dpy, DefaultScreen(dpy));
    float height = DisplayHeight(dpy, DefaultScreen(dpy));
	float x = offset_x / width;
    float y = offset_y / height;
	float w = output_width / width;
    float h = output_height / height;
    float fmatrix[] =
    {
        w, 0, x,
        0, h, y,
        0, 0, 1
    };

    // Convert matrix type
    long matrix[9];
    for (int i = 0; i < 9; i++)
        *(float*)(matrix + i) = fmatrix[i];

    // Fetch matrix property
    Atom matrix_prop = XInternAtom(dpy, "Coordinate Transformation Matrix", True);
    if (!matrix_prop)
    {
        printf("Error: Mapping not supported on this tablet "
            "(Could not find 'Coordinate Transformation Matrix' property\n");
        return;
    }

    // Fetch matrix information
    Atom type;
    int format;
	unsigned long nitems, bytes_after;
    float *data;
    XGetDeviceProperty(dpy, (XDevice*)dev, matrix_prop, 0, 9, False,
		AnyPropertyType, &type, &format, &nitems,
        &bytes_after, (unsigned char**)&data);

    // Set new information
	XChangeDeviceProperty(dpy, (XDevice*)dev, matrix_prop, type, format,
        PropModeReplace, (unsigned char*)matrix, 9);
    
    // Free old info
    XFree(data);
    XFlush(dpy);
}

Device::~Device()
{
    // Close the device when deleted
    XCloseDevice(dpy, (XDevice*)dev);
}

string Monitor::DisplayInfo() const
{
    return name + " (" + 
        std::to_string(width) + "x" + 
        std::to_string(height) + ")";
}

void Monitor::MapTo(Device *device) const
{
    printf("Mapping tablet to %i %i %i %i\n", 
        x, y, width, height);
    
    device->SetMap(x, y, width, height);
}

DeviceManager::DeviceManager()
{
    // Open display and get tablet type id
    dpy = XOpenDisplay(nullptr);
    Atom styles_type = XInternAtom(dpy, "STYLUS", True);
    Atom tablet_type = XInternAtom(dpy, "TABLET", True);

    // Fetch the xinput devices
    int	ndevices;
    auto info = XListInputDevices(dpy, &ndevices);
    devices.reserve(ndevices);

    // Load the tablet devices
    for (int i = 0; i < ndevices; i++)
    {
        if (info[i].type != 0L)
        {
            printf("%s (%s)\n", info[i].name, XGetAtomName(dpy, info[i].type));
            if (info[i].type == styles_type || info[i].type == tablet_type)
            {
                Device* dev = new Device(dpy, info[i]);
                devices.emplace_back(dev);
            }
        }
    }

    // Fetch monitor info
    int monitor_count;
    Window root = RootWindow(dpy, DefaultScreen(dpy));
    XRRMonitorInfo *m = XRRGetMonitors(dpy, root, True, &monitor_count);
    for (int i = 0; i < monitor_count; i++)
    {
        XRRMonitorInfo monitor = m[i];
        string name = XGetAtomName(dpy, monitor.name);
        monitors.emplace_back(name, monitor.x, monitor.y, 
            monitor.width, monitor.height);
    }

    this->info = info;
}

// Returns the device with that name
Device *DeviceManager::DeviceByName(string name) const
{
    for (Device *dev : devices)
        if (dev->GetName() == name)
            return dev;
    return nullptr;
}

const Monitor& DeviceManager::GetMonitorByName(string name) const
{
    for (const Monitor &monitor : monitors)
        if (monitor.GetName() == name)
            return monitor;
    
    return monitors[0];
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

    // Clean up device list
    XFreeDeviceList(info);

    // Close X display
    XCloseDisplay(dpy);
}
