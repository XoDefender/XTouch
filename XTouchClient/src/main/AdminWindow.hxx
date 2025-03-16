enum class ProcessUserActions : uint32_t
{
	AddUser,
	RemoveUser,
};

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
    Gtk::Button *removeUserBtn;
    Gtk::Label *status;

private:
    void ProcessUser(ProcessUserActions action);
    void ProcessWidgets();
};