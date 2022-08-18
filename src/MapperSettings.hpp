/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QWidget>
#include <QCheckBox>
#include <array>
#include "Mapper.hpp"
#include "VectorWidget.hpp"

class MapperSettings : public QWidget
{
    Q_OBJECT

public:
    MapperSettings(const DeviceManager &dm, QWidget *parent = nullptr);
    virtual ~MapperSettings() = default;

    // Mapper parse throughs
    inline void SetMap(std::array<float, 4> map) { mapper->SetMap(map); }
    inline std::array<float, 4> GetMap() const { return mapper->GetMap(); }
    inline void MapTo(Device *device) const { mapper->MapTo(device); }
    inline void SetDevice(Device *device) { mapper->SetDevice(device); }
    inline void SetMonitor(int id) { mapper->SetMonitor(id); }
    
    void SignalChanged(std::function<void()> func) { on_changed = func; }
    void Update();

public slots:
    void OnChanged();

private:
    void OnKeepAspect();
    void OnFill();

    std::function<void()> on_changed;
    Mapper *mapper { nullptr };

    VectorWidget<2> *start { nullptr };
    VectorWidget<2> *end { nullptr };
    QCheckBox *keep_aspect { nullptr };

};
