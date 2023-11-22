enum class LoginState : uint32_t
{
    Local,
    Server,
};

class LoginWindow
{
public:
    Gtk::Window *window;

public:
    void OpenWindow(LoginState loginState);
    LoginWindow();

private:
    Glib::RefPtr<Gtk::Builder> uiBuilder;
    Gtk::Button *loginBtn;
    Gtk::Entry *loginInput;
    Gtk::Entry *passwordInput;
    Gtk::Label *loginStatus;

    LoginState loginState;

private:
    void ProcessQueries();
    void ProcessWidgets();
    void OnLoginClick();
};