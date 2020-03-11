#pragma once

#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <array>
#include <functional>
using std::array;

template<int size>
class VectorWidget
{
public:
    template<typename ...Args>
    VectorWidget(Args ...args)
    {
        const char *names[] = { args... };
        
        for (int i = 0; i < size; i++)
        {
            labels[i] = Gtk::Label(string(names[i]) + ": ");
            labels[i].set_margin_left(5);
            labels[i].set_margin_right(10);

            entries[i].signal_key_release_event().connect(
                sigc::mem_fun(this, &VectorWidget::OnChanged));
        }
    }

    VectorWidget(Gtk::Grid &grid, int index)
        : VectorWidget(grid, index, 
            "x", "y", "z", "w") {}

    void AttachGrid(Gtk::Grid &grid, int index)
    {
        for (int i = 0; i < size; i++)
        {
            grid.attach(labels[i],  i*2 + 0, index);
            grid.attach(entries[i], i*2 + 1, index);
        }
    }

    template<typename ...Args>
    void Update(Args ...args) 
    {
        float values[] = { args... }; 
        for (int i = 0; i < size; i++)
            entries[i].set_text(std::to_string(values[i]));
    }

    array<float, size> GetValues()
    {
        array<float, size> values;
        for (int i = 0; i < size; i++)
            values[i] = std::atof(entries[i].get_text().c_str());

        return values;
    }

    void SignalChanged(std::function<void()> func) 
    { 
        on_changed = func; 
    }

    bool OnChanged(GdkEventKey*)
    {
        if (on_changed)
            on_changed();

        return true;
    }

private:
    std::function<void()> on_changed;
    Gtk::Label labels[size];
    Gtk::Entry entries[size];

};
