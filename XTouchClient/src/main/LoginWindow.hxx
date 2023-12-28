class LoginWindow
{
public:
    Gtk::Window *window;

public:
    void OpenWindow();
    LoginWindow();

private:
    Glib::RefPtr<Gtk::Builder> uiBuilder;
    Gtk::Button *loginBtn;
    Gtk::Entry *loginInput;
    Gtk::Entry *passwordInput;
    Gtk::Label *loginStatus;

private:
    bool ProcessLoginInput();
    void LoginUser();
    void ProcessWidgets();
    void OnLoginClick();
};