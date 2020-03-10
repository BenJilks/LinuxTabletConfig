#include <iostream>
#include <pwd.h>
#include <sys/dir.h> 
#include <gtkmm/application.h>
#include "Config.hpp"
#include "Window.hpp"
#include "DeviceManager.hpp"
#include "Mapper.hpp"
using std::string;

int main(int argc, char **argv)
{
	struct passwd *pw = getpwuid(getuid());
	mkdir((string(pw->pw_dir) + "/.local").c_str(), 0777);
	mkdir((string(pw->pw_dir) + "/.local/share").c_str(), 0777);
	mkdir((string(pw->pw_dir) + "/.local/share/LinuxTabletConfig").c_str(), 0777);

	string data_path = string(pw->pw_dir) + "/.local/share/LinuxTabletConfig";
	Config config(data_path + "/config.json");
	auto app = Gtk::Application::create(
		argc, argv, "org.benjilks.ltc");

	DeviceManager dm;
	LTCWindow window(dm, config);

	return app->run(window);
}
