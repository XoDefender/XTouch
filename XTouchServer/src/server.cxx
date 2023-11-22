#include <iostream>
#include <initializer_list>
#include <string>

#include "ocl_net.hxx"

using namespace std;

class CustomServer : public olc::net::server_interface<MsgTypes>
{
public:
	CustomServer(uint16_t nPort) : olc::net::server_interface<MsgTypes>(nPort)
	{
	}

protected:
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<MsgTypes>> client)
	{
		// olc::net::message<MsgTypes> msg;
		// msg.header.id = MsgTypes::ServerAccept;
		// client->Send(msg);
		return true;
	}

	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<MsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	virtual void OnMessage(std::shared_ptr<olc::net::connection<MsgTypes>> client, olc::net::message<MsgTypes> &msg)
	{
		switch (msg.header.id)
		{
		}
	}
};

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
	if (PassFileDataToVector("../../Config", fileData, '#'))
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

	CustomServer server(60000);
	server.Start();
	server.ConnectToMySQL(ip, name, password, schema);

	while (1)
	{
		server.Update(-1, true);
	}

	return 0;
}
