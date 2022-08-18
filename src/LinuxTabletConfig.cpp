/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string>
#include <iostream>
#include <pwd.h>
#include <unistd.h> 
#include <sys/dir.h> 
#include <sys/stat.h> 

#include <QApplication>
#include "Config.hpp"
#include "Window.hpp"
#include "DeviceManager.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

	struct passwd *pw = getpwuid(getuid());
	mkdir((std::string(pw->pw_dir) + "/.local").c_str(), 0777);
	mkdir((std::string(pw->pw_dir) + "/.local/share").c_str(), 0777);
	mkdir((std::string(pw->pw_dir) + "/.local/share/LinuxTabletConfig").c_str(), 0777);

	string data_path = std::string(pw->pw_dir) + "/.local/share/LinuxTabletConfig";
	Config config(data_path + "/config.json");

	DeviceManager dm;
	LTCWindow window(dm, config);
    window.show();
	return app.exec();
}

