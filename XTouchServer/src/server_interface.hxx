
#pragma once

#include "net_common.hxx"
#include "net_tsqueue.hxx"
#include "net_message.hxx"
#include "net_connection.hxx"

#include <mysql.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace sql;

template <typename T>
class server_interface
{
public:
	server_interface(uint16_t port) : m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

	virtual ~server_interface() { Stop(); }

	void ConnectToMySQL(std::string ip, std::string name, std::string password, std::string schema)
	{
		Driver *driver = get_driver_instance();
		Connection *con = driver->connect(ip, name, password);

		con->setSchema(schema);

		stmt = con->createStatement();
	}

	bool Start()
	{
		try
		{
			WaitForClientConnection();
		}
		catch (std::exception &e)
		{
			std::cerr << "[SERVER] Exception: " << e.what() << "\n";
			return false;
		}

		std::cout << "[SERVER] Started!\n";
		return true;
	}

	void Stop()
	{
		m_qMessagesIn.clear();
		std::cout << "[SERVER] Stopped!\n";
	}

	// TODO:Kirill - implement incoming connections handler
	void WaitForClientConnection()
	{
		std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

		// Create a new connection to handle this client
		std::shared_ptr<connection<T>> newconn = std::make_shared<connection<T>>(connection<T>::owner::server, m_asioContext, std::move(socket), m_qMessagesIn);

		if (OnClientConnect(newconn))
		{
			// Connection allowed, so add to container of new connections
			m_deqConnections.push_back(std::move(newconn));

			// And very important! Issue a task to the connection's
			// asio context to sit and wait for bytes to arrive!
			m_deqConnections.back()->ConnectToClient(nIDCounter++);

			std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
		}
		else
		{
			std::cout << "[-----] Connection Denied\n";
		}

		WaitForClientConnection();
	};

	void MessageClient(std::shared_ptr<connection<T>> client, const message<T> &msg)
	{
		if (client && client->IsConnected())
		{
			client->Send(msg);
		}
		else
		{
			OnClientDisconnect(client);
			client.reset();
			m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), client), m_deqConnections.end());
		}
	}

	void MessageAllClients(const message<T> &msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr)
	{
		bool bInvalidClientExists = false;

		for (auto &client : m_deqConnections)
		{
			if (client && client->IsConnected())
			{
				if (client != pIgnoreClient)
					client->Send(msg);
			}
			else
			{
				// The client couldnt be contacted, so assume it has disconnected.
				OnClientDisconnect(client);
				client.reset();

				// Set this flag to then remove dead clients from container
				bInvalidClientExists = true;
			}
		}

		// Remove dead clients, all in one go - this way, we dont invalidate the container as we iterated through it.
		if (bInvalidClientExists) m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), nullptr), m_deqConnections.end());
	}

	void
	Update(size_t nMaxMessages = -1, bool bWait = false)
	{
		if (bWait)
			m_qMessagesIn.wait();

		size_t nMessageCount = 0;
		while (nMessageCount < nMaxMessages && !m_qMessagesIn.empty())
		{
			auto msg = m_qMessagesIn.pop_front();
			OnMessage(msg.remote, msg.msg);
			nMessageCount++;
		}
	}

protected:
	virtual bool
	OnClientConnect(std::shared_ptr<connection<T>> client)
	{
		return false;
	}
	virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client) {}
	virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T> &msg) {}

protected:
	int socket;
	tsqueue<owned_message<T>> m_qMessagesIn;
	std::deque<std::shared_ptr<connection<T>>> m_deqConnections;
	std::thread m_threadContext;
	uint32_t nIDCounter = 10000;
	Statement *stmt;
}