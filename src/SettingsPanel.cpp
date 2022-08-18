/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SettingsPanel.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

SettingsPanel::SettingsPanel(const DeviceManager &dm, Config &config, QWidget *parent)
    : QWidget(parent, {})
    , device(nullptr)
    , dm(dm)
    , config(config)
{
    auto layout = new QGridLayout();
    layout->setMargin(5);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setLayout(layout);

    auto mode_label = new QLabel("Mode: ", this);
    layout->addWidget(mode_label, 0, 0);

    mode_select = new QComboBox(this);
    mode_select->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    mode_select->addItem("Abaslute");
    mode_select->addItem("Relative");
    layout->addWidget(mode_select, 1, 0);

    auto monitor_label = new QLabel("Monitor: ", this);
    layout->addWidget(monitor_label, 0, 1);

    monitor_select = new QComboBox(this);
    monitor_select->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    for (int i = 0; i < dm.GetMonitorCount(); i++)
        monitor_select->addItem(dm.GetMonitor(i).GetName().c_str());
    layout->addWidget(monitor_select, 1, 1);

    mapper = new MapperSettings(dm, this);
    mapper->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mapper->SetMap(device_config.GetMap());
    mapper->SignalChanged([this]() { MappingChanged(); });
    layout->addWidget(mapper, 2, 0, 1, 2);

    connect(mode_select, &QComboBox::currentTextChanged, this, &SettingsPanel::ModeChanged);
    connect(monitor_select, &QComboBox::currentTextChanged, this, &SettingsPanel::MonitorChanged);
}

void SettingsPanel::SelectDevice(Device *device)
{
    this->device = device;
    device_config = config.GetDevice(device->GetName());
    mode_select->setCurrentIndex(device->GetMode());
    monitor_select->setCurrentText(device_config.GetMonitor().c_str());

    mapper->SetMap(device_config.GetMap());
    mapper->SetDevice(device);
    mapper->Update();
}

void SettingsPanel::ModeChanged()
{
    int mode = mode_select->currentIndex();
    device->SetMode(mode);

    printf("Mode: %i\n", mode);
}

void SettingsPanel::MonitorChanged()
{
    int monitor = monitor_select->currentIndex();
    string monitor_name = monitor_select->currentText().toStdString();
    device_config.SetMonitor(monitor_name);
    config.SaveDevice(device_config);

    mapper->SetMonitor(monitor);
    mapper->MapTo(device);
    mapper->repaint();
    printf("Monitor: %i(%s)\n", monitor, monitor_name.c_str());
}

void SettingsPanel::MappingChanged()
{
    if (mapper == nullptr)
        return;

    device_config.SetMap(mapper->GetMap());
    config.SaveDevice(device_config);

    mapper->MapTo(device);
}

