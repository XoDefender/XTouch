#include "Server.hxx"

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

string GetSubstringAfterSeparator(char separator, string data, int offsetAfterSeparator = 0)
{
    return data.substr(data.find(separator) + offsetAfterSeparator);
}

void ParseConfigData(initializer_list<string *> args)
{
    vector<string> fileData;
    if (PassFileDataToVector("../../../XTouchServer/res/Config", fileData, '#'))
    {
        int i = 0;
        for (auto arg : args)
        {
            *arg = GetSubstringAfterSeparator('=', fileData[i], 2);
            i++;
        }
    }
}

int main()
{
    string ip, name, password, schema;
    ParseConfigData({&ip, &name, &password, &schema});

    Server server(60000);
    server.ConnectToMySQL(ip, name, password, schema);
    server.Start();

    return 0;
}