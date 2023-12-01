#include <iostream>
#include <sstream>
#include <fstream>

#include "global.hxx"
#include "Client.hxx"

#include "LoginWindow.hxx"
#include "AdminWindow.hxx"
#include "MainWindow.hxx"
#include "InFolderWindow.hxx"
#include "ModelFileManager.hxx"

using namespace std;

string GetSubstringAfterSeparator(char separator, string data, int offsetAfterSeparator = 0)
{
    return data.substr(data.find(separator) + offsetAfterSeparator);
}

bool PassFileDataToVector(string filePath, vector<string> &data, char ignoreStingSymbol = ' ')
{
    fstream dataFile;
    dataFile.open(filePath);

    if (dataFile.is_open())
    {
        string tp;
        while (getline(dataFile, tp))
        {
            if (tp[0] != ignoreStingSymbol && tp != "")
                data.push_back(tp);
        }

        dataFile.close();

        return true;
    }

    return false;
}

void ParseGladeData()
{
    vector<string> fileData;
    PassFileDataToVector("/home/xodefender/Apps/XTouch/XTouchClient/res/UIData", fileData);

    global::loginWindowUI = fileData[0];
    global::adminWindowUI = fileData[1];
    global::mainWindowUI = fileData[2];
    global::viewportWindowUI = fileData[3];
    global::mainWindowCSS = fileData[4];
    global::inFolderWindowCSS = fileData[5];
    global::viewportWindowCSS = fileData[6];
}

void ParseGlobalData()
{
    vector<string> fileData;
    PassFileDataToVector("/home/xodefender/Apps/XTouch/XTouchClient/res/Config", fileData, '#');

    global::serverIp = GetSubstringAfterSeparator('=', fileData[0], 2);
    global::serverPort = stoi(GetSubstringAfterSeparator('=', fileData[1], 2));

    global::saveFolderPath = GetSubstringAfterSeparator('=', fileData[2], 2);
    global::testModelFolder = GetSubstringAfterSeparator('=', fileData[3], 2);

    global::loginFilePath = GetSubstringAfterSeparator('=', fileData[6], 2);

    string toBool = GetSubstringAfterSeparator('=', fileData[7], 2);
    global::downloadStepFile = (toBool.at(0) == '1');
}

void ParseData()
{
    ParseGlobalData();
    ParseGladeData();
}

void InitWindows()
{
    loginWindow = new LoginWindow();
    adminWindow = new AdminWindow();
    mainWindow = new MainWindow();
    inFolderWindow = new InFolderWindow();
    modelFileManager = new ModelFileManager();
}

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    global::app = Gtk::Application::create(argc, argv, "");

    ParseData();
    InitWindows();

    try
    {
        Client::GetInstance().Connect(global::serverIp.c_str(), to_string(global::serverPort).c_str());
        loginWindow->OpenWindow(LoginState::Server);
    }
    catch (...)
    {
        loginWindow->OpenWindow(LoginState::Local);
    }

    return global::app->run(*loginWindow->window);
}