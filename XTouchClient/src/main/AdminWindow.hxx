class AdminWindow
{
public:
    Gtk::Window *window;

public:
    void OpenWindow();
    AdminWindow();

private:
    Glib::RefPtr<Gtk::Builder> uiBuilder;
    Gtk::Entry *loginInput;
    Gtk::Entry *passwordInput;
    Gtk::Button *addUserBtn;

private:
    void AddUser();
    void ProcessWidgets();
};