#pragma once

namespace net
{
	template <typename T>
	class net_client
	{
	public:
		net_client()
		{
		}

		virtual ~net_client()
		{
			Disconnect();
		}

	public:
		bool Connect(const char *ipAddress, const char *portNum)
		{
			try
			{
				addrinfo hints, *p;
				memset(&hints, 0, sizeof(hints));
				hints.ai_family = AF_UNSPEC;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_flags = AI_PASSIVE;

				int gAddRes = getaddrinfo(ipAddress, portNum, &hints, &p);

				clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

				if (clientfd == -1)
					throw;

				int connect_reply = connect(clientfd, p->ai_addr, p->ai_addrlen);
				if (connect_reply == -1)
				{
					close(clientfd);
					throw;
				}
				else
				{
					efd = epoll_create1(0);
					event.events = EPOLLIN;
					event.data.fd = clientfd;
					epoll_ctl(efd, EPOLL_CTL_ADD, clientfd, &event);
				}

				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		void Disconnect()
		{
		}

	public:
		void SendMessage(net::message<MsgTypes> imsg, MsgTypes type)
		{
			imsg.header.id = type;
			char buffer[sizeof(imsg.header) + imsg.size()];
			memcpy((void *)buffer, &imsg.header.id, sizeof(imsg.header.id));
			memcpy((void *)(buffer + sizeof(imsg.header.size)), &imsg.header.size, sizeof(imsg.header.size));
			if (imsg.size())
				memcpy((void *)(buffer + sizeof(imsg.header)), imsg.body.data(), imsg.size());

			write(clientfd, buffer, sizeof(imsg.header) + imsg.size());
		}

		int ReadMessage(int clientFd, net::message<MsgTypes> &omsg)
		{
			char buffer[8];
			int readlen = read(clientFd, buffer, sizeof(buffer));

			int headerVal = 0;
			int iDataSize = 0;
			memcpy(&headerVal, buffer, 4);
			memcpy(&iDataSize, buffer + 4, 4);

			char bufferData[iDataSize];
			omsg.body.resize(iDataSize);

			std::size_t totalBytesReceived = 0;
			while (totalBytesReceived < iDataSize)
			{
				std::size_t bytesRead = read(clientFd, bufferData + totalBytesReceived, iDataSize);
				totalBytesReceived += bytesRead;
			}

			memcpy(omsg.body.data(), bufferData, iDataSize);

			omsg.header.id = (MsgTypes)headerVal;

			if (errno != EAGAIN && !readlen)
				return -1;
			return 0;
		}

	protected:
		int clientfd;
		int efd;
		epoll_event event;
		epoll_event events[1];
	};
}
