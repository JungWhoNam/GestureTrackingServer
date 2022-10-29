// based on https://github.com/eminfedar/async-sockets-cpp
#if defined(__linux__) || defined(__APPLE__)
#include "async-sockets/tcpsocket.hpp"
#elif _WIN32
#include "async-sockets-wins/tcpsocket.hpp"
#endif

#include <iostream>

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