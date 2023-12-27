#include <gtkmm.h>

extern class LoginWindow *loginWindow;
extern class AdminWindow *adminWindow;
extern class MainWindow *mainWindow;
extern class InFolderWindow *inFolderWindow;
extern class PaintWindow *paintWindow;
extern class ModelFileManager *modelFileManager;
extern class OcctGtkViewer *aGtkWin;

namespace global
{
    extern Glib::RefPtr<Gtk::Application> app;

    extern std::string currentUserName;
    extern std::string currentUserHash;
    extern std::string currentUserGroup;
    extern std::string currentModelName;
    extern std::string currentModelFolder;
    extern std::string currentFileName;

    extern std::string serverIp;
    extern uint16_t serverPort;

    extern std::string saveModelPath;
    extern std::string testModelFolder;

    extern std::string saveImagePath;
    extern std::string saveImagePrefix;

    extern std::string loginWindowUI;
    extern std::string adminWindowUI;
    extern std::string mainWindowUI;
    extern std::string viewportWindowUI;
    extern std::string paintWindowUI;

    extern std::string mainWindowCSS;
    extern std::string inFolderWindowCSS;
    extern std::string viewportWindowCSS;
    extern std::string paintWindowCSS;
}