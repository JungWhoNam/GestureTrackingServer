#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char const* argv[])
{
    int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Socket creating error." << std::endl;
		exit(EXIT_FAILURE);
	}

    // Set options to reuse the address and port
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int))) {
        std::cerr << "setsockopt failed" << std::endl;
		exit(EXIT_FAILURE);
    };
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int))) {
        std::cerr << "setsockopt failed" << std::endl;
		exit(EXIT_FAILURE);
    };

    uint16_t port = 8000;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

	if (bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0) {
		std::cerr << "Cannot bind the socket." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(sock, 3) < 0) {
		std::cerr << "Error: Server can't listen the socket." << std::endl;
		exit(EXIT_FAILURE);
	}

    { 
        sockaddr_in newSocketInfo;
        socklen_t newSocketInfoLength = sizeof(newSocketInfo);

        int newSock;
        if ((newSock = accept(sock, (sockaddr*)&newSocketInfo, &newSocketInfoLength)) < 0) {
            std::cerr << "Error while accepting a new connection." << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string message = "Hello from server";
        while (true) {
            // send messages
            send(newSock, message.c_str(), message.length(), 0);

            // sleeps for 3 second
            usleep(3 * 1000000);
        }

        // closing the connected socket
        close(newSock);
    }
	
    // closing the listening socket
	shutdown(sock, SHUT_RDWR);

	return 0;
}
