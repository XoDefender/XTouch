#include "net_server.hxx"

using namespace std;
using namespace sql;

MsgTypes operator|(MsgTypes first_type, MsgTypes second_type)
{
	using underlying_type = std::underlying_type_t<MsgTypes>;
	return static_cast<MsgTypes>(static_cast<underlying_type>(first_type) | static_cast<underlying_type>(second_type));
}

class CustomServer : public net_server<MsgTypes>
{
public:
	CustomServer(uint16_t nPort) : net_server<MsgTypes>(nPort, 10)
	{
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

	string GetSubstringAfterSeparator(char separator, string data, int offsetAfterSeparator = 0)
	{
		return data.substr(data.find(separator) + offsetAfterSeparator);
	}

	void ParseConfigData(initializer_list<string *> args)
	{
		vector<string> fileData;
		if (PassFileDataToVector("/home/xodefender/Apps/XTouch/XTouchServer/res/Config", fileData, '#'))
		{
			int i = 0;
			for (auto arg : args)
			{
				*arg = GetSubstringAfterSeparator('=', fileData[i], 2);
				i++;
			}
		}
	}

	void ConnectToMySQL(string ip, string name, string password, string schema)
	{
		Driver *driver = get_driver_instance();
		Connection *con = driver->connect(ip, name, password);
		con->setSchema(schema);
		stmt = con->createStatement();
	}
};

int main()
{
	string ip, name, password, schema;
	CustomServer server(60000);
	server.ParseConfigData({&ip, &name, &password, &schema});
	server.ConnectToMySQL(ip, name, password, schema);
	server.Start();

	return 0;
}
