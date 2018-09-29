#include <iostream>
#include <gtk/gtk.h>
#include "DeviceManager.hpp"
#include "Mapper.hpp"

// Devices
static const DeviceManager dm;
static Device *curr_dev = nullptr;
static Mapper mp(dm);

// Control widgets
static GtkWidget *mode_box = nullptr;

static void on_select_devices(GtkComboBox *combo_box, gpointer user_data)
{
	// Fetch selected device
	gchar *name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
	curr_dev = dm.DeviceByName(name);
	g_free(name);

	// If the mode box has been loaded, display the current mode
	if (mode_box != nullptr)
		gtk_combo_box_set_active(GTK_COMBO_BOX(mode_box), curr_dev->GetMode());
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

static GtkWidget *tablet_settings()
{
	GtkWidget *settings, *mode_l, *mapper;

	// Create settings pannel
	settings = gtk_grid_new();
	gtk_widget_set_hexpand(settings, TRUE);
	gtk_widget_set_vexpand(settings, TRUE);

	// Create mode setting UI
	mode_l = gtk_label_new("Mode");
	mode_box = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_box), "Absolute");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_box), "Relitive");
	g_signal_connect(mode_box, "changed", G_CALLBACK(on_select_mode), NULL);
	gtk_widget_set_hexpand(mode_box, TRUE);
	gtk_widget_set_margin_start(mode_box, 10);
	
	// If a device has been selected, then set value, otherwise set to default
	if (curr_dev != nullptr)
		gtk_combo_box_set_active(GTK_COMBO_BOX(mode_box), curr_dev->GetMode());
	else
		gtk_combo_box_set_active(GTK_COMBO_BOX(mode_box), 0);

	// Create tablet map UI
	mapper = gtk_drawing_area_new();
	gtk_widget_set_size_request(mapper, 200, 200);
	gtk_widget_set_margin_top(mapper, 10);
	g_signal_connect(mapper, "draw", G_CALLBACK(on_mapper_draw), NULL);

	// Add widgets to settings pannel
	gtk_grid_attach(GTK_GRID(settings), mode_l, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(settings), mode_box, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(settings), mapper, 0, 1, 2, 1);
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
	GtkWidget *dev_select = device_select();
	GtkWidget *settings = tablet_settings();
	gtk_container_add(GTK_CONTAINER(content), dev_select);
	gtk_container_add(GTK_CONTAINER(content), settings);
	gtk_container_add(GTK_CONTAINER(window), content);

	// Show UI content
	gtk_widget_show_all(window);
}

int main (int argc, char **argv)
{
	GtkApplication *app;
	int status;

	app = gtk_application_new("org.benjilks.linuxtabletconfig", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run(G_APPLICATION (app), argc, argv);
	g_object_unref(app);

	return status;
}
