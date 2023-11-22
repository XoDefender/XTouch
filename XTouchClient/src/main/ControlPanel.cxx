#include <iostream>
#include "global.hxx"
#include "ControlPanel.hxx"

// Подключение сигналов к панели управления окном
void ControlPanel::ProcessPanel(Glib::RefPtr<Gtk::Builder> uiBuilder, Gtk::Window *window, std::string screenName)
{
    if (screenName == "InFolderScreen")
    {
        uiBuilder->get_widget<Gtk::EventBox>("IFS_CloseScreen", closeWindow);
        uiBuilder->get_widget<Gtk::EventBox>("IFS_FullScreen", fullscreen);
    }
    else if (screenName == "MainScreen")
    {
        uiBuilder->get_widget<Gtk::EventBox>("MS_CloseScreen", closeWindow);
        uiBuilder->get_widget<Gtk::EventBox>("MS_FullScreen", fullscreen);
    }
    else
    {
        uiBuilder->get_widget<Gtk::EventBox>("CloseScreen", closeWindow);
        uiBuilder->get_widget<Gtk::EventBox>("FullScreen", fullscreen);
    }

    closeWindow->signal_button_press_event().connect(sigc::bind(sigc::mem_fun(*this, &ControlPanel::CloseWindow), window));
    fullscreen->signal_button_press_event().connect(sigc::bind(sigc::mem_fun(*this, &ControlPanel::Iconify), window));
}

bool ControlPanel::CloseWindow(GdkEventButton *theEvent, Gtk::Window *window)
{
    window->hide();
    return true;
}

bool ControlPanel::Iconify(GdkEventButton *theEvent, Gtk::Window *window)
{
    window->iconify();
    return true;
}

void ControlPanel::ShowWindow(Gtk::Window *window)
{
    window->fullscreen();
    window->show_all();

    // Добавляем окно в очередь. Пока очередь не пуста, приложение работает
    global::app->add_window(*window);
}
