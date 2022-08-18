/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "MapperSettings.hpp"
#include <QCheckBox>
#include <QBoxLayout>
#include <QPushButton>
#include <iostream>

MapperSettings::MapperSettings(const DeviceManager &dm, QWidget *parent)
    : QWidget(parent, {})
{
    auto layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    layout->setContentsMargins(0, 10, 0, 0);
    setLayout(layout);

    mapper = new Mapper(dm, this);
    mapper->SignalChanged([this]()
    {
        Update();
        if (on_changed)
            on_changed();
    });
    layout->addWidget(mapper);

    auto settings_widget = new QWidget(this);
    auto settings_layout = new QBoxLayout(QBoxLayout::Direction::Down);
    settings_widget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    settings_widget->setLayout(settings_layout);
    layout->addWidget(settings_widget, 0, Qt::AlignTop);

    auto fill = new QPushButton("Fill", this);
    keep_aspect = new QCheckBox("Keep Aspect Ratio", this);
    settings_layout->addWidget(fill);
    settings_layout->addWidget(keep_aspect, 0, Qt::AlignCenter);

    start = new VectorWidget<2>("Start X", "Start Y");
    start->SignalChanged([this]() { OnChanged(); });
    settings_layout->addWidget(start);

    end = new VectorWidget<2>("End X", "End Y");
    end->SignalChanged([this]() { OnChanged(); });
    settings_layout->addWidget(end);

    connect(fill, &QPushButton::clicked, this, &MapperSettings::OnFill);
    connect(keep_aspect, &QCheckBox::stateChanged, this, &MapperSettings::OnKeepAspect);
}

void MapperSettings::Update()
{
    auto map = mapper->GetMap();
    start->Update(map[0], map[1]);
    end->Update(map[2], map[3]);
    mapper->repaint();
}

void MapperSettings::OnChanged()
{
    std::array<float, 4> map;
    std::array<float, 2> start_values = start->GetValues();
    std::array<float, 2> end_values = end->GetValues();
    map[0] = start_values[0];
    map[1] = start_values[1];
    map[2] = end_values[0];
    map[3] = end_values[1];

    mapper->SetMap(map);
    mapper->repaint();

    if (on_changed)
        on_changed();
}

void MapperSettings::OnKeepAspect()
{
    mapper->SetKeepAspect(keep_aspect->isChecked());
}

void MapperSettings::OnFill()
{
    mapper->Fill();
    Update();

    if (on_changed)
        on_changed();
}
