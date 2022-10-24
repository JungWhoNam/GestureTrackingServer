#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>

#include <math.h>
#include <chrono>

#include "async-sockets/tcpserver.hpp"

#define PI 3.14159265

int main()
{
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

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    while (true) {
        // Compute a value to send
        float t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0;
        float v = sin(t * PI * 0.075);
        // std::cout << "t=" << t << ", v=" << v << std::endl;

        for(TCPSocket* client : tcpClients) {
            client->Send(std::to_string(v));
        }

        // Sleeps for 0.5 second
        usleep(0.01 * 1000000);
    }

    // Close the server before exiting the program.
    tcpServer.Close();

    return 0;
}