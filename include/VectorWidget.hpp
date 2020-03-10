#pragma once

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>

template<unsigned size>
class VectorWidget : public Gtk::Box
{
public:
    VectorWidget()
    {
        static const char *names[] = { "x", "y", "z", "w" };
        
        for (int i = 0; i < size; i++)
        {
            labels[i] = Gtk::Label(string(names[i]) + ": ");
            labels[i].set_margin_left(5);
            labels[i].set_margin_right(10);
            add(labels[i]);
            add(entries[i]);
        }
    }

private:
    Gtk::Label labels[size];
    Gtk::Entry entries[size];

};
