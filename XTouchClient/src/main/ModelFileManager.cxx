#include "global.hxx"

#include "OcctGtkViewer.hxx"
#include "InFolderWindow.hxx"
#include "MainWindow.hxx"
#include "ModelFileManager.hxx"

#include "IconvWrapper.hxx"

#include <OSD.hxx>
#include <OSD_Environment.hxx>

using namespace std;

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

    if (!global::downloadStepFile)
    {
        if (!(global::currentFileName.find(".txt") != string::npos))
            relativeFilePathStructure
                << global::saveFolderPath << inFolderWindow->ToXbfFileExtension(global::currentFileName) << "";
        else
            relativeFilePathStructure << global::saveFolderPath << global::currentFileName << "";
    }
    else
    {
        relativeFilePathStructure << global::saveFolderPath << global::currentFileName << "";
    }

    string relativeFilePath = relativeFilePathStructure.str();

    ofstream stepFile;
    stepFile.open(relativeFilePath);

    net::message<MsgTypes> iMsg;

    if (!global::downloadStepFile)
    {
        if (!(global::currentFileName.find(".txt") != string::npos))
            iMsg << inFolderWindow->ToXbfFileExtension(global::currentFileName).c_str();
        else
            iMsg << global::currentFileName.c_str();
    }
    else
    {
        iMsg << global::currentFileName.c_str();
    }

    net::message<MsgTypes> oMsg = Client::GetInstance().SendRequestToServer(MsgTypes::GetModelFile, iMsg);

    char fileData[oMsg.size()];

    int sizeToCopy = oMsg.size();

    // Physically copy the data from the vector into the user variable
    std::memcpy(fileData, oMsg.body.data(), sizeToCopy);

    // Shrink the vector to remove read bytes, and reset end position
    oMsg.body.resize(0);

    // Recalculate the message size
    oMsg.header.size = oMsg.size();

    // Write to the file
    int i = 0;
    while (i < sizeof(fileData))
    {
        stepFile << fileData[i];
        i++;
    }

    isFileReady = true;
}

void ModelFileManager::DecodeStepFileToUTF8(string relativeFilePath)
{
    iconv_wrapper conv{"CP1251"};

    // название для перекодированного файла
    string encodedFileName = relativeFilePath + "Encoded.STEP";

    fstream infile;
    ofstream outfile(encodedFileName);

    infile.open(relativeFilePath, ios::in);
    if (infile.is_open())
    {
        string tp;
        while (getline(infile, tp))
        {
            outfile << conv.convert(tp) << endl;
        }

        // удаляем изначальный файл
        infile.close();
        std::remove(relativeFilePath.c_str());

        // переименовываем перекодированный файл
        outfile.close();
        rename(encodedFileName.c_str(), relativeFilePath.c_str());

        // глобальная переменная, которая содержит путь к получившемуся файлу
        pathToFile = relativeFilePath;
    }
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

    inFolderWindow->ResetFileBlockSelection();

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

    if (!global::downloadStepFile)
    {
        if (!(global::currentFileName.find(".txt") != string::npos))
            relativeFilePathStructure << global::saveFolderPath << inFolderWindow->ToXbfFileExtension(global::currentFileName) << "";
        else
            relativeFilePathStructure << global::saveFolderPath << global::currentFileName << "";
    }
    else
    {
        relativeFilePathStructure << global::saveFolderPath << global::currentFileName << "";
    }

    return relativeFilePathStructure.str();
}

void ModelFileManager::OpenTestStepFile()
{
    pathToFile = global::testModelFolder;
    OpenOcctWindow();
}

void ModelFileManager::OpenFile(string relativeFilePath)
{
    if (!global::downloadStepFile)
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

            inFolderWindow->ResetFileBlockSelection();
        }
    }
    else
    {
        if (relativeFilePath.find(".STEP") != string::npos ||
            relativeFilePath.find(".step") != string::npos ||
            relativeFilePath.find(".stp") != string::npos)
        {
            DecodeStepFileToUTF8(relativeFilePath);
            OpenOcctWindow();
        }
        else if (relativeFilePath.find(".txt") != string::npos)
        {
            if (needToDownload)
                downloadFileThread.join();

            string ProcessFileRequest = "xdg-open " + string("\"") + relativeFilePath + string("\"");
            system(ProcessFileRequest.c_str());

            inFolderWindow->ResetFileBlockSelection();
        }
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
