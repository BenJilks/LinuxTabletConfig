#include <iostream>
#include <gtk/gtk.h>
#include "DeviceManager.hpp"

DeviceManager dm;
Device *curr_dev;

static void on_select_devices(GtkComboBox *combo_box, gpointer user_data)
{
	if (gtk_combo_box_get_active(combo_box) != 0)
	{
		gchar *name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
		curr_dev = dm.DeviceByName(name);
		g_free (name);
	}
}

static GtkWidget *device_select()
{
	GtkWidget *dev_select = gtk_grid_new();
	GtkWidget *dev_label = gtk_label_new("Device: ");
	gtk_grid_attach(GTK_GRID(dev_select), dev_label, 0, 0, 1, 1);

	GtkWidget *combo_box = gtk_combo_box_text_new();
	for (string device : dm.DeviceNames())
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), device.c_str());
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);
	gtk_widget_set_hexpand(combo_box, TRUE);
	gtk_grid_attach(GTK_GRID(dev_select), combo_box, 1, 0, 1, 1);
	return dev_select;
}

static GtkWidget *tablet_settings()
{
	GtkWidget *settings = gtk_button_new_with_label("Testing");
	gtk_widget_set_hexpand(settings, TRUE);
	gtk_widget_set_vexpand(settings, TRUE);
	return settings;
}

static void activate(GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Linux Tablet Config");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

	GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	GtkWidget *dev_select = device_select();
	GtkWidget *settings = tablet_settings();
	gtk_container_add(GTK_CONTAINER(content), dev_select);
	gtk_container_add(GTK_CONTAINER(content), settings);
	gtk_container_add(GTK_CONTAINER(window), content);

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
