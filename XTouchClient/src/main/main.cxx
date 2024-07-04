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
    PassFileDataToVector(global::res_dir_path + "UIData", fileData, '#');

    global::loginWindowUI = global::res_dir_path + fileData[0];
    global::adminWindowUI = global::res_dir_path + fileData[1];
    global::mainWindowUI = global::res_dir_path + fileData[2];
    global::viewportWindowUI = global::res_dir_path + fileData[3];
    global::paintWindowUI = global::res_dir_path + fileData[4];

    global::mainWindowCSS = global::res_dir_path + fileData[5];
    global::inFolderWindowCSS = global::res_dir_path + fileData[6];
    global::viewportWindowCSS = global::res_dir_path + fileData[7];
    global::paintWindowCSS = global::res_dir_path + fileData[8];
}

void GetConfigData()
{
    std::vector<std::string> fileData;
    PassFileDataToVector(global::res_dir_path + "Config", fileData, '#');

    global::serverIp = GetSubstringAfterSeparator('=', fileData[0], 2);
    global::serverPort = stoi(GetSubstringAfterSeparator('=', fileData[1], 2));

    global::saveModelPath = global::res_dir_path + GetSubstringAfterSeparator('=', fileData[2], 2);
    global::testModelFolder = GetSubstringAfterSeparator('=', fileData[3], 2);

    global::saveImagePath = global::res_dir_path + GetSubstringAfterSeparator('=', fileData[4], 2);
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

    global::app = Gtk::Application::create();

    std::string options;
    if(argc > 1) 
    {
        options = argv[1];
        if(options == "--res-dir") {
            global::res_dir_path = argv[2];
        }
    }

    if(!global::res_dir_path.size()) {
        global::res_dir_path = "../../../XTouchClient/res/";
    }

    GetApplicationData();
    InitWindows();

    Client::GetInstance().Connect(global::serverIp.c_str(), std::to_string(global::serverPort).c_str());
    loginWindow->OpenWindow();

    return global::app->run(*loginWindow->window);
}