/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <json/json.h>
#include <array>
#include <string>
using std::string;
using std::array;

class Config
{
public:
    Config(string path);

    class Device
    {
    public:
        Device() {}
        Device(string name, Json::Value value) 
            : name(name)
            , config(value) {}

        const string &GetName() const { return name; }
        const Json::Value &GetConfig() const { return config; }

        int GetMode() const;
        void SetMode(int mode);

        string GetMonitor() const;
        void SetMonitor(string monitor);

        array<float, 4> GetMap() const;
        void SetMap(array<float, 4> map);

    private:
        Json::Value config;
        string name;

    };

    Device GetDevice(string name) const;
    void SaveDevice(Device device);

private:
    Json::Value config;
    string path;

};
