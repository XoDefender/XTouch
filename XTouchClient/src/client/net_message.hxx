#pragma once
#include "net_common.hxx"

namespace olc
{
	namespace net
	{
		///[OLC_HEADERIFYIER] START "MESSAGE"

		// Message Header is sent at start of all messages. The template allows us
		// to use "enum class" to ensure that the messages are valid at compile time
		template <typename T>
		struct message_header
		{
			T id{};
			uint32_t size = 0;
		};

		// Message Body contains a header and a std::vector, containing raw bytes
		// of infomation. This way the message can be variable length, but the size
		// in the header must be updated.
		template <typename T>
		struct message
		{
			// Header & Body vector
			message_header<T> header{};
			std::vector<uint8_t> body;

			// returns size of entire message packet in bytes
			size_t size() const
			{
				return body.size();
			}

			// Override for std::cout compatibility - produces friendly description of message
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

				// Cache current size of vector, as this will be the point we insert the data
				size_t i = msg.body.size();

				// Resize the vector by the size of the data being pushed
				msg.body.resize(msg.body.size() + actualSize + 1);

				// Physically copy the data into the newly allocated vector space
				std::memcpy(msg.body.data() + i, data, actualSize + 1);

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be "chained"
				return msg;
			}

			friend message<T> &operator<<(message<T> &msg, std::string data)
			{
				int actualSize = data.size();

				// Cache current size of vector, as this will be the point we insert the data
				size_t i = msg.body.size();

				// Resize the vector by the size of the data being pushed
				msg.body.resize(msg.body.size() + actualSize);

				// Physically copy the data into the newly allocated vector space
				std::memcpy(msg.body.data() + i, data.c_str(), actualSize);

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be "chained"
				return msg;
			}

			friend message<T> &operator>>(message<T> &msg, std::string& data)
			{
				int sizeToCopy = msg.body.size();
				
				std::string tmp(msg.body.begin(), msg.body.end());
				data = tmp;

				// Shrink the vector to remove read bytes, and reset end position
				msg.body.resize(sizeToCopy);

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be "chained"
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

				// Cache the location towards the end of the vector where the pulled data starts
				int i = msg.body.size() - (sizeToCopy + 1);

				// Physically copy the data from the vector into the user variable
				std::memcpy(data, msg.body.data() + i, sizeToCopy + 1);

				// Shrink the vector to remove read bytes, and reset end position
				msg.body.resize(i);

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be "chained"
				return msg;
			}
		};

		// An "owned" message is identical to a regular message, but it is associated with
		// a connection. On a server, the owner would be the client that sent the message,
		// on a client the owner would be the server.

		// Forward declare the connection
		template <typename T>
		class connection;

		template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;

			// Again, a friendly string maker
			friend std::ostream &operator<<(std::ostream &os, const owned_message<T> &msg)
			{
				os << msg.msg;
				return os;
			}
		};

		///[OLC_HEADERIFYIER] END "MESSAGE"
	}
}