#include "global.hxx"

#include "OcctGtkViewer.hxx"
#include "InFolderWindow.hxx"
#include "MainWindow.hxx"
#include "ModelFileManager.hxx"

#include "IconvWrapper.hxx"

#include <OSD.hxx>
#include <OSD_Environment.hxx>
#include <filesystem>

using namespace std;

namespace fs = std::filesystem;

string ModelFileManager::ConvertToString(char *a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++)
    {
        if (a[i] != '0' && a[i] != '1' && a[i] != '2' && a[i] != '3' && a[i] != '4' && a[i] != '5' && a[i] != '6' && a[i] != '7' && a[i] != '8' && a[i] != '9')
            break;

        s = s + a[i];
    }
    return s;
}

void ModelFileManager::GetDataFromServer()
{
    ostringstream relativeFilePathStructure;

    if (!fs::exists(global::saveModelPath)) 
        fs::create_directory(global::saveModelPath);

    if (!(global::currentFileName.find(".txt") != string::npos))
        relativeFilePathStructure << global::saveModelPath << inFolderWindow->ConvertExtensionToXbf(global::currentFileName) << "";
    else
        relativeFilePathStructure << global::saveModelPath << global::currentFileName << "";

    string relativeFilePath = relativeFilePathStructure.str();

    ofstream stepFile;
    stepFile.open(relativeFilePath);

    net::message<MsgTypes> iMsg;

    if (!(global::currentFileName.find(".txt") != string::npos))
        iMsg << inFolderWindow->ConvertExtensionToXbf(global::currentFileName).c_str();
    else
        iMsg << global::currentFileName.c_str();

    net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(MsgTypes::GetModelFile, iMsg);
    stepFile.write((char *)oMsg.body.data(), oMsg.body.size());
    isFileReady = true;
}

void ModelFileManager::OpenOcctWindow()
{
    SetupX11Backend();

    Glib::signal_timeout().connect([this]()
                                   {
                                       if (hasDisplayedModel)
                                       {
                                           openFileThread.join();
                                            
                                           ShowWindow(aGtkWin);
                                           aGtkWin->CenterModel();
                                           aGtkWin->HideWidgets();
                                           aGtkWin->CreateModelTree();

                                           hasDisplayedModel = false;

                                           Glib::signal_timeout().connect(sigc::mem_fun(*this, &ModelFileManager::HideMainWindow), 300);

                                           return false;
                                       }

                                       return true; },
                                   100);

    Glib::signal_timeout().connect([this]()
                                   {
                                    if(isFileReady)
                                    {
                                        isFileReady = false;

                                        if(downloadFileThread.joinable())
                                            downloadFileThread.join();

                                     if (!hasCreatedOcct)
                                       {
                                           aGtkWin = new OcctGtkViewer(pathToFile);
                                           hasCreatedOcct = true;
                                           openFileThread = thread([this](){ 
                                           aGtkWin->DisplayModel();
                                           hasDisplayedModel = true; });

                                           return false;
                                       }
                                       else
                                       {
                                            aGtkWin->OpenWindow(pathToFile);
                                            openFileThread = thread([this](){
                                            aGtkWin->DisplayModel();
                                            hasDisplayedModel = true; });

                                            return false;
                                       }   
                                    }

                                    return true; },
                                   100);
}

bool ModelFileManager::HideMainWindow()
{
    mainWindow->hide();
    inFolderWindow->StopDownloadAnimation();
    return false;
}

void ModelFileManager::SetupX11Backend()
{
    OSD::SetSignal(false);
    OSD::SetSignalStackTraceLength(10);

    OSD_Environment aBackend("GDK_BACKEND");
    aBackend.SetValue("x11");
    aBackend.Build();
}

string ModelFileManager::GetRelativeFilePath()
{
    ostringstream relativeFilePathStructure;

    if (!(global::currentFileName.find(".txt") != string::npos))
        relativeFilePathStructure << global::saveModelPath << inFolderWindow->ConvertExtensionToXbf(global::currentFileName) << "";
    else
        relativeFilePathStructure << global::saveModelPath << global::currentFileName << "";

    return relativeFilePathStructure.str();
}

void ModelFileManager::OpenTestStepFile()
{
    pathToFile = global::testModelFolder;
    OpenOcctWindow();
}

void ModelFileManager::OpenFile(string relativeFilePath)
{
    if (relativeFilePath.find(".xbf") != string::npos)
    {
        pathToFile = relativeFilePath;
        OpenOcctWindow();
    }
    else if (relativeFilePath.find(".txt") != string::npos)
    {
        if (needToDownload)
            downloadFileThread.join();

        string ProcessFileRequest = "xdg-open " + string("\"") + relativeFilePath + string("\"");
        system(ProcessFileRequest.c_str());

        inFolderWindow->StopDownloadAnimation();
    }
}

void ModelFileManager::ProcessFile()
{
    if (global::testModelFolder != "nullptr")
    {
        OpenTestStepFile();
        return;
    }

    string relativeFilePath = GetRelativeFilePath();
    ifstream file;
    file.open(relativeFilePath);

    if (!file)
    {
        needToDownload = true;
        downloadFileThread = thread(&ModelFileManager::GetDataFromServer, this);
        OpenFile(relativeFilePath);
    }
    else
    {
        needToDownload = false;
        isFileReady = true;
        OpenFile(relativeFilePath);
    }
}
