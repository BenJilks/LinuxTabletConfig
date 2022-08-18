/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QMainWindow>
#include <QComboBox>
#include "DeviceManager.hpp"
#include "SettingsPanel.hpp"
#include "Config.hpp"

class LTCWindow : public QMainWindow
{
    Q_OBJECT

public:
    LTCWindow(const DeviceManager &dm, Config &config,
              QWidget *parent = nullptr);

    virtual ~LTCWindow() = default;

private:
    void DeviceSelected();

    QComboBox *device_select { nullptr };
    SettingsPanel *settings { nullptr };

    const DeviceManager &dm;
    Config &config;

};
