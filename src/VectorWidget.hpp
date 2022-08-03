#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QBoxLayout>
#include <array>
#include <functional>

template<int length>
class VectorWidget : public QWidget
{
public:
    template<typename ...Args>
    VectorWidget(Args ...args)
    {
        auto layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
        setLayout(layout);

        const char *names[] = { args... };
        for (int i = 0; i < length; i++)
        {
            auto label = new QLabel(names[i], this);
            label->setContentsMargins(5, 0, 10, 0);
            layout->addWidget(label);

            entries[i] = new QLineEdit(this);
            connect(entries[i], &QLineEdit::textChanged, this, &VectorWidget<length>::OnTextChanged);
            layout->addWidget(entries[i]);
        }
    }

    virtual ~VectorWidget() = default;

    template<typename ...Args>
    void Update(Args ...args) 
    {
        float values[] = { args... }; 
        for (int i = 0; i < length; i++)
            entries[i]->setText(std::to_string(values[i]).c_str());
    }

    std::array<float, length> GetValues()
    {
        std::array<float, length> values;
        for (int i = 0; i < length; i++)
            values[i] = std::atof(entries[i]->text().toLocal8Bit().data());
        return values;
    }

    void SignalChanged(std::function<void()> func) 
    { 
        on_changed = func; 
    }

    bool OnChanged()
    {
        if (on_changed)
            on_changed();
        return true;
    }

    void OnTextChanged(const QString&)
    {
        OnChanged();
    }

private:
    std::function<void()> on_changed;
    std::array<QLineEdit*, length> entries;

};
