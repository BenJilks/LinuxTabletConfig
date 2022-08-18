/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Window.hpp"
#include <QLabel>
#include <QBoxLayout>

LTCWindow::LTCWindow(const DeviceManager &dm, Config &config, QWidget *parent)
    : QMainWindow(parent, {})
    , dm(dm)
    , config(config)
{
    setWindowTitle("Linux Tablet Config");

    auto main_widget = new QWidget(this);
    auto layout = new QBoxLayout(QBoxLayout::Direction::Down);
    layout->setMargin(6);
    main_widget->setLayout(layout);
    setCentralWidget(main_widget);

    auto device_widget = new QWidget(main_widget);
    auto device_layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    device_widget->setLayout(device_layout);
    layout->addWidget(device_widget);

    auto device_label = new QLabel("Device: ", device_widget);
    device_layout->addWidget(device_label);

    device_select = new QComboBox(device_widget);
    device_select->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    device_layout->addWidget(device_select);
    if (dm.HasDevices())
    {
        for (auto name : dm.DeviceNames())
            device_select->addItem(name.c_str());
    }

    settings = new SettingsPanel(dm, config, main_widget);
    layout->addWidget(settings);

    connect(device_select, &QComboBox::currentTextChanged, this, &LTCWindow::DeviceSelected);
    DeviceSelected();
}

void LTCWindow::DeviceSelected()
{
    auto device_name = device_select->currentText().toStdString();
    auto device = dm.DeviceByName(device_name);
    settings->SelectDevice(device);
}

