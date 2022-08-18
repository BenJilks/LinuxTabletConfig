/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once
#include <vector>
#include <string>
#include <memory>
using std::vector;
using std::string;

#define MODE_ABSOLUTE 0
#define MODE_RELATIVE 1

typedef struct _XDisplay Display;
typedef struct _XDeviceInfo XDeviceInfo;
typedef struct _XValuatorInfo XValuatorInfo;
typedef unsigned long XID;

class Device
{
public:
    Device(Display *dpy, XDeviceInfo info);
    int GetMode();
    float GetAspectRatio();
    void SetMode(int mode);
    void SetMap(int x, int y, int width, int height);

    inline string GetName() const { return name; }
    inline int GetId() const { return id; }
    ~Device();

private:
    XValuatorInfo *GetClass(XID c_class);

    string name;
    long unsigned int id;
    void *dev;
    std::unique_ptr<XDeviceInfo> info;
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
