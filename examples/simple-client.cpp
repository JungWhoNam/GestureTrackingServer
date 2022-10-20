#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() 
{
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Socket creating error." << std::endl;
		exit(EXIT_FAILURE);
	}

	uint16_t port = 8000;
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
		std::cerr << "Invalid address/ Address not supported." << std::endl;
		exit(EXIT_FAILURE);
	}

	{
		int client_fd;
		if ((client_fd = connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) < 0) {
			std::cerr << "Connection Failed." << std::endl;
			exit(EXIT_FAILURE);
		}

		const uint16_t BUFFER_SIZE = 0xFFFF;
		char tempBuffer[BUFFER_SIZE];
        int messageLength;
		while(true) {
			if ((messageLength = recv(sock, tempBuffer, BUFFER_SIZE, 0)) > 0) {
				tempBuffer[messageLength] = '\0';
				std::cout << std::string(tempBuffer, messageLength) << std::endl;
			}
		}

		close(client_fd);
	}

	return 0;
}
