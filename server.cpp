// based on https://github.com/eminfedar/async-sockets-cpp
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>

#include <math.h>
#include <chrono>

#include "tcpserver.hpp"
#include "json.hpp"

#define PI 3.14159265

int main()
{
    // Initialize socket.
    TCPServer tcpServer([](int errorCode, std::string errorMessage){
        std::cout << "Socket creation error:" << errorCode << " : " << errorMessage << std::endl;
        exit(EXIT_FAILURE);
    });

    // Create a list containing connected clients.
    std::list<TCPSocket*> tcpClients;

    // When a new client connected:
    tcpServer.onNewConnection = [&tcpClients](TCPSocket *newClient) {
        std::cout << "New client: [" << newClient->remoteAddress() << ":" << newClient->remotePort() << "]" << std::endl << std::flush;
        
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

            tcpClients.remove(newClient);

            std::cout << "Currently opended sockets: ";
            for (std::list<TCPSocket*>::iterator it= tcpClients.begin(); it != tcpClients.end(); ++it) {
                std::cout << (*it)->remoteAddress() << ":" << (*it)->remotePort() << " ";
            }
            std::cout << '\n' << std::flush;
        };
    };

    // Bind the server to a port.
    tcpServer.Bind(8888, [&tcpServer](int errorCode, std::string errorMessage) {
        // BINDING FAILED:
        std::cerr << errorCode << " : " << errorMessage << std::endl;
        tcpServer.Close();
        exit(EXIT_FAILURE);
    });

    // Start Listening the server.
    tcpServer.Listen([&tcpServer](int errorCode, std::string errorMessage) {
        // LISTENING FAILED:
        std::cerr << errorCode << " : " << errorMessage << std::endl;

        if (errorCode == SOCKET_ERROR) { // fail to listen(...)
            tcpServer.Close();
            exit(EXIT_FAILURE);
        }
    });

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    while (true) {
        // Create a json object to send
        float t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0;
        nlohmann::ordered_json j;
        j["HAND_LEFT"] = { -1, sin(t * PI * 0.075), 1};
        j["HAND_RIGHT"] = { 1, cos(t * PI * 0.075), 1};
        j["SPINE_CHEST"] = { 0, 0, 0 };
        
        // serialize the json and send it to clients
        std::string msg = j.dump();
        for(TCPSocket* client : tcpClients) {
            client->Send(msg);
        }

        // Sleeps for 0.5 second
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Close the server before exiting the program.
    tcpServer.Close();

    return 0;
}