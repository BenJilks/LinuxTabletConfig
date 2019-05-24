#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm> 
#include <gtk/gtk.h>
#include <pwd.h>
#include <sys/stat.h>
#include "DeviceManager.hpp"
#include "Mapper.hpp"
using std::string;

// User Directory
static string data_path = "/.local/share/LinuxTabletConfig";
static struct passwd *pw = getpwuid(getuid());
static string home_dir = "";

// Devices
static const DeviceManager dm;
static Device *curr_dev = nullptr;
static Mapper mp(dm);

// Control widgets
static GtkWidget *mapper_ui = nullptr;
static GtkWidget *mode_box = nullptr;
static GtkWidget *monitor_box = nullptr;

// trim from start
static inline string &ltrim(string &s) 
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline string &rtrim(string &s) 
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline string &trim(string &s) 
{
    return ltrim(rtrim(s));
}

static void load_value(string key, string value)
{
	std::cout << key << " = " << value << std::endl;
	if (key == "mode")
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(mode_box), 
			value == "absolute" ? 0 : 1);
	}
	else if (key == "monitor")
	{
		int monitor = atoi(value.c_str());
		mp.SetMonitor(monitor);
		gtk_combo_box_set_active(GTK_COMBO_BOX(monitor_box), monitor);
	}
	else if (key == "area")
	{
		float start_x, end_x, start_y, end_y;
		sscanf(value.c_str(), "%f,%f,%f,%f", 
			&start_x, &end_x, &start_y, &end_y);

		mp.SetMapping(start_x, end_x, start_y, end_y);
		gtk_widget_queue_draw(mapper_ui);
	}
}

// Load the current state from the config file
static void load_info()
{
	struct stat info;
	string line;
	string key, value;

	// Check if directory exists
	home_dir = string(pw->pw_dir) + data_path;
	if(stat(home_dir.c_str(), &info) != 0)
		mkdir(home_dir.c_str(), 0777);
	
	string config_path = home_dir + "/tablet.conf";
	std::ifstream file(config_path);
	if (file.good())
	{
		while (std::getline(file, line))
		{
			std::istringstream line_s(line);
			if (std::getline(line_s, key, '=') && std::getline(line_s, value))
				load_value(trim(key), trim(value));
		}
	}

	if (curr_dev != nullptr)
		mp.MapTo(curr_dev);
}

// Save the state to the config file
static void save_info()
{
	std::cout << "Saving config..." << std::endl;

	string config_path = home_dir + "/tablet.conf";
	std::ofstream file(config_path);
	file << "mode = " << (curr_dev->GetMode() ? "relative" : "absolute") << std::endl;
	file << "monitor = " << mp.GetMonitor() << std::endl;
	file << "area = " << mp.GetAreaString() << std::endl;
}

static void on_select_devices(GtkComboBox *combo_box, gpointer user_data)
{
	// Fetch selected device
	gchar *name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
	if (name != nullptr)
	{
		curr_dev = dm.DeviceByName(name);
		g_free(name);

		// If the mode box has been loaded, display the current mode
		if (mode_box != nullptr)
			gtk_combo_box_set_active(GTK_COMBO_BOX(mode_box), curr_dev->GetMode());
	}
}

static GtkWidget *device_select()
{
	GtkWidget *dev_select, *dev_label, 
		*combo_box;

	// Create device select UI
	dev_select = gtk_grid_new();
	dev_label = gtk_label_new("Device: ");
	gtk_grid_attach(GTK_GRID(dev_select), dev_label, 0, 0, 1, 1);

	// Create device selection box
	combo_box = gtk_combo_box_text_new();
	for (string device : dm.DeviceNames())
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), device.c_str());
	g_signal_connect(combo_box, "changed", G_CALLBACK(on_select_devices), NULL);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);
	gtk_widget_set_hexpand(combo_box, TRUE);
	gtk_grid_attach(GTK_GRID(dev_select), combo_box, 1, 0, 1, 1);
	return dev_select;
}

static void on_select_mode(GtkComboBox *combo_box, gpointer user_data)
{
	// If a device has not been selected, then ignore command
	if (curr_dev == nullptr)
	{
		printf("Error: No device selected\n");
		return;
	}

	// Set tablet mode
	int mode = gtk_combo_box_get_active(combo_box);
	curr_dev->SetMode(mode);
}

static void on_select_monitor(GtkComboBox *combo_box, gpointer user_data)
{
	int monitor_id = gtk_combo_box_get_active(combo_box);
	const Monitor &monitor = dm.GetMonitor(monitor_id);
	mp.SetMonitor(monitor_id);
	monitor.MapTo(curr_dev);

	gtk_widget_queue_draw(mapper_ui);
}

static gboolean on_mapper_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	unsigned int width, height;
	GtkStyleContext *context;

	// Fetch draw area info
	context = gtk_widget_get_style_context(widget);
	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	// Draw mapper contentbackground
	gtk_render_background(context, cr, 0, 0, width, height);
	mp.Draw(cr, width, height);

	return FALSE;
}

static gboolean on_mapper_mouse_move(GtkWidget *widget, GdkEventMotion *event)
{
	int x, y;
	GdkModifierType state;

	// Fetch mouse position and state
	if (event->is_hint)
		gdk_window_get_device_position(event->window, event->device, &x, &y, &state);
	else
	{
		x = event->x;
		y = event->y;
		state = (GdkModifierType)event->state;
	}

	mp.MouseMoved(x, y);
	gtk_widget_queue_draw(widget);
	return TRUE;
}

static gboolean on_mapper_button_down(GtkWidget *widget, GdkEventButton *event)
{
	if (event->button == 1)
	{
		if (event->state != 0x100)
		{
			mp.MouseDown();
		}
		else
		{
			if (curr_dev != nullptr)
				mp.MapTo(curr_dev);
			mp.MouseUp();
		}
	}
	return TRUE;
}

static void mode_select(GtkWidget *settings, int pos)
{
	GtkWidget *mode_l;

	// Create mode setting UI
	mode_l = gtk_label_new("Mode");
	mode_box = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_box), "Absolute");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_box), "Relitive");
	g_signal_connect(mode_box, "changed", G_CALLBACK(on_select_mode), NULL);
	gtk_widget_set_hexpand(mode_box, TRUE);
	gtk_widget_set_margin_start(mode_box, 10);
	gtk_widget_set_margin_top(mode_box, 10);
	
	// If a device has been selected, then set value, otherwise set to default
	if (curr_dev != nullptr)
		gtk_combo_box_set_active(GTK_COMBO_BOX(mode_box), curr_dev->GetMode());
	else
		gtk_combo_box_set_active(GTK_COMBO_BOX(mode_box), 0);

	// Add to settings pannel
	gtk_grid_attach(GTK_GRID(settings), mode_l, 0, pos, 1, 1);
	gtk_grid_attach(GTK_GRID(settings), mode_box, 1, pos, 1, 1);
}

static void monitor_select(GtkWidget *settings, int pos)
{
	GtkWidget *monitor_l;
	int i;

	monitor_l = gtk_label_new("Monitor");
	monitor_box = gtk_combo_box_text_new();

	// Connect change signal
	g_signal_connect(monitor_box, "changed", 
		G_CALLBACK(on_select_monitor), NULL);

	// Add monitors to options
	for (i = 0; i < dm.GetMonitorCount(); i++)
	{
		const Monitor &m = dm.GetMonitor(i);
		gtk_combo_box_text_append_text(
			GTK_COMBO_BOX_TEXT(monitor_box), 
			m.GetName().c_str());
	}

	gtk_widget_set_hexpand(monitor_box, TRUE);
	gtk_widget_set_margin_start(monitor_box, 10);
	gtk_widget_set_margin_top(monitor_box, 10);
	gtk_combo_box_set_active(GTK_COMBO_BOX(monitor_box), 0);

	// Add to settings pannel
	gtk_grid_attach(GTK_GRID(settings), monitor_l, 0, pos, 1, 1);
	gtk_grid_attach(GTK_GRID(settings), monitor_box, 1, pos, 1, 1);
}

static GtkWidget *tablet_settings()
{
	GtkWidget *settings;

	// Create settings pannel
	settings = gtk_grid_new();
	gtk_widget_set_hexpand(settings, TRUE);
	gtk_widget_set_vexpand(settings, TRUE);

	// Create tablet map UI
	mapper_ui = gtk_drawing_area_new();
	gtk_widget_set_size_request(mapper_ui, 400, 200);
	gtk_widget_set_vexpand(mapper_ui, True);
	gtk_widget_set_margin_top(mapper_ui, 10);
	gtk_widget_set_events(mapper_ui, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(mapper_ui, "motion_notify_event", G_CALLBACK(on_mapper_mouse_move), NULL);
	g_signal_connect(mapper_ui, "button_press_event", G_CALLBACK(on_mapper_button_down), NULL);
	g_signal_connect(mapper_ui, "button_release_event", G_CALLBACK(on_mapper_button_down), NULL);
	g_signal_connect(mapper_ui, "draw", G_CALLBACK(on_mapper_draw), NULL);

	// Add widgets to settings pannel
	mode_select(settings, 0);
	monitor_select(settings, 1);
	gtk_grid_attach(GTK_GRID(settings), mapper_ui, 0, 2, 2, 1);
	return settings;
}

static void activate(GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;

	// Set window prefrences
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Linux Tablet Config");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

	// Create UI content
	GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_margin_start(content, 10);
	gtk_widget_set_margin_end(content, 10);
	gtk_widget_set_margin_top(content, 10);
	gtk_widget_set_margin_bottom(content, 10);
	
	GtkWidget *dev_select = device_select();
	GtkWidget *settings = tablet_settings();
	gtk_container_add(GTK_CONTAINER(content), dev_select);
	gtk_container_add(GTK_CONTAINER(content), settings);
	gtk_container_add(GTK_CONTAINER(window), content);
	load_info();

	// Show UI content
	gtk_widget_show_all(window);
}

int main (int argc, char **argv)
{
	GtkApplication *app;
	int status;

	vector<char*> gtk_argv = { argv[0] };
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--startup"))
		{
			if (!dm.HasDevices())
			{
				std::cout << "Could not find any devices" << std::endl;
				return -1;
			}

			curr_dev = dm.DefaultDevice();
			load_info();
			return 0;
		}
		else
		{
			gtk_argv.push_back(argv[i]);
		}
	}


	app = gtk_application_new("org.benjilks.linuxtabletconfig", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run(G_APPLICATION (app), gtk_argv.size(), &gtk_argv[0]);
	g_object_unref(app);

	save_info();
	return status;
}
