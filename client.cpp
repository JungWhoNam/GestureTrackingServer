// based on https://github.com/eminfedar/async-sockets-cpp
#include <iostream>

#include "tcpsocket.hpp"
#include "json.hpp"

using namespace std;

int main()
{
    // Initialize socket.
    TCPSocket tcpSocket([](int errorCode, std::string errorMessage){
        cout << "Socket creation error:" << errorCode << " : " << errorMessage << endl;
    });

    // Start receiving from the host.
    tcpSocket.onMessageReceived = [](string message) {
        cout << "Message from the Server: " << message << endl;

        nlohmann::ordered_json j = nlohmann::ordered_json::parse(message);
        if (j == nullptr || j["hand_left"] == nullptr || j["hand_right"] == nullptr) return;
        cout << "----> hand_left : " << j["hand_left"] << endl;
        cout << "----> hand_right: " << j["hand_right"] << endl;

        std::cout << std::flush;
    };
    // If you want to use raw bytes instead of std::string:
    /*
    tcpSocket.onRawMessageReceived = [](const char* message, int length) {
        cout << "Message from the Server: " << message << "(" << length << ")" << endl;
    };
    */
    
    // On socket closed:
    tcpSocket.onSocketClosed = [](int errorCode){
        cout << "Connection closed: " << errorCode << endl;
    };

    // Connect to the host.
    tcpSocket.Connect("localhost", 8888, [&] {
        cout << "Connected to the server successfully." << endl;

        // Send String:
        // tcpSocket.Send("Hello Server!");
    },
    [](int errorCode, std::string errorMessage){
        // CONNECTION FAILED
        cout << errorCode << " : " << errorMessage << endl;
    });

    // You should do an input loop so the program will not end immediately:
    // Because socket listenings are non-blocking.
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        // tcpSocket.Send(input);
        getline(cin, input);
    }

    tcpSocket.Close();

    return 0;
}