#include "global.hxx"

using namespace sql;
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
ModelFileManager *modelFileManager;
OcctGtkViewer *aGtkWin;

string global::loginWindowUI;
string global::adminWindowUI;
string global::mainWindowUI;
string global::viewportWindowUI;

string global::mainWindowCSS;
string global::inFolderWindowCSS;
string global::viewportWindowCSS;

string global::serverIp;
uint16_t global::serverPort;

string global::saveFolderPath;
string global::testModelFolder;

string global::loginFilePath;

bool global::downloadStepFile;