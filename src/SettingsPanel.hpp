/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QWidget>
#include <QComboBox>
#include "DeviceManager.hpp"
#include "MapperSettings.hpp"
#include "Config.hpp"

class SettingsPanel : public QWidget
{
    Q_OBJECT

public:
    SettingsPanel(const DeviceManager &dm, Config &config,
                  QWidget *parent = nullptr);

    virtual ~SettingsPanel() = default;

    void SelectDevice(Device *device);

private:
    void ModeChanged();
    void MonitorChanged();
    void MappingChanged();

    const DeviceManager &dm;
    Config &config;

    QComboBox *mode_select { nullptr };
    QComboBox *monitor_select { nullptr };
    MapperSettings *mapper { nullptr };

    Device *device { nullptr };
    Config::Device device_config;
};
