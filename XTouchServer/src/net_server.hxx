
#pragma once

#include <iostream>
#include "net_modules.hxx"

template <typename T>
class net_server
{
public:
	net_server(uint16_t port, int maxConnections)
	{
		try
		{
			this->maxConnections = maxConnections;
			socket_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (socket_fd == -1)
				throw;

			fcntl(socket_fd, F_SETFL, O_NONBLOCK);

			memset(&serverAddr, 0, sizeof(sockaddr_in));
			serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
			serverAddr.sin_port = htons(port);
			serverAddr.sin_family = AF_INET;

			if (bind(socket_fd, (sockaddr *)&serverAddr, sizeof(serverAddr)) != 0)
				throw;
			if (listen(socket_fd, maxConnections) == -1)
				throw;

			events = (epoll_event *)calloc(maxConnections, sizeof(epoll_event));
			event.events = EPOLLIN;
			event.data.fd = socket_fd;

			efd = epoll_create(maxConnections);
			epoll_ctl(efd, EPOLL_CTL_ADD, socket_fd, &event);
		}
		catch (...)
		{
			std::cout << "Some error\n";
		}
	}

	virtual ~net_server() {}

	int ConnectClient()
	{
		struct sockaddr clientSocketData;
		socklen_t clientSocketDataSize = sizeof(clientSocketData);
		int clientSocketFd = accept(socket_fd, &clientSocketData, &clientSocketDataSize);
		char clientIp[INET_ADDRSTRLEN];
		uint16_t clientPort;
		if (clientSocketData.sa_family == AF_INET)
		{
			struct sockaddr_in *clientSocketDataIp4 = (struct sockaddr_in *)&clientSocketData;
			clientPort = ntohs(clientSocketDataIp4->sin_port);
			inet_ntop(AF_INET, &(clientSocketDataIp4->sin_addr), clientIp, INET_ADDRSTRLEN);
		}

		std::cout << "Client connected = Ip: " << std::string(clientIp) << "  Port: " << clientPort << std::endl;

		SessionManager::GetInstance()->RegisterUser(std::string(clientIp), clientPort, clientSocketFd);

		event.events = EPOLLIN;
		event.data.fd = clientSocketFd;
		epoll_ctl(efd, EPOLL_CTL_ADD, clientSocketFd, &event);

		return clientSocketFd;
	}

	void DisconnectClient(int clientFd)
	{
		struct sockaddr_in peerAddress;
		socklen_t peerAddressLength = sizeof(peerAddress);
		if (getpeername(clientFd, reinterpret_cast<sockaddr *>(&peerAddress), &peerAddressLength) == 0)
		{
			std::string clientIp = std::string(inet_ntoa(peerAddress.sin_addr));
			int clientPort = ntohs(peerAddress.sin_port);

			std::cout << "Client disconnected Ip: " << std::string(clientIp) << "  Port: " << clientPort << std::endl;

			SessionManager::GetInstance()->UnregisterUser(clientIp, clientPort);

			close(clientFd);
			epoll_ctl(efd, EPOLL_CTL_DEL, clientFd, NULL);
		}
	}

	void SendMessage(std::string message, MsgTypes type, int clientFd)
	{
		net::message<MsgTypes> imsg;
		imsg.header.id = type;
		imsg << message.data();

		char buffer[sizeof(imsg.header.id) + imsg.body.size()];
		memcpy((void *)buffer, &imsg.header.id, sizeof(imsg.header.id));
		memcpy((void *)(buffer + sizeof(imsg.header.id)), imsg.body.data(), imsg.body.size());

		write(clientFd, buffer, sizeof(imsg.header.id) + imsg.body.size());
	}

	int ReadMessage(int clientFd, net::message<MsgTypes> &omsg)
	{
		char buffer[1024];
		int readlen = read(clientFd, buffer, sizeof(buffer));

		int headerVal = 0;
		int iDataSize = 0;
		memcpy(&headerVal, buffer, 4);
		memcpy(&iDataSize, buffer + 4, 4);

		omsg.body.resize(iDataSize);
		memcpy(omsg.body.data(), buffer + 8, iDataSize);

		omsg.header.id = (MsgTypes)headerVal;

		if(errno != EAGAIN && !readlen) return -1;
		return 0;
	}

	void ProcessRequest(net::message<MsgTypes> &imsg)
	{
		switch (imsg.header.id)
		{
		case MsgTypes::PasswordLogin:
		{
			char login[50];
			char pass[50];
			imsg >> pass;
			imsg >> login;

			std::cout<<pass<<std::endl;
			std::cout<<login<<std::endl;

			break;
		}
		
		default:
			break;
		}
	}

	void Start()
	{
		try
		{
			while (1)
			{
				int activeEvents = epoll_wait(efd, events, 1, -1);
				for (int i = 0; i < activeEvents; i++)
				{
					if (events[i].data.fd == socket_fd)
					{
						int clientFd = ConnectClient();
						SendMessage("Hello from server", MsgTypes::ServerAccept, clientFd);
					}
					else
					{
						net::message<MsgTypes> omsg;
						if (ReadMessage(events[i].data.fd, omsg) == -1)
						{
							DisconnectClient(events[i].data.fd);
						}
						else
						{
							ProcessRequest(omsg);
						}
					}
				}
			}
		}
		catch (...)
		{
			std::cout << "Some error\n";
			return;
		}
	}

	// 	void Stop()
	// 	{
	// 		m_qMessagesIn.clear();
	// 	}

	// 	void WaitForClientConnection()
	// 	{
	// 		std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

	// 		// Create a new connection to handle this client
	// 		std::shared_ptr<connection<T>> newconn = std::make_shared<connection<T>>(connection<T>::owner::server, m_asioContext, std::move(socket), m_qMessagesIn);

	// 		if (OnClientConnect(newconn))
	// 		{
	// 			// Connection allowed, so add to container of new connections
	// 			m_deqConnections.push_back(std::move(newconn));

	// 			// And very important! Issue a task to the connection's
	// 			// asio context to sit and wait for bytes to arrive!
	// 			m_deqConnections.back()->ConnectToClient(nIDCounter++);

	// 			std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
	// 		}
	// 		else
	// 		{
	// 			std::cout << "[-----] Connection Denied\n";
	// 		}

	// 		WaitForClientConnection();
	// 	};

	// 	void MessageClient(std::shared_ptr<connection<T>> client, const message<T> &msg)
	// 	{
	// 		if (client && client->IsConnected())
	// 		{
	// 			client->Send(msg);
	// 		}
	// 		else
	// 		{
	// 			OnClientDisconnect(client);
	// 			client.reset();
	// 			m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), client), m_deqConnections.end());
	// 		}
	// 	}

	// 	void MessageAllClients(const message<T> &msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr)
	// 	{
	// 		bool bInvalidClientExists = false;

	// 		for (auto &client : m_deqConnections)
	// 		{
	// 			if (client && client->IsConnected())
	// 			{
	// 				if (client != pIgnoreClient)
	// 					client->Send(msg);
	// 			}
	// 			else
	// 			{
	// 				// The client couldnt be contacted, so assume it has disconnected.
	// 				OnClientDisconnect(client);
	// 				client.reset();

	// 				// Set this flag to then remove dead clients from container
	// 				bInvalidClientExists = true;
	// 			}
	// 		}

	// 		// Remove dead clients, all in one go - this way, we dont invalidate the container as we iterated through it.
	// 		if (bInvalidClientExists) m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), nullptr), m_deqConnections.end());
	// 	}

	// protected:
	// 	virtual bool
	// 	OnClientConnect(std::shared_ptr<connection<T>> client)
	// 	{
	// 		return false;
	// 	}
	// 	virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client) {}
	// 	virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T> &msg) {}

protected:
	int maxConnections;
	int socket_fd;
	int efd;
	struct epoll_event event;
	struct epoll_event *events;
	sockaddr_in serverAddr;
	// std::deque<std::shared_ptr<connection<T>>> m_deqConnections;
};