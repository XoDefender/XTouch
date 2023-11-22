#pragma once

class ControlPanel
{
public:
    Gtk::EventBox *closeWindow;
    Gtk::EventBox *fullscreen;

public:
    void ProcessPanel(Glib::RefPtr<Gtk::Builder> uiBuilder, Gtk::Window *window, std::string screenName);
    bool CloseWindow(GdkEventButton *theEvent, Gtk::Window *window);
    bool Iconify(GdkEventButton *theEvent, Gtk::Window *window);
    void ShowWindow(Gtk::Window *window);
};
