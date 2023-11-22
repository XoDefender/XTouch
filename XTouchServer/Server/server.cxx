#include <iostream>
#include <initializer_list>
#include <string>

#include "../ocl_net.hxx"

using namespace std;

enum class MsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,

	PasswordLogin,

	GetModels,
	GetModelFiles,

	AddModelToFavorite,
	RemoveModelFromFavorite,

	GetModelByName,
	GetFavoriteModels,

	SortModels,
	SortFavoriteModels,
	SortFiles,

	GetModelFile,
};

class CustomServer : public olc::net::server_interface<MsgTypes>
{
public:
	CustomServer(uint16_t nPort) : olc::net::server_interface<MsgTypes>(nPort)
	{
	}

private:
	struct UserData
	{
		string name;
		string password;
		string group;
		string ip;
		string observingModel;
		string startSession;
	};

	UserData userData;

protected:
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<MsgTypes>> client)
	{
		// olc::net::message<MsgTypes> msg;
		// msg.header.id = MsgTypes::ServerAccept;
		// client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<MsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection<MsgTypes>> client, olc::net::message<MsgTypes> &msg)
	{
		switch (msg.header.id)
		{
		case MsgTypes::PasswordLogin:
		{
			char userName[1024];
			char userPassword[1024];

			msg >> userPassword >> userName;

			userData.name = string(userName);
			userData.password = string(userPassword);

			std::ostringstream queryStruct;
			queryStruct << "select user_group from users where user_name='" << userName
						<< "' and user_password='" << userPassword << "'";
			std::string query = queryStruct.str();

			ResultSet *res = stmt->executeQuery(query);

			if (res->next())
			{
				userData.group = res->getString(1);
				msg.header.id = MsgTypes::ServerAccept;
			}
			else
				msg.header.id = MsgTypes::ServerDeny;

			msg << "tmp";

			client->Send(msg);

			break;
		}

		case MsgTypes::GetModels:
		{
			msg.body.clear();

			int modelsAmount = 0;

			string query;
			query = "select model_name, model_folder, model_create_date from models where model_name != '' and model_available_from = 'user'";

			ResultSet *res = stmt->executeQuery(query);

			while (res->next())
				modelsAmount++;

			string modelsAmountStr = to_string(modelsAmount);

			res = stmt->executeQuery(query);

			while (res->next())
			{
				string fileName = res->getString(1);
				string categoryName = res->getString(2);
				string dateName = res->getString(3);

				msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

				// если модель находится в избранных, то разукрашиваем кнопку
				ostringstream queryStruct;
				queryStruct << "select * from favorites where user_name ='" << userData.name << "' and model_name ='" << fileName.c_str() << "'";
				string querySelectFavorites = queryStruct.str();

				ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

				if (isModelFavoriteRes->next())
					msg << "true";
				else
					msg << "false";

				delete isModelFavoriteRes;
			}

			msg << modelsAmountStr.c_str();

			client->Send(msg);

			delete res;

			break;
		}

			// 	case MsgTypes::AddUserToTable:
			// 	{
			// 		std::ostringstream queryStructCheckUser;
			// 		queryStructCheckUser << "select * from users where name='" << userData.name << "'";

			// 		std::string query = queryStructCheckUser.str();
			// 		ResultSet *res = stmt->executeQuery(query);

			// 		if (res->next())
			// 			msg << "This user already exists!";
			// 		else
			// 		{
			// 			std::ostringstream queryStructAddUser;
			// 			queryStructAddUser << "INSERT INTO users VALUES('" << userData.name << "','" << userData.password << "', 'user')";
			// 			std::string query = queryStructAddUser.str();

			// 			stmt->execute(query);

			// 			msg << "The user is added";
			// 		}

			// 		client->Send(msg);

			// 		break;
			// 	}

			// 	case MsgTypes::AddModelToFavorite:
			// 	{
			// 		char modelName[1024];
			// 		msg >> modelName;

			// 		ostringstream queryStruct;
			// 		queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << modelName << "'";
			// 		string queryCheckIfFavorite = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(queryCheckIfFavorite);

			// 		if (!res->next())
			// 		{
			// 			ostringstream queryStruct;
			// 			queryStruct << "INSERT INTO favorites VALUES(default,'" << userData.name << "','" << modelName << "')";
			// 			string queryAddFavorite = queryStruct.str();

			// 			stmt->execute(queryAddFavorite);
			// 		}

			// 		msg << "Added model to favorites";

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::DeleteModelFromFavorite:
			// 	{
			// 		char modelName[1024];
			// 		msg >> modelName;

			// 		ostringstream queryStruct;
			// 		queryStruct << "DELETE FROM favorites WHERE user = '" << userData.name << "' and model = '" << modelName << "'";
			// 		string queryDeleteFavorite = queryStruct.str();

			// 		stmt->execute(queryDeleteFavorite);

			// 		msg << "Deleted model from favorites";

			// 		client->Send(msg);

			// 		break;
			// 	}

			// 	case MsgTypes::GetModelsData:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;
			// 		query = "select model_name, model_folder, create_data from details where model_name != '' and awailable_from = 'users'";

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}
			// 	break;

			// 	case MsgTypes::GetCreateDateSortedModelsAsc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;
			// 		query = "select model_name, model_folder, create_data from details where model_name != '' and awailable_from = 'users' order by create_data asc";

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetCreateDateSortedModelsDesc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;
			// 		query = "select model_name, model_folder, create_data from details where model_name != '' and awailable_from = 'users' order by create_data desc";

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetCreateDateSortedFavoriteModelsAsc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;

			// 		ostringstream queryStruct;
			// 		queryStruct << "Select model_name, model_folder, create_data from details where exists (select 1 from favorites where favorites.user = '" << userData.name << "' and favorites.model = details.model_name) order by create_data asc";
			// 		query = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetCreateDateSortedFavoriteModelsDesc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;

			// 		ostringstream queryStruct;
			// 		queryStruct << "Select model_name, model_folder, create_data from details where exists (select 1 from favorites where favorites.user = '" << userData.name << "' and favorites.model = details.model_name) order by create_data desc";
			// 		query = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetOpenDateSortedModelsAsc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;
			// 		query = "select model_name, model_folder, create_data from details where model_name != '' and awailable_from = 'users' order by create_data asc";

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetOpenDateSortedModelsDesc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;
			// 		query = "select model_name, model_folder, create_data from details where model_name != '' and awailable_from = 'users' order by create_data desc";

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetOpenDateSortedFavoriteModelsAsc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;

			// 		ostringstream queryStruct;
			// 		queryStruct << "Select model_name, model_folder, create_data from details where exists (select 1 from favorites where favorites.user = '" << userData.name << "' and favorites.model = details.model_name) order by create_data asc";
			// 		query = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetOpenDateSortedFavoriteModelsDesc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;

			// 		ostringstream queryStruct;
			// 		queryStruct << "Select model_name, model_folder, create_data from details where exists (select 1 from favorites where favorites.user = '" << userData.name << "' and favorites.model = details.model_name) order by create_data desc";
			// 		query = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetAlphabetSortedModelsAsc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;
			// 		query = "select model_name, model_folder, create_data from details where model_name != '' and awailable_from = 'users' order by model_name asc";

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetAlphabetSortedModelsDesc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;
			// 		query = "select model_name, model_folder, create_data from details where model_name != '' and awailable_from = 'users' order by model_name desc";

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetAlphabetSortedFavoriteModelsAsc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;

			// 		ostringstream queryStruct;
			// 		queryStruct << "Select model_name, model_folder, create_data from details where exists (select 1 from favorites where favorites.user = '" << userData.name << "' and favorites.model = details.model_name) order by model_name asc";
			// 		query = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetAlphabetSortedFavoriteModelsDesc:
			// 	{
			// 		int modelsAmount = 0;

			// 		string query;

			// 		ostringstream queryStruct;
			// 		queryStruct << "Select model_name, model_folder, create_data from details where exists (select 1 from favorites where favorites.user = '" << userData.name << "' and favorites.model = details.model_name) order by model_name desc";
			// 		query = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetModelDataByName:
			// 	{
			// 		int modelsAmount = 0;
			// 		char searchInput[1024];

			// 		msg >> searchInput;

			// 		ostringstream queryStruct;
			// 		queryStruct << "select model_name, model_folder, create_data from details where awailable_from = 'users' and model_name like '%"
			// 					<< searchInput << "%'";

			// 		string query = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetFavoriteModelsData:
			// 	{
			// 		int modelsAmount = 0;

			// 		ostringstream queryStruct;
			// 		queryStruct << "Select model_name, model_folder, create_data from details where exists (select 1 from favorites where favorites.user = '" << userData.name << "' and favorites.model = details.model_name)";
			// 		string query = queryStruct.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			modelsAmount++;

			// 		string modelsAmountStr = to_string(modelsAmount);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			string categoryName = res->getString(2);
			// 			string dateName = res->getString(3);

			// 			msg << fileName.c_str() << categoryName.c_str() << dateName.c_str();

			// 			// если модель находится в избранных, то разукрашиваем кнопку
			// 			ostringstream queryStruct;
			// 			queryStruct << "select * from favorites where user ='" << userData.name << "' and model ='" << fileName.c_str() << "'";
			// 			string querySelectFavorites = queryStruct.str();

			// 			ResultSet *isModelFavoriteRes = stmt->executeQuery(querySelectFavorites);

			// 			if (isModelFavoriteRes->next())
			// 				msg << "true";
			// 			else
			// 				msg << "false";

			// 			delete isModelFavoriteRes;
			// 		}

			// 		msg << modelsAmountStr.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::SetActiveModel:
			// 	{
			// 		char modelFolder[1024];
			// 		msg >> modelFolder;
			// 		userData.observingModel = modelFolder;

			// 		msg << "Selected active model";
			// 		client->Send(msg);
			// 	}

				case MsgTypes::GetModelFiles:
				{
					int filesAmountInt = 0;
					char modelName[256];
					msg >> modelName;

					ostringstream query_struct_check_user;
					query_struct_check_user << "select model_file_name, model_folder from models where model_name = '"
											<< modelName << "' and model_available_from = 'user'";

					string query = query_struct_check_user.str();

					ResultSet *res = stmt->executeQuery(query);

					while (res->next()) filesAmountInt++;

					string filesAmountString = to_string(filesAmountInt);

					res = stmt->executeQuery(query);

					while (res->next())
					{
						string fileName = res->getString(1);
						msg << fileName.c_str();
					}

					msg << filesAmountString.c_str();

					client->Send(msg);

					delete res;

					break;
				}

			// 	case MsgTypes::GetModelFileNames:
			// 	{
			// 		ostringstream query_struct_check_user;
			// 		query_struct_check_user << "select file_name from details where model_name = '"
			// 								   "' and model_folder = '"
			// 								<< userData.observingModel << "' and awailable_from = '"
			// 								<< userData.group << "'";
			// 		string query = query_struct_check_user.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			msg << fileName.c_str();
			// 		}

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetOpenDateSortedModelFilesAsc:
			// 	{
			// 		int filesAmountInt = 0;

			// 		ostringstream query_struct_check_user;
			// 		query_struct_check_user << "select file_name, model_folder from details where model_name = '"
			// 								   "' and model_folder = '"
			// 								<< userData.observingModel << "' and awailable_from = '"
			// 								<< userData.group << "' order by create_data asc";
			// 		string query = query_struct_check_user.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			filesAmountInt++;

			// 		string filesAmountString = to_string(filesAmountInt);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			msg << fileName.c_str();
			// 		}

			// 		msg << filesAmountString.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetOpenDateSortedModelFilesDesc:
			// 	{
			// 		int filesAmountInt = 0;

			// 		ostringstream query_struct_check_user;
			// 		query_struct_check_user << "select file_name, model_folder from details where model_name = '"
			// 								   "' and model_folder = '"
			// 								<< userData.observingModel << "' and awailable_from = '"
			// 								<< userData.group << "' order by create_data desc";
			// 		string query = query_struct_check_user.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			filesAmountInt++;

			// 		string filesAmountString = to_string(filesAmountInt);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			msg << fileName.c_str();
			// 		}

			// 		msg << filesAmountString.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetCreateDateSortedModelFilesAsc:
			// 	{
			// 		int filesAmountInt = 0;

			// 		ostringstream query_struct_check_user;
			// 		query_struct_check_user << "select file_name, model_folder from details where model_name = '"
			// 								   "' and model_folder = '"
			// 								<< userData.observingModel << "' and awailable_from = '"
			// 								<< userData.group << "' order by create_data asc";
			// 		string query = query_struct_check_user.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			filesAmountInt++;

			// 		string filesAmountString = to_string(filesAmountInt);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			msg << fileName.c_str();
			// 		}

			// 		msg << filesAmountString.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetCreateDateSortedModelFilesDesc:
			// 	{
			// 		int filesAmountInt = 0;

			// 		ostringstream query_struct_check_user;
			// 		query_struct_check_user << "select file_name, model_folder from details where model_name = '"
			// 								   "' and model_folder = '"
			// 								<< userData.observingModel << "' and awailable_from = '"
			// 								<< userData.group << "' order by create_data desc";
			// 		string query = query_struct_check_user.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			filesAmountInt++;

			// 		string filesAmountString = to_string(filesAmountInt);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			msg << fileName.c_str();
			// 		}

			// 		msg << filesAmountString.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetAlphabetSortedModelFilesAsc:
			// 	{
			// 		int filesAmountInt = 0;

			// 		ostringstream query_struct_check_user;
			// 		query_struct_check_user << "select file_name, model_folder from details where model_name = '"
			// 								   "' and model_folder = '"
			// 								<< userData.observingModel << "' and awailable_from = '"
			// 								<< userData.group << "' order by file_name asc";
			// 		string query = query_struct_check_user.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			filesAmountInt++;

			// 		string filesAmountString = to_string(filesAmountInt);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			msg << fileName.c_str();
			// 		}

			// 		msg << filesAmountString.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}

			// 	case MsgTypes::GetAlphabetSortedModelFilesDesc:
			// 	{
			// 		int filesAmountInt = 0;

			// 		ostringstream query_struct_check_user;
			// 		query_struct_check_user << "select file_name, model_folder from details where model_name = '"
			// 								   "' and model_folder = '"
			// 								<< userData.observingModel << "' and awailable_from = '"
			// 								<< userData.group << "' order by file_name desc";
			// 		string query = query_struct_check_user.str();

			// 		ResultSet *res = stmt->executeQuery(query);

			// 		while (res->next())
			// 			filesAmountInt++;

			// 		string filesAmountString = to_string(filesAmountInt);

			// 		res = stmt->executeQuery(query);

			// 		while (res->next())
			// 		{
			// 			string fileName = res->getString(1);
			// 			msg << fileName.c_str();
			// 		}

			// 		msg << filesAmountString.c_str();

			// 		client->Send(msg);

			// 		delete res;

			// 		break;
			// 	}
			// 	break;
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
