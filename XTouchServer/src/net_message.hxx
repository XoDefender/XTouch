#pragma once
#include "net_common.hxx"

namespace net
{
	template <typename T>
	struct message_header
	{
		T id{};
		uint32_t size = 0;
	};

	template <typename T>
	struct message
	{
		message_header<T> header{};
		std::vector<uint8_t> body;
		size_t size() const
		{
			return body.size();
		}
		friend std::ostream &operator<<(std::ostream &os, const message<T> &msg)
		{
			os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
			return os;
		}

		friend message<T> &operator<<(message<T> &msg, const char *data)
		{
			int actualSize = 0;
			while (data[actualSize] != '\0')
			{
				actualSize++;
			}
			size_t i = msg.body.size();
			msg.body.resize(msg.body.size() + actualSize + 1);
			std::memcpy(msg.body.data() + i, data, actualSize + 1);
			msg.header.size = msg.size();
			return msg;
		}

		friend message<T> &operator<<(message<T> &msg, std::string data)
		{
			int actualSize = data.size();
			size_t i = msg.body.size();
			msg.body.resize(msg.body.size() + actualSize);
			std::memcpy(msg.body.data() + i, data.c_str(), actualSize);
			msg.header.size = msg.size();

			return msg;
		}

		friend message<T> &operator>>(message<T> &msg, std::string &data)
		{
			int sizeToCopy = msg.body.size();

			std::string tmp(msg.body.begin(), msg.body.end());
			data = tmp;

			msg.body.resize(sizeToCopy);
			msg.header.size = msg.size();

			return msg;
		}

		friend message<T> &operator>>(message<T> &msg, char *data)
		{
			int sizeToCopy = 0;
			std::vector<uint8_t>::reverse_iterator rIt = msg.body.rbegin() + 1;

			while (*rIt != '\0')
			{
				if (rIt == msg.body.rend() - 1)
				{
					sizeToCopy++;
					break;
				}

				++rIt;
				sizeToCopy++;
			}

			int i = msg.body.size() - (sizeToCopy + 1);
			std::memcpy(data, msg.body.data() + i, sizeToCopy + 1);
			msg.body.resize(i);
			msg.header.size = msg.size();

			return msg;
		}
	};

	template <typename T>
	class connection;

	template <typename T>
	struct owned_message
	{
		std::shared_ptr<connection<T>> remote = nullptr;
		message<T> msg;

		friend std::ostream &operator<<(std::ostream &os, const owned_message<T> &msg)
		{
			os << msg.msg;
			return os;
		}
	};
}
