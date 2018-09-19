#pragma once
#include <vector>
#include <string>
using std::vector;
using std::string;

class Device
{
public:
    Device(string name, int id) :
        name(name), id(id) {}
    
    inline string GetName() const { return name; }
    inline int GetId() const { return id; }

private:
    string name;
    int id;

};

struct _XDisplay;
class DeviceManager
{
public:
    DeviceManager();
    vector<string> DeviceNames();

    inline bool HasDevices() const { return devices.size() > 0; }

    ~DeviceManager();

private:
    vector<Device> devices;
    _XDisplay *dpy;

};
