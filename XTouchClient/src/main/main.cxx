#include <iostream>
#include <sstream>
#include <fstream>

#include "global.hxx"

#include "Client.hxx"
#include "LoginWindow.hxx"
#include "AdminWindow.hxx"
#include "MainWindow.hxx"
#include "InFolderWindow.hxx"
#include "PaintWindow.hxx"
#include "ModelFileManager.hxx"

std::string GetSubstringAfterSeparator(char separator, std::string data, int offsetAfterSeparator = 0)
{
    return data.substr(data.find(separator) + offsetAfterSeparator);
}

bool PassFileDataToVector(std::string filePath, std::vector<std::string> &data, char ignoreStingSymbol = ' ')
{
    std::fstream dataFile;
    dataFile.open(filePath);

    if (dataFile.is_open())
    {
        std::string tp;
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

void GetUIData()
{
    std::vector<std::string> fileData;
    PassFileDataToVector("../../../XTouchClient/res/UIData", fileData, '#');

    global::loginWindowUI = fileData[0];
    global::adminWindowUI = fileData[1];
    global::mainWindowUI = fileData[2];
    global::viewportWindowUI = fileData[3];
    global::paintWindowUI = fileData[4];

    global::mainWindowCSS = fileData[5];
    global::inFolderWindowCSS = fileData[6];
    global::viewportWindowCSS = fileData[7];
    global::paintWindowCSS = fileData[8];
}

void GetConfigData()
{
    std::vector<std::string> fileData;
    PassFileDataToVector("../../../XTouchClient/res/Config", fileData, '#');

    global::serverIp = GetSubstringAfterSeparator('=', fileData[0], 2);
    global::serverPort = stoi(GetSubstringAfterSeparator('=', fileData[1], 2));

    global::saveModelPath = GetSubstringAfterSeparator('=', fileData[2], 2);
    global::testModelFolder = GetSubstringAfterSeparator('=', fileData[3], 2);

    global::saveImagePath = GetSubstringAfterSeparator('=', fileData[4], 2);
    global::saveImagePrefix = GetSubstringAfterSeparator('=', fileData[5], 2);
}

void GetApplicationData()
{
    GetConfigData();
    GetUIData();
}

void InitWindows()
{
    loginWindow = new LoginWindow();
    adminWindow = new AdminWindow();
    mainWindow = new MainWindow();
    inFolderWindow = new InFolderWindow();
    modelFileManager = new ModelFileManager();
    paintWindow = new PaintWindow();
}

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    global::app = Gtk::Application::create(argc, argv, "");

    GetApplicationData();
    InitWindows();

    Client::GetInstance().Connect(global::serverIp.c_str(), std::to_string(global::serverPort).c_str());
    loginWindow->OpenWindow();

    return global::app->run(*loginWindow->window);
}