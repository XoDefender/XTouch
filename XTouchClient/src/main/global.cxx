#include "global.hxx"

using namespace std;
using namespace Glib;

string global::currentUserName;
string global::currentUserHash;
string global::currentUserGroup;
string global::currentModelName;
string global::currentModelFolder;
string global::currentFileName;

RefPtr<Gtk::Application> global::app;

LoginWindow *loginWindow;
AdminWindow *adminWindow;
MainWindow *mainWindow;
InFolderWindow *inFolderWindow;
PaintWindow* paintWindow;
ModelFileManager *modelFileManager;
OcctGtkViewer *aGtkWin;

string global::loginWindowUI;
string global::adminWindowUI;
string global::mainWindowUI;
string global::viewportWindowUI;
string global::paintWindowUI;

string global::mainWindowCSS;
string global::inFolderWindowCSS;
string global::viewportWindowCSS;
string global::paintWindowCSS;

string global::serverIp;
uint16_t global::serverPort;

string global::saveModelPath;
string global::testModelFolder;

string global::saveImagePath;
string global::saveImagePrefix;

string global::res_dir_path;