#include <thread>

#include "ControlPanel.hxx"

class ModelFileManager : public ControlPanel
{
public:
    void OpenOcctWindow();
    void ProcessFile();

private:
    std::string pathToFile;
    int client = 0;

    std::string ConvertToString(char *a, int size);

    bool ServerConnection();
    void GetDataFromServer();

    bool HideMainWindow();

    void SetupX11Backend();

    std::string GetRelativeFilePath();

    void OpenTestStepFile();
    void OpenFile(std::string relativeFilePath);

    bool hasCreatedOcct = false;
    bool isFileReady = false;
    bool hasDisplayedModel = false;
    bool needToDownload = false;

    std::thread downloadFileThread;
    std::thread openFileThread;
};