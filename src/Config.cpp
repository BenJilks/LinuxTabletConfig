#include <fstream>
#include "Config.hpp"

Config::Config(string path)
    : path(path)
{
    // Parse file into config
    std::ifstream file(path);
    if (!file.good())
        config = Json::Value(Json::ValueType::objectValue);
    else
        file >> config;
    file.close();
}

Config::Device Config::GetDevice(string name) const
{
    auto device_config = config[name];
    if (!device_config.isObject())
        device_config = Json::Value(Json::ValueType::objectValue);

    return Device(name, device_config);
}

void Config::SaveDevice(Device device)
{
    config[device.GetName()] = device.GetConfig();

    std::ofstream file(path);
    file << config;
    file.close();
    printf("Saved to %s\n", path.c_str());
}

int Config::Device::GetMode() const
{
    auto mode_json = config["mode"];
    if (!mode_json.isString())
        return 0;

    string mode_string = mode_json.asString();
    return mode_string == "absolute" ? 0 : 1;
}

void Config::Device::SetMode(int mode)
{
    Json::Value value(mode == 0 ? "absolute" : "relative");
    config["mode"] = value;
}

string Config::Device::GetMonitor() const
{
    auto monitor_json = config["monitor"];
    if (!monitor_json.isString())
        return "";
    
    return monitor_json.asString();
}

void Config::Device::SetMonitor(string monitor)
{
    Json::Value value(monitor);
    config["monitor"] = monitor;
}


array<float, 4> Config::Device::GetMap() const
{
    auto map_json = config["map"];
    if (!map_json.isArray() && map_json.size() != 4)
        return { 0, 0, 1, 1 };
    
    return {
        map_json[0].asFloat(), map_json[1].asFloat(), 
        map_json[2].asFloat(), map_json[3].asFloat() };
}

void Config::Device::SetMap(array<float, 4> map)
{
    Json::Value value(Json::ValueType::arrayValue);
    for (int i = 0; i < 4; i++)
        value[i] = map[i];

    config["map"] = value;
}
