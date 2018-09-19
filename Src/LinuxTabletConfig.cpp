#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>
#include <X11/XKBlib.h>

/*
#include <gtk/gtk.h>
static void activate (GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;

	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "Window");
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
	gtk_widget_show_all (window);
}

int main (int argc, char **argv)
{
	GtkApplication *app;
	int status;

	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
*/

void scan_wacom()
{
	XDeviceInfo *info;
	int	ndevices, i;
	Atom wacom_prop;
	Display *dpy;

	dpy = XOpenDisplay(nullptr);
	wacom_prop = XInternAtom(dpy, XI_TABLET, True);
	if (wacom_prop == None)
		return;

	info = XListInputDevices(dpy, &ndevices);
	for (i = 0; i < ndevices; i++)
	{
		if (info[i].use == IsXPointer || info[i].use == IsXKeyboard)
			continue;

		if (info[i].type == wacom_prop)
			printf("%s: %i\n", info[i].name, info[i].id);
	}

	XFreeDeviceList(info);
}

int main()
{
	scan_wacom();
	return 0;
}
