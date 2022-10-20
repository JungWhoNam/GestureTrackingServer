#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>

#include "async-sockets/tcpserver.hpp"

int main()
{
    int currIndex = 0;
    std::string messages[4] = {"Hello", "Planet", "Earth", "..."};

    TCPServer tcpServer;
    std::list<TCPSocket*> tcpClients;

    // When a new client connected:
    tcpServer.onNewConnection = [&](TCPSocket *newClient) {
        std::cout << "New client: [" << newClient->remoteAddress() << ":" << newClient->remotePort() << "]" << std::endl;
        
        tcpClients.push_back(newClient);

        // newClient->onMessageReceived = [newClient](std::string message) {
        //     std::cout << newClient->remoteAddress() << ":" << newClient->remotePort() << " => " << message << std::endl;
        //     newClient->Send("OK!");
        // };

        // If you want to use raw bytes
        /*
        newClient->onRawMessageReceived = [newClient](const char* message, int length) {
            cout << newClient->remoteAddress() <<s ":" << newClient->remotePort() << " => " << message << "(" << length << ")" << endl;
            newClient->Send("OK!");
        };
        */

        newClient->onSocketClosed = [newClient, &tcpClients](int errorCode) {
            std::cout << "Socket closed:" << newClient->remoteAddress() << ":" << newClient->remotePort() << " -> " << errorCode << std::endl;
            std::cout << std::flush;

            tcpClients.remove_if([newClient](TCPSocket*& var){
                return (var->remoteAddress().compare(newClient->remoteAddress()) == 0) 
                    && (var->remotePort() == newClient->remotePort());
            });

            std::cout << "Currently opended sockets: ";
            for (std::list<TCPSocket*>::iterator it= tcpClients.begin(); it != tcpClients.end(); ++it) {
                std::cout << (*it)->remoteAddress() << ":" << (*it)->remotePort() << " ";
            }
            std::cout << '\n';
        };
    };

    // Bind the server to a port.
    tcpServer.Bind(8888, [](int errorCode, std::string errorMessage) {
        // BINDING FAILED:
        std::cerr << errorCode << " : " << errorMessage << std::endl; 
    });

    // Start Listening the server.
    tcpServer.Listen([](int errorCode, std::string errorMessage) {
        // LISTENING FAILED:
        std::cerr << errorCode << " : " << errorMessage << std::endl; 
    });

    while (true) {
        for(TCPSocket* client : tcpClients) {
            client->Send(messages[currIndex]);
        }

        currIndex += 1;
        if (currIndex >= sizeof(messages) / sizeof(std::string)) {
            currIndex = 0;
        }

        // Sleeps for 3 second
        usleep(3 * 1000000);
    }

    // Close the server before exiting the program.
    tcpServer.Close();

    return 0;
}