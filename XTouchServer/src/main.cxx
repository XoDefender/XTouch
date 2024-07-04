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

void ParseConfigData(string config_path, initializer_list<string *> args)
{
    vector<string> fileData;
    if (PassFileDataToVector(config_path, fileData, '#'))
    {
        int i = 0;
        for (auto arg : args)
        {
            *arg = GetSubstringAfterSeparator('=', fileData[i], 2);
            i++;
        }
    }
}

int main(int argc, char* argv[])
{
    string ip, name, password, schema;
    string res_dir_path;
    string option;
    
    if(argc > 1) 
    {
        option = argv[1];
        if (option == "--res-dir") {
            res_dir_path = string(argv[2]);
        }
    }

    if(!res_dir_path.size()) {
        res_dir_path = "../../../XTouchServer/res/";
    }
    
    ParseConfigData(res_dir_path + "Config", {&ip, &name, &password, &schema});

    Server server(60000, res_dir_path);
    server.ConnectToMySQL(ip, name, password, schema);
    server.Start();

    return 0;
}